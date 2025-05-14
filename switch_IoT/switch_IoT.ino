#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "B24.04";          // WiFi SSID
const char* password = "baycuruoi";   // WiFi Password
const char* server = "http://192.168.1.128:8000"; // FastAPI Server IP

const char* mqtt_server = "anhpn.ddns.net"; // MQTT Broker IP
const char* mqtt_topic = "iot_lab/switch_001";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = D8;  // LED connected to D8
const int LED_PIN = D8;  // LED connected to D8
const int switchId = 1; // Switch ID

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW); // Default OFF

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");

    // Fetch switch status from API
    fetchSwitchStatus();

    // Connect to MQTT broker
    client.setServer(mqtt_server, 1883);
    client.setCallback(mqttCallback);
    connectToMQTT();
}

void fetchSwitchStatus() {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        String url = String(server) + "/get_switch_status/?switch_id=" + String(switchId);

        Serial.print("Requesting: ");
        Serial.println(url);
        
        http.begin(client, url);
        int httpCode = http.GET();

        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println("Response: " + payload);

            // Parse JSON response
            StaticJsonDocument<200> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                int status = doc["status"];
                digitalWrite(ledPin, status ? HIGH : LOW);
                Serial.println(status ? "LED ON" : "LED OFF");
            } else {
                Serial.println("JSON Parsing Error");
            }
        } else {
            Serial.print("HTTP Request Failed, Code: ");
            Serial.println(httpCode);
        }

        http.end();
    }
}

void connectToMQTT() {
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP8266Client")) {
            Serial.println("Connected!");
            client.subscribe(mqtt_topic);
        } else {
            Serial.print("Failed, retrying in 5s...");
            delay(5000);
        }
    }
}


void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received: ");
    Serial.println(topic);

    payload[length] = '\0';  // Ensure it's null-terminated
    String message = String((char*)payload);
    Serial.println("Payload: " + message);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (!error) {
        int switch_id = doc["switch_id"];
        int status = doc["status"];

        if (switch_id == 1) {
            digitalWrite(LED_PIN, status ? HIGH : LOW);
            Serial.print("LED turned ");
            Serial.println(status ? "ON" : "OFF");
        }
    } else {
        Serial.println("JSON Parsing Failed!");
    }
}


void loop() {
    if (!client.connected()) {
        connectToMQTT();
    }
    client.loop(); // Keep MQTT connection alive
}
