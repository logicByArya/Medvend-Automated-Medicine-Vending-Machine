#include <ESP32Servo.h>
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>


// Servo control pins
int servoPin1 = 13;  // Servo 1
int servoPin2 = 12;  // Servo 2
int servoPin3 = 14;  // Servo 3

Servo servo1;  // Create Servo object for Servo 1
Servo servo2;  // Create Servo object for Servo 2
Servo servo3;  // Create Servo object for Servo 3

// int closeAngle = 80;
// int openAngle = 50;

int closeAngle = 75;
int openAngle = 20;



// Wi-Fi and Firebase configuration
#define WIFI_SSID "Aryahhh"
#define WIFI_PASSWORD "arya@123" 
#define API_KEY "AIzaSyA4OH86Xq0FH2szZ8qnQRNr2OHP7x1MLDI"
#define DATABASE_URL "https://medvent-4b331-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define USER_EMAIL "device@gmail.com"
#define USER_PASSWORD "12345678"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
String uid;
FirebaseData stream;

void streamCallback(StreamData data) {
  Serial.println("NEW DATA!");
  String p = data.dataPath();
  Serial.println(p);
  printResult(data);

  FirebaseJson jVal = data.jsonObject();
  FirebaseJsonData Amoxicillin;
  FirebaseJsonData Ibuprofen;
  FirebaseJsonData Paracetamol;

  jVal.get(Amoxicillin, "Amoxicillin");
  jVal.get(Ibuprofen, "Ibuprofen");
  jVal.get(Paracetamol, "Paracetamol");

  if (Amoxicillin.success) {
    Serial.println("Success data med1");
    int value = Amoxicillin.to<int>();
    Serial.printf("Amoxicillin quantity: %d\n", value);
    for(int i =0 ; i < value ; i++)
    {
      dispmed1(i);
    }
  }
    if (Ibuprofen.success) {
    Serial.println("Success data med1");
    int value = Ibuprofen.to<int>();
    Serial.printf("IBUprofen quantity: %d\n", value);
    for(int i =0 ; i < value ; i++)
    {
      dispmed2(i);
    }
  }
    if (Paracetamol.success) {
    Serial.println("Success data med1");
    int value = Paracetamol.to<int>();
    Serial.printf("paracetamol quantity: %d\n", value);
    for(int i =0 ; i < value ; i++)
    {
      dispmed3(i);
    }
  }
}
// void streamCallback(StreamData data) {
//   Serial.println("NEW DATA!");
//   String p = data.dataPath();
//   Serial.println(p);
//   printResult(data);

//   FirebaseJson jVal = data.jsonObject();
//   FirebaseJsonData quantity;

//   // Retrieve quantity for Amoxicillin
//   jVal.get(quantity, "Amoxicillin/quantity");
//   if (quantity.success) {
//     int amoxicillinQty = quantity.to<int>();
//     Serial.printf("Amoxicillin quantity: %d\n", amoxicillinQty);
//     for(int i = 0; i < amoxicillinQty; i++) {
//       dispmed1(i);
//     }
//   }

//   // Retrieve quantity for Ibuprofen
//   jVal.get(quantity, "Ibuprofen/quantity");
//   if (quantity.success) {
//     int ibuprofenQty = quantity.to<int>();
//     Serial.printf("Ibuprofen quantity: %d\n", ibuprofenQty);
//     for(int i = 0; i < ibuprofenQty; i++) {
//       dispmed2(i);
//     }
//   }

//   // Retrieve quantity for Paracetamol
//   jVal.get(quantity, "Paracetamol/quantity");
//   if (quantity.success) {
//     int paracetamolQty = quantity.to<int>();
//     Serial.printf("Paracetamol quantity: %d\n", paracetamolQty);
//     for(int i = 0; i < paracetamolQty; i++) {
//       dispmed3(i);
//     }
//   }
// }


void streamTimeoutCallback(bool timeout) {
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void setup() {

  // Attach servos to corresponding pins
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
   servo1.write(90);
  servo2.write(75);
  servo3.write(110);
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Firebase setup
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;

  // Getting the user UID
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Stream setup
  if (!Firebase.beginStream(stream, "devices/" + uid + "/data"))
    Serial.printf("stream begin error, %s\n\n", stream.errorReason().c_str());

  Firebase.setStreamCallback(stream, streamCallback, streamTimeoutCallback);
}


// void dispmed1() {
// servo1.write(openAngle);
// delay(1000);
// servo1.write(closeAngle);
// delay(1000); 
// }

// void dispmed2() {
// servo2.write(openAngle);
// delay(1000);
// servo2.write(closeAngle);
// delay(1000); 
// }

// void dispmed3() {
// servo3.write(openAngle);
// delay(1000);
// servo3.write(closeAngle);
// delay(1000);    
// }

void dispmed1(int num) {
  Serial.printf("%d. Dispensing Amoxicillin\n", num);
  servo1.write(35);
  delay(1000);
  servo1.write(90);
  delay(1000);
}

void dispmed2(int num) {
  Serial.printf("%d. Dispensing Ibuprofen\n", num);
  servo2.write(openAngle);
  delay(1000);
  servo2.write(closeAngle);
  delay(1000);
}

void dispmed3(int num) {
  Serial.printf("%d. Dispensing Paracetamol\n", num);
  servo3.write(40);
  delay(1000);
  servo3.write(110);
  delay(1000);
}
void loop() {
  // Main loop
}


