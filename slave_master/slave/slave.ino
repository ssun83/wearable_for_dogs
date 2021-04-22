/*
    Descripcion:
    Este arduino envia los datos
*/

////////////////////// ARDUINO MAESTRO /////////////////

#include <Wire.h>
#include <I2C_Anything.h>

const byte I2C_SLAVE_ADDR = 0x20;
float number = 12.5;
float number_2 = 10.4;

void setup()
{
  Wire.begin();
}
void loop()
{
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  I2C_writeAnything (number);
  I2C_writeAnything (number_2);
//  Wire.write(number);
//  Wire.write(number_2);
  Wire.endTransmission();
  number = number + 1.0;
  number_2 = number_2 + 1.0;
}
