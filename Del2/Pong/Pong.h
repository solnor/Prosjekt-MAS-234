#ifndef Pong_h
#define Pong_h

#include <FlexCAN.h>
#include <SPI.h>
#include <Wire.h>

#include <Metro.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Can
{
private:
  ;
public:
  Can();
  CAN_message_t outMsg;
  CAN_message_t inMsg;
  void send_();
  void setData(int buf[8], int id);
  void receive_();
  int id;
};

class Ball
{
private:
  int _x;
  int _y;
  int _radius; 
  int _startX;
  int _startY;
  int _startVelX;
  int _startVelY;
public:
  Ball();
  Ball(int xPos, int yPos, int ballRadius, int xVel, int yVel);
  int xVel;
  int yVel;
  int getX();
  int getY();
  int getTop();
  int getBottom();
  int getLeft();
  int getRight();
  void setX_(int x);
  void setY_(int y);
  void setVelX(int velX);
  void setVelY(int velY);
  void display_(Adafruit_SSD1306* displayPointer);
  void reset_(int velX, int velY, int xPos, int yPos);
  void update_();
};

class Paddle
{
private:
  void _update();
public:
  Paddle(int xPos, int yPos, int len, int vel);
  void display_(Adafruit_SSD1306* displayPointer);
  int x;
  int y;
  int len;
  int vel;
  int getX();
  int getY();
  int getTop();
  int getBottom();
  int getLeft();
  int getRight();
};

class Score
{
private:
  int _x;
  int _y;
public:
  Score(int xPos, int yPos, Adafruit_SSD1306* displayPointer);
  int playerScore;
  int opponentScore;
  void update_(int playerScoreChange, int opponentScoreChange);
  void display_(Adafruit_SSD1306* displayPointer);
};

class Pong
{
private:
  Adafruit_SSD1306 display;
  Adafruit_SSD1306 *_displayPtr;
  int _width  = 128;
  int _height = 64;
  Ball _ball;
  Paddle _playerPaddle;
  Paddle _opponentPaddle;
  Score _score;
  Can _can;
  int _data[8];
  int _getRandomValue(int minVal, int maxVal);
  Metro _canTimer;
  bool master;
public:
  Pong();
  bool playing = false;
  void waitForInput();
  void update_();
  void show();
  bool isJoyUp = false;
  bool isJoyDown = false;
  bool isJoyClicked = false;
  int groupNumberPlayer = 0;
  int groupNumberOpponent = 0;
};

#endif
