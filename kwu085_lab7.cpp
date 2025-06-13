#include "Timer.h"
//include "Adafruit_LiquidCrystal.h"
// Depending on the LiquidCrystal library you are able to install, it might be:
#include "LiquidCrystal.h"
#include "pitches.h"

// Sound Variables  
int buzzer = 8;

// == Song 1 ==
int song1[] = {
NOTE_E4, NOTE_C5, NOTE_B1, NOTE_F3, NOTE_C4, 
NOTE_A4, NOTE_A4, NOTE_GS5, NOTE_C5, NOTE_CS4, 
NOTE_AS4, NOTE_C5, NOTE_DS4, NOTE_CS5, NOTE_GS4, 
NOTE_C3, NOTE_E3, NOTE_DS5, NOTE_D4, NOTE_D3
};
int song1_time[] = {
2, 1, 2, 1, 1, 4, 8, 16, 8, 4, 4, 1, 8, 4, 2, 4, 4, 16, 4, 2
};

// == Song 2 ==

int song2[] = {
  NOTE_FS5, NOTE_D2, NOTE_DS5, NOTE_G2, NOTE_B3, 
  NOTE_C2, NOTE_G5, NOTE_D6, NOTE_CS5, NOTE_AS4, 
  NOTE_DS6, NOTE_D3, NOTE_CS4, NOTE_E5, NOTE_DS6,
   NOTE_E4, NOTE_B4, NOTE_F4, NOTE_E6, NOTE_DS4
};

int song2_time[] = {
  2, 2, 4, 8, 1, 8, 4, 4, 16, 8, 2, 4, 16, 8, 2, 4, 16, 4, 8, 1
};

// == Song 3 == 

int song3[] = {
  NOTE_A5, NOTE_D4, NOTE_D6, NOTE_DS3, NOTE_G4, 
  NOTE_B2, NOTE_F2, NOTE_A3, NOTE_AS2, NOTE_B5, 
  NOTE_C6, NOTE_C3, NOTE_GS3, NOTE_G2, NOTE_FS5, 
  NOTE_AS4, NOTE_GS2, NOTE_CS3, NOTE_C3, NOTE_AS2
};

int song3_time[] = {
  1, 2, 16, 4, 16, 2, 16, 1, 1, 2, 1, 8, 2, 16, 8, 1, 16, 4, 1, 2
};


// LCD variables
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Depending on the LiquidCrystal library you are able to install, it might be:
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Task Structure Definition
typedef struct task {
   int state;                  // Tasks current state
   unsigned long period;       // Task period
   unsigned long elapsedTime;  // Time elapsed since last task tick
   int (*TickFct)(int);        // Task tick function
} task;


const unsigned char tasksNum = 4;
task tasks[tasksNum]; // We have 4 tasks

// Task Periods

const unsigned long periodLCDOutput = 100;
const unsigned long periodJoystickInput = 100;
const unsigned long periodSoundOutput = 100;
const unsigned long periodController = 500;


// GCD 
const unsigned long tasksPeriodGCD = 100;

// Task Function Definitions
int TickFct_LCDOutput(int state);
int TickFct_JoystickInput(int state);
int TickFct_SoundOutput(int state);
int TickFct_Controller(int state);

// Task Enumeration Definitions
enum LO_States {LO_init, LO_StartMenu, LO_StartMenuSelect,LO_setScreen, LO_PlaySong, LO_PlaySongSelect};
enum JI_States {JI_init, JI_jystk};
enum SO_States {SO_init, SO_Song1On, SO_Song2On, SO_Song3On, SO_SoundOff, SO_pausing};
enum C_States {C_init, C_T1};



void TimerISR() { // TimerISR actually defined here
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
     if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = 0;
     }
     tasks[i].elapsedTime += tasksPeriodGCD;
  }
}


void LCDWriteLines(String line1, String line2, String line3, String line4) {     
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(8, 0);
  lcd.print(line2);
  lcd.setCursor(0, 1);
  lcd.print(line3);
  lcd.setCursor(8, 1);
  lcd.print(line4);
}

