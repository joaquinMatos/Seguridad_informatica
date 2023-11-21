/*  es una biblioteca fundamental de Arduino que proporciona funciones (constantes,tipo de datos byte*/
#include <Arduino.h>
/* permite una conexion wifi   */
#include <WiFi.h>

/*  permite la conexión y el acceso a la plataforma de Firebase */
#include <Firebase_ESP_Client.h>
/*  proporcionar una forma segura de almacenar y recuperar tokens de autorización de Firebase. */
#include "addons/TokenHelper.h"
/*  la interacción con la base de datos en tiempo real  */
#include "addons/RTDBHelper.h"

//#define WIFI_SSID "SM51"
//#define WIFI_PASSWORD "sm51holamundo"
//#define WIFI_SSID "DIT-IoT"
//#define WIFI_PASSWORD "DITCisco98"

#define WIFI_SSID "Internet RV_11AB50"
#define WIFI_PASSWORD "000011AB50"
//Larry
#define DATABASE_URL "https://casa-a1165-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyB0Oi_U0AZ70wQ47SEmZg3Xnd3k9WZq3Qs"
//Oviedo
//#define DATABASE_URL "https://jardin-oviedo-default-rtdb.firebaseio.com"
//#define API_KEY "AIzaSyAargj5ZWLDU46ENIA3xsc1ZpZEDartyVM"

