////////////// ARDUINO ESCLAVO /////////

#include <Wire.h>
#include <I2C_Anything.h>

const byte I2C_SLAVE_ADDR = 0x20;


void setup() {
  Wire.begin(I2C_SLAVE_ADDR);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);

}

volatile float Gy, Gz;

void loop() {

  Serial.print(Gy);
  Serial.print("\t");
  Serial.println(Gz);

}

void receiveEvent(int howMany)
{

  //while (Wire.available())
  // {
  I2C_readAnything(Gy);
  I2C_readAnything(Gz);
  delay(100);

  //}
}
