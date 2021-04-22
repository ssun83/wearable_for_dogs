// Code: Send 3 random numeros using BLE

// libraries
#include <ArduinoBLE.h>

// config BLE service
BLEService randomService("19B10010-E8F2-537E-4F6C-D104768A1214");
BLEStringCharacteristic randomvaluesCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 11);

void setup(){
  Serial.begin(9600);
  while(!Serial);

  // BLE initialization
  if(!BLE.begin()){
    Serial.println("Starting BLE failed!");
    while(1);
  }

  // config BLE
  BLE.setLocalName("RandomBLE");
  BLE.setAdvertisedService(randomService);
  randomService.addCharacteristic(randomvaluesCharacteristic);
  BLE.addService(randomService);
  randomvaluesCharacteristic.writeValue("0,0,0");
  BLE. advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop(){

  // poll for BLE events
  BLE.poll();

  // generate random values
  int var1 = random(0,999);
//  int var2 = random(0,999);
//  int var3 = random(0,999);
  //String message = String(var1) + "," + String(var2) + "," + String(var3);
  String message = String(var1);

  // write values on ble
  Serial.println(message);
  randomvaluesCharacteristic.writeValue(message);
}
