#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Your_SSID";       // Replace with your SSID
const char* password = "Your_Password"; // Replace with your Password

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);

    // Check if Wi-Fi credentials are set
    WiFi.begin(ssid, password);
    
    if (WiFi.status() != WL_CONNECTED) {
        // Create a hotspot with the last four digits of the MAC address
        String hotspotSSID = "RunMachine_" + String(WiFi.macAddress().substring(9, 14));
        WiFi.softAP(hotspotSSID.c_str(), "12345678");  // Default password
        Serial.println("Hotspot created: " + hotspotSSID);
    } else {
        Serial.println("Connected to WiFi: " + String(ssid));
    }

    // Set up server routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/command", HTTP_POST, handleCommand);
    server.on("/wifi", HTTP_POST, handleWifiUpdate);

    // Start the server
    server.begin();
    Serial.println("Server started");
}

void loop() {
    server.handleClient();
}

// Function to handle the root path
void handleRoot() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    String html = "<html><body>";
    html += "<h1>Device Control</h1>";
    html += "<h2>Update Wi-Fi Credentials</h2>";
    html += "<form action=\"/wifi\" method=\"POST\">";
    html += "SSID: <input type=\"text\" name=\"ssid\"><br>";
    html += "Password: <input type=\"password\" name=\"password\"><br>";
    html += "<input type=\"submit\" value=\"Update Wi-Fi\">";
    html += "</form><br>";

    html += "<h2>Send Command</h2>";
    html += "<button onclick=\"sendCommand('M1')\">Command M1</button>";
    html += "<button onclick=\"sendCommand('M2')\">Command M2</button>";
    html += "<button onclick=\"sendCommand('M3')\">Command M3</button>";
    html += "<button onclick=\"sendCommand('M4')\">Command M4</button>";
    html += "<button onclick=\"sendCommand('M5')\">Command M5</button>";
    html += "<button onclick=\"sendCommand('M6')\">Command M6</button>";
    html += "<script>";
    html += "function sendCommand(command) {";
    html += "fetch('/command', { method: 'POST', body: command })";
    html += ".then(response => response.text())";
    html += ".then(data => console.log(data));";
    html += "}";
    html += "</script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

// Function to handle the /command POST request
void handleCommand() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    if (server.hasArg("plain")) {
        String command = server.arg("plain");
        Serial.println("Received command: " + command);
        server.send(200, "text/plain", "Command received and printed to serial");
    } else {
        server.send(400, "text/plain", "No command in the request body");
    }
}

// Function to handle the /wifi POST request
void handleWifiUpdate() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    if (server.hasArg("ssid") && server.hasArg("password")) {
        String newSSID = server.arg("ssid");
        String newPassword = server.arg("password");
        
        // Save new SSID and password (this is just a placeholder for actual saving logic)
        Serial.println("New SSID: " + newSSID);
        Serial.println("New Password: " + newPassword);

        // Attempt to connect to the new Wi-Fi
        WiFi.begin(newSSID.c_str(), newPassword.c_str());
        if (WiFi.status() == WL_CONNECTED) {
            server.send(200, "text/plain", "Wi-Fi updated and connected");
            Serial.println("Connected to new Wi-Fi: " + newSSID);
        } else {
            server.send(400, "text/plain", "Failed to connect to new Wi-Fi");
        }
    } else {
        server.send(400, "text/plain", "No Wi-Fi details provided");
    }
}
