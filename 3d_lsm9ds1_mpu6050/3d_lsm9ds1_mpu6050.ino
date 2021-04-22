/*
  Descripcion:
  - Este codigo enviar los angles para el visualizador 3d
*/

// library
#include <Wire.h>
#include <Arduino_LSM9DS1.h>
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
Madgwick filter2;

void setup() {

  // start serial monitor
  Serial.begin(115200);

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

  // configuracion magnetometro
  IMU.setMagnetFS(0); // Rangos -> 0: 400uT, 1: 800uT, 2: 1200uT, 3: 1600uT
  IMU.setMagnetODR(7); // Rangos -> 1: 1.25Hz, 2: 2.5Hz, 3: 5Hz, 4: 10Hz, 5: 20Hz, 6: 40Hz, 7:80Hz, 8:400Hz
  IMU.setMagnetOffset(-5.789795, 32.664185, -18.919678);
  IMU.setMagnetSlope(2.488734, 2.438466, 2.596766);
  //  IMU.setMagnetOffset(0, 0, 0); //  uncalibrated
  //  IMU.setMagnetSlope (1, 1, 1); //  uncalibrated

  float sensorRate = min(IMU.getGyroODR(), IMU.getMagnetODR());
  //  Serial.println(sensorRate);
  //  Serial.println(IMU.getMagnetODR());
  //  Serial.println(IMU.getGyroODR());
  filter.begin(sensorRate);
  filter2.begin(sensorRate);
}

void loop() {

  // raw values
  int16_t _Ax, _Ay, _Az, _Gx, _Gy, _Gz, t1;
  float Ax, Ay, Az, Gx, Gy, Gz;
  float _Ax2, _Ay2, _Az2, _Gx2, _Gy2, _Gz2, _Mx2, _My2, _Mz2;
  float Ax2, Ay2, Az2, Gx2, Gy2, Gz2, Mx2, My2, Mz2;

  // euler angles
  float roll, pitch, yaw;
  float roll2, pitch2, yaw2;

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
    IMU.readMagnet(_Mx2, _My2, _Mz2);
  }
  Ax2 = -_Ay2; Ay2 = -_Ax2; Az2 = _Az2;
  Gx2 = -_Gy2; Gy2 = -_Gx2; Gz2 = _Gz2;
  Mx2 = -_Mx2; My2 = -_My2; Mz2 = _Mz2;

  // update filter
  filter.updateIMU(Gx, Gy, Gz, Ax, Ay, Az);
  // filter2.updateIMU(Gx2, Gy2, Gz2, Ax2, Ay2, Az2);
  filter2.update(Gx2, Gy2, Gz2, Ax2, Ay2, Az2, Mx2, My2, Mz2);

  // get euler angles
  roll = filter.getRoll();
  pitch = filter.getPitch();
  yaw = filter.getYaw();

  roll2 = filter2.getRoll();
  pitch2 = filter2.getPitch();
  yaw2 = filter2.getYaw();

  // print euler angles
  Serial.print(yaw);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.print(roll);
  Serial.print(",");
  Serial.print(yaw2);
  Serial.print(",");
  Serial.print(pitch2);
  Serial.print(",");
  Serial.println(roll2);

  //  Serial.print(yaw);
  //  Serial.print(",");
  //  Serial.print(yaw2);
  //  Serial.print(",");
  //  Serial.print(roll);
  //  Serial.print(",");
  //  Serial.print(roll2);
  //  Serial.print(",");
  //  Serial.print(pitch);
  //  Serial.print(",");
  //  Serial.println(pitch2);

  delay(10);

}
