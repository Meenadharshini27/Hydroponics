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
#define MAX_LED_DELAY 3600  // 3600 Secs => 60 Mins => 1 Hour.
int LedDelay = MAX_LED_DELAY;
bool LedStatus = false;

#define O2M 12 // D6
#define MAX_O2M_DELAY 300   // 300 Secs  =>  5 Mins.
int O2MDelay = MAX_O2M_DELAY;
bool O2MStatus = false;

const char* ssid = "ASSA";          // Replace with your WiFi SSID
const char* password = "creative";  // Replace with your WiFi Password
WiFiClientSecure client;            // Secure client for HTTPS
HTTPClient http;                    // Http Request object
int tds_observed = 0;               // Previously Measured TDS Reading
int water_level_observed = 0;       //

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
}

void saveValues(String id, String value) {
  String url = "https://hydroponics-md.azurewebsites.net/push/" + id + "/" + value;
  Serial.println(url);
  http.begin(client, url);
  int response = http.GET();
  http.end();
}

void loop() {
  Serial.println("Looping...");
  if (WiFi.status() == WL_CONNECTED) {
    HandleWaterLevel();   // 1. Check and Write Water Pump Status
    HandleO2Motor();      // 2. O2 Motor
    HandleGrowLED();      // 3. Grow LED 
    HandleTDS();          // 4. TDS
    Serial.println("Completed...");
  }
  else {
    Serial.println("Unable to Connect Wifi...");
  }
}

void HandleWaterLevel() {
  int level;
  level = digitalRead(FLT);
  if (water_level_observed != level) {
    digitalWrite(IN2, level == HIGH ? LOW : HIGH);
    saveValues("1", level == HIGH ? "OFF" : "ON");
    water_level_observed = level;
  }
}

void HandleGrowLED() {
  if (LedDelay == 1) {
    //Reached Timeout;
    digitalWrite(LED, LedStatus ? LOW : HIGH);
    saveValues("3", LedStatus ? "ON" : "OFF");
    LedStatus = !LedStatus;
    LedDelay = MAX_LED_DELAY;
  }
  else {
    LedDelay--;
  }
Serial.print("LED Delay: ");
Serial.println(LedDelay);
}

void HandleO2Motor() {
  if (O2MDelay == 1) {
    //Reached Timeout;
    digitalWrite(O2M, O2MStatus ? LOW : HIGH);
    saveValues("2", O2MStatus ? "ON" : "OFF");
    O2MStatus = !O2MStatus;
    O2MDelay = MAX_O2M_DELAY;
  }
  else {
    O2MDelay--;
  }
Serial.print("O2 Delay: ");
Serial.println(O2MDelay);
}

void HandleTDS() {
  float SensorValue = analogRead(A0) - 7.0;
  float ecValue = SensorValue * 3.3 / ADC_RESOLUTION;
  int tds_current = ecValue * 1000;

  if (tds_observed != tds_current) {
    saveValues("4", String(tds_current));
    tds_observed = tds_current;
  }

  if (tds_current < 300) {
    // Poor or Insufficient Nutirients, Drain Water in System and Refill
    digitalWrite(IN4, HIGH);
    // Turn ON Drain Pump for 2 Minutes...
    delay(2000 * 60);
    digitalWrite(IN4, LOW);
  }
}