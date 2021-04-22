/*
  Descripcion:
  - Codigo 3D para el MPU6050
*/

// library
#include <Wire.h>
#include <MadgwickAHRS.h>

// variables
#define MPU 0x68              // I2c address, IMU 1
#define A_R 16384.0           // convertion ratio accelerometer
#define G_R 131.0             // convertion ratio gyroscope
#define RAD_A_DEG = 57.295779 // convertion rad to degrees 180/pi
// #define gscale 70e-3; // Angular rate FS =+-2000
#define gscale 1;

// start filters
Madgwick filter;

void setup() {

  // start serial monitor
  Serial.begin(115200);

  // start MPU 1 communication
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(byte(0));
  Wire.endTransmission(true);

  float sensorRate = 100;
  //  Serial.println(sensorRate);
  //  Serial.println(IMU.getMagnetODR());
  //  Serial.println(IMU.getGyroODR());
  filter.begin(sensorRate);
}

void loop() {

  // raw values
  int16_t _Ax, _Ay, _Az, _Gx, _Gy, _Gz, t1;
  float Ax, Ay, Az, Gx, Gy, Gz;

  // euler angles
  float roll, pitch, yaw;

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

  // update filter
  filter.updateIMU(Gx, Gy, Gz, Ax, Ay, Az);

  // get euler angles
  roll = filter.getRoll();
  pitch = filter.getPitch();
  yaw = filter.getYaw();

  // print euler angles
  Serial.print(yaw);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.println(roll);

  delay(10);

}
