#define IR_LED_PIN  D6  // Set the pin for the IR LED

// void setup() {
//     pinMode(IR_LED_PIN, OUTPUT);
//     tone(IR_LED_PIN, 38000);  // Generate 38kHz signal
// }

// void loop() {
//     delay(1000);  // Keep running
// }


#define BUTTON_PIN  D7   // GPIO0 - Nút nhấn
#define LED_PIN     D8   // GPIO2 - Đèn LED

bool checkButtonAtStartup() {
    unsigned long startTime = millis();
    bool pressed = false;

    // Kiểm tra giữ nút 3 giây rồi thả
    while (millis() - startTime < 3000) {
        if (digitalRead(BUTTON_PIN) == LOW) { // Nút đang được nhấn
            pressed = true;
        } else if (pressed) { // Nếu đã nhấn trước đó và giờ thả ra
            return false;  // Không kiểm tra điều kiện 2 nữa
        }
        delay(1);
    }
    if(!pressed)  return false;
    digitalWrite(LED_PIN, HIGH); // Bật LEDQ
            while (digitalRead(BUTTON_PIN) == LOW)
                delay(1);
            digitalWrite(LED_PIN, LOW); // Bật LED

    // Kiểm tra nhấn 2 lần trong 2 giây
    int pressCount = 0;
    startTime = millis();
    while (millis() - startTime < 2000) {
        if (digitalRead(BUTTON_PIN) == LOW) {
            digitalWrite(LED_PIN, HIGH); // Bật LED
            while (digitalRead(BUTTON_PIN) == LOW)
                delay(100);
            delay(100);
            digitalWrite(LED_PIN, LOW); // Bật LED
            pressCount++;
        }
        delay(100);
    }

    if (pressCount == 2) return true;
    
    Serial.println(pressCount);
    return false;
}

bool boot_mode = false;
void setup() {
    pinMode(IR_LED_PIN, OUTPUT);
    tone(IR_LED_PIN, 38000);  // Generate 38kHz signal

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    Serial.begin(115200);
    Serial.println("Staart");

    boot_mode = checkButtonAtStartup();
    Serial.println(boot_mode ? "TRUE" : "FALSE");
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    delay(boot_mode ? 1800 : 200);
    digitalWrite(LED_PIN, LOW);
    delay(!boot_mode ? 1800 : 200);
}
