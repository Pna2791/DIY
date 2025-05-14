String inputCommand = "";

void setup() {
  // USB Serial for debug/input
  Serial.begin(115200);

  // Serial2 (GPIO17 TX, GPIO16 RX)
  Serial2.begin(115200, SERIAL_8N1, 13, 15);

  Serial.println("System ready. Type 'BOOT' to send boot script to Serial2.");
}

void loop() {
  // 1. Forward data from Serial2 to Serial
  if (Serial2.available()) {
    char c = Serial2.read();
    Serial.print(c);
  }

  // 2. Read user input from Serial (USB)
  if (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputCommand == "BOOT") {
        sendBootScriptToSerial2();
      }
      inputCommand = "";  // Reset after processing
    } else {
      inputCommand += c;
    }
  }

  delay(10);  // Optional: reduce CPU usage
}

void sendBootScriptToSerial2() {
  Serial.println("Sending boot script to Serial2...");

  const char* script =
    "for mmcdev in 0 1 2; do "
    "if fatload mmc ${mmcdev} 1000000 u-boot.emmc; then go 1000000; fi; "
    "if fatload mmc ${mmcdev} 1000000 s905_autoscript; then autoscr 1000000; fi; "
    "done\n";

  Serial2.print(script);
  Serial.println("Script sent.");
}
