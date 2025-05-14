#include <ESP8266WiFi.h>
// #include <espnow.h>
#include <VL53L0X.h>
#define led     4   //D2
#define ID      6

VL53L0X sensor;

 
void setup() {
    pinMode(led, OUTPUT);
    digitalWrite(led, 1);
    Serial.begin(115200);
    //set VL53LOX 
    // Wire.begin();

    Wire.begin(13, 12);
    sensor.setTimeout(500);
    delay(1000);
    Serial.println("starting");
    if (!sensor.init()) Serial.println("Failed to detect and initialize sensor!");
    else                Serial.println("Successful to detect and initialize sensor!");

    // sensor.startContinuous();
    Serial.println("Ready!!!");
}

int led_status = 0;
int mean_value = 0;
void loop() {
    // int value = sensor.readRangeContinuousMillimeters();
    int value = sensor.readRangeSingleMillimeters();
    Serial.print(mean_value);
    Serial.print('\t');
    // Serial.println(value_single);
    Serial.println(value);
    if(abs(value-mean_value) > 8)   digitalWrite(led, 1);
    else                            digitalWrite(led, 0);
    mean_value = mean_value*0.6 + value*0.4;

    // if(value < 100 && value > 10){
    //     // led_status = ~led_status;
    //     digitalWrite(led, 1);
    //     delay(1000);
    // }
    // digitalWrite(led, 0);
    delay(10);
}
