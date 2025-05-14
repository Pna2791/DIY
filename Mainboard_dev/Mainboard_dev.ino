#define button_enter 34
#define button_up 36
#define button_down 39
#define power_pin 22
#define TONE_OUTPUT_PIN 19
#include <TFT_eWidget.h>

#include <EEPROM.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>


#include <SPI.h>
#include <TFT_eSPI.h>


#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define BLACK  0x0000
#define WHITE  0xFFFF



TFT_eSPI tft = TFT_eSPI();  // TFT instance
#define SCREEN_HEIGHT   240
#define SCREEN_WIDTH    320
#define TEXT_SIZE_BIG 4  // Big text size for "RunMachine"
#define TEXT_SIZE_SMALL 2 // Small text size for "Version 6.2"



#include "FontMaker.h"
#include <FreeDefaultFonts.h>
void setpx(int16_t x,int16_t y,uint16_t color){
    tft.drawPixel(x,y,color); //Thay đổi hàm này thành hàm vẽ pixel mà thư viện led bạn dùng cung cấp
}
MakeFont myfont(&setpx);


void startup_screen(){
    // Display "RunMachine" with "Run" in red and "Machine" in blue
    tft.setTextSize(TEXT_SIZE_BIG);
    tft.setCursor(10, 10);
    tft.setTextColor(TFT_RED);
    tft.print("Run");

    tft.setTextColor(TFT_BLUE);
    tft.print("Machine");

    // Display "Version 6.2" in green, smaller size
    tft.setTextSize(TEXT_SIZE_SMALL);
    tft.setCursor(10, 60);
    tft.setTextColor(TFT_GREEN);
    tft.print("Version 6.2");
}


esp_now_peer_info_t peerInfo[6];
uint8_t nodeAddress[6][8] = {
  {0x78, 0x21, 0x84, 0x79, 0x79, 0x11},
  {0x78, 0x21, 0x84, 0x79, 0x79, 0x22},
  {0x78, 0x21, 0x84, 0x79, 0x79, 0x33},
  {0x78, 0x21, 0x84, 0x79, 0x79, 0x44},
  {0x78, 0x21, 0x84, 0x79, 0x79, 0x55},
  {0x78, 0x21, 0x84, 0x79, 0x79, 0x66}
};

typedef struct parameter {
    int speed=4;
    int capacity=10;
    bool type;
    int size=6;
} parameter;
parameter params;

String success;
typedef struct struct_message {
    int id;
    int value;
} struct_message;
struct_message sending_data;
struct_message recei_data;


class Power {
private:
    int power_pin_prv;

public:
    Power(int pin) {
        power_pin_prv = pin;
        pinMode(power_pin_prv, OUTPUT);
        digitalWrite(power_pin_prv, HIGH); // Default to ON (1)
    }

    void turn_on() {
        digitalWrite(power_pin_prv, HIGH);
        Serial.println("Power ON");
    }

    void turn_off() {
        digitalWrite(power_pin_prv, LOW);
        Serial.println("Power OFF");
    }
};


class LedController {
private:
    bool led_stt[6] = {0, 0, 0, 0, 0, 0};
    int latchPin, clockPin, dataPin;

public:
    LedController(int latch, int clock, int data) {
        latchPin = latch;
        clockPin = clock;
        dataPin = data;
        pinMode(latchPin, OUTPUT);
        pinMode(clockPin, OUTPUT);
        pinMode(dataPin, OUTPUT);
    }

    void led_change() {
        byte numberToDisplay = 0; // Initialize shift register data
        for (byte i = 0; i < 6; i++)
            if (led_stt[i])  // If led_status[i] is 1, set the corresponding bit
                numberToDisplay |= (1 << (i + 1));  // Shift to correct LED position

        // Send data to shift register
        digitalWrite(latchPin, LOW);
        shiftOut(dataPin, clockPin, LSBFIRST, ~numberToDisplay);
        digitalWrite(latchPin, HIGH);
    }

    void led_write(int index, int stt) {
        if (index >= 0 && index < 6) {
            led_stt[index] = stt;
            led_change();
        }
    }

    void around(int time_my_delay = 100) {
        Serial.println("Around");

        // Turn off all LEDs
        memset(led_stt, 0, sizeof(led_stt));
        led_change();

        // Turn on LEDs one by one
        for (int i = 0; i < 6; i++) {
            led_write(i, 1);
            my_delay(time_my_delay);
        }

        // Turn off LEDs one by one
        for (int i = 0; i < 6; i++) {
            led_write(i, 0);
            my_delay(time_my_delay);
        }
    }
};




