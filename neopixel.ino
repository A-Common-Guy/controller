#include "ESP32TimerInterrupt.h"
#include "ESP32_ISR_Timer.h"
#include "WiFi.h"


void IRAM_ATTR TimerHandler0(void)
{
  if (ledpower) {
    status_led.setPixelColor(0, status_led.Color(0, 0, 0));
    ledpower = false;
  }
  else if (ledstate == RED_BLINK) {
    status_led.setPixelColor(0, status_led.Color(255, 0, 0));
    ledpower = true;

  }
  else if (ledstate == BLUE_BLINK) {
    status_led.setPixelColor(0, status_led.Color(0, 0, 255));
    ledpower = true;

  }
  

}
