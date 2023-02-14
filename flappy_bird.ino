#include <Wire.h>
#include "SH1106Wire.h"

#include "images.h"

#define MAIN_MENU_SCREEN 0
#define GAMEPLAY_SCREEN 1
#define GAMEOVER_SCREEN 2

#define BIRD_WIDTH 14
#define BIRD_HEIGHT 9

SH1106Wire display(0x3c, D1, D2);

float wall[4];
int empty[4];
int space = 32;
int widthOfPassage = 30;

int gameState = MAIN_MENU_SCREEN;

const int buttonPin = D3;
int buttonState = 0;

int score = 0;
int isJumping = 0;
int currentTime = 0;
int direction = 1;
int play = 0;

float birdX = 5.00;
float birdY = 22.00;

unsigned long ton = 0;

void setup() {
  display.init();
  display.flipScreenVertically();
  generateWalls();
  display.drawRect(0, 0, 128, 63);
  delay(634);

  pinMode(buttonPin, INPUT);
  
  pinMode(A0, INPUT);
  Serial.begin(9600);
}

void loop() {
  display.clear();
  buttonState = digitalRead(buttonPin);

  if (gameState == MAIN_MENU_SCREEN) {
    showMainMenuScreen();
  } else if (gameState == GAMEPLAY_SCREEN) {
    showGamePlayScreen();
  } else if (gameState == GAMEOVER_SCREEN) {
    showGameOverScreen();
  }

  display.display();
}

void showMainMenuScreen() {
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 4, "Flappy ");
  display.drawXbm(0, 0, 128, 64, background);
  drawBird(20, 32);

  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 44, "Press to start");

  if (buttonState == HIGH) {
    gameState = GAMEPLAY_SCREEN;
  }
}

void showGamePlayScreen() {
  int val = analogRead(A0) * 12.5 / 100;
  display.setFont(ArialMT_Plain_10);
  display.drawString(3, 0, String(score));

    if (buttonState == HIGH) {
      if (isJumping == 0) {
        currentTime = millis();
        ton = millis();
        direction = 1;
        play = 1;
        isJumping = 1;
      }
    } else {
      isJumping = 0;
    }

    for (int i = 0; i < 4; i++) {
      // Draw the wall
      display.setColor(WHITE);
      display.fillRect(wall[i], 0, 6, 64);
      display.setColor(BLACK);
      display.fillRect(wall[i], empty[i], 6, widthOfPassage);

      // Move the wall and check if need to move the position of the wall
      wall[i] -= 0.01;

      if (wall[i] < 0) {
        score = score + 1;
        empty[i] = random(8, 32);
        wall[i] = 128 + val;
      }

      // Check for collision
      if (wall[i] <= birdX + 7 && birdX + 7 <= wall[i] + 6) {
        if (birdY < empty[i] || birdY + BIRD_HEIGHT > empty[i] + widthOfPassage) {
          gmaeOver();
        }
      }
    }

    drawBird(birdX, birdY);

    if ((currentTime + 185) < millis()) {
      direction = 0;
    }

    if ((ton + 40) < millis()) {
      play = 0;
    }

    if (direction == 0) {
      birdY += 0.01;
    } else {
      birdY -= 0.03;
    }

    if (birdY >= 64 || birdY < -3) { // Check if the bird collided with the upper bound or the lower bound
      gmaeOver();
    }
    
    display.drawRect(0, 0, 128, 63);
}

void showGameOverScreen() {
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 4, "Score: ");
  display.drawXbm(0, 0, 128, 64, background);
  display.drawString(20, 25, String(score));

  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 44, "Press to start");
  
  if (buttonState == HIGH) {
    score = 0;
    gameState = GAMEPLAY_SCREEN;
  }
}

void drawBird(int x, int y) {
  display.setColor(WHITE);
  display.drawXbm(x, y, BIRD_WIDTH, BIRD_HEIGHT, bird);
}

void gmaeOver() {
  gameState = GAMEOVER_SCREEN;
  birdY = 22;
  delay(500);
  generateWalls();
}

void generateWalls() {
  for (int i = 0; i < 4; i++) {
    wall[i] = 128 + ((i + 1) * space);
    empty[i] = random(8, 32);
  }
}
