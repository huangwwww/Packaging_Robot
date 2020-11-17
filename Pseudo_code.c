
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
void end_message(struct input *);
/***** Includes *****/
#include <xc.h>
#include "configBits.h"
#include "lcd.h"


/***** Constants *****/
const char keys[] = "123A456B789C*0#D"; 
//const int turn_page_delay = 1000;

void main(void) {
    
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
    TRISB = 0xFF;
    TRISC = 0x00;
    TRISD = 0x00; // All output mode on port D for the LCD
    TRISE = 0x00;
    
    /************************** A/D Converter Module **************************/
    ADCON0 = 0x00;  // Disable ADC
    ADCON1 = 0b00001111; // Set all A/D ports to digital (pg. 222)
    // </editor-fold>
    
    INT1IE = 1; // Enable RB1 (keypad data available) interrupt
    ei(); // Enable all interrupts
    
    /* Initialize LCD. */
    initLCD();
    
    
    //standby mode user interface
    printf("   RTC   ");
    __lcd_newline(); //RTC on first line
    printf("Press Any Key   ");
    __lcd_home();
    
    
}

void interrupt interruptHandler(void){
    
    struct input theInput;
    //Input: name of struct
    theInput = init_struct();
    struct input* Input=&theInput;
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
        ////////////end_message(Input);
        
        ////////////////////////PSEUDOCODE/////////////////////////////////////////////////
        int remainB=-1;
        int remainN=-1;
        int remainS=-1;
        int remainW=-1;
        
        Ask for user input;
        clamp the box;
        Correct Lid Position;
        Open Box Sector:knob rotae 45 degrees counterclowise
        active two converyor belts;
        if(number of steps==4){
            rotate 45 degrees counter-clockwise;
            load;
            rotate 90 degrees counter-clockwise;
            load;
            rotate 90 degrees counter-clockwise;
            load;
            rotate 90 degrees counter-clockwise;
            load;
        }
        if(number of steps==5){
            rotate 45 degrees counter-clockwise;
            load;
            rotate 90 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 90 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
        }
        if(number of steps==6){
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 90 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
        }
        if(number of steps==7){
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
        }
        if(number of steps==8){
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
            rotate 45 degrees counter-clockwise;
            load;
        }
        go to reservoir;
        close box sector: stepper turns knob by 45 degrees clockwise;
        lift the box;
        show termination message;
        
        ///////////////////////////////////////PSEUDO CODES///////////////////////////////////////////
        
        
        
        
        
        INT1IF = 0;  // Clear interrupt flag bit to signify it's been handled
        
        
    }
}

//////////////////////////////////////////////PSEUDOCODE///////////////////////////////////////////
void CorrectPosition(void){
    
    set stepper speed;
    while(knob is not aligned with sticker on the box){
        stepper turn clockwise continuesly;
        
    }
    stop stepper motor;
}

void open(void){
    stepper motor on;
    rotate the knob by 45 degrees conunter-clockwise;
}

void load(struct input* MyInput,int x, int last,int* B, int* N, int* S, int* W){
    int numB,numN,numS,numW;
    int countB,countN,countS,countW;
    countB=0;
    countN=0;
    countS=0;
    countW=0;
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
    while((countB<numB)|(countN<numN)|(countS<numS)|(countW<numW)){
        while((sensorB==1)|(sensorN==1)|(sensorS==1)(sensorW==1)){
            if(sensorB==1){
                if(countB<numB){
                    dispense B to funnel;
                    countB+=1;  
                }
                if((last==1)&(countB==numB)){
                    dispense to reservoir B;
                    *B+=1;
                }
            }
            if(sensorN==1){
                if(countN<numN){
                    dispense N to funnel;
                    countN+=1;  
                }
                if((last==1)&(countN==numN)){
                    dispense to reservoir N;
                    *N+=1;
                }
            }
            if(sensorS==1){
                if(countS<numS){
                    dispense S to funnel;
                    countS+=1;  
                }
                if((last==1)&(countS==numS)){
                    dispense to reservoir S;
                    *S+=1;
                }
            }
            if(sensorW==1){
                if(countW<numW){
                    dispense W to funnel;
                    countW+=1;  
                }
                if((last==1)&(countW==numW)){
                    dispense to reservoir W;
                    *W+=1;
                }
            }
        }
        
    }
    delay: wait for fasteners to slide down from funnel;
}

void reservoir(int* B, int* N, int* S, int* W){
    for(i=0;i<50;i++){
        while((sensorB==0)&(sensorN==0)&(sensorS==0)&(sensorW==0)){
            continue;
        }
        while((sensorB==1)|(sensorN==1)|(sensorS==1)(sensorW==1)){
            if(sensorB==1){
                dispense to reservoirB;
                *B+=1;
            }
            if(sensorN==1){
                dispense to reservoirN;
                *N+=1;
            }
            if(sensorS==1){
                dispense to reservoirS;
                *S+=1;
            }
            if(sensorW==1){
                dispense to reservoirW;
                *W+=1;
            }
        }
    }
}


