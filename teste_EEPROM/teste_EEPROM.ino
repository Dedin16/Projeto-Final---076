#include <Wire.h>

#define i2cAddress 0x50 //Endereço da memoria
 
void setup(void)
{
  Serial.begin(115200);
  Wire.begin();  //inicializando I2C

  int sizeBuf = 21;
  char buf[sizeBuf] = "teste gravando eeprom";
  
  Serial.print("Gravando... \n");
  for(int i = 0; i < sizeBuf; i++)
  {
    writeI2CEEPROM(i2cAddress,i,buf[i]);
  }
  
  Serial.print("Lendo... \n");
  for(int i = 0; i < sizeBuf; i++)
  {
    Serial.print((char)readI2CEEPROM(i2cAddress,i));  
  }
  Serial.print("\nFim... \n");
}


void loop() {}

void writeI2CEEPROM(int i2cAddr, byte memoryAddress, byte data)
{
  Wire.beginTransmission(i2cAddr);//iniciando barramento para o endereco I2C da memoria
  Wire.write(memoryAddress);//informando endereco da memoria 
  Wire.write(data);//dados
  Wire.endTransmission();//finalizando transmissao do emissor
  delay(5);
}

byte readI2CEEPROM(int i2cAddr, byte memoryAddress)
{
   byte data = 0xFF;
 
   Wire.beginTransmission(i2cAddr);//iniciando barramento para o endereco I2C da memoria
   Wire.write(memoryAddress);//informando endereco da memoria
   Wire.endTransmission();//finalizando transmissao do emissor
   Wire.requestFrom(i2cAddr,1);//avisando a memoria que espera-se dado (1 byte)
   if (Wire.available()){
     data = Wire.read();
  }
  return data;
}
