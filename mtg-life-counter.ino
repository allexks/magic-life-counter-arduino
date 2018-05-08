/* This my Magic: The Gathering^{TM} Life Counter project.
    Setup:
    + Adafruit monochrome 0.96" OLED 192x32 I2C protocol display
    + 4 buttons (2 for each player)

    (c) 2018 YALISHANDA
    (Alexander Ignatov)
*/

// For the button debounce:
#include <Bounce2.h>

// Buttons pins
// !!! NOTE
// In order for this sketch to work,
// the pins must be next to each other and
// arranged in an increasing order
// like so:
#define PLAYER1_UP 2
#define PLAYER1_DOWN 3
#define PLAYER2_UP 4
#define PLAYER2_DOWN 5 

// For the display:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);

const uint16_t displayWidth = SSD1306_LCDWIDTH;
const uint16_t displayHeight = SSD1306_LCDHEIGHT;

// Initialize debounce objects for the 4 buttons
Bounce bounces[4];

// Declaring time stamps
unsigned long buttonLastPress[4];
unsigned long buttonLastScoreChange[4];

const unsigned long AFTER_PRESS_OFFSET = 500; //ms
const unsigned long CONTINUOUS_PRESS_OFFSET = 50; //ms

const int BOUNCE_INTERVAL = 5; //ms

// Scores
short player1;
short player2;

boolean isBrawl = false;

// Helper functions
void initializeScore();
void updateScore();
boolean checkForButtonPress(int buttonPin);
void buttonAction(int buttonPin);

void setup() {
  Serial.begin(9600); // for debugging only

  for (int i=PLAYER1_UP; i <= PLAYER2_DOWN; i++) {
    pinMode(i, INPUT_PULLUP);
    bounces[i - PLAYER1_UP].attach(i);
    bounces[i - PLAYER1_UP].interval(BOUNCE_INTERVAL);
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  //splashscreen
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.println("Magic: The Gathering");
  display.println("Life Counter!");
  display.print("(c) 2018 Yalishanda");
  display.display();
  delay(2000);
  
  // initialize the life counter
  initializeScore();
  updateScore();
}

void loop() {

  boolean buttonStates[4];
  
  for (int i=PLAYER1_UP; i<=PLAYER2_DOWN; i++) {
    buttonStates[i - PLAYER1_UP] = checkForButtonPress(i);
  }
  
  if (buttonStates[0] && buttonStates[2]) {
    initializeScore();
  }

  updateScore();
}

boolean checkForButtonPress(int buttonPin) {
  size_t index = buttonPin - PLAYER1_UP;
  
  boolean changed = bounces[index].update();
  int val = bounces[index].read();

  if (val == LOW) {
    if (changed) {
      // on the butten press
      buttonLastPress[index] = millis();
      buttonLastScoreChange[index] = millis();
      buttonAction(buttonPin);
    }
    // on the continuous pressing of the button
    if (millis() >= buttonLastPress[index] + AFTER_PRESS_OFFSET) {
      if (millis() >= buttonLastScoreChange[index] + CONTINUOUS_PRESS_OFFSET) {
        buttonLastScoreChange[index] = millis();
        buttonAction(buttonPin);
      }
    }
    

    return true;
  }

  return false;
}

void buttonAction(int buttonPin) {
  switch (buttonPin) {
      case PLAYER1_UP:
        player1++;
        break;
      case PLAYER1_DOWN:
        player1--;
        break;
      case PLAYER2_UP:
        player2++;
        break;
      case PLAYER2_DOWN:
        player2--;
        break;
   }
}

void initializeScore() {
  // at the time of the first call each player
  // gets a life total of 20
  // then with each call of the function life totals alternate
  // between 20 and 30
  player1 = player2 = 20 + ((int)isBrawl)*10;

  isBrawl = !isBrawl;
}


void updateScore() {
  display.clearDisplay();

  display.setTextColor(WHITE, BLACK);
  display.setTextSize(3);
  display.setTextWrap(false);

  uint16_t offsetX = 12;
  uint16_t offsetY = 6;

  // Left-hand player life total
  display.setCursor(offsetX, offsetY);
  display.print(player1);

  // Right-hand player life total
  display.setCursor(displayWidth - 4 * offsetX, offsetY);
  display.print(player2);

  // draw the central line
  display.drawLine(displayWidth / 2, 0,  displayWidth / 2, displayHeight, WHITE);
  display.display();
}

