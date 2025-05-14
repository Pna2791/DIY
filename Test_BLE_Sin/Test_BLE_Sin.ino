#include <BluetoothSerial.h>
#include <math.h>

BluetoothSerial SerialBT;

const int frameDelayMs = 40;          // 40 milliseconds between samples
const float frequency = 0.1;          // 0.1 Hz = 1 cycle every 10 seconds
const float amplitude = 1.0;
const float pi = 3.14159265;

unsigned long previousMillis = 0;
float timeSec = 0.0;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Sine_01Hz");  // Bluetooth device name
  Serial.println("Bluetooth 0.1 Hz Sine Generator Started");
}

void loop() {
  unsigned long currentMillis = millis();

  // Send sine/cosine values every 40 ms
  if (currentMillis - previousMillis >= frameDelayMs) {
    previousMillis = currentMillis;

    float sineValue = amplitude * sin(2 * pi * frequency * timeSec);
    float cosValue  = amplitude * cos(2 * pi * frequency * timeSec);

    SerialBT.print(sineValue, 4);
    SerialBT.print(" ");
    SerialBT.println(cosValue, 4);

    Serial.print(sineValue, 4);
    Serial.print(" ");
    Serial.println(cosValue, 4);

    timeSec += frameDelayMs / 1000.0;
  }

  // Read and print incoming Bluetooth data to Serial Monitor
  if (SerialBT.available()) {
    String incoming = SerialBT.readStringUntil('\n');
    Serial.print("Received from Bluetooth: ");
    Serial.println(incoming);
  }
}
