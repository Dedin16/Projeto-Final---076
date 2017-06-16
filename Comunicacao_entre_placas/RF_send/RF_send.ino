#include <SPI.h>  
#include "RF24.h"

RF24 myRadio (7, 8);    //(CE,CSN)
byte addresses[][7] = {"canal1","canal2"};


char incomingByte;
char mensagem[1000];
int i=0;
char fim=0;


void setup()
{
  Serial.begin(115200);
  delay(1000);
  
  myRadio.begin();  
  myRadio.setChannel(124); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS );
  myRadio.openReadingPipe(1, addresses[1]); 
  myRadio.openWritingPipe( addresses[0]);
  delay(1000);
  Serial.println("Funcionando");
  myRadio.startListening();
}


void loop()
{

  //Transmissao
  if(Serial.available()>0)        //Se ha dados na porta serial
  {       
          myRadio.stopListening();      //Modo envio
          Serial.print("Eu:");
          while(Serial.available()>0)   //Ate terminar leitura dos dados
               {
                  incomingByte= Serial.read(); 
                  Serial.print(incomingByte);
                  if(!myRadio.write(&incomingByte, sizeof(char)))   //Envia caracter para RF
                  {
                    Serial.println("Erro na Transmisão");
                  }
                }
          myRadio.startListening();   //Modo escuta
  }               

  //Recepcao
  if (myRadio.available())       //Ha dados vindos do RF
  {
    while (myRadio.available())
    {
        myRadio.read( &incomingByte, sizeof(incomingByte) );        //Le caracter do RF
        if(incomingByte!='\n')
        {
            mensagem[i]= incomingByte;
            i++;
        }
        else
        {
            mensagem[i]='\0';                                       // Caracter de fim de linha
            i=0;
            fim=1;
        }
      
    }

  }

  if(fim)   //Se houver mensagem recebida
  {
      Serial.print("Pessoa:");
      Serial.println(mensagem);
      fim=0;
  }

}
