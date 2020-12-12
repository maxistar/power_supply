#include "Arduino.h"

class Button {
    int buttonPin;
    char pressed; 
    void (*changeCallbackPtr)(char) = NULL;
    void (*pressCallbackPtr)() = NULL;
    void (*releaseCallbackPtr)() = NULL;
    
    //debouncing staff
    char isDebouncing;
    long debounceLastTime;  
    void checkSwitches();
    void _onPressed();
    void _onReleased();
    void _onChanged(char state);

    public:
    Button(int buttonPin);
    void setChangeCallback(void (*changeCallbackPtr)(char));
    void setPressCallback(void (*pressCallbackPtr)());
    void setReleaseCallback(void (*releaseCallbackPtr)());
    void loop();
    void setup();
};