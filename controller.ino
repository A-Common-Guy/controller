#include <Adafruit_ADS1X15.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
//#include <nRF24L01.h>
#include <RF24.h>
#include "ESP32TimerInterrupt.h"
#include "ESP32_ISR_Timer.h"
#include "WiFi.h"
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPI.h>
#include <SD.h>


//paths for files
#define PATH_CALIBRATION_DATA "/calibration_data/calibration_data.json"
#define DIR_CALIBRATION_DATA "/calibration_data"

//oled display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//calibration
#define LOG Serial.println("LOG")
int first = 1;
int no_cal_data = 0;
int16_t center[4] = {13000,13000,13000,13000};
int16_t deviation[4] = {500,500,500,500};
int16_t floor_[4] = {30,30,30,30};
int16_t ceil_[4] = {26000,26000,26000,26000};
int16_t floor_deviation[4] = {30,30,30,30};
int16_t ceil_deviation[4] = {26000,26000,26000,26000};


//ADC init
Adafruit_ADS1115 ads;
int16_t adc[4];
//wire coms init

#define SDA 21
#define SCL 22


//calibration issues
int recalibrate=0;

//Blinking led
#define TIMER0_INTERVAL_MS 500
ESP32Timer ITimer0(2);
volatile uint8_t ledstate;
volatile bool ledpower;
void IRAM_ATTR TimerHandler0(void);
//LED STATES
#define RED_BLINK 0
#define BLUE_BLINK 1


//controller states
#define HANDSHAKING_SEND 0
#define SENDING 2
#define LOST_CONNECTION 4
//controller pins

//controller buttons
#define DX_BTN 1 //TODO
#define SX_BTN 1 //TODO
#define KEY1 34
//lever buttons
#define DX_RETRO 13
#define SX_RETRO 12



//blinker
ESP32Timer ITimer(1);
ESP32_ISR_Timer ISR_Timer;
#define LED_TOGGLE_INTERVAL_MS        500L


//Json handler
const size_t capacity=1024;
DynamicJsonDocument doc(capacity);


//startup status pin
#define STATUS_PIN 15 
Adafruit_NeoPixel status_led = Adafruit_NeoPixel(1, STATUS_PIN, NEO_GRB + NEO_KHZ800);

int state;//state-machine state

int live_detector;//connection health indicator


//sending payload
typedef struct packet {
  int potvalues[4];
  int flag;
  int dxbtn;
  int sxbtn;
  int analdx;
} packet;

//receiving payload
typedef struct ack {
  float speed;
  int battery;
  int isAlive;
} ack;

packet payload;
ack ackpack;




//Radio comms devices
#define PIN_RF24_CSN            32            // CSN PIN for RF24 module.
#define PIN_RF24_CE             27            // CE PIN for RF24 module.
//Software MISO 33
//Software MOSI 25
//Software SCK  26
#define TIMEOUT 1000 //max time of live=1s
//Init the software SPI for the radio
RF24 radio(PIN_RF24_CE, PIN_RF24_CSN);
byte rf24_tx[6] = "1SNSR";    // Address used when transmitting data.


unsigned long last_reading;

//neopixel values
uint8_t r, g, b;

void setup() {  

  Serial.begin(115200);

  //button init
  pinMode(KEY1, INPUT);

  //init the SD card
  if (!SD.begin(5)) {
    Serial.println("SD card initialization failed!");
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
  }
  //create filesystem
  File root = SD.open(DIR_CALIBRATION_DATA);
  if (!root.isDirectory()) {
    createDir(SD,DIR_CALIBRATION_DATA);
  }
  root.close();


  
  //connect to adc
  Wire.begin (SDA, SCL);
  ads.setGain(GAIN_ONE);//+-4.096V

  //save energy
  WiFi.mode(WIFI_OFF);
  btStop();


  status_led.begin();
  if (!ads.begin(0x48)) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  //radio variables startup
  // Show debug information for NRF24 tranceiver.
  nrf24_setup();
  last_reading = 0;


  //automa initialization
  state = HANDSHAKING_SEND;

    //init oled
  oled_init();
  //show logo on oled
  display_logo();
 // display.display(); 

  //blinker init at 0.5s
  ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0);
  ledstate = RED_BLINK;
  ledpower = false;
}


void loop() {
  if(first){
    if(SD.exists(PATH_CALIBRATION_DATA)){
      no_cal_data = 0;
    }

    if(digitalRead(KEY1) || no_cal_data){
      recalibrate=1;
    }
    else{
      Serial.println("Reading calibration data");
      getJSonFromFile(&doc,PATH_CALIBRATION_DATA);
      for(int i=0;i<4;i++){
        center[i] = doc["center"][i];
        deviation[i] = doc["deviation"][i];
        floor_[i] = doc["floor"][i];
        ceil_[i] = doc["ceil"][i];
        floor_deviation[i] = doc["floor_deviation"][i];
        ceil_deviation[i] = doc["ceil_deviation"][i];
      }
    }
  }     
  if (recalibrate && first) {

    ITimer0.stopTimer();
    status_led.setPixelColor(0, status_led.Color(0, 255, 0));
    status_led.show();
    delay(1000);
    Serial.println("Calibration");
    calibrate();
    ESP.restart();

  }
  first=0;

  //finite state automa:
  //Serial.println(state);
  switch (state) {
    case HANDSHAKING_SEND:
      ledstate = RED_BLINK;
      compose_payload(88,center,deviation,floor_,ceil_);
      if (radio.write(&payload, sizeof(packet))) {
        radio.read(&ackpack, sizeof(ackpack));
        if (ackpack.speed == 88 && ackpack.battery == 88) {
          state = SENDING;
          last_reading = millis();
          live_detector = ackpack.isAlive;
        }
      }
      break;
    case SENDING:
      ledstate = BLUE_BLINK;

      compose_payload(live_detector,center,deviation,floor_,ceil_);
      if (radio.write(&payload, sizeof(packet))) {
        radio.read(&ackpack, sizeof(ackpack));
        if (((ackpack.isAlive) == live_detector) && (ackpack.isAlive % 5 != 0)) {
          if ((millis() - last_reading) > TIMEOUT) {
            state = LOST_CONNECTION;
          }
        }
        else {
          last_reading = millis();
        }
        live_detector = ackpack.isAlive;
      }
      else {
        if ((millis() - last_reading) > TIMEOUT) {
          state = LOST_CONNECTION;
        }
      }



      break;
    case LOST_CONNECTION:
      state = HANDSHAKING_SEND;
      break;
  }
  verbose_sending();
  verbose_pots();
  delay(10);
  status_led.show();
}
