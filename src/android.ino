#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define TdsSensorPin A0
#define ADC_RESOLUTION 1024.0 
#define TdsFactor 0.5  // Calibration factor
#define LED 16 // D0
#define IN1 5  // D1
#define IN2 4  // D2
#define IN3 0  // D3
#define IN4 2  // D4
#define FLT 14 // D5
#define O2M 12 // D6

const char* ssid = "srmp jio";  // Replace with your WiFi SSID
const char* password = "7598150639";  // Replace with your WiFi Password
const char* serverUrl = "http://192.168.29.157:3000/push/4/";  // API Endpoint

WiFiClient client;  // Create a WiFiClient object
HTTPClient http;

String requestUrl = "";

int response;
int counter = 0;
bool status = false;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(O2M, OUTPUT);
    pinMode(FLT, INPUT);
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) { 
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

        // HTTP request
        requestUrl = String(serverUrl) + String(tds);
        http.begin(client, requestUrl);  // Use WiFiClient in http.begin()        
        response = http.GET();
        Serial.print("HTTP Response Code: ");
        Serial.println(response);
        http.end();  // Close connection


        if (digitalRead(FLT) == HIGH) {
          HTTPClient http;
          requestUrl = String("http://192.168.29.157:3000/push/1/ON") + String(tds);
          http.begin(client, requestUrl);  // Use WiFiClient in http.begin()        
          response = http.GET();
          Serial.print("HTTP Response Code: ");
          Serial.println(response);
          http.end();  // Close connection
          
          digitalWrite(IN1, HIGH);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, HIGH);
          digitalWrite(IN4, LOW);
        }
        else {
          HTTPClient http;
          requestUrl = String("http://192.168.29.157:3000/push/1/OFF") + String(tds);
          http.begin(client, requestUrl);  // Use WiFiClient in http.begin()        
          response = http.GET();
          Serial.print("HTTP Response Code: ");
          Serial.println(response);
          http.end();  // Close connection
          digitalWrite(IN1, LOW);
          digitalWrite(IN2, LOW);
          digitalWrite(IN3, LOW);
          digitalWrite(IN4, LOW);
        }

        if (counter % 1 == 0 && status == true) {
          counter = 0;
          status = false;
          requestUrl = String("http://192.168.29.157:3000/push/2/ON");
          http.begin(client, requestUrl);  // Use WiFiClient in http.begin()        
          response = http.GET();
          Serial.print("HTTP Response Code: ");
          Serial.println(response);
          http.end();  // Close connection

          requestUrl = String("http://192.168.29.157:3000/push/3/ON");
          http.begin(client, requestUrl);  // Use WiFiClient in http.begin()        
          response = http.GET();
          Serial.print("HTTP Response Code: ");
          Serial.println(response);
          http.end();  // Close connection

          digitalWrite(LED, HIGH);
          digitalWrite(O2M, HIGH);
        }
        else if (counter % 60 == 0 && status == false) {
          counter = 0;
          status = true;
          requestUrl = String("http://192.168.29.157:3000/push/2/OFF");
          http.begin(client, requestUrl);  // Use WiFiClient in http.begin()        
          response = http.GET();
          Serial.print("HTTP Response Code: ");
          Serial.println(response);
          http.end();  // Close connection

          requestUrl = String("http://192.168.29.157:3000/push/3/OFF");
          http.begin(client, requestUrl);  // Use WiFiClient in http.begin()        
          response = http.GET();
          Serial.print("HTTP Response Code: ");
          Serial.println(response);
          http.end();  // Close connection

          digitalWrite(LED, LOW);
          digitalWrite(O2M, LOW);
        }
    } else {
        Serial.println("Unable to Connect to WiFi...");
    }
    counter++;
    delay(40000);  // Wait for 1 Minute
}
