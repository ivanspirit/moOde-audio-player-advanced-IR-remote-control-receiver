#include <Arduino.h>

#include <HID-Project.h>                    //include HID_Project library
#include <HID-Settings.h>

#include "IRLremote.h"

// Choose a valid PinInterrupt or PinChangeInterrupt* pin of your Arduino board
#define pinIR 2

// Choose the IR protocol of your remote. See the other example for this.
//CNec IRLremote; // pionieer remote
//CPanasonic IRLremote;
CHashIR IRLremote;
//#define IRLremote Sony12

#define pinLed 17 //int RXLED = 17;  // The RX LED has a defined Arduino pin
#define pinButtLed 6 //button led
#define pinButton 7 //button
#define pinEnS1 1 //encoder S1
#define pinEnS2 0 //encoder S2
#define pinEnKey 3 //encoder Key
#define pinRelay 5 //relay output

#define ENC_OFFS  0
#define ENCODER_USE_INTERRUPTS

#include <Encoder.h>
// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(1, 0);
//   avoid using pins with LEDs attached
long oldPosition  = 0;

bool  Relayflag = false;
bool  RpiIsReseting = false;
bool  RpiPoweredlag = false;

#include <ezButton.h>
ezButton Butt(pinButton);
ezButton EncKey(pinEnKey);

#include <singleLEDLibrary.h>
sllib Led(pinButtLed);

#define LED_POWEROFF_BREATH 1000
#define LED_IDLE_BREATH 500
#define IDLE_LED_TIME 500  // 500 ms idle time and the last LED status is active
unsigned long old_idle_timer = 0;


#define RPI_POWEROFF_TIME 10000 // 10 sec time to wait before shut down the power
#define RPI_POWEROFF_BLINK 300 // blink speed during the wait to poweroff 
#define RPI_POWERON_TIME 10000 // 10 sec time to wait before shut down the power
#define RPI_POWERON_BLINK 100 // blink speed during the wait to poweroff 
//
void doloops();
///////////////////////////////////////////// Functions
void  resetLedIdleTimer(){
  old_idle_timer = millis();
}

void  setVolUp(){
  if(!RpiPoweredlag)return;
  Led.setBlinkSingle(100);
  Consumer.write(MEDIA_VOLUME_UP); // ok
  Serial.print(F("\nVol +\n")); 
  resetLedIdleTimer();
}

void  setVolDwn(){
  if(!RpiPoweredlag)return;
  Led.setBlinkSingle(100);
  Consumer.write(MEDIA_VOLUME_DOWN); // ok
  Serial.print(F("\nVol -\n")); 
  resetLedIdleTimer();
}

void  setMute(){
  if(!RpiPoweredlag)return;
  Led.setBlinkSingle(100);
  Consumer.write(MEDIA_VOLUME_MUTE); // ok
  Serial.print(F("\nMute\n")); 
    if (IRLremote.available())
      {
        // Get the new data from the remote
        auto data = IRLremote.read();
      }
  resetLedIdleTimer();
}

void  doRpiReset(){
  Led.setBlinkSingle(100);
  RpiIsReseting = true;
  Consumer.write(HID_CONSUMER_RESET); // ok
  Serial.print(F("\nReset\n")); 
  resetLedIdleTimer();
}

void  doRpiPowerOff(){
unsigned long poweroff_timer = millis();
  Led.setOffSingle();
  Consumer.write(HID_CONSUMER_POWER); 
  Serial.print(F("\nPowerOff - signal\n")); 

  while(millis() < (poweroff_timer + RPI_POWEROFF_TIME)){
    Led.blinkSingle(RPI_POWEROFF_BLINK);
    if (IRLremote.available())
      {
        // Get the new data from the remote
        auto data = IRLremote.read();
      }
    doloops();  
    yield();
  }
  RpiPoweredlag = false;
  Relayflag = false;
  Serial.print(F("\nPowerOff - relay off\n")); 
  Led.setOffSingle();
  resetLedIdleTimer();
}

void  doRpiPowerOn(){
unsigned long poweron_timer = millis();
  Led.setOnSingle();
  RpiPoweredlag = true;
  Relayflag = true;
  digitalWrite(pinRelay, Relayflag);
  Serial.print(F("\nPowerOn - relay on\n")); 
  while(millis() < (poweron_timer + RPI_POWERON_TIME)){
    Led.blinkSingle(RPI_POWERON_BLINK);
    if (IRLremote.available())
      {
        // Get the new data from the remote
        auto data = IRLremote.read();
      }
    doloops();
    yield();
  }
  resetLedIdleTimer();
}

void doRpiPower(){
  if(!RpiPoweredlag){
    doRpiPowerOn();
  } else{
    if(!digitalRead(pinEnKey)){
      doRpiReset();
    }else{
      doRpiPowerOff();
    }
  }
}

