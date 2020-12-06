// Cool ARduino OLED INA (CAROLINA) Power Supply
//
// Required Libraries
// https://github.com/adafruit/Adafruit_INA219
// https://github.com/adafruit/Adafruit_SSD1306

#include "CarolinaPowerSupply.h"


CarolinaPowerSupply carolinaPowerSupply;

void setup() {
  carolinaPowerSupply.setup();


}

void loop() {
  carolinaPowerSupply.loop();
}
