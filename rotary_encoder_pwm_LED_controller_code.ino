#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define ENCODER_CLK 2
#define ENCODER_DT 3
#define ENCODER_SW 4
#define LED_PIN 9

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);

volatile int encoderPos = 0;
int lastCLK = LOW;
bool ledState = true; // Start with LED ON
int brightness = 0;   // Initial brightness (0%)

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  
  // Set up OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  display.clearDisplay();
  display.display();

  // Set up encoder pins
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  // Set up LED pin
  pinMode(LED_PIN, OUTPUT);

  // Attach interrupts for encoder rotation
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), updateEncoder, CHANGE);
}

void loop() {
  // Handle button press to toggle LED state
  if (digitalRead(ENCODER_SW) == LOW) {
    ledState = !ledState; // Toggle LED state
    delay(300);           // Debounce
  }

  // Update LED brightness
  if (ledState) {
    analogWrite(LED_PIN, map(brightness, 0, 100, 0, 255)); // Apply PWM
  } else {
    analogWrite(LED_PIN, 0); // Turn off LED
  }

  // Display feedback on OLED
  updateDisplay();
}

void updateEncoder() {
  int currentCLK = digitalRead(ENCODER_CLK);
  if (currentCLK != lastCLK && currentCLK == HIGH) {
    if (digitalRead(ENCODER_DT) != currentCLK) {
      encoderPos++; // Clockwise rotation
    } else {
      encoderPos--; // Counterclockwise rotation
    }

    // Limit encoder position and set brightness
    if (encoderPos > 4) encoderPos = 4; // Maximum position for 100%
    if (encoderPos < 0) encoderPos = 0; // Minimum position for 0%
    brightness = encoderPos * 25;       // Map to 0%, 25%, 50%, 75%, 100%
  }
  lastCLK = currentCLK;
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("LED State: ");
  display.println(ledState ? "ON" : "OFF");

  display.setCursor(0, 16);
  display.print("Brightness: ");
  display.print(brightness);
  display.println("%");

  display.display();
}
