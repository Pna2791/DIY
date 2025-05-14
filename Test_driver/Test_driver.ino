#include <AccelStepper.h>

// Define the stepper motor connections
#define STEP_PIN 12
#define DIR_PIN 14
#define ENABLE_PIN 32

// Create an instance of the AccelStepper class
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

void setup() {
    Serial.begin(115200);
    // Set the enable pin as OUTPUT and enable the driver
    pinMode(ENABLE_PIN, OUTPUT);
    digitalWrite(ENABLE_PIN, LOW);  // Enable the driver

    // Set maximum speed and acceleration
    stepper.setMaxSpeed(9600); // steps per second
    stepper.setAcceleration(4800); // steps per second^2
}

void loop() {
    // Move the stepper to a specific position
    Serial.println("move to 19200");
    stepper.moveTo(19200); // Move to 2000 steps
    stepper.runToPosition(); // Block until the target position is reached

    delay(1000); // Wait for a second

    // Move back to the starting position
    Serial.println("move to 0");
    stepper.moveTo(0); // Move back to 0 steps
    stepper.runToPosition(); // Block until the target position is reached

    delay(1000); // Wait for a second
}