// Task Function Definitions
int cursorPosX = 0; //blink on posX
int cursorPosY = 0; //blink on posY
int menuOption = 0; // 0 = start menu, 1-3 = song 1-3 menu 
int songOption = 0; // 1-3 = song 1-3
int controls = 0;
int star = 0; // bool star show *
int play = 0; // bool play song
int pause = 0; // bool pause song
int confirmation = 1; //button click
// Task 1
int TickFct_LCDOutput(int state) {
  switch (state) { // State Transitions
    case LO_init:
      LCDWriteLines("Song 1 ", "Song 2", "Song 3  ", "Start");
      state = LO_StartMenu;
    break;
    
    case LO_StartMenu:
      LCDWriteLines("Song 1", "Song 2", "Song 3", "Start");
      if(play == 1){
        state = LO_PlaySong;
      }
      else if(confirmation == 0){
        state = LO_StartMenuSelect;
      }
      else{
        state = LO_StartMenu;
      }
    break;

    case LO_StartMenuSelect:
      if(play == 1){
        state = LO_setScreen;
      }
      else if(confirmation == 1) {
        state = LO_StartMenu;
      }
      else{
        state = LO_StartMenuSelect;
      }
    break;

    case LO_setScreen:
      state = LO_PlaySong;
    break;
    
    case LO_PlaySong:
       
      if(confirmation == 0){
        state = LO_PlaySongSelect;
      }
      if(songOption == 1) {
        
        LCDWriteLines("Playing", "Song 1", "Pause", "Play");
      }
      else if(songOption == 2) {
        
        LCDWriteLines("Playing", "Song 2", "Pause", "Play");
      }
      else if(songOption == 3) { 
        LCDWriteLines("Playing", "Song 3", "Pause", "Play");
      }
      else{
        state = LO_init;
      }
    break;

    case LO_PlaySongSelect: 
      if(confirmation == 1){
        state = LO_PlaySong;
      }
      if(songOption == 1) {
        LCDWriteLines("Playing", "Song 1", "Pause", "Play");
      }
      else if(songOption == 2) {
        LCDWriteLines("Playing", "Song 2", "Pause", "Play");
      }
      else if(songOption == 3) {
        LCDWriteLines("Playing", "Song 3", "Pause", "Play");
      }
      else{
        state = LO_init;
      }
    break;
  }

  switch (state) { // State Actions
    case LO_init:
      cursorPosX = 0;
      cursorPosY = 0;
      play = 0;
    break;
    case LO_StartMenu:
      if(cursorPosX == 0 && cursorPosY == 0){
        lcd.setCursor(6, 0);
        lcd.blink();
      }
      else if(cursorPosX == 1 && cursorPosY == 0){
        lcd.setCursor(7, 0);
        lcd.blink();
      }
       else if(cursorPosX == 0 && cursorPosY == 1){;
        lcd.setCursor(6, 1);
        lcd.blink();
      }
       else if(cursorPosX == 1 && cursorPosY == 1){
        lcd.setCursor(7, 1);
        lcd.blink();
      }
      //Serial.println("Menu");
    break;
    case LO_StartMenuSelect:
      if(cursorPosX == 0 && cursorPosY == 0){
        //get rid of * if change selection
        lcd.setCursor(7, 0);
        lcd.print(" ");
        lcd.setCursor(6, 1);
        lcd.print(" ");
        lcd.setCursor(7, 1);
        lcd.print(" ");

        lcd.setCursor(6, 0);
        lcd.print("*"); //print * at correct pos
        lcd.setCursor(6, 0);
        lcd.blink();

        //songOption
        songOption = 1;
      }
      else if(cursorPosX == 1 && cursorPosY == 0){
        //get rid of * if change selection
        lcd.setCursor(6, 0);
        lcd.print(" ");
        lcd.setCursor(6, 1);
        lcd.print(" ");
        lcd.setCursor(7, 1);
        lcd.print(" ");

        lcd.setCursor(7, 0);
        lcd.print("*"); //print * at correct pos
        lcd.setCursor(7, 0);
        lcd.blink();

         //songOption
        songOption = 2;
      }
       else if(cursorPosX == 0 && cursorPosY == 1){
        //get rid of * if change selection
        lcd.setCursor(7, 0);
        lcd.print(" ");
        lcd.setCursor(6, 0);
        lcd.print(" ");
        lcd.setCursor(7, 1);
        lcd.print(" ");

        lcd.setCursor(6, 1);
        lcd.print("*"); //print * at correct pos
        lcd.setCursor(6, 1);
        lcd.blink();

        //songOption
        songOption = 3;
      }
       else if(cursorPosX == 1 && cursorPosY == 1){
        //get rid of * if change selection
        lcd.setCursor(7, 0);
        lcd.print(" ");
        lcd.setCursor(6, 1);
        lcd.print(" ");
        lcd.setCursor(6, 0);
        lcd.print(" ");

        lcd.setCursor(7, 1);
        lcd.print("*"); //print * at correct pos
        lcd.setCursor(7, 1);
        lcd.blink();

         //songOption
        play = 1;
      }
   
      //Serial.println("Select");
    break;

    case LO_setScreen:
      lcd.clear();
    break;

    case LO_PlaySong:
      if(cursorPosX == 0 && cursorPosY == 0){
        lcd.setCursor(6, 1);
        lcd.blink();
      }
      else if(cursorPosX == 1 && cursorPosY == 0){
        lcd.setCursor(7, 1);
        lcd.blink();
      }
       else if(cursorPosX == 0 && cursorPosY == 1){
        lcd.setCursor(6, 1);
        lcd.blink();
      }
       else if(cursorPosX == 1 && cursorPosY == 1){
        lcd.setCursor(7, 1);
        lcd.blink();
      }
    break;

    case LO_PlaySongSelect:
      if(cursorPosX == 0 && cursorPosY == 0){
        lcd.setCursor(7, 1);
        lcd.print(" ");

        lcd.setCursor(6, 1);
        lcd.print("*"); //print * at correct pos
        lcd.setCursor(6, 1);
        lcd.blink();
        pause = 1;
      }
      else if(cursorPosX == 1 && cursorPosY == 0){
        lcd.setCursor(6, 1);
        lcd.print(" ");

        lcd.setCursor(7, 1);
        lcd.print("*"); //print * at correct pos
        lcd.setCursor(7, 1);
        lcd.blink();
        pause = 0;
      }
       else if(cursorPosX == 0 && cursorPosY == 1){
        lcd.setCursor(7, 1);
        lcd.print(" ");

        lcd.setCursor(6, 1);
        lcd.print("*"); //print * at correct pos
        lcd.setCursor(6, 1);
        lcd.blink();
        pause = 1;
      }
       else if(cursorPosX == 1 && cursorPosY == 1){
        lcd.setCursor(6, 1);
        lcd.print(" ");

        lcd.setCursor(7, 1);
        lcd.print("*"); //print * at correct pos
        lcd.setCursor(7, 1);
        lcd.blink();
        pause = 0;
      }
    break;

  }
  return state;
}

