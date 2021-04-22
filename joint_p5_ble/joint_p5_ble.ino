/*
  Descripcion:
  Este codigo usa la tecnologia BLE y p5.js
*/

// libraries
#include <ArduinoBLE.h>
#include <Wire.h>
#include <Arduino_LSM9DS1.h>
#include <MadgwickAHRS.h>

// set BLE services
BLEService WearableService("19B10010-E8F2-537E-4F6C-D104768A1214");
BLEStringCharacteristic WearableROM("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, 3);

// variables
#define MPU 0x68              // I2c address, IMU 1
#define A_R 16384.0           // convertion ratio accelerometer
#define G_R 131.0             // convertion ratio gyroscope
#define RAD_A_DEG = 57.295779 // convertion rad to degrees 180/pi
// #define gscale 70e-3; // Angular rate FS =+-2000
#define gscale 1;

// start filters
Madgwick filter;
Madgwick filter2;

void setup() {

  // start serial
  Serial.begin(115200);
  while (!Serial);

  // start MPU 1 communication
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(byte(0));
  Wire.endTransmission(true);

  // start MPU 2 communication
  if (!IMU.begin())
  {
    while (1);
  }
  // configuracion acelerometro
  IMU.setAccelFS(2);  // Rangos -> 0: 2g, 1: 24g, 2: 4g, 3: 8g
  IMU.setAccelODR(3); // Rangos -> 1: 10Hz, 2: 50Hz, 3: 119Hz, 4: 238Hz, 5: 476Hz
  IMU.setAccelOffset(0.000091, -0.028406, -0.007399);
  IMU.setAccelSlope(1.000338, 0.993624, 1.001161);
  //  IMU.setAccelOffset(0, 0, 0);  //   uncalibrated
  //  IMU.setAccelSlope (1, 1, 1);  //   uncalibrated

  // configuracion giroscopio
  IMU.gyroUnit = DEGREEPERSECOND;
  // IMU.gyroUnit = RADIANSPERSECOND;
  IMU.setGyroFS(3); // Rangos -> 0: 245º/s, 1: 500º/s, 2: 1000º/s, 3: 2000º/s
  IMU.setGyroODR(3); // Rangos -> 1: 10Hz, 2: 50Hz, 3: 119Hz, 4: 238Hz, 5: 476Hz
  IMU.setGyroOffset(0.424835, 0.507385, -0.077393);
  IMU.setGyroSlope(1.175011, 1.000000, 1.140555);
  //  IMU.setGyroOffset (0, 0, 0);  // = uncalibrated
  //  IMU.setGyroSlope  (1, 1, 1);  // = uncalibrated

  float sensorRate = min(IMU.getGyroODR(), IMU.getMagnetODR());
  //  Serial.println(sensorRate);
  //  Serial.println(IMU.getMagnetODR());
  //  Serial.println(IMU.getGyroODR());
  filter.begin(sensorRate);
  filter2.begin(sensorRate);

  // BLE initialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("WearableROM");
  BLE.setAdvertisedService(WearableService);
  WearableService.addCharacteristic(WearableROM);
  BLE.addService(WearableService);
  WearableROM.writeValue("0");
  BLE.advertise();
//  Serial.println("Bluetooth device active, waiting for connections...");

}

String rom_ble;

void loop() {

  BLE.poll();

  // raw values
  int16_t _Ax, _Ay, _Az, _Gx, _Gy, _Gz, t1;
  float Ax, Ay, Az, Gx, Gy, Gz;
  float _Ax2, _Ay2, _Az2, _Gx2, _Gy2, _Gz2;
  float Ax2, Ay2, Az2, Gx2, Gy2, Gz2;

  // euler angles
  float roll, pitch, yaw;
  float roll2, pitch2, yaw2;
  float rom_ori, rom_ij;

  // read MPU 1 data
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);
  _Ax = Wire.read() << 8 | Wire.read();
  _Ay = Wire.read() << 8 | Wire.read();
  _Az = Wire.read() << 8 | Wire.read();
  t1 = Wire.read() << 8 | Wire.read();
  _Gx = Wire.read() << 8 | Wire.read();
  _Gy = Wire.read() << 8 | Wire.read();
  _Gz = Wire.read() << 8 | Wire.read();
  Wire.endTransmission();
  Ax = float(_Ax) / A_R;  Ay = float(_Ay) / A_R;  Az = float(_Az) / A_R;
  Gx = float(_Gx) / G_R;  Gy = float(_Gy) / G_R;  Gz = float(_Gz) / G_R;

  // read MPU 2 data
  if (IMU.accelAvailable() && IMU.gyroAvailable())
  {
    // read IMU raw value
    IMU.readAccel(_Ax2, _Ay2, _Az2);
    IMU.readGyro(_Gx2, _Gy2, _Gz2);
  }
  Ax2 = -_Ay2; Ay2 = -_Ax2; Az2 = _Az2;
  Gx2 = -_Gy2; Gy2 = -_Gx2; Gz2 = _Gz2;

  // update filter
  filter.updateIMU(Gx, Gy, Gz, Ax, Ay, Az);
  filter2.updateIMU(Gx2, Gy2, Gz2, Ax2, Ay2, Az2);

  // get euler angles
  roll = filter.getRoll();
  // pitch = filter.getPitch();
  // yaw = filter.getYaw();

  roll2 = filter2.getRoll();
  // pitch2 = filter2.getPitch();
  // yaw2 = filter2.getYaw();

  // calculate rom
  rom_ori = 180 - (roll2 - roll);

  if (roll2 < 0 && -90 < roll2 && roll < -90 && -180 < roll) {
    rom_ij = 360 - rom_ori;
  }
  else if (roll2 < 0 && -90 < roll2 && roll > 0 && roll < 90) {
    rom_ij = 360 - rom_ori;
  }
  else if (roll2 < -90 && -180 < roll2 && roll > 0 && roll < 90) {
    rom_ij = rom_ori - 360;
  }
  else {
    rom_ij = rom_ori;
  }

  // write values on BLE
  rom_ble = String(int(rom_ij));
  //Serial.print(roll); Serial.print(","); Serial.println(roll2);
  Serial.println(rom_ij);
  WearableROM.writeValue(rom_ble);

  delay(10);
}
