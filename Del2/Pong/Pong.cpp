#include "Arduino.h"
#include "Pong.h"

#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5

//Constructor. Initialiserer medlemmer, lager objekter av type Ball, Paddle, og Score
Pong::Pong()
  :display(Adafruit_SSD1306(OLED_DC, OLED_RESET, OLED_CS)), 
  _displayPtr(&display),
  _width(128),
  _height(64), 
  _ball(Ball(_width/2, _height/2, 2, 1, 1)),
  _playerPaddle(Paddle(5,_height/2,20,2)),
  _opponentPaddle(Paddle(_width-5, _height/2, 20, 2)),
  _score(Score(_width/2,0, _displayPtr)),
  _can(Can()),
  _data{1,0,0,0,0,0,0,0},
  _canTimer(Metro(10)),
  master(true)
{
  //Initialiserer OLED-displayed og fjerner grafikken som allerede måtte være der
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
}
/*
 *Venting på første input fra spillere. Bestemmer hvem som er master og hvem som starter med ballen.
 *Denne koden skal lastes opp til begge spillenheter. 
 *Den første som trykker blir master i spillet og master blir satt sann.
 *Her simulerer det å flytte joystick til nedre posisjon at motspillers enhet er master.
 *Dette er ikke utviklet ferdig.
*/
void Pong::waitForInput()
{
  if(isJoyClicked)
  {
    //Setter CAN-meldings-data og ballposisjon. 
    //Sender data som gir signal til motspillers enhet om at gjeldende enhet er master
    _can.id = groupNumberPlayer + 50;
    _can.setData(_data, groupNumberPlayer + 20);
    _can.send_();
    _ball.setX_(_playerPaddle.getRight()+(_ball.getX()-_ball.getLeft()+2));
    _ball.setY_(_playerPaddle.getY()+_playerPaddle.len/2);
    
    master = true;
    playing = true;
  }
  if(isJoyDown)
  {
    //Sjekker etter innkommende meldinger fra den andre enheten. Setter ballposisjon.
    _can.receive_();
    _ball.setX_(_opponentPaddle.getLeft()-(_ball.getRight()-_ball.getX())-2);
    _ball.setY_(_opponentPaddle.getY()+_opponentPaddle.len/2);
    
    master = false;
    playing = true;
  }
  //Hvis enten spiller eller motspiller har startet spill:
  if(playing)
  {
    //Sett x- og y-hastighet på ballen til en tilfeldig verdi.
    _ball.setVelX(_getRandomValue(1,5));
    _ball.setVelY(_getRandomValue(-4,4));
    //Hvis gjeldende enhet ikke er master, skal ballen ha motsatt x-hastighet ettersom koordinatsystemet er speilvendt.
    if(!master)
    {
      _ball.xVel = -_ball.xVel;
    }
  }
  
}