int jyX;
int jyY;
// Task 2
int TickFct_JoystickInput(int state) {
  switch (state) { // State Transitions
    case JI_init:
      state = JI_jystk;
    break;
    case JI_jystk:
      state = JI_init;
    break;
  }

   switch (state) { // State Actions
    case JI_jystk:
      if(jyY > 1000){
        //menu control
        cursorPosX = cursorPosX - 1;
      }
      else if(jyY < 50){
        //menu control
        cursorPosX = cursorPosX + 1;
      }
      else if(jyX > 1000){
        //menu control
        cursorPosY = cursorPosY + 1;

      }
      else if(jyX < 50){
        //menu control
        cursorPosY = cursorPosY - 1;
      }


      if(cursorPosX > 1){
          cursorPosX = 1;
        }
      else if(cursorPosX < 0){
          cursorPosX = 0;
        }
    
      if(cursorPosY > 1){
          cursorPosY = 1;
        }
      else if(cursorPosY < 0){
          cursorPosY = 0;
        }
    break;
  }
  return state;
}

// Sound Output
int counter = 0;
int counterfull = 0;
int note = 0;
int TickFct_SoundOutput(int state) {
  switch (state) { // State Transitions
    case SO_init:
      if(songOption == 1 && play == 1){
        state = SO_Song1On;
      }
      else if(songOption == 2 && play == 1){
        state = SO_Song2On;
      }
      else if(songOption == 3 && play == 1){
        state = SO_Song3On;
      }
      else{
        state = SO_init;
      }
    break;

    case SO_Song1On:
      if(pause == 1){
        state = SO_pausing;
      }
      else if(counter >= song1_time[note]) {
         state = SO_SoundOff;
         note++;
         counter = 0;
         note = note % 20;
         //play = 0;
      }
    break;

    case SO_Song2On:
      if(pause == 1){
        state = SO_pausing;
      }
      else if(counter >= song2_time[note]) {
         state = SO_SoundOff;
         note++;
         counter = 0;
         note = note % 20;
         //play = 0 ;
      }
    break;

    case SO_Song3On:
      if(pause == 1){
        state = SO_pausing;
      }
      else if(counter >= song3_time[note]) {
         state = SO_SoundOff;
         note++;
         counter = 0;
         note = note % 20;
         //play = 0 ;
      }
    break;

    case SO_SoundOff:
      if(songOption == 1){
        state = SO_Song1On;
      }
      else if(songOption == 2){
        state = SO_Song2On;
      }
      else if(songOption == 3){
        state = SO_Song3On;
      }
      else{
        state = SO_init;
      }
    break;
    
    case SO_pausing:
      if(pause == 0){
        if(songOption == 1 && play == 1){
          state = SO_Song1On;
        }
        else if(songOption == 2 && play == 1){
          state = SO_Song2On;
        }
        else if(songOption == 3 && play == 1){
          state = SO_Song3On;
        }
      }

  }
   switch (state) { // State Actions
    case SO_init:
      //Serial.println("init");
    break;

    case SO_Song1On:
      tone(buzzer, song1[note], periodSoundOutput * song1_time[note]);
      counter++;
      counterfull++;
      if(counterfull >= 96){
        songOption =0;
        counterfull = 0;
      }
      //Serial.println("song1");
    break;

    case SO_Song2On:
      tone(buzzer, song2[note], periodSoundOutput * song2_time[note]);
      counter++;
      counterfull++;
      if(counterfull >= 122){
        songOption =0;
        counterfull = 0;
      }
      //Serial.println("song2");
    break;
    
    case SO_Song3On:
      tone(buzzer, song3[note], periodSoundOutput * song3_time[note]);
      counter++;
      counterfull++;
      if(counterfull >= 120){
        songOption =0;
        counterfull = 0;
      }
      //Serial.println("song3");
    break;

    case SO_SoundOff:
      noTone(buzzer);
      //Serial.println("Sound off");
    break;

  }
  return state;
}

