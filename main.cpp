#include <Arduino.h>
#include <ESP32Servo.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

const char *ssid = "POLIZEI";
const char *password = "050919901902199509092018";

const char *ipAddress = "192.168.0.138";
const char *subnetMask = "255.255.255.0";
const char *gateway = "192.168.0.1";

const int ledPin = 2;  // GPIO pin connected to the built-in LED on the ESP32
const int FishServoPin = 5;      // GPIO pin connected to the servo
const int ShrimpsServoPin = 23;  // GPIO pin connected to the servo

// Define the GPIO pins for motor 1
const int in1 = 4;  // IN1
const int in2 = 6;  // IN2
int motorSpeed = 225;

AsyncWebServer server(80);
Servo myServoFish;
Servo myServoShrimps;

void feedFish();
void feedShrimps();
void motorCW(int speed);
void motorStop();
void co2();

void setup() {
  Serial.begin(115200);

  // Set static IP
  IPAddress staticIP = IPAddress(192, 168, 0, 138);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gatewayIP(192, 168, 0, 1);
  WiFi.config(staticIP, gatewayIP, subnet);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String response =
        "<div style='text-align: center; margin-top: 20px;'>"
        "<iframe src='http://192.168.0.3/webcam/?action=stream' width='1280' "
        "height='720' style='border: none;'></iframe>"
        "<br><br>"

        "<button style='margin: 20px; padding: 10px 20px; font-size: 28px; "
        "background-color: "
        "#4CAF50; color: white; border: none; border-radius: 5px; cursor: "
        "pointer;' onclick='feedFish()'>Fische</button>"

        "<button style='margin: 20px; padding: 10px 20px; font-size: 28px; "
        "background-color: "
        "#4CAF50; color: white; border: none; border-radius: 5px; cursor: "
        "pointer;' onclick='feedShrimps()'>Garnelen</button>"
        "<br>"

        "<button style='margin: 20px; padding: 10px 20px; font-size: 28px; "
        "background-color: "
        "#4287f5; color: white; border: none; border-radius: 5px; cursor: "
        "pointer;' onclick='co2()'>CO2</button>"

        "</div>"

        "<script>"
        "function feedFish(){fetch('/fish').then(response => "
        "response.text()).then(data => console.log(data));}"

        "function feedShrimps(){fetch('/shrimps').then(response => "
        "response.text()).then(data => console.log(data));}"

        "function co2(){fetch('/co2').then(response => "
        "response.text()).then(data => console.log(data));}"

        "</script>";
    request->send(200, "text/html", response);
  });

  server.on("/fish", HTTP_GET, [](AsyncWebServerRequest *request) {
    feedFish();
    request->send(200, "text/plain", "Fish Fed!");
  });

  server.on("/shrimps", HTTP_GET, [](AsyncWebServerRequest *request) {
    feedShrimps();
    request->send(200, "text/plain", "Shrimps Fed!");
  });

  server.on("/co2", HTTP_GET, [](AsyncWebServerRequest *request) {
    co2();
    request->send(200, "text/plain", "co2 dipped!");
  });

  server.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  myServoFish.attach(FishServoPin);
  myServoShrimps.attach(ShrimpsServoPin);
}

void loop() {}

void feedFish() {
  myServoFish.write(70);
  delay(1000);
  myServoFish.write(90);
}

void feedShrimps() {
  myServoShrimps.write(70);
  delay(1000);
  myServoShrimps.write(90);
}

void co2() {
  motorCW(motorSpeed);
  delay(500);
  motorStop();
}

void motorCW(int speed) {
  digitalWrite(in1, LOW);
  analogWrite(in2, speed);
}

void motorStop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}