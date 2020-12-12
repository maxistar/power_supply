#ifndef CAROLINA_POWER_SUPPLY_CPP_
#define CAROLINA_POWER_SUPPLY_CPP_

// Copyright 2020 Max Starikov


#include "./Arduino.h"
#include "./CarolinaPowerSupply.h"

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include "Button.h"
#include "Timer.h"

Adafruit_INA219 ina219;

#define OLED_RESET 4
#define BUTTON_PIN 12
#define MAX_SESSIONS_NUMBER 10

#define DISPLAY_MODE_CURRENT 1
#define DISPLAY_MODE_CAPACITY 2
#define DISPLAY_MODE_QUESTION 3

#define MODE_DISCHARGING 0 
#define MODE_CHARGING 1 

#define EEPROM_SKIP 2

Adafruit_SSD1306 display(OLED_RESET);



double busvoltage = 0;
double current_mA = 0;
double capacity = 0;
double capacityW = 0;
int counterEEPROM = 0;


unsigned long previousTime = 0;

Button button = Button(BUTTON_PIN);
Timer timer1 = Timer(5000);

byte displayMode = DISPLAY_MODE_CURRENT;
byte chargingMode = MODE_DISCHARGING;

int numberSessions = 0;
int sessionIndex = 0;

struct Session {
  byte mode;
  double ah;
  double wh;
  int duration; 
};



void readFromEEPROM() {
  int address = 0;
  Session ses;
  EEPROM.get(address, numberSessions);
  address += sizeof(int);
  EEPROM.get(address, sessionIndex);
  address += sizeof(int);
  address += sessionIndex * sizeof(Session);
  EEPROM.get(address, ses);
  capacity = ses.ah;
  capacityW = ses.wh;
  chargingMode = ses.mode;
  Serial.println("capacity");
  Serial.println(numberSessions);
}

void storeToEEPROM() {
  int address = 0;
  Session ses;
  address += sizeof(int);
  address += sizeof(int);
  address += sessionIndex * sizeof(Session);
  EEPROM.get(address, ses);
  ses.ah = capacity;
  ses.wh = capacityW;
  ses.mode = chargingMode;
  EEPROM.put(address, ses);
}

void nextSession() {
  storeToEEPROM();
  sessionIndex++;
  if (sessionIndex > MAX_SESSIONS_NUMBER) {
    sessionIndex = 0;
  }
  numberSessions++;
  capacity = 0;
  capacityW = 0; 

  int address = 0;
  Session ses;
  ses.ah = 0;
  ses.wh = 0;
  ses.mode = chargingMode;
  numberSessions = EEPROM.put(address, numberSessions);
  address += sizeof(int);
  sessionIndex = EEPROM.put(address, sessionIndex);
  address += sizeof(int);
  address += sessionIndex * sizeof(Session);
  EEPROM.get(address, ses);

  display.print("next session");
  display.print(sessionIndex);
}


void storeToEEPROMIfNeeded() {
  if (counterEEPROM == 0) {
    storeToEEPROM();
  }
  counterEEPROM++;
  if (counterEEPROM > EEPROM_SKIP) {
    counterEEPROM = 0;
  }
}

void readCurrent() {
  // read data from ina219
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
}

void countCapacity() {
  readCurrent();
  unsigned long newTime = micros();
  if (previousTime != 0) {   
    if (
      (current_mA > 0 && chargingMode == MODE_DISCHARGING) ||
      (current_mA < 0 && chargingMode == MODE_CHARGING)
    ) { 
      unsigned long timeDiff = previousTime - newTime;
      //3600 - number seconds in hour, 1000 mA - in 1A, 1000000 - mS in 1 second  
      double capacityDiff = ((double)timeDiff * abs(current_mA)) / 3600000000000;
      double capacityDiffW = ((double)timeDiff * abs(current_mA * busvoltage)) / 3600000000000;
      
      capacity += capacityDiff;
      capacityW += capacityDiffW;
      storeToEEPROMIfNeeded();
    }
  }
  previousTime = newTime;
}


void initDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.display();
}

