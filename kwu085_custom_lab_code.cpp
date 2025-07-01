#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include <SevSegShift.h>

#define SHIFT_PIN_SHCP 6
#define SHIFT_PIN_STCP 5
#define SHIFT_PIN_DS   4
SevSegShift display7seg(SHIFT_PIN_DS, SHIFT_PIN_SHCP, SHIFT_PIN_STCP, 1, true);

#define eeprom 0x50 
Adafruit_PCD8544 display = Adafruit_PCD8544(8, 7, 4, 3, 2);



typedef struct task {
  int state;
  int period;
  int elapsedTime;
  int (*TickFct)(int);

} task;

int delay_gcd;
const unsigned short tasksNum = 2;
task tasks[tasksNum];

//joystick
int JS_X = A1;
int JS_Y = A2;
int j_x; 
int j_y;
int cntrl;
int holding;

void hold(){
  if(j_y >500 && j_y < 530 && j_x > 500 && j_x < 530){
    holding = 0; //no input
  }
  else{
    holding = 1;
  }
}

//eeprom
int data[4] = {1, 2, 3, 4};
int arr2[4] = {};

//game var
int lvl = 0;
int winLose = 0;

void vals(){
  //joysticks
  j_x = analogRead(JS_X);
  j_y = analogRead(JS_Y);
  
  if(j_y >500 && j_y < 530 && j_x > 500 && j_x < 530){
    cntrl = 0; //no input
  }
  else if(j_y > 900 && j_x >500 && j_x < 600){
    cntrl = 1; //right
  }
  else if(j_y < 300 && j_x >500 && j_x < 600){
    cntrl = 2; //left
  }
  else if(j_x > 900 && j_y >500 && j_y < 600){
    cntrl = 3; //up
  }
  else if(j_x < 300 && j_y >500 && j_y < 600){
    cntrl = 4; //down
  }
  
}

void testPrint(){
  int printlvl = lvl+1;
  Serial.println("state: lvl");
  Serial.println(lvl);
  Serial.print("cntrl: ");
  Serial.println(cntrl);
  //Serial.print("holding: ");
  //Serial.println(holding);
}



void storing(){
  unsigned int address = 0; //first address of the EEPROM
  
  for (address = 0; address < 4; address++) {
    writeEEPROM(eeprom, address, data[address]);
  }

  //store eeprom data into vector
  for (address = 0; address < 4; address++) {
    arr2[address] = readEEPROM(eeprom, address), DEC;
    //Serial.println(arr2[address]);
  }
}

//game state
enum SM1_States { SM1_INIT, lvl0, lvl1, lvl2, lvl3, done};
int SM1_Tick(int state1) {
  vals();
  hold();
  switch (state1) { // State transitions
    case SM1_INIT:
      lvl = 0;
      state1 = lvl0;
      break;
    case lvl0:
      if(cntrl == 0){
        state1 = lvl0;
      }
      else if(cntrl == arr2[0]){
        lvl = 1;
        state1 = lvl1;
      }
      else if((cntrl != arr2[0] || cntrl != 0)){
        winLose = 2; //lose
        state1 = done;
      }
      break;

    case lvl1:
      if(cntrl == 0){
        state1 = lvl1;
      }
      else if(cntrl == arr2[1]){
        lvl = 2;
        state1 = lvl2;
      }
      else if((cntrl != arr2[1] || cntrl != 0)){
        winLose = 2; //lose
        state1 = done;
      }
      break;

    case lvl2:
      if(cntrl == 0){
        state1 = lvl2;
      }
      else if(cntrl == arr2[2]){
        lvl = 3;
        state1 = lvl3;
      }
      else if((cntrl != arr2[2] || cntrl != 0)){
        winLose = 2; //lose
        state1 = done;
      }
      break;

    case lvl3:
      if(cntrl == 0){
        state1 = lvl3;
      }
      else if(cntrl == arr2[3]){
        winLose = 1; //win
        lvl = 4;
        state1 = done;
      }
      else if((cntrl != arr2[3] || cntrl != 0)){
        winLose = 2; //lose
        state1 = done;
      }
      break;

    case done:
      state1 = done;
      break;
  }

  switch (state1) { // State Action
    case SM1_INIT:
      //Serial.print("state: init");
      break;
    case lvl0:
      gamelvl(lvl);
      Serial.println(lvl);

      break;
    case lvl1:
      gamelvl(lvl);
      Serial.println(lvl);
      break;
    case lvl2:
      gamelvl(lvl);
      Serial.println(lvl);
      break;
    case lvl3:
      gamelvl(lvl);
      Serial.println(lvl);
      break;
    case done:
      if(winLose == 1){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(BLACK);
        //Serial.println(winLose);
        display.clearDisplay();
        display.println("YOU WIN  :)");
        display.display();
      }
      if(winLose == 2){
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(BLACK);
        //Serial.println(winLose);
        display.clearDisplay();
        display.println("YOU LOST :(");
        display.display();
      }
      
      break;
  }

  return state1;
}

