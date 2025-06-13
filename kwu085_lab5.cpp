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
    return 2; // up
  }
  else {
    return 0;
  }

}

unsigned char cflag = 0; // count flag
unsigned int count = 0;  // display count
unsigned long tcnt = 0; // tick count
//unsigned int speed[3] = {4,2,1}; // array of speeds
unsigned int val = 2; // value to determine speed
int stk;
int btn;
int spd;

void tickSpeed(){
  if(stk == 1){
    val++;
  }
  else if(stk == 2){
    val--;
  }

  if(val <= 1){
    val = 1;
  }
  else if(val >= 3){
    val = 3;
  }
}


enum CN_States { CN_SMStart, CN_init, CN_Begin, CN_BeginHold, CN_Pause, CN_PauseHold} CN_State;

void TickFct_Count() {

   switch ( CN_State ) { //Transitions
      case CN_SMStart:
        CN_State = CN_init;

      case CN_init:
        if(btn == 0){
         CN_State = CN_Begin;
        }
        else{
         CN_State = CN_init;
        }
        break;

      case CN_Begin:
        if(btn == 0){
          CN_State = CN_Begin;
        }
        else if(btn == 1)
        {
          CN_State = CN_BeginHold;
        }
        break;

      case CN_BeginHold:
        if(btn == 0){
          CN_State = CN_Pause;
        }
        else if(btn == 1){
          CN_State = CN_BeginHold;
        }
        break;

      case CN_Pause:
        if(btn == 0){
          CN_State = CN_Pause;
        }
        else if(btn == 1)
        {
          CN_State = CN_PauseHold;
        }
        break;

      case CN_PauseHold:
        if(btn == 0){
          CN_State = CN_Begin;
        }
        else if(btn == 1){
          CN_State = CN_PauseHold;
        }
        break;
   }

   switch ( CN_State ) { //State actions
      case CN_SMStart:
        count = 0;
        break;

      case CN_init:
        Serial.println(cflag);
        outNum(count);
        break;

      case CN_Begin:
        Serial.println(cflag);
        cflag++;
        if(val == 3 && cflag >= 1){
          count++;
          cflag = 0;
        }
        else if(val == 2 && cflag >= 2){
          count++;
          cflag = 0;
        }
        else if(val == 1 && cflag >= 4){
          count++;
          cflag = 0;
        }

        if(count >= 10){
          count = 0;
        }
        outNum(count);
        break;
      case CN_BeginHold:
        Serial.println(cflag);
        cflag++;
        if(val == 3 && cflag >= 1){
          count++;
          cflag = 0;
        }
        else if(val == 2 && cflag >= 2){
          count++;
          cflag = 0;
        }
        else if(val == 1 && cflag >= 4){
          count++;
          cflag = 0;
        }
        
        if(count >= 10){
          count = 0;
        }
        outNum(count);
        break;
      case CN_Pause:
        Serial.println(cflag);
        outNum(count);
        break;
      case CN_PauseHold:
        Serial.println(cflag);
        outNum(count);
        break;
   }
}



void setup() {
  // put your setup code here, to run once:
  pinMode(sw, INPUT_PULLUP);
  TimerSet(500);
  TimerOn();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // call tick function here
  btn = digitalRead(10);
  stk = readStick();
  tickSpeed();
  TickFct_Count();

  while (!TimerFlag){}  // Wait for BL's period
  TimerFlag = 0;  
}

