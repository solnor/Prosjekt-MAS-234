#include <FlexCAN.h>

//Lager et meldings-objekt
static CAN_message_t msg;

void setup(void)
{
  //Starter Can1
  Can1.begin();  
}

void loop()
{
  //Hvis det er tilgjengelig data på Can1, les meldingen, og send samme meldingen tilbake
  while (Can1.available()) 
  {
    Can1.read(msg);
    Can1.write(msg); 
  } 
  delay(50);
}