void doloops(){
  Led.update();
  Butt.loop(); // MUST call the loop() function first
  EncKey.loop(); // MUST call the loop() function first
  digitalWrite(pinRelay, Relayflag);
  //digitalWrite(pinLed, !Relayflag);
}

////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  // Set board LED to output
  pinMode(pinLed, OUTPUT);
  // Set ButtonLED to output
  pinMode(pinButtLed, OUTPUT);
  // Set relay pin to output
  pinMode(pinRelay, OUTPUT);
  // Set EnKey to input
  pinMode(pinEnKey, INPUT_PULLUP);
  // Set Button to input
  pinMode(pinButton, INPUT_PULLUP);
  //pinMode(pinButton, INPUT);

  digitalWrite(pinButtLed, HIGH);

  Consumer.begin();                         //initialize computer connection
  delay(2000);                              //wait for computer to connect


  Butt.setDebounceTime(20); // set debounce time to 50 milliseconds
  EncKey.setDebounceTime(20); // set debounce time to 50 milliseconds

  // Start serial debug output, wait up to 5 sec. to connect
unsigned long serial_timer = millis();
  Serial.begin(115200);
  do{
    if(Serial){
      Serial.print(F("Serial connected ... "));
    } 
  } while ( (millis() < (serial_timer + 5000)) && !Serial);
  Serial.println(F("StartUp"));

  // Start reading the remote. PinInterrupt or PinChangeInterrupt* will automatically be selected
  if (!IRLremote.begin(pinIR)){
   Serial.println(F("You did not choose a valid IR pin."));
  }

  digitalWrite(pinButtLed, LOW);

}

void loop() {

/// update functions
  doloops();

/// Idle Led 
 if(millis() >= (old_idle_timer + IDLE_LED_TIME)){
    if(RpiPoweredlag){                              // powered on status led
      //Led.setBreathSingle(LED_IDLE_BREATH);
      Led.setFlickerSingle();
    } else {                                        // powered off status led
      Led.setBreathSingle(LED_POWEROFF_BREATH);
    }
    RpiIsReseting = false;
    old_idle_timer = millis();
  }

/// Encoder functions
  long newPosition = myEnc.read();
   if (newPosition != oldPosition) {
    if(newPosition > (oldPosition + ENC_OFFS)){
      Serial.print(newPosition);
      setVolUp();
      oldPosition = newPosition;
    }
    if(newPosition < (oldPosition - ENC_OFFS)){
      Serial.print(newPosition);
      setVolDwn();
      oldPosition = newPosition;
    }
 }

/// Buttons functions
  if(Butt.isPressed()){
    Serial.println(F("The Butt is pressed"));
    doRpiPower();
  }

  if(Butt.isReleased())
    Serial.println(F("The Butt is released"));

  if(EncKey.isPressed()){
    Serial.println(F("The EncKey is pressed"));
  }

  if(EncKey.isReleased()){
    Serial.println(F("The EncKey is released"));
    if(!RpiIsReseting){
      setMute();
    }
  }

/// IR Remote functions
  // Check if we are currently receiving data
  //if (!IRLremote.receiving()) {
    // Run code that disables interrupts, such as some led strips
  //}

  if (IRLremote.receiving()) {
    digitalWrite(pinLed, LOW);
  }

  // Check if new IR protocol data is available
  if (IRLremote.available())
  {
    // Light Led
    digitalWrite(pinLed, LOW);

    // Get the new data from the remote
    auto data = IRLremote.read();

    // Print the protocol data
    Serial.print(F("Address: 0x"));
    Serial.println(data.address, HEX);
    Serial.print(F("Command: 0x"));
    Serial.println(data.command, HEX);
    Serial.println();

    if( (data.address == 0x41) && (data.command == 0xF35A4422)){ // IR Vol - samsung remote
      setVolDwn();
    }

    //if( (data.address == 0x5AA5) && (data.command == 0x42)){ // IR Vol + pionieer remote
    if( (data.address == 0x41) && (data.command == 0x546FAF22)){ // IR Vol + samsung remote
      setVolUp();
    }

    //if( (data.address == 0x5AA5) && (data.command == 0x56)){ // IR Mute samsung remote
    if( (data.address == 0x41) && (data.command == 0x6C296622)){ // IR Mute pionieer remote
      setMute();
    }

    //if( (data.address == 0x5AA5) && (data.command == 0x5A)){ // power on/off pionieer remote
    if( (data.address == 0x41) && (data.command == 0x35B52422)){ // power on/off samsung remote
      doRpiPower();
    }

    //if( (data.address == 0x5AA5) && (data.command == 0x5D)){ // reset pionieer remote
    if( (data.address == 0x41) && (data.command == 0x8B52D822)){ // reset samsung remote
      doRpiReset();
    }

  digitalWrite(pinLed, HIGH);

  }

}