#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

const char* ssid = "Module2_AP";  // Replace with the SSID of Module 2's Access Point
const char* password = "password";  // Replace with the password of Module 2's Access Point

const char* serverName = "http://192.168.4.1/getstate";


Servo myservo;
int servoPin = 4;

void setup() {
  Serial.begin(115200);
  myservo.attach(servoPin);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin(serverName);

    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      String payload = http.getString();
      if (payload == "open") {
        myservo.writeMicroseconds(1900);
        Serial.println("Set servo to OPEN position (1900)");
      } else if (payload == "close") {
        myservo.writeMicroseconds(1100);
        Serial.println("Set servo to CLOSE position (1100)");
      } else {
        Serial.println("Invalid payload");
      }
    } else {
      Serial.print("HTTP request failed. Error: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Disconnected from WiFi. Reconnecting...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
  }

  delay(1000);
}
