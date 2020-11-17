/*
 * Author: Weimin (Cheryl) Huang
 *
 * Created on Jan 30, 2018
 * Modified on April 10, 2018
 */
  

/////struct type name in lower case
struct setType{
    int B;
    int N;
    int S;
    int W;
};
struct comp{
    struct setType type;
    int setNum;
};
struct input{
    int stepNum;
    struct comp C1;
    struct comp C2;
    struct comp C3;
    struct comp C4;
    struct comp C5;
    struct comp C6;
    struct comp C7;
    struct comp C8;
};
//////Variable names in structs do not have "_"

/***** Function Prototypes *****/
struct input init_struct(void);
int valid_step(int);
int valid_set_type(struct setType);
int valid_set_num(int,int);
int convert_1(int);
int convert_2(int,int);
void input_set_type(struct input *,int);
void input_step_num(struct input *);
void input_set_num(struct input *, struct setType,int);
void userInput(struct input * );
void end_message(struct input *,int,int*,int*,int*,int*);
void servoLifting(void);
void servoLowering(void);
void load(struct input* ,int,int,int,int);//FOR EACH COMPARTMENT
void reservoir(int* ,int* ,int* ,int* ,int,int,int);
void boxRotate2(float, int);
void clamp(void);
void unclamp(void);
void startKey(void);
void closeBox(void);
unsigned short readADC(char channel);
void correctPosition(void);
void pack(struct input* ,int,int,int);
void motor_normal(void);
void motor_reverse(void);
void motor_off(void);
int gearTeeth(float);



/***** Includes *****/
#include <xc.h>
#include "configBits.h"
#include <stdio.h>
#include "lcd.h"
#include "lcd.h"
#include "I2C.h"

/***** Macros *****/
#define __bcd_to_num(num) (num & 0x0F) + ((num & 0xF0)>>4)*10


/***** Constants *****/
const char keys[] = "123A456B789C*0#D";

int standBy=1;
int counter;
int end=0;

void main(void) {
    
    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    /******************************* OSCILLATOR *******************************/
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCTUNEbits.PLLEN = 0;
    /********************************* PIN I/O ********************************/
    /* Write outputs to LATx, read inputs from PORTx. Here, all latches (LATx)
     * are being cleared (set low) to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. 0 --> output; 1 --> input. Default is  1. */
    TRISA = 0x00; // All inputs (this is the default, but is explicated here for learning purposes)
    TRISB = 0b11110010;
    TRISC = 0b01000000;
    TRISD = 0b00000000; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    ////PWM SETTING
    T2CON=0b00000111;
    CCP1CON = 0b00001100;
    CCP2CON=0b00001100;
    PR2 = 255;
    ////PWM SETTING
    
    //BOX LIFTED AND UNCLAMPED
    
    CCPR1L=0B01111101;
    //CCPR2L=0b10001111;
    CCPR2L=0b00010111;

    INTCONbits.GIE=1;
    INT1IE = 1; // Enable RB1 (keypad data available) interrupt
    ei(); // Enable all interrupts
    I2C_Master_Init(100000); //Initialize I2C Master with 100 kHz clock
    /* Initialize LCD. */
    initLCD();
    
    //priority
    TMR0IP=1;
    INT1IP=0;
    IPEN=1;
    INTCONbits.PEIE=1;
    
    
    unsigned char time[7]; // Create a byte array to hold time read from RTC
    unsigned char i; // Loop counter
    
    
    
    while(1){
    
        //INT1IE = 0;
        //I2C_Master_Init(100000);
        /* Reset RTC memory pointer. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition

        /* Read current time. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        for(i = 0; i < 6; i++){
            time[i] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition
        
        /* Print received data to LCD. */
        __lcd_clear();
        __lcd_home();
        printf("%02x:%02x:%02x", time[2],time[1],time[0]); // HH:MM:SS
        __lcd_newline();
        printf("Press Any Key   ");
        __delay_ms(1000);
    }
    
}

void interrupt low_priority interruptHandler(void){
    
    
    
    struct input theInput;
    //Input: name of struct
    theInput = init_struct();
    struct input* Input=&theInput;
    int start;
    int Bremain=0;
    int Nremain=0;
    int Sremain=0;
    int Wremain=0;
    int* remain_B=&Bremain;
    int* remain_N=&Nremain;
    int* remain_S=&Sremain;
    int* remain_W=&Wremain;
    int OpTime;
    int H1,M1,S1,H2,M2,S2;
    //interrupt: begins when user presses any key
    if(INT1IF){
        
        
    
    
        
        
        while (PORTBbits.RB1==0){
            continue;
        }
        //wait until interrupt key press released
        while (PORTBbits.RB1==1){
            continue;
        }
        userInput(Input);
        
        startKey();
        //enable timer
        //state
        standBy=0;
        TMR0IE=1;
        T0CON=0b11001000;
        __lcd_clear();
        
        //disable KPD
        LATCbits.LATC6=1;
        // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    /******************************* OSCILLATOR *******************************/
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 0;
    OSCTUNEbits.PLLEN = 0;
    /********************************* PIN I/O ********************************/
    /* Write outputs to LATx, read inputs from PORTx. Here, all latches (LATx)
     * are being cleared (set low) to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. 0 --> output; 1 --> input. Default is  1. */
    TRISA = 0b11000010; 
    TRISB = 0b00000000;
    TRISC = 0b10000000;
    TRISD = 0b00000011; 
    TRISE = 0b00000011;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00000000; // RE2-0 ANALOG
    ADCON2bits.ADFM = 1; // Right justify A/D result
    // </editor-fold>
        LATCbits.LATC6=1;
        
        
        
        
        
        //Start Timing
        
        /////////////////////////RTC/////////////////////////////////////////////
        printf("start timing");
        I2C_Master_Init(100000);
        /* Declare local variables. */
        unsigned char time[7]; // Create a byte array to hold time read from RTC
        unsigned char k; // Loop counter
        /* Reset RTC memory pointer. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition
        /* Read current time. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        for(k = 0; k < 6; k++){
            time[k] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition    
        H1=time[2];
        M1=time[1];
        S1=time[0];
        
        //////////////////////RTC///////////////////////////////////////////
        
        
        //activate conveyor belts
        motor_normal();
        
        //Lower the Box to put in, using continuous servo
        
        servoLowering();
        
        __delay_ms(1000);
        
        //Correct the Position of the Box
        
        correctPosition();
        
        //start packing the compartments
        pack(Input,H1,M1,S1);
        printf("back to interrupt");
        __delay_ms(2000);
        
        //Remaining fasteners go to the reservoir
        __lcd_clear();
        printf("Reservoir");
        reservoir(remain_B,remain_N,remain_S,remain_W,H1,M1,S1);
        
        //Close the Box
        closeBox();
        
        unclamp();
        __delay_ms(2000);
        
        printf("rotate back");
        boxRotate2(67.5,2);
        
        //Lift the Box
        servoLifting();
        
        
        
        //Turn off two DC motors
        motor_off();
        
        __lcd_clear();
        printf("stop timing");
        //STOP TIMING
        //////////////////////////RTC////////////////////////////////////////
        I2C_Master_Init(100000);
        /* Reset RTC memory pointer. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition

        /* Read current time. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        
        for(k = 0; k < 6; k++){
            time[k] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition
        H2=time[2];
        M2=time[1];
        S2=time[0];
        ////////////////////////////RTC///////////////////////////
        
        //Compute operation time
        if(H2>=H1){
            OpTime=(H2*3600+M2*60+S2)-(H1*3600+M1*60+S1);
        }
        else{
            OpTime=24*3600-(H1*3600+M1*60+S1)+(H2*3600+M2*60+S2);
        }
        
        
        end_message(Input,OpTime,remain_B,remain_N,remain_S,remain_W);
       
         TRISD = 0b00000010;
    // <editor-fold defaultstate="collapsed" desc="Machine Configuration">
    /********************************* PIN I/O ********************************/
    /* Write outputs to LATx, read inputs from PORTx. Here, all latches (LATx)
     * are being cleared (set low) to ensure a controlled start-up state. */  
    LATA = 0x00;
    LATB = 0x00; 
    LATC = 0x00;
    LATD = 0x00;
    LATE = 0x00;

    /* After the states of LATx are known, the data direction registers, TRISx
     * are configured. 0 --> output; 1 --> input. Default is  1. */
    TRISA = 0xFF; // All inputs (this is the default, but is explicated here for learning purposes)
    TRISB = 0b11110111;
    TRISC = 0b01000000;
    TRISD = 0b00000000; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00000111; // Set all A/D ports to digital (pg. 222)
    ADCON2bits.ADFM = 1;
    // </editor-fold>
           
    I2C_Master_Init(100000);
    I2C_Master_Start(); // Start condition
    
    //state
    TMR0IE=0;
    standBy=1;
        INT1IF = 0;  // Clear interrupt flag bit to signify it's been handled
        
        
    }
    
    
}

