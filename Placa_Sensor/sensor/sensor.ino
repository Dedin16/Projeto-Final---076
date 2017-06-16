#include <Wire.h>
#include <TimerOne.h>
#include <SPI.h>  
#include "RF24.h"

#define i2cAddress 0x50 //Endereço da EEPROM (I2C)

RF24 myRadio (7, 8);    //(CE,CSN)
byte addresses[][7] = {"canal1","canal2"};

char incomingByte;
char mensagem[1000];
int i=0, tempo=0;
char fim=0;



void setup()
{
  //Inicializa porta serial
  Serial.begin(115200);
  delay(1000);
  Serial.println("Funcionando");

  //Incializa Timer
  Timer1.initialize(500000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
  delay(1000);

  //Inicializa RF
  myRadio.begin();  
  myRadio.setChannel(124); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS );
  myRadio.openReadingPipe(1, addresses[1]); 
  myRadio.openWritingPipe( addresses[0]);
  delay(1000);
  myRadio.startListening();

  //Inicializa pino do sensor A0
  pinMode(A0, INPUT); 

  //Inicializa I2C
  Wire.begin();

  clear_mem();
  
}

void write_mem(int i2cAddr, byte memoryAddress, byte data)
{
  Wire.beginTransmission(i2cAddr);//iniciando barramento para o endereco I2C da EEPROM
  Wire.write(memoryAddress);      //informando endereco da memoria 
  Wire.write(data);          
  Wire.endTransmission();         //finalizando transmissao do emissor
  delay(5);
}

byte read_mem(int i2cAddr, byte memoryAddress)
{
   byte data = 0xFF;
 
   Wire.beginTransmission(i2cAddr);   //iniciando barramento para o endereco I2C da EEPROM
   Wire.write(memoryAddress);         //informando endereco da memoria
   Wire.endTransmission();            //finalizando transmissao do emissor
   Wire.requestFrom(i2cAddr,1);       //ler 1 byte
   if (Wire.available()){
     data = Wire.read();
  }
  return data;
}

int dados_mem(){
   int numero;
   byte numero_dados [2];
   numero_dados[0]= read_mem(i2cAddress,0);
   numero_dados[1]= read_mem(i2cAddress,1);
   numero= atoi(numero_dados);
   return numero;
  }

void clear_mem(){
   write_mem(i2cAddress,(0),'0');
   write_mem(i2cAddress,(1),'0');
}


void write_dado(byte dado){
   int num;
   byte num_dados [2];
   num_dados[0]= read_mem(i2cAddress,0);
   num_dados[1]= read_mem(i2cAddress,1);
   num = atoi(num_dados);
   num++;
   write_mem(i2cAddress,(num+1),dado);
   sprintf(num_dados, "%d",num);
   write_mem(i2cAddress,0,num_dados[0]);
   write_mem(i2cAddress,1,num_dados[1]);
  }

byte read_dado(int posicao){
  byte dado;
  dado=read_mem(i2cAddress,(posicao+1));
  return dado;
}

void timerIsr()
{   
    tempo++;
    //int sensorValue = analogRead(A0);    
    //float voltage = sensorValue*(5.0/1023.0);
    //float T=((voltage-0.6)*100);
    

}

void loop(){
    int sensorValue,lido=0;
    char valor[3];
    char valor_lido[3];

    if(tempo>8){
        sensorValue = analogRead(A0);
        sprintf(valor, "%d",sensorValue);
        Serial.print("O valor gravado é: ");
        Serial.println(valor);
        write_dado(valor[0]);
        write_dado(valor[1]);
        write_dado(valor[2]);
        valor_lido[0]=read_dado(1);
        valor_lido[1]=read_dado(2);
        valor_lido[1]=read_dado(3);
        lido= atoi(valor_lido);
        Serial.print("O valor lido é: ");
        Serial.println(valor_lido);
        tempo=0;
    }


  
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
            Serial.println("VIVO");
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



