#ifndef CAROLINA_POWER_SUPPLY_CPP_
#define CAROLINA_POWER_SUPPLY_CPP_

// Copyright 2020 Max Starikov


#include "./Arduino.h"
#include "./CarolinaPowerSupply.h"

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>

Adafruit_INA219 ina219;

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

double busvoltage = 0;
double current_mA = 0;
double copacity = 0;

unsigned long previousTime = 0;

CarolinaPowerSupply::CarolinaPowerSupply() {
  
}

void CarolinaPowerSupply::setup() {
  // initialize ina219 with default measurement range of 16V, 400mA
  ina219.begin();
  ina219.setCalibration_16V_400mA(); // set measurement range to 16V, 400mA

  // initialize OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.display();
}

void CarolinaPowerSupply::loop() {
  // read data from ina219
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();

  // show data on OLED
  display.clearDisplay();

  display.setCursor(0, 0);
  display.print(busvoltage);
  display.print(" V");

  display.setCursor(50, 0);
  display.print(abs(current_mA));
  display.print(" mA");

  display.setCursor(0, 10);
  if (current_mA > 0) {
    display.print("discharging");
  } else {
    display.print("charging");
  }

  unsigned long newTime = micros();
  if (previousTime != 0) {    
    unsigned long timeDiff = previousTime - newTime;
    //3600 - number seconds in hour, 1000 mA - in 1A, 1000000 - mS in 1 second  
    double copacityDiff = ((double)timeDiff * abs(current_mA)) / 3600000000000;
    copacity += copacityDiff;
  }
  previousTime = newTime;

  display.setCursor(0, 20);
  display.print(copacity);
  display.print(" mAh");

  display.display();
  delay(1000);

}

#endif  // CAROLINA_POWER_SUPPLY_H_
