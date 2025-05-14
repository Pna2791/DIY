#define pin_A 12
#define pin_B 11
#define pin_E 10


void speed(int val){
    Serial.println(val);
    if(val==0){
        digitalWrite(pin_E, 0);
        return;
    }
    if(val > 0){
        digitalWrite(pin_A, 0);
        digitalWrite(pin_B, 1);
    }
    if(val < 0){
        digitalWrite(pin_A, 1);
        digitalWrite(pin_B, 0);
    }
    analogWrite(pin_E, abs(val));
}

void setup() {
    pinMode(pin_A, OUTPUT);
    pinMode(pin_B, OUTPUT);
    pinMode(pin_E, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    for(int i=-255; i<256; i++){
        speed(i);
        delay(100);
    }
    delay(10000);
    for(int i=-255; i<256; i++){
        speed(-i);
        delay(100);
    }
    delay(10000);
}
