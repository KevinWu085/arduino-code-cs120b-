#include "Timer.h"

int a = 3;
int b = 2;
int c = 4;
int d = 5;
int e = 6;
int f = 7;
int g = 8;
int dp = 9;
int sw = 10;
int B[2] = {11, 12};
int LEDS[7] {a, b, c, d, e, f, g};
int nums[11][7] {
  {1, 1, 1, 1, 1, 1, 0}, //0
  {0, 1, 1, 0, 0, 0, 0}, //1
  {1, 1, 0, 1, 1, 0, 1}, //2
  {1, 1, 1, 1, 0, 0, 1}, //3
  {0, 1, 1, 0, 0, 1, 1}, //4
  {1, 0, 1, 1, 0, 1, 1}, //5
  {1, 0, 1, 1, 1, 1, 1}, //6
  {1, 1, 1, 0, 0, 0, 0}, //7
  {1, 1, 1, 1, 1, 1, 1}, //8
  {1, 1, 1, 1, 0, 1, 1}, //9
  {0, 0, 0, 0, 0, 0, 0}, //off
};

void outNum(int x) { //outputs number x on the display
  for (int i = 0; i < 7; i++) {
    if (nums[x][i] == 1) {
      digitalWrite(LEDS[i], HIGH);
    }
    else {
      digitalWrite(LEDS[i], LOW);
    }
  }
}

int readStick() { //returns 1 if the joystick was up, 2 if it is down, 0 for anything else
  // you may have to read from A0 instead of A1 depending on how you orient the joystick
  if (analogRead(A1) > 800) {
    return 1; // down
  }
  else if (analogRead(A1) < 200) {
    return 1; // up
  }
  else if (analogRead(A0) < 200) {
    return 1; // up
  }
   else if (analogRead(A0) > 800) {
    return 1; // up
  }
  else {
    return 0;
  }

}

unsigned char clkflag = 1; // count flag
unsigned char cnflag = 0;
unsigned char waitflag = 50;
unsigned int count = 10;  // display count
unsigned long waitcnt = 4; // tick count
int btn;
int stk;
int tick = 10;
int waitTick = 10;
int lightTick = 100;
int sync;

//display SM1
enum CN_States {CN_SMStart, CN_init, CN_OnHold, CN_On, CN_OffHold, CN_control, CN_action} CN_State;

void TickFct_Count() {

   switch ( CN_State ) { //Transitions
      case CN_SMStart:
        CN_State = CN_init;
        break;

      case CN_init:
        if(btn == 0){
          CN_State  = CN_OnHold;
        }
        else if(btn == 1){
          CN_State = CN_init;
        }
        break;

      case CN_OnHold:
        if(btn == 0){
          CN_State  = CN_OnHold;
        }
        else if(btn == 1){
          CN_State = CN_On;
        }
        break;

      case CN_On:
        if(readStick() == 1){
          CN_State = CN_action;
        }
        else if(count <= 0){
          CN_State = CN_init;
        }
        else if(btn == 0){
          CN_State = CN_OffHold;
        }
        else if(count > 0){
          CN_State = CN_On;
        }
        break;
      
      case CN_OffHold:
        if(btn == 1 && cnflag >= 30){
          CN_State = CN_control;
        }
        else if(btn == 1 && cnflag < 30){
          CN_State = CN_init;
        }
        else if(btn == 0){
          cnflag++;
          CN_State = CN_OffHold;
        }
        break;

      case CN_control:
        waitflag--;
        if(waitflag <= 0){
          waitflag = 50;
          CN_State = CN_init;
        }
        else if (waitflag > 0){
          CN_State = CN_control;
        }
        break;

      case CN_action:
        CN_State = CN_On;

        break;
   }

   switch ( CN_State ) { //State actions
      case CN_SMStart:
        break;

      case CN_init:
        if(waitcnt <= 0){
          waitcnt == 4;
        }
        if(count <= 0){
          count = 10;
        }
        outNum(10);
        sync = 1;
        Serial.println("init");
        break;

      case CN_OnHold:
        sync = 2;
        outNum(10);
        Serial.println("Onhold");
        break;

      case CN_On:
        sync = 3;
        tick--;
        if(tick == 0){
          outNum(count-1);
          count--;
        }

        if(tick < 0){
          tick = 10;
        }
        Serial.println("on");
        break;
      
      case CN_OffHold:
        sync = 4;
        outNum(10);
        Serial.println("OffHold");
        break;

      case CN_control:
        sync = 5;
        waitTick--;
        if(waitTick == 0){
          outNum(waitcnt);
          waitcnt--;
        }

        if(waitTick <= 0){
          waitTick = 10;
        }
       
        Serial.println("control");
        break;
      case CN_action:
        count = 10;
        break;
   }
}

//LED SM2
enum CLK_States {CLK_SMStart, CLK_Blink, CLK_onHold, CLK_On, CLK_control} CLK_State;

void TickFct_clock() {

   switch ( CLK_State ) { //Transitions
      case CLK_SMStart:
        CLK_State = CLK_Blink; 
        break;
        
      case CLK_Blink:
        if(sync == 2){
          CLK_State = CLK_onHold;
        }
        else if(sync == 1){
          CLK_State = CLK_Blink;
        }

        break;
      case CLK_onHold:
        if(sync == 3){
          CLK_State = CLK_On;
        }
        else if(sync == 2){
          CLK_State = CLK_onHold;
        }

        break;

      case CLK_On:
        if(sync == 5){
          CLK_State = CLK_control;
        }
        if(sync != 3){
          CLK_State = CLK_Blink;
        }
        else if(sync == 3){
          CLK_State = CLK_On;
        }
        break;

      case CLK_control:
        if(sync != 5){
          CLK_State = CLK_Blink;
        }
        else{
          CLK_State = CLK_control;
        }
        break;
   }

   switch ( CLK_State ) { //State actions
      case CLK_SMStart:
        break;
        
      case CLK_Blink:
        if(lightTick <= 0){
            lightTick = 10;
          }
        if(clkflag == 1){
          clkflag = 0;
          digitalWrite(B[0], HIGH);
        }
        else if(clkflag == 0){
          clkflag++;
          digitalWrite(B[0], LOW);
        }
        digitalWrite(B[1], LOW);
        //Serial.println("blink");
        break;

      case CLK_onHold:
        //Serial.println("OnHold");
        digitalWrite(B[0], LOW);
        digitalWrite(B[1], HIGH);
        break;

      case CLK_On:
        //Serial.println("On");
        digitalWrite(B[0], LOW);
        digitalWrite(B[1], HIGH);
        break;
      
       case CLK_control:
        //Serial.println("control");
        digitalWrite(B[0], LOW);
        digitalWrite(B[1], HIGH);
        break;
      
   }
}

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 2; i++) {
    pinMode(B[i], OUTPUT);
  }
  pinMode(sw, INPUT_PULLUP);
  TimerSet(100);
  TimerOn();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // call tick function here
  btn = digitalRead(10);
  //Serial.println(count);
  //Serial.println(btn);
  TickFct_Count();
  TickFct_clock();
  while (!TimerFlag){}  // Wait for BL's period
  TimerFlag = 0;  
}

