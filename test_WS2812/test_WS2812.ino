#include <FastLED.h>

#define LED_PIN     5    // Pin connected to the data line of WS2812
#define NUM_LEDS    42   // Number of LEDs in the strip
#define BRIGHTNESS  100  // Brightness of the LEDs
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define t_delay     100

CRGB leds[NUM_LEDS];    // Declare the LED array

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  // Simple color wipe effect with red, green, and blue
  colorWipe(CRGB::Red, t_delay);   // Red wipe with a delay of 50ms between LEDs
  colorWipe(CRGB::Green, t_delay); // Green wipe with a delay of 50ms between LEDs
  colorWipe(CRGB::Blue, t_delay);  // Blue wipe with a delay of 50ms between LEDs

  // Add more effects here as needed...
}

// Function to create a color wipe effect
void colorWipe(CRGB color, int wait) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;        // Set the current LED to the specified color
    FastLED.show();         // Update the LED strip
    // delay(wait);            // Wait for the specified delay
  }
    // delay(1000);   
}
