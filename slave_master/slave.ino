/*
    Descripcion:
    Este arduino envia los datos
*/

////////////////////// ARDUINO MAESTRO /////////////////

#include "Wire.h";
const byte I2C_SLAVE_ADDR = 0x20;
int number = 12;

void setup()
{
    Wire.begin();
}
void loop()
{
    Wire.beginTransmission(I2C_SLAVE_ADDR);
    Wire.write(number);
    Wire.endTransmission();
}
