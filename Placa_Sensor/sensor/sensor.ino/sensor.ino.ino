#include <Wire.h>
#include <TimerOne.h>
#include <SPI.h>  
#include "RF24.h"

#define i2cAddress 0x50 //Endereço da EEPROM (I2C)

RF24 myRadio (7, 8);    //(CE,CSN)
byte addresses[][4] = {"ch1","ch2"};

char incomingByte;
char mensagem[100];
int i=0, tempo=0;
char fim=0, temperatura=0;
int tempo_aquisicao=10;



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
  delay(1000); 

  //Inicializa I2C
  Wire.begin();
  delay(1000);
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

void grava_temperatura()
{
        int sensorValue = analogRead(A0);
        byte hiByte = highByte(sensorValue);
        byte loByte = lowByte(sensorValue);
        write_dado(hiByte);
        write_dado(loByte);
  
}

void loop(){
    
    if(tempo>tempo_aquisicao){
        grava_temperatura();
        int tamanho=dados_mem();
        byte valor_lido_hi =read_dado(tamanho-1);
        byte valor_lido_lo =read_dado(tamanho);
        int eepromValue = (valor_lido_hi<<8) + valor_lido_lo;
        float voltage = eepromValue*(5.0/1023.0);
        float T=((voltage-0.6)*100);
        Serial.println("A última temperatura gravada foi:");
        Serial.print(T);
        Serial.println("ºC");
        Serial.println("DADO em HEX:");
        Serial.print(valor_lido_hi, HEX);
        Serial.println(valor_lido_lo, HEX);
        Serial.print("DAdos na memória:  ");
        Serial.println(tamanho);

        char  enter='\n';
        /*
        unsigned char lido_lo=valor_lido_lo;
        unsigned char lido_hi=valor_lido_hi;
        
        char  enter='\n';

        Serial.println(lido_hi,HEX);
        Serial.println(lido_lo,HEX);
        */
        
        myRadio.stopListening();      //Modo envio
    
        if(!myRadio.write(&valor_lido_hi, sizeof(byte)))   //Envia caracter para RF
              {
                   Serial.println("Erro na Transmisão");
              }
       
        if(!myRadio.write(&valor_lido_lo, sizeof(byte)))   //Envia caracter para RF
              {
                   Serial.println("Erro na Transmisão");
              }

        if(!myRadio.write(&enter, sizeof(char)))   //Envia caracter para RF
              {
                   Serial.println("Erro na Transmisão");
              }
        
        myRadio.startListening();   //Modo escuta
        
        tempo=0;
        temperatura=1;
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

  if(temperatura)
  {
    temperatura=0;
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

  if(fim)   //Trata a mensagem recebida
  {
      Serial.print("Pessoa:");
      Serial.println(mensagem);
      fim=0;
  }
  }



