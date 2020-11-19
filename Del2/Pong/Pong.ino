#include "Pong.h"

int groupNumberPlayer = 10;
int groupNumberOpponent = 10;

//Joystick-pinner
const int JOY_LEFT = 18;
const int JOY_RIGHT = 17;
const int JOY_CLICK =19;
const int JOY_UP  = 22;
const int JOY_DOWN = 23;

//Lager et Pong-objekt
Pong pong;

void setup() {
  //Starter Serial - brukes kun til debugging i Serial Monitor
  Serial.begin(9600);
  //Starter Can0 og Can1. Disse må være koblet sammen for fler-spiller
  Can0.begin();
  Can1.begin();
  //Setter medlemmene groupNumberPlayer og groupNumberOpponent. Dette brukes til identifikasjon på meldingene som sendes over CAN.
  pong.groupNumberPlayer = groupNumberPlayer;
  pong.groupNumberOpponent = groupNumberOpponent;
  //Setter joystick pinnene til input og til pullup, slik at pinnene ikke fluktuerer mellom høy og lav verdi.
  pinMode(JOY_UP, INPUT_PULLUP);
  pinMode(JOY_DOWN, INPUT_PULLUP); 
  pinMode(JOY_CLICK, INPUT_PULLUP);

  delay(1000);
  //Før spillet starter, venter det på input fra både nåværende spiller eller motspiller
  while(pong.playing == false)
  {
    inputHandler();
    pong.waitForInput();
  }
}


void loop() {
  //Hvis pong-spillet er startet, kjør play-funksjonen
  if(pong.playing)
  {
    play();
  }
}
//Denne funksjonen sjekker etter inputs fra spiller, oppdaterer og viser grafikk. Et delay på 20ms er brukt for å unngå at ballen går for fort.
void play()
{
  inputHandler();
  pong.update_();
  pong.show();
  delay(20);
}
//Her settes boolean-verdier basert på bruker-input. Spillogikken bruker dette senere blant annet til oppdatering av spillerplater
void inputHandler()
{
  // Hvis joysticken er i øvre posisjon:
  if(!digitalRead(JOY_UP))
  {
    pong.isJoyUp = true;
  }
  else if(digitalRead(JOY_UP))
  {
    pong.isJoyUp = false;
  }
  // Hvis joysticken er i nedre posisjon:
  if(!digitalRead(JOY_DOWN))
  {
    pong.isJoyDown = true;
  }
  else if(digitalRead(JOY_DOWN))
  {
    pong.isJoyDown = false;
  }
  // Hvis joysticken er trykket ned:
  if(!digitalRead(JOY_CLICK))
  {
    pong.isJoyClicked = true;
  }
  else if(digitalRead(JOY_CLICK))
  {
    pong.isJoyClicked = false;
  }
}
