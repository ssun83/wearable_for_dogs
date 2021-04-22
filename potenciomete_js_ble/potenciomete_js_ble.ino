#include <ArduinoBLE.h>

BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create service

// create switch characteristic and allow remote device to read and write
//BLEByteCharacteristic ledCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
// create button characteristic and allow remote device to get notifications
BLEIntCharacteristic buttonCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

int sensorValue = 0;
int sensorPin = A0;    // select the input pin for the potentiometer

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("PotenciometerBLE");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(ledService);

  // add the characteristics to the service
  //ledService.addCharacteristic(ledCharacteristic);
  ledService.addCharacteristic(buttonCharacteristic);

  // add the service
  BLE.addService(ledService);

  //ledCharacteristic.writeValue(0);
  buttonCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // poll for BLE events
  BLE.poll();

  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue/4);
  buttonCharacteristic.writeValue(sensorValue/4);
  
//  if (millis() % 1000 == 0) {
//
//    sensorValue --;
//    if (sensorValue < 0) {
//      sensorValue = 255;
//    }
//    Serial.println(sensorValue);
//    buttonCharacteristic.writeValue(sensorValue);
//
//  }

}
