#include <SPI.h>  
#include "RF24.h"

RF24 myRadio (7, 8);
byte addresses[][7] = {"canal1","canal2"};


char incomingByte;
int i=0,j=0;
char msg;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  
  myRadio.begin();  
  myRadio.setChannel(124); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ;
  myRadio.openReadingPipe(1, addresses[1]); 
  myRadio.openWritingPipe( addresses[0]);
  delay(1000);
  Serial.print("Funcionando");
}


void loop()
{
  if(Serial.available()>0)
  {       
          Serial.print("Eu:");
          while(Serial.available()>0)
               {
                  incomingByte= Serial.read(); 
                  Serial.print(incomingByte);
                  myRadio.write(&incomingByte, sizeof(char));
                }
  }             

                /*
                Serial.print("I received: ");
                Serial.println(incomingByte);
                for(j=0;j<i;j++){
                  Serial.println(incomingByte[j]);
                  myRadio.write(&incomingByte[j], sizeof(char)); 
                }
                msg=0;
                i=0;
                */
      


}