Power powerControl(power_pin);
LedController ledController(4, 16, 2); // latchPin, clockPin, dataPin


volatile unsigned long pressTime = 0;
volatile unsigned long releaseTime = 0;
volatile bool buttonPressed = false;
volatile bool pressed_enter = false;
void IRAM_ATTR buttonISR() {
    unsigned long currentTime = millis();

    if (digitalRead(button_enter) == LOW) { // Button pressed (falling edge)
        pressTime = currentTime;
        buttonPressed = true;
    } else { // Button released (rising edge)
        releaseTime = currentTime;
        buttonPressed = false;

        unsigned long deltaTime = releaseTime - pressTime;

        if(deltaTime < 100) // Ignore noise
            return;
        
        if (deltaTime > 3000) { // If held > 3 seconds, turn off power
            powerControl.turn_off();
            return;
        }

        // Others press behavior
        pressed_enter = true;
    }
}


bool is_enter(){
    if(!pressed_enter)  return false;

    pressed_enter = false;
    return true;
}
bool up(){
    return !digitalRead(button_up);
}
bool down(){
    return !digitalRead(button_down);
}


void load_setup(){
    EEPROM.begin(128);
    params.speed = EEPROM.read(0);
    params.capacity = 10*EEPROM.read(1);
    if(EEPROM.read(2))  params.type = 1;
    else                params.type = 0;
    params.size = EEPROM.read(3);
    EEPROM.end();
}
void save_setup(){
    EEPROM.begin(128);
    EEPROM.write(0, params.speed);
    EEPROM.write(1, params.capacity/10);
    EEPROM.write(2, params.type);
    EEPROM.write(3, params.size);
    EEPROM.end();
}




#define TONE_PWM_CHANNEL 0
#define TONE_FREQUENCY 1000  // 1kHz
#define TONE_RESOLUTION 8    // 8-bit resolution (0-255)
void sound(int value){
    if(value == 0){
        ledcWriteNote(TONE_PWM_CHANNEL, NOTE_F, 3);
        my_delay(200);
    }else{
        ledcWriteNote(TONE_PWM_CHANNEL, NOTE_F, 5);
        my_delay(100);
    }
    ledcWriteTone(TONE_PWM_CHANNEL, 0);
}


void setup() {
    Serial.begin(115200);

    pinMode(button_up, INPUT_PULLUP);
    pinMode(button_down, INPUT_PULLUP);
    pinMode(button_enter, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(button_enter), buttonISR, CHANGE);
    
    // ledcSetup(TONE_PWM_CHANNEL, TONE_FREQUENCY, TONE_RESOLUTION);
    // ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
    
    tft.init();
    tft.setRotation(1);  // Landscape mode
    tft.fillScreen(TFT_BLACK);
    tft.setFreeFont(&FreeSans9pt7b);  // Load a font that supports Vietnamese

    load_setup();

    startup_screen();
    ledController.around();
}

void myfont_print(int x, int y, const char* message, uint16_t color) {
    tft.setCursor(x, y);       // Set position
    tft.setTextColor(color);   // Set text color (transparent background)
    tft.print(message);        // Print message
}


int box_height = 48;
void show_home(float pos){
    if(pos == 0)  Serial.println("[In home] practice now");
    if(pos == 1)  Serial.println("[In home] setup");

    pos += 0.8;
    tft.fillScreen(BLACK);
    tft.fillRect(20, pos*box_height, SCREEN_WIDTH-40, box_height, BLUE);
    // myfont_print(100, 1*box_height, "Bắt đầu bài tập", YELLOW);
    // myfont_print(100, 2*box_height, "Thiết lập", YELLOW);

    myfont.print(100, 1*box_height, "Bắt đầu bài tập", YELLOW, 0);
    myfont.print(100, 2*box_height, "Thiết lập", YELLOW, 0);

    // String mess = "Tốc độ: " + String(params.speed) + "     " + String(params.capacity);
    // if(params.type)  mess += " lần ";
    // else            mess += " giây ";
    // mess += "    " + String(params.size) + " góc";
  
    // myfont.print(40, 4*box_height, mess, YELLOW, 0);
}


int in_home(){
    // 0 practice now
    // 1 setup
    int pos = 0;
    show_home(pos);
    ledController.around();
    while(true){
        if(is_enter()) return pos;
        if(up() || down()){
            pos = 1 - pos;
            show_home(pos);
            my_delay(300);
        }
    }
}

void my_delay(int value){
    ArduinoOTA.handle();
    delay(value);
}

void loop() {
    int value = in_home();
}