//7seg display
enum SM2_States { SM2_INIT, sevlvl0, sevlvl1, sevlvl2, sevlvl3, sevlvl4, done2};
int SM2_Tick(int state2) {
  switch (state2) { // State transitions
    case SM2_INIT:
      state2 = sevlvl0;
      break;
    case sevlvl0:
      if(lvl == 0){
        state2 = sevlvl0;
      }
      else if(lvl == 1){
        state2 = sevlvl1;
      }
      break;

    case sevlvl1:
      if(lvl == 1){
        state2 = sevlvl1;
      }
      else if(lvl == 2){
        state2 = sevlvl2;
      }
      break;

    case sevlvl2:
      if(lvl == 2){
        state2 = sevlvl2;
      }
      else if(lvl == 3){
        state2 = sevlvl3;
      }
      break;

    case sevlvl3:
      if(lvl == 3){
        state2 = sevlvl3;
      }
      if(lvl == 4){
        state2 = done2;
      }
      if(winLose == 2){
        state2 = done2;
      }
      break;

    case done2:
      state2 = done2;
      break;
  }

  switch (state2) { // State Action
    case SM1_INIT:
      break;
    case lvl0:
      
      break;
    case lvl1:
      if(lvl == 1){
        Serial.println("state1");
        display7seg.setNumber(8);
        display7seg.refreshDisplay();
      }
      break;
    case lvl2:
      if(lvl == 2){
        Serial.println("state2");
        display7seg.setNumber(88);
        display7seg.refreshDisplay();
      }
      break;
    case lvl3:
      if(lvl == 3){
        Serial.println("state3");
        display7seg.setNumber(888);
        display7seg.refreshDisplay();
      }
      break;
    case done2:
      if(lvl == 4){
        Serial.println("state4");
        display7seg.setNumber(8888);
        display7seg.refreshDisplay();
      }
      break;
  }

  return state2;
}

void setup(void){
  Wire.begin();
  Serial.begin(9600);
  //eeprom
  storing();

  // 4 digit 7 seg display
  byte numDigits = 4;
  byte digitPins[] = {9, 10, 11, 12}; 
  byte segmentPins[] = {0, 1, 2, 3, 4, 5, 6, 7}; 
  bool resistorsOnSegments = false; 
  byte hardwareConfig = COMMON_CATHODE; 
  bool updateWithDelays = false; 
  bool leadingZeros = false; 
  bool disableDecPoint = true;
  
  display7seg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  display7seg.setBrightness(100);
  
  //nokia
  display.begin();
  display.clearDisplay();
  display.setContrast(57);
  display.setTextSize(1);
  display.setTextColor(BLACK);

  unsigned char i = 0;
  tasks[i].state = SM1_INIT;
  tasks[i].period = 1000;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM1_Tick;
  i++;
  tasks[i].state = SM2_INIT;
  tasks[i].period = 1000;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM2_Tick;
  
  delay_gcd = 1000;
}

void loop(){
  unsigned char j;
  for (j = 0; j < tasksNum; ++j) {
    if ( (millis() - tasks[j].elapsedTime) >= tasks[j].period) {
      tasks[j].state = tasks[j].TickFct(tasks[j].state);
      tasks[j].elapsedTime = millis(); 
    }
  }

  //Serial.print("x: ");
  //Serial.println(j_x);
  //Serial.print("y: ");
  //Serial.println(j_y);
}

//defines the writeEEPROM function
void writeEEPROM(int daddress, unsigned int eeaddress, byte data ) {
  Wire.beginTransmission(daddress);
  Wire.write((int)(eeaddress >> 8)); 
  Wire.write((int)(eeaddress & 0xFF)); 
  Wire.write(data);
  Wire.endTransmission();
  delay(5);
}

//defines the readEEPROM function
byte readEEPROM(int daddress, unsigned int eeaddress ) {
  byte redata = 0xFF;
  Wire.beginTransmission(daddress);
  Wire.write((int)(eeaddress >> 8));
  Wire.write((int)(eeaddress & 0xFF)); 
  Wire.endTransmission();
  Wire.requestFrom(daddress,1);
  redata =  Wire.read();
  return redata;

}

// nokia print
void gamelvl(int i){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(BLACK);
  if(arr2[i] == 1){
    display.clearDisplay();
    display.println("LEFT");
    display.display();
  }
  if(arr2[i] == 2){
    display.clearDisplay();
    display.println("RIGHT");
    display.display();
  }
  if(arr2[i] == 3){
    display.clearDisplay();
    display.println("UP");
    display.display();
  }
  if(arr2[i] == 4){
    display.clearDisplay();
    display.println("DOWN");
    display.display();
  }
}