void Pong::update_()
{
  //Styrer spillerplater basert på input fra joystick
  if(isJoyUp == true)
  {
    //Fjerner y-hastigheten til spillerplaten fra y-posisjonen.
    _playerPaddle.y -= _playerPaddle.vel;
    //Hvis spillerplaten har høyere hastighet enn 1, 
    //kan y-posisjonen blir mindre enn null (gå over topp av skjerm) i ett hopp,
    //I dette tilfellet settes y-posisjon med en gang lik 0.
    //Siden spillersplates posisjon måles fra topp, 
    //er toppen av platen det samme som posisjonen til platen
    if(_playerPaddle.getY() < 0)
    {
      _playerPaddle.y = 0;
    }
  }
  if(isJoyDown == true)
  {
    //Legger til y-hastigheten til posisjonen til y-posisjonen
    _playerPaddle.y += _playerPaddle.vel;
    //Hvis nedre del av spillerplate går under bunn av skjerm,
    //Settes ballposisjon til lengden av spillerplate trekt ifra høyden av skjermen
    //(Ettersom spillerplates blir posisjon blir målt fra øvre venstre hjørne)
    if(_playerPaddle.getBottom() > _height)
    {
      _playerPaddle.y = _height - _playerPaddle.len;
    }
  }

  //Hvis ballens topp når toppen av skjermen eller ballens bunn når bunnen av skjermen, snus ballhastighet.
  if(_ball.getTop() <= 0 || _ball.getBottom() >=_height)
  {
    _ball.yVel = -_ball.yVel;
  }
  //Hvis ballposisjon går forbi enten høyre eller venste del av skjermen, regnes dette som en scoring
  if(_ball.getLeft() <= 0 || _ball.getRight() >=_width)
  {
    //Hvis ball går forbi venstre side, gir dette poeng til motspiller. 
    //Resetter deretter ballens posisjon til spillerplate og setter en tilfeldig ballhastighet.
    if(_ball.getLeft() <= 0)
    {
      _score.update_(0,1);
      _ball.reset_(_getRandomValue(1,4), _getRandomValue(-4,4),_playerPaddle.getRight()+(_ball.getX()-_ball.getLeft()+2), _playerPaddle.getY()+_playerPaddle.len/2);
    }
    //Hvis ball går forbi høyre side, gir dette poeng til spiller. Ballen resettes.
    else if(_ball.getRight() >=_width)
    {
      _score.update_(1,0);
      _ball.reset_(_getRandomValue(-4,-1), _getRandomValue(-4,4), _opponentPaddle.getLeft()-(_ball.getRight()-_ball.getX())-2, _opponentPaddle.getY()+_opponentPaddle.len/2);
    }
  }

  //Sjekker om ballen er på venstre halvdel av skjermen. 
  //Sjekker deretter etter kollisjon med spillerplate og snur eventuelt x-hastighet.
  if(_ball.getX() <= _width/2)
  {
    if(_ball.getLeft() - 1 <= _playerPaddle.getRight() && _ball.getLeft() >= _playerPaddle.getLeft()-1)
    {
      if(_ball.getBottom() >= _playerPaddle.getTop() && _ball.getTop() <= _playerPaddle.getBottom())
      {
        _ball.xVel = -_ball.xVel;
        if(master)
        {
          _ball.setX_(_playerPaddle.getRight()+(_ball.getX()-_ball.getLeft())+2);
        }
      }
    }
  }
  //Sjekker om ballen er på høyre halvdel av skjermen. 
  //Sjekker deretter etter kollisjon med motspillerplate og snur eventuelt x-hastighet
  else if(_ball.getX() > _width/2)
  {
    if(_ball.getRight() + 1 >= _opponentPaddle.getLeft() && _ball.getRight() <= _opponentPaddle.getRight()+1)
    {
      if(_ball.getBottom() >= _opponentPaddle.getTop() && _ball.getTop() <= _opponentPaddle.getBottom())
      {
        _ball.xVel = -_ball.xVel;
        if(master)
        {
          _ball.setX_(_opponentPaddle.getLeft()-(_ball.getRight()-_ball.getX())-2);
        }
      }
    }
  }

  //Sender ut ballposisjon og spillerplateposisjon hvert tiende millisekund
  if(_canTimer.check())
  {
    _data[0] = _playerPaddle.y;
    if(master)
    {
    _data[1] = _width - _ball.getX();
    _data[2] = _ball.getY();
    }
    
    _can.setData(_data, _can.id);
    _can.send_();
  }

  //Sjekker etter innkommende meldinger
  _can.receive_();
  //Hvis master, skal ballens posisjon kalkuleres, hvis ikke skal den settes lik det dataen tilsier i CAN-meldingen
  if(master)
  {
    _ball.update_();
  }
  else
  {
    _ball.setX_(_can.inMsg.buf[1]);
    _ball.setY_(_can.inMsg.buf[2]);
    
  }
  //Motstanders spillerplate settes lik utsendt posisjon fra motspiller.
  _opponentPaddle.y = _can.inMsg.buf[0];
}
//Tegner all grafikken på displayet og viser det. 
//Bruker pointers som et handle til displayet så andre metoder i andre classes vet hva de skal tegne på.
void Pong::show()
{
  display.clearDisplay();
  _ball.display_(_displayPtr);
  _playerPaddle.display_(_displayPtr);
  _opponentPaddle.display_(_displayPtr);
  _score.display_(_displayPtr);
  display.display();
}
//Returnerer et tilfeldig tall mellom minVal og maxVal
int Pong::_getRandomValue(int minVal, int maxVal)
{
  int _val = 0;
  //Kjører så lenge den tilfeldige verdien blir null siden 
  //denne funksjonen blir brukt til å gi ballen en tilfeldig hastighet 
  //der null hastighet lover et lite spennende spill
  while(_val == 0)
  {
    //Må sette et seed for random()-funksjonen. 
    //Bruker tid gått siden reboot av Teensy-enhet siden dette alltid endrer seg.
    randomSeed(millis()+_ball.getY());
    _val = random(minVal, maxVal);
  }
  return _val;
}
//Constructor. Initialiserer utmeldingsdata
Can::Can()
{
  outMsg.ext = 0;
  outMsg.id = 0;
  outMsg.len = 3;
}
//Sender melding
void Can::send_()
{
  Can0.write(outMsg);
}
//Setter utmeldingsdata
void Can::setData(int buf[8], int id)
{
  outMsg.id = id;
  for(int i = 0; i<8; i++)
  {
    outMsg.buf[i] = buf[i];
  }
}
//Mottar melding dersom det er en tilgjengelig
void Can::receive_()
{
  while (Can1.available()) 
  {
    Can1.read(inMsg);
  }
}

