#include "ESP32_Pinoo.h"
#include "addons/TokenHelper.h"

#define API_KEY "AIzaSyCH3ElArRaPAoPiIiJFp1_P_wfz7pIaIF8"
#define FIREBASE_PROJECT_ID "pinoo-bcc1d"                
#define USER_EMAIL ""     //USER EMAIL GOES HERE
#define USER_PASSWORD ""  //USER PASSWORD GOES HERE
#define PINOO_ID ""       //PINOO DEVICE ID GOES HERE

static Network *instance = NULL;

Network::Network(){
  instance = this;
}

WiFiManager wm;

void Network::resetCredentials(){
  wm.resetSettings();
  Serial.println("Restarting...");
  ESP.restart(); // builtin, safely restarts the ESP. 
}

void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WIFI CONNECTED! BUT WAIT FOR THE LOCAL IP ADDR");
}

void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.print("LOCAL IP ADDRESS: ");
  Serial.println(WiFi.localIP());
  instance->firebaseInit();
}

void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WIFI DISCONNECTED!");
  
  WiFi.mode(WIFI_STA); // Wi-Fi' PROVISIONUNG ICIN STATION MODA ALIYORUZ. EĞER TANIMLANMAZSA ESP AÇILIRKEN KENDİNİ "STA+AP"(STATION + ACCESS POINT) MODUNDA BAŞLATIYOR.
  wm.resetSettings(); //BU SATIR HER ESP32 SIFIRLANDIĞINDA TEKRARDAN PROVISION ADIMINI YAPABİLMEMİZ İÇİN. KOYMAZSAK BİR KEZ SORUYOR VE O ŞİFREYİ EZBERLİYOR.
    
  bool res;
  res = wm.autoConnect("Pinoo_ESP32","12345678"); // ESP'Yİ STATİON OLARAK KULLANIP CİHAZLARIN ONA BU İSİM VE ŞİFREYLE BAĞLANMASINI SAĞLIYORUZ.

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      Serial.println("Connected :)");
      Serial.println();
      Serial.print("Connected with IP: ");
      Serial.println(WiFi.localIP());
      Serial.println();
  }  
}

