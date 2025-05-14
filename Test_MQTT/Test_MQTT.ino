#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "B24.04";
const char* password = "baycuruoi";

// MQTT server details
// const char* mqtt_server = "192.168.1.123";
const char* mqtt_server = "anhpn.ddns.net";
const int mqtt_port = 1883; // Default MQTT port
const char* mqtt_topic = "creator/onoff"; // Topic to subscribe to

// GPIO pin to control
const int power_pin = 15;

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Function to handle received MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i]; // Convert payload to a string
  }
  Serial.println(message);

  // Check for specific message and control the pin
  if (message == "PC turn on") {
    Serial.println("Turning on power pin for 1 second...");
    digitalWrite(power_pin, HIGH); // Turn on pin
    delay(1000);                   // Wait 1 second
    digitalWrite(power_pin, LOW);  // Turn off pin
    Serial.println("Power pin turned off.");
  }
}

// Function to connect to WiFi
void setupWiFi() {
  delay(100);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT broker...");
    if (client.connect("ESP32_Client")) { // Client ID
      Serial.println("Connected to MQTT broker!");
      client.subscribe(mqtt_topic); // Subscribe to topic
      Serial.print("Subscribed to topic: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Set pin mode
  pinMode(power_pin, OUTPUT);
  digitalWrite(power_pin, LOW); // Ensure pin starts in LOW state

  // Connect to WiFi
  setupWiFi();

  // Configure MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // Reconnect to MQTT if disconnected
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop(); // Maintain MQTT connection
}
