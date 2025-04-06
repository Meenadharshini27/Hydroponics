#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#define TdsSensorPin A0
#define ADC_RESOLUTION 1024.0 
#define TdsFactor 0.5  // Calibration factor

#define IN1 5  // D1
#define IN3 5  // D1
#define IN2 4  // D2
#define IN4 0  // D3
#define FLT 2  // D4

#define LED 14 // D5
#define O2M 12 // D6

int O2M_INTERVAL = 2 * 60 * 1000; // 2 Mins
int LED_INTERVAL = 3 * 60 * 1000; // 3 Mins
int lastO2MTime = 0;
int lastLEDTime = 0;
bool ledStatus = true;
bool o2mStatus = true;

const char* ssid = "srmp jio";  // Replace with your WiFi SSID
const char* password = "7598150639";  // Replace with your WiFi Password
const char* serverUrl = "https://hydroponics-md.azurewebsites.net/push/2/OFF";  // Azure API Base URL

WiFiClientSecure client;  // Secure client for HTTPS
HTTPClient http;

unsigned long previousMillis = 0;
const long interval = 40000; // 40 seconds

bool status = false;

void setup() {
    Serial.begin(115200);
    
    Serial.print("Connecting to WiFi...");
    unsigned long wifiStartTime = millis();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (millis() - wifiStartTime > 20000) { // Timeout after 20 seconds
            Serial.println("\nWiFi Connection Failed!");
            return;
        }
    }
    Serial.println("\nConnected to WiFi!");

    // // Disable SSL certificate validation (useful if Azure uses self-signed certificates)
    client.setInsecure();

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(O2M, OUTPUT);
    pinMode(FLT, INPUT);
    digitalWrite(IN1, LOW);
    
    
    digitalWrite(O2M, HIGH);
    digitalWrite(LED, HIGH);
    
    saveValues("2",o2mStatus? "OFF" : "ON");
    saveValues("3",ledStatus? "OFF" : "ON");


}

void saveValues(String id, String value) {
  String url = "https://hydroponics-md.azurewebsites.net/push/" + id + "/" + value;
  http.begin(client, url);
  Serial.print("Connecting to: ");
  Serial.println(url);
  int response = http.GET();
  Serial.print("HTTP Response Code: ");
  Serial.println(response);
  http.end();
}

void loop() {
  unsigned long start = millis();

  if (WiFi.status() == WL_CONNECTED) {
    // 1. Check and Write Water Pump Status
    int level;
    level = digitalRead(FLT);
    if (level == HIGH) {
      Serial.println("Water Level: HIGH");
      // Water Refilled, Switch Off Pump
      digitalWrite(IN2, LOW);
      saveValues("1", "OFF");
    }
    else {
      Serial.println("Water Level: LOW");
      // Low Water in System, Turn ON Pump
      saveValues("1", "ON");
      digitalWrite(IN2, HIGH);
    }


    unsigned long currentMillis = millis();

    // 2. O2 Motor
    if (currentMillis - lastO2MTime >= O2M_INTERVAL) {
        lastO2MTime = currentMillis;  // Reset timer
        o2mStatus = !o2mStatus;  // Toggle state
        digitalWrite(O2M, o2mStatus ? HIGH : LOW);  // Apply new state
        Serial.print("O2 Motor is now: ");
        Serial.println(o2mStatus ? "ON" : "OFF");
        saveValues("2",o2mStatus? "OFF" : "ON");
    }

    // **Grow LED Logic**
    if (currentMillis - lastLEDTime >= LED_INTERVAL) {
        lastLEDTime = currentMillis;  // Reset timer
        ledStatus = !ledStatus;  // Toggle state
        digitalWrite(LED, ledStatus ? HIGH : LOW);  // Apply new state
        Serial.print("Grow LED is now: ");
        Serial.println(ledStatus ? "ON" : "OFF");
        saveValues("3", ledStatus? "OFF" : "ON");
    }

    // 4. Measure TDS and Log Values - Check Plant Nutrients
    float SensorValue = analogRead(A0) - 7.0;
    float ecValue = SensorValue * 3.3 / ADC_RESOLUTION;
    int tds = ecValue * 1000;

    Serial.print("Sensor Value: ");
    Serial.print(SensorValue);
    Serial.print("V;   EC Value: ");
    Serial.print(ecValue);
    Serial.print("V;   TDS Value: ");
    Serial.print(tds);
    Serial.println(" PPM");
    saveValues("4", String(tds));
    if (tds < 300) {
      // Poor or Insufficient Nutirients, Drain Water in System and Refill
      digitalWrite(IN4, HIGH);
    }
  }
  else {
    Serial.println("Unable to Connect Wifi...");
  }
  unsigned long end = millis();
  Serial.print("Start: ");
  Serial.print(start);
  Serial.print("End: ");
  Serial.print(end);
  Serial.print("Time Taken: ");
  Serial.println(end - start);

  // if ((end - start) < 5000) {
  //   delay(5000 - (end - start));
  // }
}