/* Esta variable se utiliza para almacenar la respuesta que se obtiene al enviar una solicitud a la base de datos en tiempo real de Firebase. */
FirebaseData fbdo;
FirebaseAuth auth;
/* contiene url, token, ID proyecto */
FirebaseConfig config;
int posicion = 0;
int led3 = 18;
int led4 = 19;
int led5 = 21;
int valor_LED1;
int valor_LED2;
int valor_LED3;
bool buzzer;
int buzz = 5;
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
const int botonPin = 22;
bool valB; 
bool proceso = true;
bool cambioProceso = false; 
int trigger = 15; 
int echo = 2;
int tiempo;
int distancia;
void wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado");
  Serial.println(WiFi.localIP());
  Serial.println();
}
void firebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
void insertar() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

  if (Firebase.RTDB.setDouble(&fbdo, "Estacionamiento/distancia", distancia)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      Serial.println("Valor: ");
      printResult(fbdo);
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  if (Firebase.RTDB.setBool(&fbdo, "Estacionamiento/boton", valB)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      Serial.println("Valor: ");
      printResult(fbdo);
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
void obtener() {
  if (Firebase.RTDB.getInt(&fbdo, "Estacionamiento/buzzer")) {
    if (fbdo.dataType() == "boolean") {
      buzzer = fbdo.boolData();
      Serial.print("valor del Bizzer: ");
      Serial.println(buzzer);
      }
      } else {
        Serial.println(fbdo.errorReason()); 
        }
  if (Firebase.RTDB.getInt(&fbdo, "Estacionamiento/LEDrojo")) {
    if (fbdo.dataType() == "boolean") {
      valor_LED1 = fbdo.boolData();
      Serial.print("valor del led1: ");
      Serial.println(valor_LED1);
      }
      }else{
        Serial.println(fbdo.errorReason());
        }
  if (Firebase.RTDB.getInt(&fbdo, "Estacionamiento/LEDamarillo")) {
    if (fbdo.dataType() == "boolean") {
      valor_LED2 = fbdo.boolData();
      Serial.print("valor del led2: ");
      Serial.println(valor_LED2);  
      }
      }else {
        Serial.println(fbdo.errorReason());
        }
  if (Firebase.RTDB.getInt(&fbdo, "Estacionamiento/LEDverde")) {
    if (fbdo.dataType() == "boolean") {
      valor_LED3 = fbdo.boolData();
      Serial.print("valor del led3: ");
      Serial.println(valor_LED3); 
      }
      }else {
        Serial.println(fbdo.errorReason());
        }
}
void distan(){
        digitalWrite(trigger, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigger, LOW);
        tiempo = pulseIn(echo, HIGH);
        distancia = tiempo / 59;
        Firebase.RTDB.setDouble(&fbdo, "Estacionamiento/distancia", distancia);
}
void setup() {
  Serial.begin(115200);
  pinMode(botonPin, INPUT_PULLUP);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  // pin servo 
  miServo.attach(23);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  wifi();
  firebase();
}
void loop() {
  distan();
  obtener();
  if (proceso) {
    proceso1();
    } else {
      proceso2();
      }
  if (digitalRead(botonPin) == LOW && !cambioProceso) {
    proceso = !proceso;
    cambioProceso = true;
    }
  if (digitalRead(botonPin) == HIGH && cambioProceso) {
    cambioProceso = false;
    }
  insertar();
}
void proceso1() {
  Serial.println("Pasele...Pues..");
  miServo.write(90);
  noTone(buzz);
  valB = true;
  Firebase.RTDB.setBool(&fbdo, "Estacionamiento/boton", valB);
}
void proceso2() {
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  tiempo = pulseIn(echo, HIGH);
  distancia = tiempo / 59;
  Serial.print("La distancia es: ");
  Serial.print(distancia);
  Serial.println(" cm");
  miServo.write(0);
  delay(250);
  valB = false;
  Firebase.RTDB.setBool(&fbdo, "Estacionamiento/boton", valB);

  if (distancia > 14 && distancia < 18) {
      noTone(buzz);
      digitalWrite(led3, HIGH);
      if (valor_LED1 == 0) {
      digitalWrite(led3, LOW);
      valor_LED1 = 1;
      } else {
      valor_LED1 = 1;
      }
      delay(1500);
        distan();
      }
  if (distancia < 15) {
      valor_LED1 = 2;
      digitalWrite(led3, LOW);
      miServo.write(0);
      distan();
      
    }
  if (distancia > 14 && distancia < 18 && valor_LED1 == 1) {
          noTone(buzz);
          digitalWrite(led4, HIGH);
         if (valor_LED2 == 0) {
        digitalWrite(led4, LOW);
        valor_LED2 = 1;
        } else {
      valor_LED2 = 1;
    }
    delay(1500);
          distan();
    }
  if (distancia < 15) {
        valor_LED1 = 2;
        digitalWrite(led3, LOW);
        miServo.write(0);
      distan();
      }
  if (distancia > 14 && distancia < 18 && valor_LED1 == 1 && valor_LED2 == 1) {
          noTone(buzz);
          digitalWrite(led5, HIGH);
          if (valor_LED3 == 0) {
         digitalWrite(led5, LOW);
        valor_LED3 = 1;
       } else {
      valor_LED3 = 1;
     }
    delay(1500);
          distan();
    }
  if (distancia < 15) {
          valor_LED1 = 2;
          digitalWrite(led3, LOW);
          miServo.write(0);
          distan();
          }
  if (distancia > 14 && distancia < 18 && valor_LED1 == 1 && valor_LED2 == 1 && valor_LED3 == 1) {
            while (distancia > 14 && distancia < 18){
            miServo.write(90);
            if(distancia > 14 && distancia > 18){
            delay(2000);
           }
          distan();
          delay(3000);
          }

    } else {
    valor_LED1 = 2;
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);
    miServo.write(0);
    }

  if (distancia < 15) {
      tone(buzz, 500);
      if (buzzer == 0){
      noTone(buzz);
    }
    miServo.write(0);
    digitalWrite(led3, HIGH);
    if (valor_LED1 == 0) {
    digitalWrite(led3, LOW);
    }
    digitalWrite(led4, HIGH);
    if (valor_LED2 == 0) {
    digitalWrite(led4, LOW);
    }
    digitalWrite(led5, HIGH);
    if (valor_LED3 == 0) {
    digitalWrite(led5, LOW);
    }
    delay(500);
    digitalWrite(led3, LOW);
    if (valor_LED2 == 0) {
    digitalWrite(led4, LOW);
    } 
    digitalWrite(led4, LOW);
    if (valor_LED2 == 0) {
    digitalWrite(led4, LOW);
    }
    digitalWrite(led5, LOW);
    if (valor_LED2 == 0) {
    digitalWrite(led4, LOW);
    }
    delay(500);
    } else {
    noTone(buzz);
    }  
  /*if (distancia > 25) {
  Firebase.RTDB.setBool(&fbdo, "Estacionamiento/LEDrojo", true);
  Firebase.RTDB.setBool(&fbdo, "Estacionamiento/LEDamarillo", true);
  Firebase.RTDB.setBool(&fbdo, "Estacionamiento/LEDverde", true);
  Firebase.RTDB.setBool(&fbdo, "Estacionamiento/buzzer", true);
  }*/
}