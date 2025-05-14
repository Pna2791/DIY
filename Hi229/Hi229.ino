#define RXD2 16   // Example pin for UART RX
#define TXD2 17   // Example pin for UART TX
#define PACKET_LENGTH 13

uint8_t previousByte = 0;
int count = 0;
int angle = 0;

void setup() {
  Serial.begin(115200);    // Debug output on Serial
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);  // Serial2 for UART input
  
  Serial.println("ESP32 UART listening...");
}

void loop() {
  while (Serial2.available() > 0) {
    uint8_t byteInt = Serial2.read();

    if (byteInt == 0xA5 && previousByte == 0x5A) {
      count = 2;  // Detected start sequence
    } else {
      count++;
      if (count == 13) {
        angle = byteInt * 256 + previousByte;  // Combine last two bytes
        if (angle > 0x7FFF) {
          angle -= 0x10000;
        }
        float angleValue = angle / 10.0;
        Serial.print("Angle: ");
        Serial.println(angleValue, 1);
        count = 0;  // Reset to look for the next packet
      }
    }
    previousByte = byteInt;
  }
}
