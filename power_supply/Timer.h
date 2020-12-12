#include "Arduino.h"

class Timer {
    long timeOut;
    long timeStart;
    void (*timeoutCallback)() = NULL;

    public:
    Timer(long milliseconds);
    void setTimeoutCallback(void (*timeoutCallbackPtr)());
    void loop();
    void setup();
};