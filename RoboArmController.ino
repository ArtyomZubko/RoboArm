#include <Servo.h> 
 
Servo myservo1,myservo2,myservo3,myservo4; 

 
void setup() 
{ 
  Serial.begin(9600);
 
  myservo1.attach(12); 
  myservo2.attach(11);
  myservo3.attach(10);
  myservo4.attach(9);
} 

byte packetBuffer[5];

void loop() 
{ 
   while (Serial.available() > 0) {
      byte firstByte = Serial.read() ; //считывание первого байта
      if (firstByte != '*') //если байт не является началом пакета, продолжить поиск
        continue;
      memset(packetBuffer,0x0,sizeof(byte)*5); //обнуление приёмного буфера

      if (Serial.readBytes(packetBuffer, 5) == 5){ //если в буфер записалось 5 байт
        if (packetBuffer[4] == '#'){ //если пакет имеет корректный конец
          myservo1.write(packetBuffer[0]);
          myservo2.write(packetBuffer[1]); //запись данных буфера в сервы
          myservo3.write(packetBuffer[2]);
          myservo4.write(packetBuffer[3]);
        }
      }
      else{
        continue;
      }
   }

}