//Constructor. Initialiserer balldata
Ball::Ball(int xPos, int yPos, int ballRadius, int xVel, int yVel)
  :_x(xPos), 
  _y(yPos), 
  _radius(ballRadius),
  xVel(xVel),
  yVel(yVel)
{
  _startX = _x;
  _startY = _y;
  _startVelX = xVel;
  _startVelY = yVel;
}
//Returnerer x-ballposisjon
int Ball::getX()
{
  return _x;
}
//Returnerer y-ballposisjon
int Ball::getY()
{
  return _y;
}
//Returnerer y-posisjon til topp av ball.
int Ball::getTop()
{
  return getY()-_radius;
}
//Returnerer y-posisjon til bunn av ball.
int Ball::getBottom()
{
  return getY()+_radius+1;
}
//Returnerer x-posisjon til venstre side av ball.
int Ball::getLeft()
{
  return getX()-_radius;
}
//Returnerer x-posisjon til høyre side av ball.
int Ball::getRight()
{
  return getX()+_radius+1;
}
//Setter x-posisjon
void Ball::setX_(int x)
{
  _x = x;
}
//Setter y-posisjon
void Ball::setY_(int y)
{
  _y = y;
}
//Setter x-hastighet
void Ball::setVelX(int velX)
{
  xVel = velX;
}
//Setter y-hastighet
void Ball::setVelY(int velY)
{
  yVel = velY;
}
//Oppdaterer ballen
void Ball::update_()
{
  _x = _x+xVel;
  _y = _y+yVel;
}
//Tegner ball til display.
void Ball::display_(Adafruit_SSD1306* displayPointer)
{
  (*displayPointer).fillCircle(_x, _y, _radius, WHITE);
}
//Resetter ballens posisjon til gitt posisjon
void Ball::reset_(int velX, int velY, int xPos, int yPos)
{
  setVelX(velX);
  setVelY(velY);
  setX_(xPos);
  setY_(yPos);
}
//Constructor. Initialiserer platedata.
Paddle::Paddle(int xPos, int yPos, int len, int vel)
  :x(xPos),
  y(yPos),
  len(len),
  vel(vel)
{
}
//Returnerer x-posisjon til plate
int Paddle::getX()
{
  return x;
}
//Returnerer y-posisjon til plate
int Paddle::getY()
{
  return y;
}
//Returnerer y-posisjon til topp av plate
int Paddle::getTop()
{
  return getY();
}
//Returnerer y-posisjon til bunn av plate
int Paddle::getBottom()
{
  return getY()+len;
}
//Returnerer x-posisjon til venstre side av plate
int Paddle::getLeft()
{
  return getX();
}
//Returnerer x-posisjon til høyre side av plate
int Paddle::getRight()
{
  return getX();
}

//Tegner plate til display.
void Paddle::display_(Adafruit_SSD1306* displayPointer)
{
  (*displayPointer).writeFastVLine(x, y, len, WHITE);
}
//Constructor.
Score::Score(int xPos, int yPos, Adafruit_SSD1306* displayPointer)
  :_x(xPos-12),
  _y(yPos),
  playerScore(0),
  opponentScore(0)
{
  (*displayPointer).setTextSize(0);
  (*displayPointer).setTextColor(WHITE);
  (*displayPointer).setCursor(_x,_y);
}
//Oppdaterer score.
void Score::update_(int playerScoreChange, int opponentScoreChange)
{
  playerScore += playerScoreChange;
  opponentScore += opponentScoreChange;
}
//Tegner score til display.
void Score::display_(Adafruit_SSD1306* displayPointer)
{
  (*displayPointer).setCursor(_x,_y);
  (*displayPointer).print(String(playerScore));
  (*displayPointer).print(" - ");
  (*displayPointer).print(String(opponentScore));
}
