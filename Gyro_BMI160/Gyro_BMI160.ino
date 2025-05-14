#include <BMI160Gen.h>
#include <Wire.h>
#include "BluetoothSerial.h"


BluetoothSerial SerialBT;


#define SDA_PIN 16
#define SCL_PIN 4

#define plush_pin 19
#define dir_pin 13

#define button_pin 15
#define switch_pin 12

#define show_gyro false

int plush_width = 0;
float acc_threshold = 15000;
float gyro_threshold = 25000;

String mode = "";
bool stopped = true;
bool blocked = false;
int cur_plush = 0;

void setup() {
    Wire.begin(SDA_PIN, SCL_PIN);
    Serial.begin(115200); // initialize Serial communication
    SerialBT.begin("Creator Device"); // Set the Bluetooth device name

    pinMode(plush_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);

    // initialize device
    Serial.println("Initializing IMU device...");
    // BMI160.begin(BMI160GenClass::SPI_MODE, /* SS pin# = */10);
    BMI160.begin(BMI160GenClass::I2C_MODE);
    uint8_t dev_id = BMI160.getDeviceID();
    Serial.print("DEVICE ID: ");
    Serial.println(dev_id, HEX);

    // Set the accelerometer range to 250 degrees/second
    BMI160.setGyroRange(1000);
    BMI160.setAccelerometerRange(8);
    Serial.println("Initializing IMU device...done.");
}


float compute_3_axis(float x, float y, float z){
    float sum = (x * x) + (y * y) + (z * z);
    return sqrt(sum);
}


void fault_detection(){
    int gxRaw, gyRaw, gzRaw;
    int gx, gy, gz;

    // read raw gyro measurements from device
    BMI160.readGyro(gxRaw, gyRaw, gzRaw);
    BMI160.readAccelerometer(gx, gy, gz);

    float gyro = compute_3_axis(gxRaw, gyRaw, gzRaw);
    float acceleration = compute_3_axis(gx, gy, gz);

    if (gyro > gyro_threshold || acceleration > acc_threshold){
        analogWrite(plush_pin, 0);
        blocked = true;
        Serial.println("Fault");
    }
    if(show_gyro){
        Serial.print(gyro);
        Serial.print("\t");
        Serial.print(acceleration);
        Serial.println();
    }
}

void set_plush(int value){
    analogWrite(plush_pin, value);
    cur_plush = value;
}


void Grinder_process(){
    if(digitalRead(button_pin)){
        if(!blocked){
            if(cur_plush != plush_width){
                cur_plush++;
                if (cur_plush > plush_width)    cur_plush = plush_width;
                set_plush(cur_plush);
            }
        }

    }else{
        set_plush(0);
        blocked = false;
    }
    
}

void Driller_process(){

}

void Welding_process(){

}

void loop() {
    fault_detection();

    delay(100);
    // Check for serial commands
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        processSerialCommand(command);
    }

    // Check for serial commands
    if (SerialBT.available()) {
        String command = SerialBT.readStringUntil('\n');
        processSerialCommand(command);
    }

    if(mode == "grinder") Grinder_process();
    if(mode == "driller") Driller_process();
    if(mode == "welding") Welding_process();
}

void processSerialCommand(String command) {
    Serial.println(command);
    command.trim();  // Remove any leading/trailing whitespace
    String sub_string = command.substring(1);

    if (command.startsWith("M")) {
        set_plush(0);
        blocked = true;
        mode = sub_string;
    } else if (command.startsWith("S")) {
    }
}
