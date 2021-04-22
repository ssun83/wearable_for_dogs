/*
    Descripcion:
    Este arduino recibe los datos
*/

////////////// ARDUINO ESCLAVO /////////

#include "Wire.h"

const byte I2C_SLAVE_ADDR = 0x20;

void setup()
{
    Wire.begin(I2C_SLAVE_ADDR);
    Wire.onReceive(receiveEvent);
    Serial.begin(9600);
}
void loop()
{
    int number = 0;
    while (Wire.available())
    {
        randNumber = Wire.read();
        Serial.println(randNumber);
    }
}