void interrupt tmrInterrupt(void){
    if(TMR0IF==1){
        if(standBy==1){
            TMR0IF=0;
            return;
        }
        else{
            if(counter==0){
                LATBbits.LATB1=1;
                //PORTBbits.RB1=1;
                counter+=1;
            }
            else if(counter==2){
                LATBbits.LATB1=0;
                //PORTBbits.RB1=0;
                counter+=1;
            }
            else if(counter==6){
                counter=0;
            }
            else{
                counter+=1;
            }
        }
        
        TMR0IF=0;
        
        
    }
}
 

void userInput(struct input * MyInput){
    
    input_step_num(MyInput);
    __lcd_clear();
    __lcd_home();
    if((MyInput->stepNum)==4){
        
        input_set_type(MyInput,1);
        input_set_num(MyInput,MyInput->C1.type,1);
        
        input_set_type(MyInput,3);
        input_set_num(MyInput,MyInput->C3.type,3);
        
        input_set_type(MyInput,5);
        input_set_num(MyInput,MyInput->C5.type,5);
        
        input_set_type(MyInput,7);
        input_set_num(MyInput,MyInput->C7.type,7);
  
    }
    if((MyInput->stepNum)==5){
        
        input_set_type(MyInput,1);
        input_set_num(MyInput,MyInput->C1.type,1);
        
        input_set_type(MyInput,2);
        input_set_num(MyInput,MyInput->C2.type,2);
        
        input_set_type(MyInput,4);
        input_set_num(MyInput,MyInput->C4.type,4);
        
        input_set_type(MyInput,5);
        input_set_num(MyInput,MyInput->C5.type,5);
        
        input_set_type(MyInput,7);
        input_set_num(MyInput,MyInput->C7.type,7);
  
    }
    if((MyInput->stepNum)==6){
        
        input_set_type(MyInput,1);
        input_set_num(MyInput,MyInput->C1.type,1);
        
        input_set_type(MyInput,2);
        input_set_num(MyInput,MyInput->C2.type,2);
        
        input_set_type(MyInput,3);
        input_set_num(MyInput,MyInput->C3.type,3);
        
        input_set_type(MyInput,5);
        input_set_num(MyInput,MyInput->C5.type,5);
        
        input_set_type(MyInput,6);
        input_set_num(MyInput,MyInput->C6.type,6);
        
        input_set_type(MyInput,7);
        input_set_num(MyInput,MyInput->C7.type,7);
  
    }
    if((MyInput->stepNum)==7){
        
        input_set_type(MyInput,1);
        input_set_num(MyInput,MyInput->C1.type,1);
        
        input_set_type(MyInput,2);
        input_set_num(MyInput,MyInput->C2.type,2);
        
        input_set_type(MyInput,3);
        input_set_num(MyInput,MyInput->C3.type,3);
        
        input_set_type(MyInput,4);
        input_set_num(MyInput,MyInput->C4.type,4);
        
        input_set_type(MyInput,5);
        input_set_num(MyInput,MyInput->C5.type,5);
        
        input_set_type(MyInput,6);
        input_set_num(MyInput,MyInput->C6.type,6);
        
        input_set_type(MyInput,7);
        input_set_num(MyInput,MyInput->C7.type,7);
  
    }
    if((MyInput->stepNum)==8){
        
        input_set_type(MyInput,1);
        input_set_num(MyInput,MyInput->C1.type,1);
        
        input_set_type(MyInput,2);
        input_set_num(MyInput,MyInput->C2.type,2);
        
        input_set_type(MyInput,3);
        input_set_num(MyInput,MyInput->C3.type,3);
        
        input_set_type(MyInput,4);
        input_set_num(MyInput,MyInput->C4.type,4);
        
        input_set_type(MyInput,5);
        input_set_num(MyInput,MyInput->C5.type,5);
        
        input_set_type(MyInput,6);
        input_set_num(MyInput,MyInput->C6.type,6);
        
        input_set_type(MyInput,7);
        input_set_num(MyInput,MyInput->C7.type,7);
        
        input_set_type(MyInput,8);
        input_set_num(MyInput,MyInput->C8.type,8);
  
    }
    
}

struct input init_struct(void){
    struct input mystruct;
    mystruct.stepNum=0;
    mystruct.C1.setNum=0;
    mystruct.C1.type.B=0;
    mystruct.C1.type.N=0;
    mystruct.C1.type.S=0;
    mystruct.C1.type.W=0;
    mystruct.C2.setNum=0;
    mystruct.C2.type.B=0;
    mystruct.C2.type.N=0;
    mystruct.C2.type.S=0;
    mystruct.C2.type.W=0;
    mystruct.C3.setNum=0;
    mystruct.C3.type.B=0;
    mystruct.C3.type.N=0;
    mystruct.C3.type.S=0;
    mystruct.C3.type.W=0;
    mystruct.C4.setNum=0;
    mystruct.C4.type.B=0;
    mystruct.C4.type.N=0;
    mystruct.C4.type.S=0;
    mystruct.C4.type.W=0;
    mystruct.C5.setNum=0;
    mystruct.C5.type.B=0;
    mystruct.C5.type.N=0;
    mystruct.C5.type.S=0;
    mystruct.C5.type.W=0;
    mystruct.C6.setNum=0;
    mystruct.C6.type.B=0;
    mystruct.C6.type.N=0;
    mystruct.C6.type.S=0;
    mystruct.C6.type.W=0;
    mystruct.C7.setNum=0;
    mystruct.C7.type.B=0;
    mystruct.C7.type.N=0;
    mystruct.C7.type.S=0;
    mystruct.C7.type.W=0;
    mystruct.C8.setNum=0;
    mystruct.C8.type.B=0;
    mystruct.C8.type.N=0;
    mystruct.C8.type.S=0;
    mystruct.C8.type.W=0;
    
    return mystruct;
}

int valid_step(int x){
    int valid;
    if(x==4){
        valid=1;
    }
    else if(x==5){
        valid=1;
    }
    else if(x==6){
        valid=1;
    }
    else if(x==8){
        valid=1;
    }
    else if(x==9){
        valid=1;
    }
    else{
        valid=0;
    }
    return valid;
}

int valid_set_type(struct setType mysetType){
    
    int type;
    if((mysetType.B==1)&(mysetType.N==0)&(mysetType.S==0)&(mysetType.W==0)){
        type=1;
    }
    else if((mysetType.B==0)&(mysetType.N==1)&(mysetType.S==0)&(mysetType.W==0)){
        type=2;
    }
    else if((mysetType.B==0)&(mysetType.N==0)&(mysetType.S==1)&(mysetType.W==0)){
        type=3;
    }
    else if((mysetType.B==0)&(mysetType.N==0)&(mysetType.S==0)&(mysetType.W==1)){
        type=4;
    }
    else if((mysetType.B==1)&(mysetType.N==1)&(mysetType.S==0)&(mysetType.W==0)){
        type=5;
    }
    else if((mysetType.B==1)&(mysetType.N==0)&(mysetType.S==1)&(mysetType.W==0)){
        type=6;
    }
    else if((mysetType.B==1)&(mysetType.N==0)&(mysetType.S==0)&(mysetType.W==1)){
        type=7;
    }
    else if((mysetType.B==2)&(mysetType.N==1)&(mysetType.S==0)&(mysetType.W==0)){
        type=8;
    }
    else if((mysetType.B==2)&(mysetType.N==0)&(mysetType.S==1)&(mysetType.W==0)){
        type=9;
    }
    else if((mysetType.B==2)&(mysetType.N==0)&(mysetType.S==0)&(mysetType.W==1)){
        type=10;
    }
    else if((mysetType.B==1)&(mysetType.N==1)&(mysetType.S==0)&(mysetType.W==1)){
        type=11;
    }
    else if((mysetType.B==1)&(mysetType.N==0)&(mysetType.S==1)&(mysetType.W==1)){
        type=12;
    }
    else if((mysetType.B==1)&(mysetType.N==0)&(mysetType.S==0)&(mysetType.W==2)){
        type=13;
    }
    else if((mysetType.B==1)&(mysetType.N==1)&(mysetType.S==0)&(mysetType.W==2)){
        type=14;
    }
    else if((mysetType.B==1)&(mysetType.N==0)&(mysetType.S==1)&(mysetType.W==2)){
        type=15;
    }
    else if((mysetType.B==2)&(mysetType.N==0)&(mysetType.S==1)&(mysetType.W==1)){
        type=16;
    }
    else if((mysetType.B==2)&(mysetType.N==1)&(mysetType.S==0)&(mysetType.W==1)){
        type=17;
    }
    else if((mysetType.B==1)&(mysetType.N==2)&(mysetType.S==0)&(mysetType.W==1)){
        type=18;
    }
    else if((mysetType.B==1)&(mysetType.N==3)&(mysetType.S==0)&(mysetType.W==0)){
        type=19;
    }
    else if((mysetType.B==1)&(mysetType.N==0)&(mysetType.S==0)&(mysetType.W==3)){
        type=20;
    }
    else{
        type=-1;
    }
    
    //__lcd_clear();
    //__lcd_home();
    //printf("type%d",type);
    //__delay_ms(300);
    return type;
}