///////////////////////////////////////////PSEUDOCODE//////////////////////////////////////////////



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
        printf("Press #             ");
    }
    if(x==2){
        printf("C2: Enter Set       ");
        __lcd_newline();
        printf("Press #            ");
    }
    if(x==3){
        printf("C3: Enter Set       ");
        __lcd_newline();
        printf("Press #            ");
    }
    if(x==4){
        printf("C4: Enter Set       ");
        __lcd_newline();
        printf("Press #            ");
    }
    if(x==5){
        printf("C5: Enter Set       ");
        __lcd_newline();
        printf("Press #            ");
    }
    if(x==6){
        printf("C6: Enter Set       ");
        __lcd_newline();
        printf("Press #              ");
    }
    if(x==7){
        printf("C7: Enter Set       ");
        __lcd_newline();
        printf("Press #            ");
    }
    if(x==8){
        printf("C8: Enter Set      ");
        __lcd_newline();
        printf("Press #            ");
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

void end_message(struct input * MyInput){
    //PRINT TIME
    __lcd_clear();
    __lcd_home();
    printf("Operation Time       ");
    __lcd_newline();
    printf("delta t");
    __delay_ms(1000);
    //PRINT REMAINING FASTENERS
    __lcd_clear();
    __lcd_home();
    printf("Remaining");
    __delay_ms(1000);
    
    __lcd_clear();
    __lcd_home();
    printf("B:%d S:%d",remainB,remianS);
    __lcd_newline();
    printf("W:%d N:%d",remainW,remanN);
    __delay_ms(3000);
    
    //SUMERIZE
    __lcd_clear();
    __lcd_home();
    printf("Summary");
    __delay_ms(1000);
    __lcd_clear();
    __lcd_home();
    printf("C1: %dB %dN %dS %dW",((MyInput->C1.setNum)*(MyInput->C1.type.B)),((MyInput->C1.setNum)*(MyInput->C1.type.N)),((MyInput->C1.setNum)*(MyInput->C1.type.S)),((MyInput->C1.setNum)*(MyInput->C1.type.W)));
    __lcd_newline();
    printf("C2: %dB %dN %dS %dW",((MyInput->C2.setNum)*(MyInput->C2.type.B)),((MyInput->C2.setNum)*(MyInput->C2.type.N)),((MyInput->C2.setNum)*(MyInput->C2.type.S)),((MyInput->C2.setNum)*(MyInput->C2.type.W)));
    __delay_ms(3000);
    __lcd_clear();
    __lcd_home();
    printf("C3: %dB %dN %dS %dW",((MyInput->C3.setNum)*(MyInput->C3.type.B)),((MyInput->C3.setNum)*(MyInput->C3.type.N)),((MyInput->C3.setNum)*(MyInput->C3.type.S)),((MyInput->C3.setNum)*(MyInput->C3.type.W)));
    __lcd_newline();
    printf("C4: %dB %dN %dS %dW",((MyInput->C4.setNum)*(MyInput->C4.type.B)),((MyInput->C4.setNum)*(MyInput->C4.type.N)),((MyInput->C4.setNum)*(MyInput->C4.type.S)),((MyInput->C4.setNum)*(MyInput->C4.type.W)));
    __delay_ms(3000);
    __lcd_clear();
    __lcd_home();
    printf("C5: %dB %dN %dS %dW",((MyInput->C5.setNum)*(MyInput->C5.type.B)),((MyInput->C5.setNum)*(MyInput->C5.type.N)),((MyInput->C5.setNum)*(MyInput->C5.type.S)),((MyInput->C5.setNum)*(MyInput->C5.type.W)));
    __lcd_newline();
    printf("C6: %dB %dN %dS %dW",((MyInput->C6.setNum)*(MyInput->C6.type.B)),((MyInput->C6.setNum)*(MyInput->C6.type.N)),((MyInput->C6.setNum)*(MyInput->C6.type.S)),((MyInput->C6.setNum)*(MyInput->C6.type.W)));
    __delay_ms(3000);
    __lcd_clear();
    __lcd_home();
    printf("C7: %dB %dN %dS %dW",((MyInput->C7.setNum)*(MyInput->C7.type.B)),((MyInput->C7.setNum)*(MyInput->C7.type.N)),((MyInput->C7.setNum)*(MyInput->C7.type.S)),((MyInput->C7.setNum)*(MyInput->C7.type.W)));
    __lcd_newline();
    printf("C8: %dB %dN %dS %dW",((MyInput->C8.setNum)*(MyInput->C8.type.B)),((MyInput->C8.setNum)*(MyInput->C8.type.N)),((MyInput->C8.setNum)*(MyInput->C8.type.S)),((MyInput->C8.setNum)*(MyInput->C8.type.W)));
    __delay_ms(3000);
}
