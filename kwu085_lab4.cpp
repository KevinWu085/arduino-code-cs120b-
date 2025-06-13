//youtube demo link: https://youtu.be/mUm8n_sY6vs
#include "Timer.h"
int b[2] = {3, 4};
int lightVal = 0;
int btn = 5;
int cnt= 0;
int btnCnt = 0;
enum BL_States { BL_SMStart, BL_LedOff, BL_LedOn, BL_LedForceOff, BL_LedForceOn } BL_State;

void TickFct_Blink() {
   switch ( BL_State ) { //Transitions
      case BL_SMStart:
         BL_State = BL_LedOff; //Initial state
         break;
         
      case BL_LedOff:
         if(digitalRead(btn) == 1){
             BL_State = BL_LedForceOn;
          }
         else if(lightVal < 100){
             BL_State = BL_LedOn;
          }
         else{
            BL_State = BL_LedOff;
          }
         break;
         
      case BL_LedOn:
         if(digitalRead(btn)== 1){
             BL_State = BL_LedForceOff;
          }
         else if(lightVal < 100){
             BL_State = BL_LedForceOn;
          }
         else if(cnt >= 40){
            BL_State = BL_LedOff;
          }
         break;
               
      case BL_LedForceOn:
         BL_State = BL_LedOn;
         break;
         
      case BL_LedForceOff:
         if(digitalRead(btn) == 1){
            BL_State = BL_LedForceOn;
         } 
         else if(btnCnt == 20){
           BL_State = BL_LedOff;
         }
         else{
           BL_State = BL_LedForceOff;
            
        }

   }

   switch (BL_State ) { //State actions
      case BL_LedOff:
         cnt = 0;
         btnCnt = 0;
         digitalWrite(b[0], LOW);
         digitalWrite(b[1], HIGH);
         Serial.println(cnt);
         break;
         
      case BL_LedOn:
         cnt = cnt + 1;
         btnCnt = 0;
         digitalWrite(b[0], HIGH);
         digitalWrite(b[1], LOW);
         Serial.println(cnt);
         break;

      case BL_LedForceOn:
         cnt = 0;
         btnCnt = 0;
         Serial.println(cnt);
         break;
         
      case BL_LedForceOff:
         cnt = 0;
         btnCnt = btnCnt +1;
         digitalWrite(b[0], LOW);
         digitalWrite(b[1], HIGH);
         Serial.println(btnCnt);
         break;
        
   }
}


void setup() {
  // put your setup code here, to run once:
  TimerSet(500);
  pinMode(btn, INPUT);
  for (int i = 0; i < 2; i++) {
    pinMode(b[i], OUTPUT);
 }
 Serial.begin(9600);
 TimerOn();
}

void loop() {
  lightVal = analogRead(A0);
  //Serial.println(digitalRead(btn));
  // put your main code here, to run repeatedly:
  TickFct_Blink();
  while (!TimerFlag){ }  // Wait for BL's period
  TimerFlag = 0; 
}
