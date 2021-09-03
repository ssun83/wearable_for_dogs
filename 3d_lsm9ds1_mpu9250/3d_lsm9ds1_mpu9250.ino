/*
  Descripcion
  Este codigo enviar los angulos al visualizador 3D del arduino y mpu9250
*/

// library
#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <math.h>
#include <MadgwickAHRS.h>
#include <Wire.h>
#include <MPU9250.h>

// variables
#define gscale 1;

// variables de conversion
float toRad = 2.0 * 3.1416 / 360.0;
float toDeg = 1.0 / toRad;
#define DELAY_MS (10)
#define gscale 1;

// start filters
Madgwick filter;
MPU9250 mpu;

void setup() {

  // Start serial monitor
  Serial.begin(115200);

  // MPU 9250 connection
  Wire.begin();

  if (!mpu.setup(0x68)) {
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(500);
    }
  }

  // MPU 9250 calibration values
  mpu.setAccBias(-106.12, 66.36, -46.94);
  mpu.setGyroBias(3.86, 2.24, 0.72);
  mpu.setMagBias(236.42, 395.35, -311.45);
  mpu.setMagScale(0.91, 1.10, 1.01);
  mpu.setMagneticDeclination(-2.36);

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

  float sensorRate = min(IMU.getGyroODR(), IMU.getMagnetODR());
  filter.begin(sensorRate);
}

void loop() {

  // raw values for lsm9ds1
  static float _Ax2, _Ay2, _Az2, _Gx2, _Gy2, _Gz2, _Mx2, _My2, _Mz2;
  static float Ax2, Ay2, Az2, Gx2, Gy2, Gz2, Mx2, My2, Mz2;

  // euler angles
  static float roll, pitch, yaw;
  static float rollm, pitchm, yawm, _yawm;

  static uint32_t previousMillis = 0;
  uint32_t currentMillis = millis();

  if (currentMillis - previousMillis >= 100) {
    previousMillis = currentMillis;
    // read MPU LSM9DS1 data
    if (IMU.accelAvailable() && IMU.gyroAvailable() && IMU.magnetAvailable())
    {
      // read IMU raw value
      IMU.readAccel(_Ax2, _Ay2, _Az2);
      IMU.readGyro(_Gx2, _Gy2, _Gz2);
      IMU.readMagnet(_Mx2, _My2, _Mz2);
    }

    Ax2 = -_Ax2; Ay2 = _Ay2; Az2 = _Az2;
    Gx2 = -_Gx2; Gy2 = _Gy2; Gz2 = _Gz2;
    Mx2 = _Mx2; My2 = _My2; Mz2 = _Mz2;

    // update filter
    filter.update(Gx2, Gy2, Gz2, Ax2, Ay2, Az2, Mx2, My2, Mz2);

    // get euler angles from lsm9ds1
    roll = filter.getRoll();
    pitch = filter.getPitch();
    yaw = filter.getYaw();

    // get euler angles from mpu9250
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

    // send angles
    Serial.print(yawm);
    Serial.print(",");
    Serial.print(pitchm);
    Serial.print(",");
    Serial.print(rollm);
    Serial.print(",");
    Serial.print(yaw);
    Serial.print(",");
    Serial.print(pitch);
    Serial.print(",");
    Serial.println(roll);
  }
}
