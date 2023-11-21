#include "ESP32_Pinoo.h"      

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

Network *network;

int buttonPin = 25;  /* WE DEFINED A PIN NUMBER FOR THE BUTTON */
bool button_send = false;/* WE DEFINED A BOOL FOR THE BUTTON VALUE TO SEND */

void initNetwork(){ /* OUR LIBRARY CONNECTS TO WI-FI WITH THE CODES INSIDE  */
  network = new Network();
  network->initWiFi();
}

#define FLASH_PIN 0 /* WE CREATE A RESET BUTTON. WE PREFERED THE BOOT BUTTON ON THE CARD.   */
EasyButton resetButton(FLASH_PIN);
#define RESET_DURATION 1000  /* WHEN YOU HOLD IT FOR 1 SECOND IT WILL RESET THE CARD.   */
void reset() {  /* FUNCTION THAT WILL DELETE THE WI-FI SSID AND PASSWORD WHEN PRESSING THE RESET KEY FOR 1 SECOND.   */
  Serial.println("FLASH was held for " + String(RESET_DURATION, DEC) + "ms.");
  Serial.println("Erasing stored WiFi credentials.");
 
  network->resetCredentials(); /* clear WiFi creds.   */
}

bool writeButton() { /* THE FUNCTION THAT WRITES THE BUTTON DATA. IF IT CAN WRITE, RETURNS TRUE.  */
  button_send = digitalRead(buttonPin);
  Serial.println("Sending Button Value: " + String(button_send));
  network->firestoreUpdatePin4(button_send);
  return true;
}

void setup(){
  Serial.begin(115200);
  Serial.println();
  resetButton.begin();/* CHECK IF THE RESET BUTTON IS PRESSED.  */
  resetButton.onPressedFor(RESET_DURATION, reset);/* WHEN WE HOLD IT FOR 1 SECOND RESET FUNCTION WILL BE CALLED.  */
  
  pinMode(buttonPin, INPUT);
  
  initNetwork();
}

void loop() {
  resetButton.read();/* WE READ THE RESET BUTTON AND CHECK IF IT IS PRESSED.  */
  writeButton();
  //delay(1000);/* WE ARE WAITING 1 SECOND FOR THE SYSTEM TO SETTLE */
}