void FirestoreTokenStatusCallback(TokenInfo info){
  Serial.printf("Token Info: type = %s, status = %s\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
}

void Network::initWiFi(){
  WiFi.disconnect();
  WiFi.onEvent(WiFiEventConnected, ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiEventGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiEventDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  //WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  WiFi.mode(WIFI_STA); // Wi-Fi' PROVISIONUNG ICIN STATION MODA ALIYORUZ. EĞER TANIMLANMAZSA ESP AÇILIRKEN KENDİNİ "STA+AP"(STATION + ACCESS POINT) MODUNDA BAŞLATIYOR.
  //wm.resetSettings(); //BU SATIR HER ESP32 SIFIRLANDIĞINDA TEKRARDAN PROVISION ADIMINI YAPABİLMEMİZ İÇİN. KOYMAZSAK BİR KEZ SORUYOR VE O ŞİFREYİ EZBERLİYOR.
    
  bool res;
  res = wm.autoConnect("Pinoo_ESP32","12345678"); // ESP'Yİ STATİON OLARAK KULLANIP CİHAZLARIN ONA BU İSİM VE ŞİFREYLE BAĞLANMASINI SAĞLIYORUZ.

  if(!res) {
      Serial.println("Failed to connect :(");
      // ESP.restart();
  } 
  else {
      Serial.println("Connected with Provisioning :)");
      Serial.println();
  }  
}

void Network::firebaseInit(){
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = FirestoreTokenStatusCallback;

  Firebase.begin(&config, &auth);
}

bool Network::getpin1(){
  return pin1;
}
bool Network::getpin2(){
  return pin2;
}
bool Network::getpin3(){
  return pin3;
}
int Network::getpin7(){
  return pin7;
}
String Network::getpin10(){
  return pin10;
}

bool str_to_bool(String str){
  if (str =="true")
    return true;
  else if(str =="false")
    return false;
  else
    return NULL;
}

FirebaseJsonData jsonObj;
String my_time;

void Network::firestoreGet(){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "devices/" + String(PINOO_ID);
    
    FirebaseJson content;
    content.set("fields/isDeleted/booleanValue", String("false").c_str());

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "isDeleted")){
      
      FirebaseJson json2;
      json2.setJsonData(fbdo.payload().c_str());

      json2.get(jsonObj, "fields/pin1/booleanValue", true);
      pin1 = str_to_bool(jsonObj.stringValue);

      json2.get(jsonObj, "fields/pin2/booleanValue", true);
      pin2 = str_to_bool(jsonObj.stringValue);

      json2.get(jsonObj, "fields/pin3/booleanValue", true);
      pin3 = str_to_bool(jsonObj.stringValue);

      json2.get(jsonObj, "fields/pin7/integerValue", true);
      pin7 = jsonObj.stringValue.toInt();

      json2.get(jsonObj, "fields/pin10/stringValue", true);
      pin10 = jsonObj.stringValue;

    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}


void Network::firestoreDataUpdate(int pin8, int pin9, bool pin4_but){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "devices/" + String(PINOO_ID);
    String documentPath_data = documentPath + "/data";

    FirebaseJson content;

    content.set("fields/pin4/booleanValue", String(pin4_but).c_str());
    content.set("fields/pin8/doubleValue", String(pin8).c_str());
    content.set("fields/pin9/doubleValue", String(pin9).c_str());
    content.set("fields/isDeleted/booleanValue", String("false").c_str());
    
    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "pin4,pin8,pin9,isDeleted")){
      FirebaseJson json2;
      json2.setJsonData(fbdo.payload().c_str());

      json2.get(jsonObj, "updateTime", true);
      my_time = jsonObj.stringValue;
      json2.set("fields/createdAt/timestampValue", my_time.c_str());

      json2.remove("name");
      json2.remove("updateTime");
      json2.remove("createTime");

      if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath_data.c_str(), json2.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      }else{
        Serial.println(fbdo.errorReason());
      }
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}

void Network::firestoreDataUpdate(double temp, double humi){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "devices/" + String(PINOO_ID);
    String documentPath_data = documentPath + "/data";

    FirebaseJson content;

    content.set("fields/pin6/doubleValue", String(temp).c_str());
    content.set("fields/pin8/doubleValue", String(humi).c_str());
    content.set("fields/isDeleted/booleanValue", String("false").c_str());

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "pin6,pin8,isDeleted")){
      
      FirebaseJson json2;
      json2.setJsonData(fbdo.payload().c_str());

      json2.get(jsonObj, "updateTime", true);
      my_time = jsonObj.stringValue;
      json2.set("fields/createdAt/timestampValue", my_time.c_str());

      json2.remove("name");
      json2.remove("updateTime");
      json2.remove("createTime");

      if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath_data.c_str(), json2.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      }else{
        Serial.println(fbdo.errorReason());
      }
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}

