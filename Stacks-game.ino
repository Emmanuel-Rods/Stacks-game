/*
This whole project code was written by a human , no AI used. Yes it has it ugly parts , but its made out from the heart , 
by https://github.com/emmanuel-rods/ to report any bugs , https://github.com/emmanuel-rods/stack-game 
*/


//importing the libs
#include <MD_MAX72xx.h>
#include "animations.h"


#define MAX_DEVICES 4  // 8 * 8

const int maxX = MAX_DEVICES * 8 - 1;
const int maxY = 7;

#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10


const int button = 3;

int layer = 0;  // x axis

//initialising the matrix
int Matrix[maxX + 1][maxY + 1];

const int gameLevels[] = { 200, 150, 100, 75 };  // speed of the game

// const int gameLevels[] = { 200, 200, 200, 200 };  // temp testing code , remove later
int currentGameLevel = 0;

bool isPressed = false;  // stores the state of the button


MD_MAX72XX mx = MD_MAX72XX(MD_MAX72XX::FC16_HW, CS_PIN, MAX_DEVICES);


void setup() {
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 2);  //
  mx.clear();
  Serial.begin(9600);

  pinMode(button, INPUT_PULLUP);
}

int length = 3;  // length of the led layer


void loop() {

  // //displaying
  for (int i = 0; i <= maxY; i++) {
    mx.setPoint(i, layer, Matrix[layer][i]);
  }

  // delay(3000);
  //button checking code

  int delayCounter = 0;
  while (delayCounter < gameLevels[currentGameLevel]) {
    if ((digitalRead(button) == LOW) && (isPressed == false)) {
      //  Serial.println("pressed");
      //   layer++;
      isPressed = true;

      //check if the prevois layer and current layer are the same
      if (layer > 0) {
        //  compareLayer(currentLayer , previousLayer) ;
        compareLayers();
      }

      layer++;  // incrementing the layer

      //win condition
      //! important change this later to maxX
      if (layer > maxX && length != 0) {
        gameWon();
      }

      // hardcoded level values , i might not change these cuz i dont know the multiples of 8 , fuck you
      if (layer == 8) {
        currentGameLevel++;
      }
      if (layer == 16) {
        currentGameLevel++;
      }
      if (layer == 24) {
        currentGameLevel++;
      }
    }

    delay(10);

    delayCounter += 10;
  }


  clearLayer(layer);

  bouncingAnimation();

  //checks if the button is released
  if ((digitalRead(button) == HIGH) && (isPressed == true)) {
    isPressed = false;
  }

  // when all of the lights are out ( game over )
  if (length == 0) {
    gameOver();
  }
}

int startingPointIndex = 0;
bool goingForward = true;
int direction = 1;


void clearLayer(int layer) {
  for (int i = 0; i <= maxY; i++) {
    Matrix[layer][i] = 0;
  }
}

void bouncingAnimation() {
  for (int i = startingPointIndex; i < (length + startingPointIndex); i++) {
    Matrix[layer][i] = 1;  // setting as active
                           //to check if it has reached the last
    if (goingForward) {
      if (Matrix[layer][maxY] == 1) {  // check if it has reached the end
        // Serial.println("reached end");
        goingForward = false;
      }
    }
  }


  if (!goingForward) {
    direction = -1;
    if (Matrix[layer][0] == 1) {  //if reached first
      direction = 1;
      goingForward = true;
    }
  }

  startingPointIndex += direction;
}

void compareLayers() {

  int mismatched[] = { 0, 0, 0, 0, 0, 0, 0, 0 };  //
  //upper layer
  for (int y = 0; y <= maxY; y++) {
    if (Matrix[layer][y] == 1) {
      //lower layer
      if (Matrix[layer - 1][y] == 0) {
        //set the current index (y) to zero
        // Serial.print("missmatch on") + Serial.println(y);
        Matrix[layer][y] = 0;
        mismatched[y] = 1;
      }
    }
  }

  //debugging
  // for (int i = 0; i <= maxY; i++) {
  //   Serial.print(mismatched[i]) + Serial.print(" ");
  //   //  Serial.print("I ") + Serial.print(i);
  // }
  // Serial.println();

  // mismatched led blinking code
  bool state = false;
  int blinks = 0;
  while (blinks < 4) {
    for (int i = 0; i <= maxY; i++) {
      if (mismatched[i] == 1) {
        mx.setPoint(i, layer, state);
      }
    }
    delay(70);
    blinks++;
    state = !state;
  }

  //update the previous layer to display correctly
  for (int i = 0; i <= maxY; i++) {
    mx.setPoint(i, layer, Matrix[layer][i]);
  }

  length = 0;
  //gets the length of the current layer after we
  for (int i = 0; i <= maxY; i++) {
    length += Matrix[layer][i];
  }

  // mismatched[8];
}

// to blink available leds
void blinkAllAnimation(int maxBlinks, int duration) {
  bool state = false;  // used to flip the lights
  int blinks = 0;
  while (blinks < maxBlinks) {
    for (int x = 0; x <= maxX; x++) {
      for (int y = 0; y <= maxY; y++) {
        if (Matrix[x][y] == 1) {
          mx.setPoint(y, x, state);
        }
      }
    }
    delay(duration);
    blinks++;
    state = !state;
  }
}

// gives the illusion that the matrix is dropping / falling
void dropingAnimation() {
  int x = 0;
  while (x < maxX) {  //change this to maxX
    for (int x = 0; x <= maxX; x++) {
      for (int y = 0; y <= maxY; y++) {
        Matrix[x][y] = x < 31 ? Matrix[x + 1][y] : 0;
        mx.setPoint(y, x, Matrix[x][y]);
      }
    }
    delay(100);
    x++;
  }
}


//sets all of the matrix values to zero
void clearAll() {
  for (int x = 0; x <= maxX; x++) {
    for (int y = 0; y <= maxY; y++) {
      Matrix[x][y] = 0;
    }
  }
}


//similar to cleanAll() function  + display , but starts from the top and goes to bottom
void downwardCleanAnimation() {
  for (int x = maxX; x >= 0; x--) {
    for (int y = maxY; y >= 0; y--) {
      mx.setPoint(y, x, false);
      Matrix[x][y] = 0;
    }
    delay(15);
  }
}


void playAnimation(int animationData[]) {
  int i = 0;
  while (i < 256) {
    for (int x = maxX; x >= 0; x--) {
      for (int y = maxY; y >= 0; y--) {
        Matrix[x][y] = animationData[i];
        mx.setPoint(y, x, Matrix[x][y]);
        if (i < 256) {
          i++;
        }
      }
      delay(15);
    }
  }
}

//shows the game over  animation and resets the game
void gameOver() {
  delay(1000);
  blinkAllAnimation(5, 500);
  Serial.println("Game ended");
  playAnimation(gameOverAnimationData);
  delay(2000);
  downwardCleanAnimation();
  mx.clear();
  clearAll();
  layer = 0;
  currentGameLevel = 0;
  length = 3;
}


//shows winning animation and resets the game
void gameWon() {
  blinkAllAnimation(10, 150);
  delay(10);
  dropingAnimation();
  // clearAll();
  layer = 0;
  currentGameLevel = 0;
  length = 3;
  playAnimation(winAnimationData);
  delay(4000);
  downwardCleanAnimation();
  clearAll();
  mx.clear();
}
