#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"
#include "Timer.h"


Timer::Timer(long milliseconds){
    this->timeOut = milliseconds;
} 

void Timer::setup() {
    timeStart = millis();
}

void Timer::loop() {
    long current = millis();
    if (current - timeStart < timeOut) {
        return;
    }
    timeStart = current;
    if (timeoutCallback != NULL) {
        timeoutCallback();       
    }
}

void Timer::setTimeoutCallback(void (*timeoutCallbackPtr)()) {
    this->timeoutCallback = timeoutCallbackPtr;
}

#endif