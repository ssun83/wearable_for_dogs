#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <math.h>
#include <MadgwickAHRS.h>
#include <Wire.h>
#include <MPU9250.h>

// variables del led
int red_light_pin = 11;
int green_light_pin = 10;
int blue_light_pin = 9;

// variables de los imu
float Mx, My, Mz, Ax, Ay, Az, Gx, Gy, Gz;
float roll, pitch, yaw;
float roll2, pitch2, yaw2, _yaw2;

// variables de conversion
float toRad = 2.0 * 3.1416 / 360.0;
float toDeg = 1.0 / toRad;
#define DELAY_MS (10)
#define gscale 1;

Madgwick filter;
MPU9250 mpu;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  delay(500);

  if (!mpu.setup(0x68)) {  // change to your own address
    while (1) {
      Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
      delay(500);
    }
  }

  mpu.setAccBias(14.56, 16.46, -39.54);
  mpu.setGyroBias(3.99, 2.23, 0.77);
  mpu.setMagBias(112.08, 281.14, 148.95);
  mpu.setMagScale(0.97, 0.95, 1.09);
  mpu.setMagneticDeclination(-2.34);

  if (!IMU.begin())
  {
    while (1);
  }

  // configuracion acelerometro
  IMU.setAccelFS(2);  // Rangos -> 0: 2g, 1: 24g, 2: 4g, 3: 8g
  IMU.setAccelODR(3); // Rangos -> 1: 10Hz, 2: 50Hz, 3: 119Hz, 4: 238Hz, 5: 476Hz
  IMU.setAccelOffset(0.000091, -0.028406, -0.007399);
  IMU.setAccelSlope(1.000338, 0.993624, 1.001161);

  // configuracion giroscopio
  IMU.gyroUnit = DEGREEPERSECOND;
  // IMU.gyroUnit = RADIANSPERSECOND;
  IMU.setGyroFS(3); // Rangos -> 0: 245º/s, 1: 500º/s, 2: 1000º/s, 3: 2000º/s
  IMU.setGyroODR(3); // Rangos -> 1: 10Hz, 2: 50Hz, 3: 119Hz, 4: 238Hz, 5: 476Hz
  IMU.setGyroOffset(0.424835, 0.507385, -0.077393);
  IMU.setGyroSlope(1.175011, 1.000000, 1.140555);

  // configuracion magnetometro
  IMU.setMagnetFS(0); // Rangos -> 0: 400uT, 1: 800uT, 2: 1200uT, 3: 1600uT
  IMU.setMagnetODR(7); // Rangos -> 1: 1.25Hz, 2: 2.5Hz, 3: 5Hz, 4: 10Hz, 5: 20Hz, 6: 40Hz, 7:80Hz, 8:400Hz
  IMU.setMagnetOffset(-3.031616, 30.895386, -19.974365);
  IMU.setMagnetSlope (2.400837, 2.359905, 2.472181);

  float sensorRate = min(IMU.getGyroODR(), IMU.getMagnetODR());
  filter.begin(sensorRate);

  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);
}

void loop()
{
  float joint_angle = 0.0;
  float k0, k1, k2;
  float k20, k21, k22;
  float selfmag = 0.0;
  float othermag = 0.0;
  float a = 0.0;

  if (IMU.accelAvailable() && IMU.gyroAvailable() && IMU.magnetAvailable())
  {
    // read IMU raw value
    IMU.readAccel(Ax, Ay, Az);
    IMU.readGyro(Gx, Gy, Gz);
    IMU.readMagnet(Mx, My, Mz);
  }

  Gx = -Gx;
  Ax = -Ax;
  //Mx = -Mx;
  //My = -My;

  filter.update(Gx, Gy, Gz, Ax, Ay, Az, Mx, My, Mz);
  roll = filter.getRoll() * toRad;
  pitch = filter.getPitch() * toRad;
  yaw = filter.getYaw() * toRad;

  if (mpu.update()) {
    roll2 = -mpu.getRoll() ;
    pitch2 = -mpu.getPitch();
    _yaw2 = mpu.getYaw();
    if (_yaw2 < 0) {
      yaw2 = _yaw2 + 360.0;
    }
    else {
      yaw2 = _yaw2;
    }
  }

  k0 = cos(yaw) * cos(pitch);
  k1 = sin(pitch);
  k2 = sin(yaw) * cos(pitch);

  k20 = cos(yaw2 * toRad) * cos(pitch2 * toRad);
  k21 = sin(pitch2 * toRad);
  k22 = sin(yaw2 * toRad) * cos(pitch2 * toRad);

  selfmag = sqrt(k0 * k0 + k1 * k1 + k2 * k2);

  if (selfmag > 0.0) {
    k0 = k0 / selfmag;
    k1 = k1 / selfmag;
    k2 = k2 / selfmag;
  }
  else {
    k0 = 0.0;
    k1 = 0.0;
    k2 = 0.0;
  }

  othermag = sqrt(k20 * k20 + k21 * k21 + k22 * k22);

  if (othermag > 0.0) {
    k20 = k20 / othermag;
    k21 = k21 / othermag;
    k22 = k22 / othermag;
  }
  else {
    k20 = 0.0;
    k21 = 0.0;
    k22 = 0.0;
  }

  a = k0 * k20 + k1 * k21 + k2 * k22;

  if (a > 1.0) {
    joint_angle = 0.0;
  }
  if (a < -1.0) {
    joint_angle = 3.1416;
  }

  joint_angle = 180 - (acos(a) * toDeg);

  Serial.println(joint_angle);

  //  if (joint_angle < 40) {
  //    RGB_color(255, 0, 0); // Red
  //  }
  //  else if (joint_angle >= 40 and joint_angle < 60) {
  //    RGB_color(237, 109, 0); // Orange
  //  }
  //  else if (joint_angle >= 60 and joint_angle < 160) {
  //    RGB_color(0, 255, 0); // Green
  //  }
  //  else if (joint_angle >= 160 and joint_angle < 170) {
  //    RGB_color(237, 109, 0); // Orange
  //  }
  //  else if (joint_angle >= 170) {
  //    RGB_color(255, 0, 0); // Red
  //  }

  // roll, pitch, yaw
  //  Serial.print(pitch);
  //  Serial.print(",");
  //  Serial.print(roll);
  //  Serial.print(",");
  //  Serial.print(yaw);
  //  Serial.print(",");
  //  Serial.print(pitch2);
  //  Serial.print(",");
  //  Serial.print(roll2);
  //  Serial.print(",");
  //  Serial.println(yaw2);

  delay(DELAY_MS);

}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(red_light_pin, 255 - red_light_value);
  analogWrite(green_light_pin, 255 - green_light_value);
  analogWrite(blue_light_pin, 255 - blue_light_value);
}
