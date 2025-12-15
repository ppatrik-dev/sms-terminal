#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== DISPLAY =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI 23
#define OLED_CLK  18
#define OLED_DC   27
#define OLED_CS   5
#define OLED_RST  17

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  OLED_MOSI,
  OLED_CLK,
  OLED_DC,
  OLED_RST,
  OLED_CS
);

// ===== KEYPAD =====
const byte COLS = 3;
const byte ROWS = 4;

byte colPins[COLS] = {25, 26, 13};       // COL1, COL2, COL3
byte rowPins[ROWS] = {14, 12, 19, 16};   // ROW1, ROW2, ROW3, ROW4

char keymap[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

void setup() {
  Serial.begin(921600);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setRotation(2);
  display.setCursor(0, 0);
  display.println("Ready");
  display.display();

  // Columns as OUTPUT, set HIGH initially
  for (int c = 0; c < 3; c++) {
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], HIGH);
  }

  for (int r = 0; r < 4; r++) {
    pinMode(rowPins[r], INPUT_PULLUP);
  } 
}

char scanKeypad() {
  for (int c = 0; c < 3; c++) {
    digitalWrite(colPins[c], LOW);
    for (int r = 0; r < 4; r++) {
      if (digitalRead(rowPins[r]) == LOW) {
        while(digitalRead(rowPins[r]) == LOW);
        digitalWrite(colPins[c], HIGH);
        return keymap[r][c];
      }
    }
    digitalWrite(colPins[c], HIGH);
  }

  return '\0';
}

void loop() {
  char key = scanKeypad();

  if (key != '\0') {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Key: ");
    display.println(key);
    display.display();
  }
}
