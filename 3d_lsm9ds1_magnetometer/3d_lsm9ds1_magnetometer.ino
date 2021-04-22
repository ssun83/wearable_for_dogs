/*
  Descripcion:
  - Este codigo envia los datos del LSM9DS1 al visualizador 3D
*/

// library
#include <Arduino_LSM9DS1.h>
#include <MadgwickAHRS.h>

// variables
#define gscale 1;

// start filters
Madgwick filter;

void setup() {

  // start serial monitor
  Serial.begin(115200);

  // start LSM9DS1 communication
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
}

void loop() {

  // raw values
  float _Ax2, _Ay2, _Az2, _Gx2, _Gy2, _Gz2, _Mx2, _My2, _Mz2;
  float Ax2, Ay2, Az2, Gx2, Gy2, Gz2, Mx2, My2, Mz2;

  // euler angles
  float roll, pitch, yaw;

  // read MPU LSM9DS1 data
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
  filter.update(Gx2, Gy2, Gz2, Ax2, Ay2, Az2, Mx2, My2, Mz2);

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