// Task 4 (Unused)
int TickFct_Controller(int state) {
  switch (state) { // State Transitions
    case 0:
    break;
  }

   switch (state) { // State Actions
    case 0:
    break;
  }
  return state;
}



void InitializeTasks() {
  unsigned char i=0;
  tasks[i].state = LO_init;
  tasks[i].period = periodLCDOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_LCDOutput;
  ++i;
  tasks[i].state = JI_init;
  tasks[i].period = periodJoystickInput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_JoystickInput;
  ++i;
  tasks[i].state = SO_init;
  tasks[i].period = periodSoundOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_SoundOutput;
  ++i;
  tasks[i].state = C_init;
  tasks[i].period = periodController;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_Controller;

}

void setup() {
  // put your setup code here, to run once:
  InitializeTasks();
  pinMode(10, INPUT_PULLUP);
  TimerSet(tasksPeriodGCD);
  TimerOn();
  Serial.begin(9600);
  // Initialize Outputs
  lcd.begin(16, 2);
  // Initialize Inputs

}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("songOption: " + songOption);
  // Task Scheduler with Timer.h
  jyX = analogRead(A1);
  jyY = analogRead(A0);
  confirmation = digitalRead(10);
  
  Serial.print("x: ");
  Serial.println(cursorPosX);
  Serial.print("y: ");
  Serial.println(cursorPosY);

  //Serial.print("x: ");
  //Serial.println(jyX);
  //Serial.print("y: ");
  //Serial.println(jyY);


}
