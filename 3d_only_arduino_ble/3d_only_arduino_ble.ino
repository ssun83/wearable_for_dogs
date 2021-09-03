#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>
#include <MadgwickAHRS.h>
#include <math.h>
#include <Wire.h>
#include <MPU9250.h>
#include "SingleEMAFilterLib.h"

// set BLE services
BLEService WearableService("19B10010-E8F2-537E-4F6C-D104768A1214");
BLEStringCharacteristic WearableROM("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 3);

// IMU variables
float Mx, My, Mz, Ax, Ay, Az, Gx, Gy, Gz;
float rolll, pitchl, yawl;
float rollp, pitchp, yawp;
uint32_t previousMillis = 0;

// conversion
float toRad = 2.0 * 3.1416 / 360.0;
float toDeg = 1.0 / toRad;

#define gscale 1;

Madgwick filter;
MPU9250 mpu;

SingleEMAFilter<float> singleEMAFilter(0.6);

void setup()
{
  // start serial monitor
  Serial.begin(115200);

  // Start LSM9DS1 communication
  if (!IMU.begin())
  {
    while (1);
  }

  // configuracion acelerometro
  IMU.setAccelFS(2);  // Rangos -> 0: 2g, 1: 24g, 2: 4g, 3: 8g
  IMU.setAccelODR(3); // Rangos -> 1: 10Hz, 2: 50Hz, 3: 119Hz, 4: 238Hz, 5: 476Hz
  IMU.setAccelOffset(-0.003552, -0.023984, -0.003608);
  IMU.setAccelSlope (0.999958, 0.990807, 1.000167);

  // configuracion giroscopio
  IMU.gyroUnit = DEGREEPERSECOND;
  IMU.setGyroFS(3); // Rangos -> 0: 245º/s, 1: 500º/s, 2: 1000º/s, 3: 2000º/s
  IMU.setGyroODR(3); // Rangos -> 1: 10Hz, 2: 50Hz, 3: 119Hz, 4: 238Hz, 5: 476Hz
  IMU.setGyroOffset (0.629517, 0.368896, 0.614380);
  IMU.setGyroSlope (1.251912, 1.274917, 1.439069);

  // configuracion magnetomet ro
  IMU.setMagnetFS(0); // Rangos -> 0: 400uT, 1: 800uT, 2: 1200uT, 3: 1600uT
  IMU.setMagnetODR(7); // Rangos -> 1: 1.25Hz, 2: 2.5Hz, 3: 5Hz, 4: 10Hz, 5: 20Hz, 6: 40Hz, 7:80Hz, 8:400Hz
  IMU.setMagnetOffset(-0.401611, 30.146484, -24.879761);
  IMU.setMagnetSlope (1.178329, 1.178055, 1.205408);

  // filter rate
  float sensorRate = min(IMU.getGyroODR(), IMU.getMagnetODR());
  filter.begin(sensorRate);

  // BLE initialization
  if (!BLE.begin()) {
    while (1);
  }

  BLE.setLocalName("WOOFlex");
  BLE.setAdvertisedService(WearableService);
  WearableService.addCharacteristic(WearableROM);
  BLE.addService(WearableService);
  WearableROM.writeValue("0");
  BLE.advertise();
}

String rom_ble;

void loop()
{
  static int cuenta = 0;
  static float rollm = 1.0;
  static float pitchm = 1.0;
  static float yawm = 1.0;
  static float x1 = 1;
  static float y1 = 1;
  static float z1 = 1;

  uint32_t currentMillis = millis();
  if (currentMillis - previousMillis >= 50) {
    previousMillis = currentMillis;

    BLE.poll();

    float joint_angle = 0.0;

    if (IMU.accelAvailable() && IMU.gyroAvailable() && IMU.magnetAvailable())
    {
      IMU.readAccel(Ax, Ay, Az);
      IMU.readGyro(Gx, Gy, Gz);
      IMU.readMagnet(Mx, My, Mz);
    }

    Gx = -Gx;
    Ax = -Ax;

    // Madgwick filter and euler angles
    filter.update(Gx, Gy, Gz, Ax, Ay, Az, Mx, My, Mz);
    rolll = filter.getRoll();
    pitchl = filter.getPitch();
    yawl = filter.getYaw();

    if (cuenta == 50) {
      rollm = rolll;
      pitchm = pitchl;
      yawm = yawl;
      
      // convert to rad
      yawm = -(yawm * toRad);
      pitchm = -(pitchm * toRad);
      rollm = (rollm - 180) * 57.29578;
      x1 = cos(yawm) * cos(pitchm);
      y1 = sin(yawm) * cos(pitchm);
      z1 = sin(pitchm);
    }

    yawl = -(yawl * toRad);
    pitchl = -(pitchl * toRad);
    rolll = (rolll - 180) * 57.29578;

    float x2 = cos(yawl) * cos(pitchl);
    float y2 = sin(yawl) * cos(pitchl);
    float z2 = sin(pitchl);

    float theta1 = atan2(y1, x1);
    float theta2 = atan2(y2, x2);
    joint_angle = theta1 - theta2;

    if (joint_angle < 0) {
      joint_angle = joint_angle + 2 * 3.1416;
    }

    joint_angle = joint_angle * toDeg;

    // filter the angle to be less jumpy
    singleEMAFilter.AddValue(joint_angle);

    // write values on BLE
    rom_ble = String(int(singleEMAFilter.GetLowPass()));
    WearableROM.writeValue(rom_ble);
    
    Serial.println(rom_ble);
    
    cuenta++;
  }
}