void Network::firestoreUpdatePin4(bool pin4_but){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "devices/" + String(PINOO_ID);
    String documentPath_data = documentPath + "/data";

    FirebaseJson content;
    content.set("fields/pin4/booleanValue", String(pin4_but).c_str());
    content.set("fields/isDeleted/booleanValue", String("false").c_str());

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "pin4,isDeleted")){
      FirebaseJson json2;
      json2.setJsonData(fbdo.payload().c_str());

      json2.get(jsonObj, "updateTime", true);
      my_time = jsonObj.stringValue;
      json2.set("fields/createdAt/timestampValue", my_time.c_str());

      json2.remove("name");
      json2.remove("updateTime");
      json2.remove("createTime");

      json2.get(jsonObj, "fields/pin1/booleanValue", true);
      pin1 = str_to_bool(jsonObj.stringValue);

      json2.get(jsonObj, "fields/pin2/booleanValue", true);
      pin2 = str_to_bool(jsonObj.stringValue);

      json2.get(jsonObj, "fields/pin3/booleanValue", true);
      pin3 = str_to_bool(jsonObj.stringValue);

      json2.get(jsonObj, "fields/pin7/integerValue", true);
      pin7 = jsonObj.stringValue.toInt();

      json2.get(jsonObj, "fields/pin10/stringValue", true);
      pin10 = jsonObj.stringValue;

      if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath_data.c_str(), json2.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      }else{
        Serial.println(fbdo.errorReason());
      }
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}
void Network::firestoreUpdatePin5(bool pin5_but){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "devices/" + String(PINOO_ID);
    String documentPath_data = documentPath + "/data";

    FirebaseJson content;
    content.set("fields/pin5/booleanValue", String(pin5_but).c_str());
    content.set("fields/isDeleted/booleanValue", String("false").c_str());

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "pin5,isDeleted")){
      FirebaseJson json2;
      json2.setJsonData(fbdo.payload().c_str());

      json2.get(jsonObj, "updateTime", true);
      my_time = jsonObj.stringValue;
      json2.set("fields/createdAt/timestampValue", my_time.c_str());

      json2.remove("name");
      json2.remove("updateTime");
      json2.remove("createTime");

      if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath_data.c_str(), json2.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      }else{
        Serial.println(fbdo.errorReason());
      }
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}
void Network::firestoreUpdatePin6(double pin6){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "devices/" + String(PINOO_ID);
    String documentPath_data = documentPath + "/data";

    FirebaseJson content;
    content.set("fields/pin6/doubleValue", String(pin6).c_str());
    content.set("fields/isDeleted/booleanValue", String("false").c_str());

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "pin6,isDeleted")){
      
      FirebaseJson json2;
      json2.setJsonData(fbdo.payload().c_str());

      json2.get(jsonObj, "updateTime", true);
      my_time = jsonObj.stringValue;
      json2.set("fields/createdAt/timestampValue", my_time.c_str());

      json2.remove("name");
      json2.remove("updateTime");
      json2.remove("createTime");

      my_time = jsonObj.stringValue;
      Serial.println(my_time);
      json2.set("fields/createdAt/timestampValue", my_time.c_str());

      if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath_data.c_str(), json2.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      }else{
        Serial.println(fbdo.errorReason());
      }
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}
void Network::firestoreUpdatePin8(double pin8){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "devices/" + String(PINOO_ID);
    String documentPath_data = documentPath + "/data";

    FirebaseJson content;
    content.set("fields/pin8/doubleValue", String(pin8).c_str());
    content.set("fields/isDeleted/booleanValue", String("false").c_str());

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "pin8,isDeleted")){
      
      FirebaseJson json2;
      json2.setJsonData(fbdo.payload().c_str());

      FirebaseJsonData jsonObj;
      json2.get(jsonObj, "updateTime", true);
      my_time = jsonObj.stringValue;
      json2.set("fields/createdAt/timestampValue", my_time.c_str());

      json2.remove("name");
      json2.remove("updateTime");
      json2.remove("createTime");

      if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath_data.c_str(), json2.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      }else{
        Serial.println(fbdo.errorReason());
      }
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}
void Network::firestoreUpdatePin9(double pin9){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "devices/" + String(PINOO_ID);
    String documentPath_data = documentPath + "/data";

    FirebaseJson content;
    content.set("fields/pin9/doubleValue", String(pin9).c_str());
    content.set("fields/isDeleted/booleanValue", String("false").c_str());

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "pin9,isDeleted")){
      
      FirebaseJson json2;
      json2.setJsonData(fbdo.payload().c_str());

      FirebaseJsonData jsonObj;
      json2.get(jsonObj, "updateTime", true);
      my_time = jsonObj.stringValue;
      json2.set("fields/createdAt/timestampValue", my_time.c_str());

      json2.remove("name");
      json2.remove("updateTime");
      json2.remove("createTime");

      if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath_data.c_str(), json2.raw())) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      }else{
        Serial.println(fbdo.errorReason());
      }
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}
















