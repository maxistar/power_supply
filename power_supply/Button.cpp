#ifndef BUTTON_H
#define BUTTON_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "Button.h"

#define DEBOUNCE 50

Button::Button(int buttonPin){
    this->buttonPin = buttonPin;
} 

void Button::setup(){
    pinMode(this->buttonPin, INPUT_PULLUP);
} 


void Button::checkSwitches()
{
    if (this->isDebouncing && (this->debounceLastTime + DEBOUNCE) > millis()) {
        return; // not enough time has passed to debounce
    }
    char buttonState = digitalRead(this->buttonPin);
    char currentState = !buttonState;   // read the button

    if (currentState != this->pressed) {
        this->pressed = currentState;
        if (currentState == 1) {
            this->_onPressed();
        }
        else {
            this->_onReleased();
        }
        this->_onChanged(this->pressed);
        this->isDebouncing = 1;
    } 
    else {
        this->isDebouncing = 0; 
    }
    // ok we have waited DEBOUNCE milliseconds, lets reset the timer
    this->debounceLastTime = millis();
}

void Button::loop() {
    this->checkSwitches();      // when we check the switches we'll get the current state 
}

void Button::_onPressed() {
    if (pressCallbackPtr != NULL) {
        pressCallbackPtr();       
    }
}

void Button::_onReleased() {
    if (releaseCallbackPtr != NULL) {
        releaseCallbackPtr();      
    }
}

void Button::_onChanged(char state) {
    if (changeCallbackPtr != NULL) {
        changeCallbackPtr(state);
    }
}

void Button::setChangeCallback(void (*changeCallbackPtr)(char)) {
    this->changeCallbackPtr = changeCallbackPtr;
}
void Button::setPressCallback(void (*pressCallbackPtr)()) {
    this->pressCallbackPtr = pressCallbackPtr;
}
void Button::setReleaseCallback(void (*releaseCallbackPtr)()) {
    this->releaseCallbackPtr = releaseCallbackPtr;
}

#endif