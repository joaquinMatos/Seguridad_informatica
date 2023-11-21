#ifndef Pinoo_ESP32_
#define Pinoo_ESP32_

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <WiFiManager.h>
#include <EasyButton.h>

class Network{
private:
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;

  void firebaseInit();
  friend void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void FirestoreTokenStatusCallback(TokenInfo info);

  bool pin1 = NULL;
  bool pin2 = NULL;
  bool pin3 = NULL;
  int pin7;
  String pin10 = "";

public:
  Network();
  void initWiFi();
  void resetCredentials();
  void firestoreGet();

  bool getpin1();
  bool getpin2();
  bool getpin3();
  int getpin7();
  String getpin10();
  
  void firestoreDataUpdate(double temp, double humi);
  void firestoreDataUpdate(int pin8, int pin9, bool pin4_but);

  void firestoreUpdatePin4(bool pin4_but);
  void firestoreUpdatePin5(bool pin5_but);
  void firestoreUpdatePin6(double pin6);
  void firestoreUpdatePin8(double pin8);
  void firestoreUpdatePin9(double pin9);
};


#endif