/*
 * Dette programmet antar at Can1 og Can0 er sammenkoblet
 */


#include <FlexCAN.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include <Adafruit_GFX.h>
#include <Fonts/FreeMono9pt7b.h>

#define OLED_DC     6
#define OLED_CS     10
#define OLED_RESET  5
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);

static CAN_message_t outMsg;
static CAN_message_t inMsg;

void setup(){
  //Starter Can0 og Can1
  Can0.begin(); 
  Can1.begin();
  //Starter displayet
  display.begin(SSD1306_SWITCHCAPVCC);
  //Fjerner det som allerede er på displayet
  display.clearDisplay();

  //Setter opp meldingsdata
  outMsg.ext = 0;
  outMsg.id = 0x000;
  outMsg.len = 4;
  for(int i = 0; i <= 4; i++)
  {
    outMsg.buf[i] = i;
  }
}

int textMargin = 5;
int numMessages = 7;
int numMessagesReceived = 0;
int lastMessageID = 0;
String messages[] = {"MAS-234 - Gruppe 10", "CAN Statistikk", "---------------", "Antall mottatt: ", "Mottok sist ID: ", "---------------", "IMU-maaling z: "};

void loop() {
  //Fjerner det som er på displayet fra forrige kjøring av løkka.
  display.clearDisplay();
  
  //Tegner rektangel som fungerer som en ramme på displayet
  display.drawRect(0,0, 128, 64, WHITE);

  //Setter tekststørrelse. Denne størrelsen gir en bredde på hvert symbol til fem piksler.
  display.setTextSize(0);
  //Setter tekstfargen til hvit.
  display.setTextColor(WHITE);

  //Går igjennom messages-listen og skriver hvert string-element til skjermen.
  for(int i = 0; i < 7; i++)
  {
    display.setCursor(textMargin,8*i+textMargin);
    display.println(messages[i]);
  }
  //Viser skjermen
  display.display();

  //Sender utmeldingen fra Can0
  Can0.write(outMsg);

  //Hvis det er tilgjengelig data på Can1, leses dette.
  while (Can1.available()) 
  {
    Can1.read(inMsg);
    numMessagesReceived++;
    lastMessageID = inMsg.id;
  }
  //Både melding og id endres
  outMsg.buf[0]++;
  outMsg.id++;

  //Endrer string-elementene i messages
  messages[3] = "Antall mottatt: " + String(numMessagesReceived);
  messages[4] = "Mottok sist ID: " + String(lastMessageID);
  
  delay(250);
}