void drawCurrentDisplay() {
  // show data on OLED
  display.clearDisplay();

  display.setCursor(0, 0);
  if (chargingMode == MODE_DISCHARGING) {
    display.print("discharging");
  } else {
    display.print("charging");
  }

  display.setCursor(80, 0);
  display.print(sessionIndex);
  display.print(" ");
  display.print(numberSessions);

  display.setCursor(0, 10);
  display.print(busvoltage);
  display.print(" V");

  display.setCursor(0, 20);
  display.print(abs(current_mA));
  display.print(" mA");

  display.display();
}

void drawCapacityDisplay() {
  // show data on OLED
  display.clearDisplay();

  display.setCursor(0, 0);
  if (chargingMode == MODE_DISCHARGING) {
    display.print("discharging");
  } else {
    display.print("charging");
  }

  display.setCursor(80, 0);
  display.print(sessionIndex);
  display.print(" ");
  display.print(numberSessions);


  display.setCursor(0, 10);
  display.print(capacity);
  display.print(" mAh");

  display.setCursor(0, 20);
  display.print(capacityW);
  display.print(" mWh");

  display.display();  
}

void drawQuestionDisplay() {
  display.clearDisplay();

  display.setCursor(0, 0);

  if (chargingMode == MODE_CHARGING) {
    display.print("stop charging?");
  } else {
    display.print("stop discharging?");
  }

  display.setCursor(0, 15);
  display.print("<ok>");

  display.display();
}

void drawDisplay() {
  if (displayMode == DISPLAY_MODE_CURRENT) {
    drawCurrentDisplay();
  }

  if (displayMode == DISPLAY_MODE_CAPACITY) {
    drawCapacityDisplay();
  }

  if (displayMode == DISPLAY_MODE_QUESTION) {
    drawQuestionDisplay();
  }
}

void updateDisplayMode() {
  byte mode;
  if (current_mA > 0) {
    mode = MODE_DISCHARGING;
  } else {
    mode = MODE_CHARGING;
  }

  if (mode == chargingMode) {
    if (displayMode == DISPLAY_MODE_CURRENT) {
      displayMode = DISPLAY_MODE_CAPACITY;
    } else {
      displayMode = DISPLAY_MODE_CURRENT;
    } 
  } else {
    if (displayMode == DISPLAY_MODE_CURRENT) {
      displayMode = DISPLAY_MODE_CAPACITY;
    } else if (displayMode == DISPLAY_MODE_CAPACITY) {
      displayMode = DISPLAY_MODE_QUESTION;
    } else {
      displayMode = DISPLAY_MODE_CURRENT;
    }
  }
} 

void checkIfSwitchNeeded() {
  readCurrent();
  byte mode;
  if (current_mA > 0) {
    mode = MODE_DISCHARGING;
  } else {
    mode = MODE_CHARGING;
  }

  if (mode != chargingMode) {
    updateDisplayMode();
  }
}

void button_click(char value) {
  Serial.println("click");
  if (displayMode == DISPLAY_MODE_QUESTION) { 
    Serial.println("worked");

    if (chargingMode == MODE_CHARGING) {
      Serial.println("now discharging");
      chargingMode = MODE_DISCHARGING;
      nextSession();
    } 
    else if (chargingMode == MODE_DISCHARGING) {
      Serial.println("now charging");
      chargingMode = MODE_CHARGING;
      nextSession();
    }
    updateDisplayMode();
    drawDisplay();
  }
};

void timeout_callback() {
  countCapacity();
  updateDisplayMode();
  drawDisplay();
};

CarolinaPowerSupply::CarolinaPowerSupply() {
  
}

void CarolinaPowerSupply::setup() {

  //for (int i = 0 ; i < EEPROM.length() ; i++) {
  //  EEPROM.write(i, 0);
  //}

  Serial.begin(9600);
  Serial.println("setup");
  // initialize ina219 with default measurement range of 16V, 400mA
  ina219.begin();
  ina219.setCalibration_16V_400mA(); // set measurement range to 16V, 400mA


  
  button.setup();
  button.setPressCallback(button_click);

  timer1.setup();
  timer1.setTimeoutCallback(timeout_callback);
  
  readFromEEPROM();
  readCurrent();
  initDisplay();
  // initialize OLED display
  drawDisplay();
}

void CarolinaPowerSupply::loop() {
  button.loop();
  timer1.loop();
  //checkIfSwitchNeeded();
}

#endif  // CAROLINA_POWER_SUPPLY_H_