int valid_set_num(int t, int n){
    int valid;
    if(t==1){
        if(n==1){
            valid=1;
        }
        else if(n==2){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==2){
        if(n==1){
            valid=1;
        }
        else if (n==2){
            valid=1;
        }
        else if (n==3){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==3){
        if(n==1){
            valid=1;
        }
        else if (n==2){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==4){
        if(n==1){
            valid=1;
        }
        else if (n==2){
            valid=1;
        }
        else if (n==3){
            valid=1;
        }
        else if (n==4){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==5){
        //__lcd_clear();
        //__lcd_home();
        //printf("t==5");
        //__delay_ms(1000);
        if(n==1){
            valid=1;
        }
        else if (n==2){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==6){
        if(n==1){
            valid=1;
        }
        else if (n==2){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==7){
        if(n==1){
            valid=1;
        }
        else if (n==2){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==8){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==9){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==10){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==11){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==12){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==13){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==14){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }else if(t==15){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==16){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else if(t==17){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }else if(t==18){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }else if(t==19){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }else if(t==20){
        if(n==1){
            valid=1;
        }
        else {valid=0;}
    }
    else{
        valid=0;
    }
    return valid;
}

int convert_1(int x){
    int result;
    if(x==0){
        result = 1;
    }
    else if(x==1){
        result = 2;
    }
    else if(x==2){
        result = 3;
    }
    else if(x==3){
        result = -1;
    }
    else if(x==4){
        result = 4;
    }
    else if(x==5){
        result = 5;
    }
    else if(x==6){
        result = 6;
    }
    else if(x==7){
        result = -1;
    }
    else if(x==8){
        result = 7;
    }
    else if(x==9){
        result = 8;
    }
    else if(x==10){
        result = 9;
    }
    else if(x==11){
        result = -1;
    }
    else if(x==12){
        result = -1;
    }
    else if(x==13){
        result = 0;
    }
    else if(x==14){
        result = -1;
    }
    else if(x==15){
        result = -1;
    }
    return result;
} // x:1~15

int convert_2(int x, int y){   //x,y: 1~15  //x: left, y: right
    int result;
    int new_x;
    int new_y;
    new_x=convert_1(x);
    new_y=convert_1(y);
    result = (new_x*10)+new_y;
    return result;
}

void input_set_type(struct input * MyInput,int x){
    
    int check_set_type;
    unsigned char temp; 
    int done;
    int read;
    __lcd_clear();
    __lcd_home();
    done=0;
    check_set_type=-1;
    if(x==1){
        printf("C1: Enter Set       ");
        __lcd_newline();
        printf("Press # to Enter            ");
    }
    if(x==2){
        printf("C2: Enter Set       ");
        __lcd_newline();
        printf("Press # to Enter             ");
    }
    if(x==3){
        printf("C3: Enter Set       ");
        __lcd_newline();
        printf("Press # to Enter             ");
    }
    if(x==4){
        printf("C4: Enter Set       ");
        __lcd_newline();
        printf("Press # to Enter             ");
    }
    if(x==5){
        printf("C5: Enter Set       ");
        __lcd_newline();
        printf("Press # to Enter             ");
    }
    if(x==6){
        printf("C6: Enter Set       ");
        __lcd_newline();
        printf("Press # to Enter               ");
    }
    if(x==7){
        printf("C7: Enter Set       ");
        __lcd_newline();
        printf("Press # to Enter             ");
    }
    if(x==8){
        printf("C8: Enter Set      ");
        __lcd_newline();
        printf("Press # to Enter             ");
    }
    
    
    while(check_set_type<0){
       
        while(done==0){
            while(PORTBbits.RB1==0){
                continue;
            }
            read=(PORTB & 0xF0) >> 4;
            while(PORTBbits.RB1==1){
                continue;
            }
            Nop();
            if(read==3){
                if(x==1){
                    MyInput->C1.type.B+=1;
                }
                if(x==2){
                    MyInput->C2.type.B+=1;
                }
                if(x==3){
                    MyInput->C3.type.B+=1;
                }
                if(x==4){
                    MyInput->C4.type.B+=1;
                }
                if(x==5){
                    MyInput->C5.type.B+=1;
                }
                if(x==6){
                    MyInput->C6.type.B+=1;
                }
                if(x==7){
                    MyInput->C7.type.B+=1;
                }
                if(x==8){
                    MyInput->C8.type.B+=1;
                }
                
            }
            if(read==7){
                if(x==1){
                    MyInput->C1.type.N+=1;
                }
                if(x==2){
                    MyInput->C2.type.N+=1;
                }
                if(x==3){
                    MyInput->C3.type.N+=1;
                }
                if(x==4){
                    MyInput->C4.type.N+=1;
                }
                if(x==5){
                    MyInput->C5.type.N+=1;
                }
                if(x==6){
                    MyInput->C6.type.N+=1;
                }
                if(x==7){
                    MyInput->C7.type.N+=1;
                }
                if(x==8){
                    MyInput->C8.type.N+=1;
                }
            }
            if(read==11){
                if(x==1){
                    MyInput->C1.type.S+=1;
                }
                if(x==2){
                    MyInput->C2.type.S+=1;
                }
                if(x==3){
                    MyInput->C3.type.S+=1;
                }
                if(x==4){
                    MyInput->C4.type.S+=1;
                }
                if(x==5){
                    MyInput->C5.type.S+=1;
                }
                if(x==6){
                    MyInput->C6.type.S+=1;
                }
                if(x==7){
                    MyInput->C7.type.S+=1;
                }
                if(x==8){
                    MyInput->C8.type.S+=1;
                }
            }
            if(read==15){
                if(x==1){
                    MyInput->C1.type.W+=1;
                }
                if(x==2){
                    MyInput->C2.type.W+=1;
                }
                if(x==3){
                    MyInput->C3.type.W+=1;
                }
                if(x==4){
                    MyInput->C4.type.W+=1;
                }
                if(x==5){
                    MyInput->C5.type.W+=1;
                }
                if(x==6){
                    MyInput->C6.type.W+=1;
                }
                if(x==7){
                    MyInput->C7.type.W+=1;
                }
                if(x==8){
                    MyInput->C8.type.W+=1;
                }
            }
            if(read==14){
                done=1;
            }
        }   
    ////DEBUG 
    //        __lcd_clear();
    //        __lcd_home();
    //        printf("1: %d %d %d %d",MyInput->C3.type.B,MyInput->C3.type.N,MyInput->C3.type.S,MyInput->C3.type.W);
    //        __delay_ms(3000);
   
        if(x==1){
            check_set_type=valid_set_type(MyInput->C1.type);
            
        }
        if(x==2){
            check_set_type=valid_set_type(MyInput->C2.type);
        }
        if(x==3){
            check_set_type=valid_set_type(MyInput->C3.type);
        }
        if(x==4){
            check_set_type=valid_set_type(MyInput->C4.type);
        }
        if(x==5){
            check_set_type=valid_set_type(MyInput->C5.type);
        }
        if(x==6){
            check_set_type=valid_set_type(MyInput->C6.type);
        }
        if(x==7){
            check_set_type=valid_set_type(MyInput->C7.type);
        }
        if(x==8){
            check_set_type=valid_set_type(MyInput->C8.type);
        }
        
        ////DEBUG
        //    __lcd_clear();
        //    __lcd_home();
        //    printf("2: %d %d %d %d",MyInput->C3.type.B,MyInput->C3.type.N,MyInput->C3.type.S,MyInput->C3.type.W);
        //    __delay_ms(3000);
        
        if(check_set_type<0){
            ///////clear things that entered
            if(x==1){
                MyInput->C1.type.B=0;
                MyInput->C1.type.N=0;
                MyInput->C1.type.S=0;
                MyInput->C1.type.W=0;
            }
            if(x==2){
                MyInput->C2.type.B=0;
                MyInput->C2.type.N=0;
                MyInput->C2.type.S=0;
                MyInput->C2.type.W=0;
            }
            if(x==3){
                MyInput->C3.type.B=0;
                MyInput->C3.type.N=0;
                MyInput->C3.type.S=0;
                MyInput->C3.type.W=0;
            }
            if(x==4){
                MyInput->C4.type.B=0;
                MyInput->C4.type.N=0;
                MyInput->C4.type.S=0;
                MyInput->C4.type.W=0;
            }
            if(x==5){
                MyInput->C5.type.B=0;
                MyInput->C5.type.N=0;
                MyInput->C5.type.S=0;
                MyInput->C5.type.W=0;
            }
            if(x==6){
                MyInput->C6.type.B=0;
                MyInput->C6.type.N=0;
                MyInput->C6.type.S=0;
                MyInput->C6.type.W=0;
            }
            if(x==7){
                MyInput->C7.type.B=0;
                MyInput->C7.type.N=0;
                MyInput->C7.type.S=0;
                MyInput->C7.type.W=0;
            }
            if(x==8){
                MyInput->C8.type.B=0;
                MyInput->C8.type.N=0;
                MyInput->C8.type.S=0;
                MyInput->C8.type.W=0;
            }
            done=0;
            ////DEBUG
            //__lcd_clear();
            //__lcd_home();
            //printf("3: %d %d %d %d",MyInput->C3.type.B,MyInput->C3.type.N,MyInput->C3.type.S,MyInput->C3.type.W);
            //__delay_ms(3000);
            
            __lcd_clear();
            __lcd_home();
            printf("Input Invalid");
            __lcd_newline();
            printf("Please Try Again         ");
            
        }
    }
    
}   

void input_step_num(struct input* MyInput){
    //declare
    int read_step_num;  //0~15
    int check_step;  //0,1
    int step_num;
    unsigned char temp;   //used to putch
    
    __lcd_clear();
    __lcd_home();
    printf("Select Steps");
    __lcd_newline();
    printf("4 5 6 7 8         ");
    while (PORTBbits.RB1==0){
        continue;
    }
    //key is pressed , read what is pressed
    read_step_num = (PORTB & 0xF0) >> 4;
    while (PORTBbits.RB1==1){
        continue;
    }
    //clear screen
    __lcd_clear();
    __lcd_home();
    //check valid
    check_step=valid_step(read_step_num);
    //while invalid: keep waiting 
    while(check_step==0){
        __lcd_clear();
        __lcd_home();
        printf("Input Invalid");
        __lcd_newline();
        printf("Please Try Again         ");
        while(PORTBbits.RB1==0){
                continue;
        }
        read_step_num = (PORTB & 0xF0) >> 4;
        while(PORTBbits.RB1==1){
                continue;
        }
        Nop();
        //__lcd_clear();
        //__lcd_home();
        //temp = keys[read_step_num];
        //__delay_ms(300);
        check_step=valid_step(read_step_num);
    }
    //input valid: putch key pressed
    Nop();  // Apply breakpoint here to prevent compiler optimizations
    __lcd_clear();
    __lcd_home();
    //temp = keys[read_step_num];
    //putch(temp);   // Push the character to be displayed on the LCD
    //__delay_ms(300);
    
    MyInput->stepNum = convert_1(read_step_num);
}

void input_set_num(struct input* MyInput, struct setType MyType,int x){
    int read_set_num;  //0~15
    int check_set_num;  //0,1
    int set_num;
    unsigned char temp;   //used to putch
    __lcd_clear();
    __lcd_home();
    printf("Enter Set Number     ");
    //__lcd_newline();
    //printf("1 2 3 4            ");
    while (PORTBbits.RB1==0){
        continue;
    }
    //key is pressed , read what is pressed
    read_set_num = (PORTB & 0xF0) >> 4;
    while (PORTBbits.RB1==1){
        continue;
    }
    Nop();
    //clear screen
    //__lcd_clear();
    //__lcd_home();
    //check valid
    check_set_num=valid_set_num(valid_set_type(MyType),convert_1(read_set_num));
    while(check_set_num==0){
        ////old
        __lcd_clear();
        __lcd_home();
        printf("Input Invalid");
        __lcd_newline();
        printf("Please Try Again         ");
        while(PORTBbits.RB1==0){
                continue;
        }
        read_set_num = (PORTB & 0xF0) >> 4;
        while(PORTBbits.RB1==1){
                continue;
        }
        Nop();
        
        check_set_num=valid_set_num(valid_set_type(MyType),convert_1(read_set_num));
    }
    Nop();  // Apply breakpoint here to prevent compiler optimizations
    
    if(x==1){
        MyInput->C1.setNum = convert_1(read_set_num);
    }
    if(x==2){
        MyInput->C2.setNum = convert_1(read_set_num);
    }
    if(x==3){
        MyInput->C3.setNum = convert_1(read_set_num);
    }
    if(x==4){
        MyInput->C4.setNum = convert_1(read_set_num);
    }
    if(x==5){
        MyInput->C5.setNum = convert_1(read_set_num);
    }
    if(x==6){
        MyInput->C6.setNum = convert_1(read_set_num);
    }
    if(x==7){
        MyInput->C7.setNum = convert_1(read_set_num);
    }
    if(x==8){
        MyInput->C8.setNum = convert_1(read_set_num);
    }
    
    
    
}

void end_message(struct input * MyInput,int t,int * B, int * N, int * S,int * W){
    //PRINT TIME
    __lcd_clear();
    __lcd_home();
    printf("Operation Time       ");
    __lcd_newline();
    printf("%d s",t);
    __delay_ms(500);
    //PRINT REMAINING FASTENERS
    __lcd_clear();
    __lcd_home();
    printf("Remaining");
    __delay_ms(500);
    

   
    __lcd_clear();
    __lcd_home();
    printf("B:%d S:%d",*B,*S);
    __lcd_newline();
    printf("W:%d N:%d",*W,*N);
    __delay_ms(500);
    
    //SUMERIZE
    __lcd_clear();
    __lcd_home();
    printf("Summary");
    __delay_ms(500);
    __lcd_clear();
    __lcd_home();
    printf("C1: %dB %dN %dS %dW",((MyInput->C1.setNum)*(MyInput->C1.type.B)),((MyInput->C1.setNum)*(MyInput->C1.type.N)),((MyInput->C1.setNum)*(MyInput->C1.type.S)),((MyInput->C1.setNum)*(MyInput->C1.type.W)));
    __lcd_newline();
    printf("C2: %dB %dN %dS %dW",((MyInput->C2.setNum)*(MyInput->C2.type.B)),((MyInput->C2.setNum)*(MyInput->C2.type.N)),((MyInput->C2.setNum)*(MyInput->C2.type.S)),((MyInput->C2.setNum)*(MyInput->C2.type.W)));
    __delay_ms(500);
    __lcd_clear();
    __lcd_home();
    printf("C3: %dB %dN %dS %dW",((MyInput->C3.setNum)*(MyInput->C3.type.B)),((MyInput->C3.setNum)*(MyInput->C3.type.N)),((MyInput->C3.setNum)*(MyInput->C3.type.S)),((MyInput->C3.setNum)*(MyInput->C3.type.W)));
    __lcd_newline();
    printf("C4: %dB %dN %dS %dW",((MyInput->C4.setNum)*(MyInput->C4.type.B)),((MyInput->C4.setNum)*(MyInput->C4.type.N)),((MyInput->C4.setNum)*(MyInput->C4.type.S)),((MyInput->C4.setNum)*(MyInput->C4.type.W)));
    __delay_ms(500);
    __lcd_clear();
    __lcd_home();
    printf("C5: %dB %dN %dS %dW",((MyInput->C5.setNum)*(MyInput->C5.type.B)),((MyInput->C5.setNum)*(MyInput->C5.type.N)),((MyInput->C5.setNum)*(MyInput->C5.type.S)),((MyInput->C5.setNum)*(MyInput->C5.type.W)));
    __lcd_newline();
    printf("C6: %dB %dN %dS %dW",((MyInput->C6.setNum)*(MyInput->C6.type.B)),((MyInput->C6.setNum)*(MyInput->C6.type.N)),((MyInput->C6.setNum)*(MyInput->C6.type.S)),((MyInput->C6.setNum)*(MyInput->C6.type.W)));
    __delay_ms(500);
    __lcd_clear();
    __lcd_home();
    printf("C7: %dB %dN %dS %dW",((MyInput->C7.setNum)*(MyInput->C7.type.B)),((MyInput->C7.setNum)*(MyInput->C7.type.N)),((MyInput->C7.setNum)*(MyInput->C7.type.S)),((MyInput->C7.setNum)*(MyInput->C7.type.W)));
    __lcd_newline();
    printf("C8: %dB %dN %dS %dW",((MyInput->C8.setNum)*(MyInput->C8.type.B)),((MyInput->C8.setNum)*(MyInput->C8.type.N)),((MyInput->C8.setNum)*(MyInput->C8.type.S)),((MyInput->C8.setNum)*(MyInput->C8.type.W)));
    __delay_ms(500);
    
}

void servoLifting(void){               
    CCPR2L=0b00010111;
}

void servoLowering(void){
    CCPR2L=0b10001111;
}

void load(struct input* MyInput,int x,int h1,int m1,int s1){
    int numB,numN,numS,numW;   //NUMBER OF EACH TYPE REQUIRED FOR EACH COMPARTMENT
    int countB,countN,countS,countW;  //NUMBER OF EACH TYPE THAT HAVE BEEN DISPENSED
    int sensorN,sensorS,sensorW;
    float sensorB;
    countB=0;
    countN=0;
    countS=0;
    countW=0;
    sensorB=0.0;
    sensorN=0;
    sensorS=0;
    sensorW=0;
    //SET A FLAG FOR EACH TYPE
    int N=0;
    int B=0;
    int S=0;
    int W=0;
    int i;
    int n;//NUMBER OF LOOPS, related to rotation degrees
    n=260;
    int H2,M2,S2;
    int OpTime;
    //get number 
    if(x==1){
        numB=(MyInput->C1.type.B)*(MyInput->C1.setNum);
        numN=(MyInput->C1.type.N)*(MyInput->C1.setNum);
        numS=(MyInput->C1.type.S)*(MyInput->C1.setNum);
        numW=(MyInput->C1.type.W)*(MyInput->C1.setNum);
    }
    if(x==2){
        numB=(MyInput->C2.type.B)*(MyInput->C2.setNum);
        numN=(MyInput->C2.type.N)*(MyInput->C2.setNum);
        numS=(MyInput->C2.type.S)*(MyInput->C2.setNum);
        numW=(MyInput->C2.type.W)*(MyInput->C2.setNum);
    }
    if(x==3){
        numB=(MyInput->C3.type.B)*(MyInput->C3.setNum);
        numN=(MyInput->C3.type.N)*(MyInput->C3.setNum);
        numS=(MyInput->C3.type.S)*(MyInput->C3.setNum);
        numW=(MyInput->C3.type.W)*(MyInput->C3.setNum);
    }
    if(x==4){
        numB=(MyInput->C4.type.B)*(MyInput->C4.setNum);
        numN=(MyInput->C4.type.N)*(MyInput->C4.setNum);
        numS=(MyInput->C4.type.S)*(MyInput->C4.setNum);
        numW=(MyInput->C4.type.W)*(MyInput->C4.setNum);
    }
    if(x==5){
        numB=(MyInput->C5.type.B)*(MyInput->C5.setNum);
        numN=(MyInput->C5.type.N)*(MyInput->C5.setNum);
        numS=(MyInput->C5.type.S)*(MyInput->C5.setNum);
        numW=(MyInput->C5.type.W)*(MyInput->C5.setNum);
    }
    if(x==6){
        numB=(MyInput->C6.type.B)*(MyInput->C6.setNum);
        numN=(MyInput->C6.type.N)*(MyInput->C6.setNum);
        numS=(MyInput->C6.type.S)*(MyInput->C6.setNum);
        numW=(MyInput->C6.type.W)*(MyInput->C6.setNum);
    }
    if(x==7){
        numB=(MyInput->C7.type.B)*(MyInput->C7.setNum);
        numN=(MyInput->C7.type.N)*(MyInput->C7.setNum);
        numS=(MyInput->C7.type.S)*(MyInput->C7.setNum);
        numW=(MyInput->C7.type.W)*(MyInput->C7.setNum);
    }
    if(x==8){
        numB=(MyInput->C8.type.B)*(MyInput->C8.setNum);
        numN=(MyInput->C8.type.N)*(MyInput->C8.setNum);
        numS=(MyInput->C8.type.S)*(MyInput->C8.setNum);
        numW=(MyInput->C8.type.W)*(MyInput->C8.setNum);
    }
   
    while((countB<numB)||(countN<numN)||(countS<numS)||(countW<numW)){
        
         //////////////////////////RTC////////////////////////////////////////
        I2C_Master_Init(100000);
        unsigned char time[7]; // Create a byte array to hold time read from RTC
        unsigned char k;
        /* Reset RTC memory pointer. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition

        /* Read current time. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        
        for(k = 0; k < 6; k++){
            time[k] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition
        H2=time[2];
        M2=time[1];
        S2=time[0];
        
        if(H2>=h1){
            OpTime=(H2*3600+M2*60+S2)-(h1*3600+m1*60+s1);
        }
        else{
            OpTime=24*3600-(h1*3600+m1*60+s1)+(H2*3600+M2*60+S2);
        }
        //__lcd_clear();
        //printf("Optime1,%d",OpTime);
        //    __delay_ms(2000);
        if(OpTime>150){
            end=1;
            __lcd_clear();
            printf("return from load");
            __delay_ms(2000);
            return;
        }
        ////////////////////////////RTC///////////////////////////
        
        sensorB=((float)(readADC(6))*5/1023);
        sensorN=PORTDbits.RD1;
        sensorS=PORTCbits.RC7;
        sensorW=PORTDbits.RD0;
        while((sensorB<=3.0)&&(sensorN==0)&&(sensorS==0)&&(sensorW==0)){
            
             //////////////////////////RTC////////////////////////////////////////
        I2C_Master_Init(100000);
        unsigned char time[7]; // Create a byte array to hold time read from RTC
        unsigned char k;
        /* Reset RTC memory pointer. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition

        /* Read current time. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        
        for(k = 0; k < 6; k++){
            time[k] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition
        H2=time[2];
        M2=time[1];
        S2=time[0];
        
        if(H2>=h1){
            OpTime=(H2*3600+M2*60+S2)-(h1*3600+m1*60+s1);
        }
        else{
            OpTime=24*3600-(h1*3600+m1*60+s1)+(H2*3600+M2*60+S2);
        }
        __lcd_clear();
        printf("Optime2,%d",OpTime);
            __delay_ms(2000);
        if(OpTime>30){
            end=1;
            __lcd_clear();
            printf("return from load");
            __delay_ms(2000);
            return;
        }
        ////////////////////////////RTC///////////////////////////
            
            sensorB=((float)(readADC(6))*5/1023);
            sensorN=PORTDbits.RD1;
            sensorS=PORTCbits.RC7;
            sensorW=PORTDbits.RD0;
        }
        
        
       
        
        
        
        
        
        
        
        //
        //LATTER IS DISPENSER
        /////set flag
        if((countB<numB)&&(sensorB>3.0)){
            B=1;
        }
        if((countN<numN)&&(sensorN==1)){
            N=1;
        }
        if((countS<numS)&&(sensorS==1)){
            S=1;
        }
        if((countW<numW)&&(sensorW==1)){
            W=1;
        }
        //////////////
        //1 dispenser
        if((B==1)&(N==0)&(S==0)&(W==0)){
            for(i=0;i<n;i++){
                LATAbits.LATA5=0;
                __delay_us(2000);
                LATAbits.LATA3=1;
                __delay_us(2000);
                LATAbits.LATA5=1;
                __delay_us(2000);
                LATAbits.LATA3=0;
                __delay_us(2000);
            }
            countB+=1;  
        }
        
        if((B==0)&(N==1)&(S==0)&(W==0)){
            for(i=0;i<n;i++){
                LATBbits.LATB7=0;
                __delay_us(2000);
                LATBbits.LATB5=1;
                __delay_us(2000);
                LATBbits.LATB7=1;
                __delay_us(2000);
                LATBbits.LATB5=0;
                __delay_us(2000);
            }
            countN+=1;  
        }
        
        if((B==0)&(N==0)&(S==1)&(W==0)){
            for(i=0;i<n;i++){
                LATCbits.LATC5=0;
                __delay_us(2000);
                LATCbits.LATC1=1;
                __delay_us(2000);
                LATCbits.LATC5=1;
                __delay_us(2000);
                LATCbits.LATC1=0;
                __delay_us(2000);
            }
            countS+=1;  
        }
        
        if((B==0)&(N==0)&(S==0)&(W==1)){
            for(i=0;i<n;i++){
                LATBbits.LATB4=0;
                __delay_us(2000);
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATBbits.LATB6=0;
                __delay_us(2000);
            }
            
            countW+=1;  
        }
        
        //2 DISPENSER
        if((B==1)&(N==1)&(S==0)&(W==0)){
            for(i=0;i<n;i++){
                LATAbits.LATA5=0;
                LATBbits.LATB7=0;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB5=1;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB7=1;
                __delay_us(2000);
                LATAbits.LATA3=0;
                LATBbits.LATB5=0;
                __delay_us(2000);
            }
            countB+=1; 
            countN+=1;
        }
        
        if((B==1)&(N==0)&(S==1)&(W==0)){
            for(i=0;i<n;i++){
                LATAbits.LATA5=0;
                LATCbits.LATC5=0;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATCbits.LATC1=1;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATCbits.LATC5=1;
                __delay_us(2000);
                LATAbits.LATA3=0;
                LATCbits.LATC1=0;
                __delay_us(2000);
            }
            countB+=1;
            countS+=1;  
        }
        
        if((B==1)&(N==0)&(S==0)&(W==1)){
            for(i=0;i<n;i++){
                LATAbits.LATA5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATAbits.LATA3=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
            }
            
            countB+=1;
            countW+=1;
        }
        
        if((B==0)&(N==1)&(S==1)&(W==0)){
            for(i=0;i<n;i++){
                LATBbits.LATB7=0;
                LATCbits.LATC5=0;
                __delay_us(2000);
                LATBbits.LATB5=1;
                LATCbits.LATC1=1;
                __delay_us(2000);
                LATBbits.LATB7=1;
                LATCbits.LATC5=1;
                __delay_us(2000);
                LATBbits.LATB5=0;
                LATCbits.LATC1=0;
                __delay_us(2000);
            }
            countN+=1;
            countS+=1;  
        }
        
        if((B==0)&(N==1)&(S==0)&(W==1)){
            for(i=0;i<n;i++){
                LATBbits.LATB7=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
                LATBbits.LATB5=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATBbits.LATB7=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATBbits.LATB5=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
            }
            
            countN+=1;
            countW+=1;  
        }
        
        if((B==0)&(N==0)&(S==1)&(W==1)){
            for(i=0;i<n;i++){
                LATCbits.LATC5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
                LATCbits.LATC1=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATCbits.LATC5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATCbits.LATC1=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
            }
            
            countS+=1; 
            countW+=1;
        }
        
        if((B==0)&(N==1)&(S==1)&(W==1)){
            for(i=0;i<n;i++){
                LATBbits.LATB7=0;
                LATCbits.LATC5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
                LATBbits.LATB5=1;
                LATCbits.LATC1=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATBbits.LATB7=1;
                LATCbits.LATC5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATBbits.LATB5=0;
                LATCbits.LATC1=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
            }
            
            countN+=1; 
            countS+=1; 
            countW+=1;
        }
        
        if((B==1)&(N==0)&(S==1)&(W==1)){
            for(i=0;i<n;i++){
                LATAbits.LATA5=0;
                LATCbits.LATC5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATCbits.LATC1=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATCbits.LATC5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATAbits.LATA3=0;
                LATCbits.LATC1=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
            }
            
            countB+=1; 
            countS+=1; 
            countW+=1;
        }
        
        if((B==1)&(N==1)&(S==0)&(W==1)){
            for(i=0;i<n;i++){
                LATAbits.LATA5=0;
                LATBbits.LATB7=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB5=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB7=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATAbits.LATA3=0;
                LATBbits.LATB5=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
            }
            
            countB+=1; 
            countN+=1; 
            countW+=1;
        }
        
        if((B==1)&(N==1)&(S==1)&(W==0)){
            for(i=0;i<n;i++){
                LATAbits.LATA5=0;
                LATBbits.LATB7=0;
                LATCbits.LATC5=0;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB5=1;
                LATCbits.LATC1=1;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB7=1;
                LATCbits.LATC5=1;
                __delay_us(2000);
                LATAbits.LATA3=0;
                LATBbits.LATB5=0;
                LATCbits.LATC1=0;
                __delay_us(2000);
            }
            countB+=1; 
            countN+=1; 
            countS+=1; 
        }
        
        if((B==1)&(N==1)&(S==1)&(W==1)){
            for(i=0;i<n;i++){
                LATAbits.LATA5=0;
                LATBbits.LATB7=0;
                LATCbits.LATC5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB5=1;
                LATCbits.LATC1=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB7=1;
                LATCbits.LATC5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATAbits.LATA3=0;
                LATBbits.LATB5=0;
                LATCbits.LATC1=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
            }
            
            countB+=1; 
            countN+=1; 
            countS+=1; 
            countW+=1;
            
        }
        B=0;
        N=0;
        S=0;
        W=0;
        __delay_ms(500);
    }
    LATAbits.LATA5=0;
    LATBbits.LATB7=0;
    LATCbits.LATC5=0;
    LATBbits.LATB4=0;
    LATAbits.LATA3=0;
    LATBbits.LATB5=0;
    LATCbits.LATC1=0;
    LATBbits.LATB6=0;
}
    
void reservoir(int* rB,int* rN,int* rS,int* rW,int h1,int m1,int s1){
    int sensorN,sensorS,sensorW;
    float sensorB;
    int N=0;
    int B=0;
    int S=0;
    int W=0;
    int i;
    int j;
    int n;//rotation degrees
    int R;  //depends on actual situation
    n=260;
    R=10;
    sensorB=0.0;
    sensorN=0;
    sensorS=0;
    sensorW=0;
    int H2,M2,S2,OpTime;
   
    for(j=0;j<R;j++){
        sensorB=((float)(readADC(6))*5/1023);
        sensorN=PORTDbits.RD1;
        sensorS=PORTCbits.RC7;
        sensorW=PORTDbits.RD0;
        while((sensorB<=3.0)&&(sensorN==0)&&(sensorS==0)&&(sensorW==0)){
            sensorB=((float)(readADC(6))*5/1023);
            sensorN=PORTDbits.RD1;
            sensorS=PORTCbits.RC7;
            sensorW=PORTDbits.RD0;
            
            //////////////////////////RTC////////////////////////////////////////
        I2C_Master_Init(100000);
        unsigned char time[7]; // Create a byte array to hold time read from RTC
        unsigned char k;
        /* Reset RTC memory pointer. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010000); // 7 bit RTC address + Write
        I2C_Master_Write(0x00); // Set memory pointer to seconds
        I2C_Master_Stop(); // Stop condition

        /* Read current time. */
        I2C_Master_Start(); // Start condition
        I2C_Master_Write(0b11010001); // 7 bit RTC address + Read
        
        for(k = 0; k < 6; k++){
            time[k] = I2C_Master_Read(ACK); // Read with ACK to continue reading
        }
        
        time[6] = I2C_Master_Read(NACK); // Final Read with NACK
        I2C_Master_Stop(); // Stop condition
        H2=time[2];
        M2=time[1];
        S2=time[0];
        
        if(H2>=h1){
            OpTime=(H2*3600+M2*60+S2)-(h1*3600+m1*60+s1);
        }
        else{
            OpTime=24*3600-(h1*3600+m1*60+s1)+(H2*3600+M2*60+S2);
        }
        __lcd_clear();
        printf("Optime,%d",OpTime);
            __delay_ms(2000);
        if(OpTime>165){
            //end=1;
            __lcd_clear();
            printf("end reservoir");
            __delay_ms(2000);
            return;
        }
        ////////////////////////////RTC/////////////////////////// 
            
        }
        //B:RA2,RA3  N:RA4,RA5  S:RB0,RB1  W:RB2,RB3
        /////set flag
        if(sensorB>3.0){
            B=1;
        }
        if(sensorN==1){
            N=1;
        }
        if(sensorS==1){
            S=1;
        }
        if(sensorW==1){
            W=1;
        }
        //////////////
        //1 dispenser
        if((B==1)&(N==0)&(S==0)&(W==0)){
            for(i=0;i<n;i++){
                LATAbits.LATA3=0;
                __delay_us(2000);
                LATAbits.LATA5=1;
                __delay_us(2000);
                LATAbits.LATA3=1;
                __delay_us(2000);
                LATAbits.LATA5=0;
                __delay_us(2000);
            }
            *rB+=1;  
        }
        
        if((B==0)&(N==1)&(S==0)&(W==0)){
            for(i=0;i<n;i++){
                LATBbits.LATB5=0;
                __delay_us(2000);
                LATBbits.LATB7=1;
                __delay_us(2000);
                LATBbits.LATB5=1;
                __delay_us(2000);
                LATBbits.LATB7=0;
                __delay_us(2000);
            }
            *rN+=1;  
        }
        
        if((B==0)&(N==0)&(S==1)&(W==0)){
            for(i=0;i<n;i++){
                LATCbits.LATC1=0;
                __delay_us(2000);
                LATCbits.LATC5=1;
                __delay_us(2000);
                LATCbits.LATC1=1;
                __delay_us(2000);
                LATCbits.LATC5=0;
                __delay_us(2000);
            }
            *rS+=1;  
        }
        
        if((B==0)&(N==0)&(S==0)&(W==1)){
            for(i=0;i<n;i++){
                LATBbits.LATB6=0;
                __delay_us(2000);
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATBbits.LATB4=0;
                __delay_us(2000);
            }
            
            *rW+=1;  
        }
        
        //2 DISPENSER
        if((B==1)&(N==1)&(S==0)&(W==0)){
            for(i=0;i<n;i++){
                LATAbits.LATA3=0;
                LATBbits.LATB5=0;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB7=1;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB5=1;
                __delay_us(2000);
                LATAbits.LATA5=0;
                LATBbits.LATB7=0;
                __delay_us(2000);
            }
            *rB+=1; 
            *rN+=1;
        }
        
        if((B==1)&(N==0)&(S==1)&(W==0)){
            for(i=0;i<n;i++){
                LATAbits.LATA3=0;
                LATCbits.LATC1=0;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATCbits.LATC5=1;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATCbits.LATC1=1;
                __delay_us(2000);
                LATAbits.LATA5=0;
                LATCbits.LATC5=0;
                __delay_us(2000);
            }
            *rB+=1;
            *rS+=1;  
        }
        
        if((B==1)&(N==0)&(S==0)&(W==1)){
            for(i=0;i<n;i++){
                LATAbits.LATA3=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATAbits.LATA5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
            }
            
            *rB+=1;
            *rW+=1;
        }
        
        if((B==0)&(N==1)&(S==1)&(W==0)){
            for(i=0;i<n;i++){
                LATBbits.LATB5=0;
                LATCbits.LATC1=0;
                __delay_us(2000);
                LATBbits.LATB7=1;
                LATCbits.LATC5=1;
                __delay_us(2000);
                LATBbits.LATB5=1;
                LATCbits.LATC1=1;
                __delay_us(2000);
                LATBbits.LATB7=0;
                LATCbits.LATC5=0;
                __delay_us(2000);
            }
            *rN+=1;
            *rS+=1;  
        }
        
        if((B==0)&(N==1)&(S==0)&(W==1)){
            for(i=0;i<n;i++){
                LATBbits.LATB5=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
                LATBbits.LATB7=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATBbits.LATB5=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATBbits.LATB7=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
            }
            
            *rN+=1;
            *rW+=1;  
        }
        
        if((B==0)&(N==0)&(S==1)&(W==1)){
            for(i=0;i<n;i++){
                LATCbits.LATC1=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
                LATCbits.LATC5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATCbits.LATC1=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATCbits.LATC5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
            }
            
            *rS+=1; 
            *rW+=1;
        }
        
        if((B==0)&(N==1)&(S==1)&(W==1)){
            for(i=0;i<n;i++){
                LATBbits.LATB5=0;
                LATCbits.LATC1=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
                LATBbits.LATB7=1;
                LATCbits.LATC5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATBbits.LATB5=1;
                LATCbits.LATC1=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATBbits.LATB7=0;
                LATCbits.LATC5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
            }
            
            *rN+=1; 
            *rS+=1; 
            *rW+=1;
        }
        
        if((B==1)&(N==0)&(S==1)&(W==1)){
            for(i=0;i<n;i++){
                LATAbits.LATA3=0;
                LATCbits.LATC1=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATCbits.LATC5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATCbits.LATC1=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATAbits.LATA5=0;
                LATCbits.LATC5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
            }
            
            *rB+=1; 
            *rS+=1; 
            *rW+=1;
        }
        
        if((B==1)&(N==1)&(S==0)&(W==1)){
            for(i=0;i<n;i++){
                LATAbits.LATA3=0;
                LATBbits.LATB5=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB7=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB5=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATAbits.LATA5=0;
                LATBbits.LATB7=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
            }
            
            *rB+=1; 
            *rN+=1; 
            *rW+=1;
        }
        
        if((B==1)&(N==1)&(S==1)&(W==0)){
            for(i=0;i<n;i++){
                LATAbits.LATA3=0;
                LATBbits.LATB5=0;
                LATCbits.LATC1=0;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB7=1;
                LATCbits.LATC5=1;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB5=1;
                LATCbits.LATC1=1;
                __delay_us(2000);
                LATAbits.LATA5=0;
                LATBbits.LATB7=0;
                LATCbits.LATC5=0;
                __delay_us(2000);
            }
            *rB+=1; 
            *rN+=1; 
            *rS+=1; 
        }
        
        if((B==1)&(N==1)&(S==1)&(W==1)){
            for(i=0;i<n;i++){
                LATAbits.LATA3=0;
                LATBbits.LATB5=0;
                LATCbits.LATC1=0;
                LATBbits.LATB6=0;
                __delay_us(2000);
                LATAbits.LATA5=1;
                LATBbits.LATB7=1;
                LATCbits.LATC5=1;
                LATBbits.LATB4=1;
                __delay_us(2000);
                LATAbits.LATA3=1;
                LATBbits.LATB5=1;
                LATCbits.LATC1=1;
                LATBbits.LATB6=1;
                __delay_us(2000);
                LATAbits.LATA5=0;
                LATBbits.LATB7=0;
                LATCbits.LATC5=0;
                LATBbits.LATB4=0;
                __delay_us(2000);
            }
            
            *rB+=1; 
            *rN+=1; 
            *rS+=1; 
            *rW+=1;
        }
        B=0;
        N=0;
        S=0;
        W=0;
        //__delay_ms(500);
    }
    LATAbits.LATA5=0;
    LATBbits.LATB7=0;
    LATCbits.LATC5=0;
    LATBbits.LATB4=0;
    LATAbits.LATA3=0;
    LATBbits.LATB5=0;
    LATCbits.LATC1=0;
    LATBbits.LATB6=0;
}

void pack(struct input* MyInput,int h1,int m1,int s1){
    
    
    int y;
    y=MyInput->stepNum;
    
    if(y==4){
        //Open C7
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        //get C7 to middle position
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,7,h1,m1,s1);//7:compartment number
        if(end==1){
            return;
        }
        //
        clamp();
        __delay_ms(2000);
        boxRotate2(90.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(270.0,1);
        load(MyInput,5,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();//belts
        
        clamp();
        __delay_ms(2000);
        boxRotate2(90.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(270.0,1);
        motor_normal();//belts
        load(MyInput,3,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(90.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(270.0,1);
        load(MyInput,1,h1,m1,s1);
        if(end==1){
            return;
        }
    }
    if(y==5){
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,7,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(90.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(270.0,1);
        load(MyInput,5,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        load(MyInput,4,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(90.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(270.0,1);
        load(MyInput,2,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        load(MyInput,1,h1,m1,s1);
        if(end==1){
            return;
        }
    }
    if(y==6){
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,7,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,6,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        load(MyInput,5,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(90.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(270.0,1);
        load(MyInput,3,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        load(MyInput,2,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,1,h1,m1,s1);
        if(end==1){
            return;
        }
    }
    if(y==7){
        //C7 to C1
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,7,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,6,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        load(MyInput,5,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,4,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        load(MyInput,3,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        load(MyInput,2,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        load(MyInput,1,h1,m1,s1);
        if(end==1){
            return;
        }
    }
    if(y==8){
        //C8
        __lcd_clear();
        printf("C8");
        load(MyInput,8,h1,m1,s1);
        if(end==1){
            printf("return from pack");
            __delay_ms(2000);
            return;
        }
        //C7 to C1
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        __lcd_clear();
        printf("C7");
        load(MyInput,7,h1,m1,s1);
        if(end==1){
            return;
        }
        
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        __lcd_clear();
        printf("C6");
        load(MyInput,6,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        __lcd_clear();
        printf("C5");
        load(MyInput,5,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        __lcd_clear();
        printf("C4");
        load(MyInput,4,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        __lcd_clear();
        printf("C3");
        load(MyInput,3,h1,m1,s1);
        if(end==1){
            return;
        }
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        __lcd_clear();
        printf("C2");
        load(MyInput,2,h1,m1,s1);
        if(end==1){
            return;
        }
        
        motor_reverse();
        
        clamp();
        __delay_ms(2000);
        boxRotate2(45.0,1);
        unclamp();
        __delay_ms(2000);
        boxRotate2(315.0,1);
        motor_normal();
        __lcd_clear();
        printf("C1");
        load(MyInput,1,h1,m1,s1);
        if(end==1){
            return;
        }
        
    }
      
}

void boxRotate2(float degree, int dir){    //dir=1 counterclockwise  dir=2 clockwise
    LATAbits.LATA4=1;
    int tot=128;
    float tooth,toothNew;
    tooth=gearTeeth((float)(readADC(5))*5/1023);
    toothNew=gearTeeth((float)(readADC(5))*5/1023);
    int target;
    int cur=0;
    
    
    
    
    if (dir==1){
        //set angle
        if(degree==45.0){
            target=(int)((tot*45.0/360.0));//tested
        }
        if(degree==315.0){
            target=(int)((tot*315.0/360.0)-3);//tested
        }
        if(degree==90.0){
            target=(int)((tot*90.0/360.0)-1);//tested
        }
        if(degree==270.0){
            target=(int)((tot*270.0/360.0)-1);//tested
        }
        if(degree==360.0){
            target=(int)((tot*360.0/360.0)-10);//tested //might be -2
        }
        if(degree==67.5){
            target=(int)((tot*67.5/360.0)+1);
            printf("%d",target);
        }
        if(degree==22.5){
            target=(int)((tot*22.5/360.0)-1);
        }
        
        while(cur<target){
            tooth=toothNew;
            LATAbits.LATA2=1;
            LATAbits.LATA0=0;
            //__delay_us(500);
            __delay_ms(10);
            toothNew=gearTeeth((float)(readADC(5))*5/1023);
            if(toothNew!=tooth){
                cur+=1;
                /////
                __lcd_clear();
                printf("current edge:%d",cur);
            }
        }
        //LATBbits.LATB4=0;
        //LATBbits.LATB7=0;
    }
    if (dir==2){
        
        if(degree==45.0){
            target=(int)((tot*45.0/360.0)-1);//tested
        }
        if(degree==112.5){
            target=(int)((tot*112.5/360.0)-1);
        }
        if(degree==315.0){
            target=(int)((tot*315.0/360.0)-1);//tested
        }
        if(degree==90.0){
            target=(int)((tot*90.0/360.0)-1);//tested
        }
        if(degree==270.0){
            target=(int)((tot*270.0/360.0)-1);//tested
        }
        if(degree==360.0){
            target=(int)((tot*360.0/360.0)-3);//tested
        }
        if(degree==67.5){
            target=(int)((tot*67.5/360.0)-1);//tested
        }
        if(degree==22.5){
            target=(int)((tot*22.5/360.0)-1);
        }
        
        while(cur<target){
            
            tooth=toothNew;
            
            LATAbits.LATA2=0;
            LATAbits.LATA0=1;
            //__delay_us(500);
            __delay_ms(10);
            toothNew=gearTeeth((float)(readADC(5))*5/1023);
            if(toothNew!=tooth){
                cur+=1;
                /////
                __lcd_clear();
                printf("current edge:%d",cur);
            }
        }
    }
    LATAbits.LATA0=0;
    LATAbits.LATA2=0;
    LATAbits.LATA4=0;
}

void clamp(void){
    CCPR1L=25;
}       

void unclamp(void){
    CCPR1L=0B01111101;
}      

void startKey(void){
    __lcd_clear();
        __lcd_home();
        int start;
        printf("Press * to Start");
        while(PORTBbits.RB1==0){
            continue;
        }
        
        start = (PORTB & 0xF0) >> 4;
        while (PORTBbits.RB1==1){
            continue;
        }
        while(start!=12){
            while(PORTBbits.RB1==0){
                continue;
            }
            start = (PORTB & 0xF0) >> 4;
            while(PORTBbits.RB1==1){
                continue;
        }
            Nop();
        }
        Nop();
}

void closeBox(void){
    clamp();
    boxRotate2(360.0,2);
    boxRotate2(45.0,2);
}

unsigned short readADC(char channel){
    /* Reads the analog input from the specified analog channel.
     *
     * Arguments: channel, the byte corresponding to the channel to read
     *
     * Returns: the 10-bit value corresponding to the voltage read from
     *          the specified channel
     */
    
    ADCON0 = (channel & 0x0F) << 2; // Select ADC channel (i.e. pin)
    ADON = 1; // Enable module
    ADCON0bits.GO = 1; // Initiate sampling
    while(ADCON0bits.GO_NOT_DONE){  continue;   } // Poll for acquisition completion
    return (ADRESH << 8) | ADRESL; // Return result as a 16-bit value
}

void correctPosition(void){
    
    int tooth,toothNew;
    int count=0;
    int cur=0;
    float vol1;
    LATCbits.LATC0=1;//LED
    LATAbits.LATA4=1;
    
    //__lcd_clear();
    //printf("360 CW");
    //__delay_ms(2000);
    //__lcd_clear();
    
    boxRotate2(360.0,2);
    
    //__lcd_clear();
    //printf("360 CW finished");
    __delay_ms(500);
    
    //Keep rotating clockwise to align with sticker
    vol1=(float)(readADC(1))*5/1023;//Channel 5: RE0
    //__lcd_clear();
    //printf("try to align");
    //__delay_ms(2000);
    tooth=gearTeeth((float)(readADC(5))*5/1023);
    toothNew=gearTeeth((float)(readADC(5))*5/1023);
    LATAbits.LATA4=1;
    LATCbits.LATC0=1;
    while(vol1<1.0){
        //Keep rotating clockwise to align with sticker
        
        tooth=toothNew;
        LATAbits.LATA2=0;
        LATAbits.LATA0=1;
        //__delay_ms(10);
        toothNew=gearTeeth((float)(readADC(5))*5/1023);
        
        //__lcd_clear();
        //printf("%f",(float)(readADC(5))*5/1023);
        //__delay_ms(10);
        if(toothNew!=tooth){
           count+=1;
           //__lcd_clear();
            //printf("%d",count);
            //__delay_ms(10);//100
        }
        vol1=(float)(readADC(1))*5/1023;
        __lcd_clear();
        printf("%d",count);
        //LATAbits.LATA2=0;
        //LATAbits.LATA0=0;
        //__delay_ms(30);
        LATAbits.LATA2=0;
        LATAbits.LATA0=1;
    }
    LATAbits.LATA4=0;
    //rotate counter-clockwise to get the knob to the other side
    LATAbits.LATA2=0;
    LATAbits.LATA0=0;
    
    __lcd_clear();
    //printf("aligned,%f",vol1);
    printf("count,%f",vol1);
    __delay_ms(500);
    
    //__delay_ms(2000);
    boxRotate2(360.0,1);
    //open box sector
    __lcd_clear();
    printf("open box");
    //__delay_ms(500);
    clamp();
    __delay_ms(2000);
    boxRotate2(45.0,1);
    __delay_ms(500);///should be deleted after testing
    
    ////adding 360
    //clamp();
    //__delay_ms(2000);
    //boxRotate2(360.0,1);
    
    //Get C8 to the middle
    //unclamp();
    tooth=gearTeeth((float)(readADC(5))*5/1023);
    toothNew=gearTeeth((float)(readADC(5))*5/1023);
    __lcd_clear();
    printf("rotate back %d",count);
    //__delay_ms(200);
    LATAbits.LATA4=1;
    LATCbits.LATC0=1;
    cur=0;
    //count=(int)(count*1.1);
    while(cur<count){
        //keep rotating counter clockwise for number 'loops'
        tooth=toothNew;
        LATAbits.LATA2=1;
        LATAbits.LATA0=0;
        
        __delay_ms(20);//100
        
        //__delay_us(1000);
        toothNew=gearTeeth((float)(readADC(5))*5/1023);
        if(toothNew!=tooth){
           cur+=1;
           __lcd_clear();
            printf("%d",cur);
            //__delay_ms(10);//100
        }
    }
    LATAbits.LATA4=0;
    
    LATAbits.LATA2=0;
        LATAbits.LATA0=0;
    
    //__delay_ms(2000);///should be deleted after testing
    __lcd_clear();
    printf("67.5");
    __delay_ms(500);
    boxRotate2(67.5,1);// counter clockwise for 67.5 degrees
    
    //try adding 45 degrees
    //boxRotate2(45,1);
    //note that box is not clamped at this point
    LATCbits.LATC0=0;//LED
    LATAbits.LATA4=0;
    
    __lcd_clear();
    printf("finished");
    __delay_ms(500);
    
}  //box is clamped after this

void motor_normal(void){
    
    //LATEbits.LATE2=1;
    
    //LATBbits.LATB0=0;
    //LATBbits.LATB2=1;
    LATBbits.LATB0=1;
    LATBbits.LATB2=0;
    
}

void motor_reverse(void){
    //LATEbits.LATE2=0;
    
    LATBbits.LATB0=0;//wrong way
    LATBbits.LATB2=1;
    
}

void motor_off(void){
    //LATEbits.LATE2=0;
    
    LATBbits.LATB0=0;
    LATBbits.LATB2=0;
    
}

int gearTeeth(float vol){
    float thres;
    thres=1.50;
    if(vol<=thres){
        return 0;
    }
    else{
        return 1;
    }
}

