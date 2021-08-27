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

// LED variables
int red_light_pin = 11;
int green_light_pin = 10;
int blue_light_pin = 9;

// IMU variables
float Mx, My, Mz, Ax, Ay, Az, Gx, Gy, Gz;
float rolll, pitchl, yawl;
float rollm, pitchm, yawm, _yawm;

// conversion
float toRad = 2.0 * 3.1416 / 360.0;
float toDeg = 1.0 / toRad;

#define IMU_SAMPLERATE_DELAY_MS (10)
#define gscale 1;

Madgwick filter;
MPU9250 mpu;

SingleEMAFilter<float> singleEMAFilter(0.6);

void setup()
{
  // start serial monitor
  Serial.begin(115200);

  // MPU-9250 connection
  Wire.begin();

  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(500);
    }
  }

  // MPU 9250 calibration values
  mpu.setAccBias(-106.12, 66.36, -46.94);
  mpu.setGyroBias(3.86, 2.24, 0.72);
  mpu.setMagBias(236.42, 395.35, -311.45);
  //mpu.setMagneticDeclination(-2.36); // Lima, Peru declination
  mpu.setMagneticDeclination(+13.2); // Davis declination

  // Start LSM9DS1 communication
  if (!IMU.begin())
  {
    while (1);
  }

  // configuracion acelerometro PERU
//  IMU.setAccelFS(2);  // Rangos -> 0: 2g, 1: 24g, 2: 4g, 3: 8g
//  IMU.setAccelODR(3); // Rangos -> 1: 10Hz, 2: 50Hz, 3: 119Hz, 4: 238Hz, 5: 476Hz
//  IMU.setAccelOffset(-0.003552, -0.023984, -0.003608);
//  IMU.setAccelSlope (0.999958, 0.990807, 1.000167);

//Davis
  IMU.setAccelFS(2);
  IMU.setAccelODR(3);
  IMU.setAccelOffset(-0.006404, -0.014818, -0.002058);
  IMU.setAccelSlope (0.996882, 0.992155, 0.986763);

  // configuracion giroscopio peru
//  IMU.gyroUnit = DEGREEPERSECOND;
//  IMU.setGyroFS(3); // Rangos -> 0: 245º/s, 1: 500º/s, 2: 1000º/s, 3: 2000º/s
//  IMU.setGyroODR(3); // Rangos -> 1: 10Hz, 2: 50Hz, 3: 119Hz, 4: 238Hz, 5: 476Hz
//  IMU.setGyroOffset (0.629517, 0.368896, 0.614380);
//  IMU.setGyroSlope (1.251912, 1.274917, 1.439069);

  //davis
  IMU.setGyroFS(3);
  IMU.setGyroODR(3);
  IMU.setGyroOffset (1.540039, -0.151611, 0.713135);
  IMU.setGyroSlope (1.132959, 1.196907, 1.32000);


  // configuracion magnetomet ro PERU
//  IMU.setMagnetFS(0); // Rangos -> 0: 400uT, 1: 800uT, 2: 1200uT, 3: 1600uT
//  IMU.setMagnetODR(7); // Rangos -> 1: 1.25Hz, 2: 2.5Hz, 3: 5Hz, 4: 10Hz, 5: 20Hz, 6: 40Hz, 7:80Hz, 8:400Hz
//  IMU.setMagnetOffset(-0.401611, 30.146484, -24.879761);
//  IMU.setMagnetSlope (1.178329, 1.178055, 1.205408);

  //Davis
  IMU.setMagnetFS(0);
  IMU.setMagnetODR(7);
  IMU.setMagnetOffset(-24.375000, 145.971069, 44.835815);
  IMU.setMagnetSlope (0.129235, 0.191106, 0.136685);


  // filter rate
  float sensorRate = min(IMU.getGyroODR(), IMU.getMagnetODR());
  filter.begin(sensorRate);

  // RGB LED
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  // BLE initialization
  if (!BLE.begin()) {
    while (1);
  }

  BLE.setLocalName("WearableROM");
  BLE.setAdvertisedService(WearableService);
  WearableService.addCharacteristic(WearableROM);
  BLE.addService(WearableService);
  WearableROM.writeValue("0");
  BLE.advertise();

}

String rom_ble;

void loop()
{
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

  // mpu9250 values
  if (mpu.update()) {
    rollm = -mpu.getRoll() ;
    pitchm = -mpu.getPitch();
    _yawm = mpu.getYaw();
    if (_yawm < 0) {
      yawm = _yawm + 360.0;
    }
    else {
      yawm = _yawm;
    }
  }

  // convert to rad
  yawm = yawm * toRad;
  // pitchm = pitchm * toRad;
  // rollm = (rollm - 180) * 57.29578;
  pitchm = 0.0;
  rollm = 0.0;

  yawl = -(yawl * toRad);
  // pitchl = -(pitchl * toRad);
  // rolll = (rolll - 180) * 57.29578;
  pitchl = 0.0;
  rolll = 0.0;

  float x1 = -cos(yawm) * cos(pitchm);
  float y1 = sin(yawm) * cos(pitchm);
  float z1 = sin(pitchm);

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

  // Serial.println(joint_angle);

  // write values on BLE
  rom_ble = String(int(singleEMAFilter.GetLowPass()));
  WearableROM.writeValue(rom_ble);

  // RGB LED
  if (joint_angle < 40) {
    RGB_color(255, 0, 0); // Red
  }
  else if (joint_angle >= 40 and joint_angle < 60) {
    RGB_color(237, 109, 0); // Orange
  }
  else if (joint_angle >= 60 and joint_angle < 160) {
    RGB_color(0, 255, 0); // Green
  }
  else if (joint_angle >= 160 and joint_angle < 170) {
    RGB_color(237, 109, 0); // Orange
  }
  else if (joint_angle >= 170) {
    RGB_color(255, 0, 0); // Red
  }



  delay(IMU_SAMPLERATE_DELAY_MS);

}

// RGB LED colors
void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(red_light_pin, 255 - red_light_value);
  analogWrite(green_light_pin, 255 - green_light_value);
  analogWrite(blue_light_pin, 255 - blue_light_value);
}
