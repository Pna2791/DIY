#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "Creator2"; 
const char* password = "123456789"; 
const char* websocketHost = "anhpn.ddns.net"; // Replace with your server's IP
const uint16_t websocketPort = 8901; // The port your WebSocket server is running on

WebSocketsClient webSocket;

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectDelay = 30000; // 30 seconds

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    webSocket.begin(websocketHost, websocketPort, "/"); // Specify host, port, and path
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    
    if (WiFi.status() != WL_CONNECTED) {
        if (millis() - lastReconnectAttempt > reconnectDelay) {
            lastReconnectAttempt = millis();
            connectToWiFi();
        }
    } else {
        // Check if WebSocket is connected
        if (!webSocket.isConnected()) {  // Use isConnected instead of connected
            if (millis() - lastReconnectAttempt > reconnectDelay) {
                lastReconnectAttempt = millis();
                reconnectWebSocket();
            }
        }
    }
}

void connectToWiFi() {
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
}

void reconnectWebSocket() {
    Serial.println("Reconnecting to WebSocket...");
    webSocket.disconnect();  // Explicitly disconnect first
    webSocket.begin(websocketHost, websocketPort, "/"); // Reconnect to WebSocket
    Serial.println("WebSocket reconnected");
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_TEXT:
            if (strcmp((char*)payload, "turn_on") == 0) {
                turnOnLight();
            } else if (strcmp((char*)payload, "turn_off") == 0) {
                turnOffLight();
            }
            break;
    }
}

void turnOnLight() {
    Serial.println("Light turned ON");
}

void turnOffLight() {
    Serial.println("Light turned OFF");
}
