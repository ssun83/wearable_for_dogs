/*
  Descripcion:
  Este codigo lee dos IMU MPU6050 y usa el filtro de Madwick
*/

// librerias y defines
#include <Arduino.h>
#include <Wire.h>
#include <MadgwickAHRS.h>

#define MPU 0x68              // direccion I2C: IMU 1
#define A_R 16384.0           // ratio de conversion acelerometro
#define G_R 131.0             // ratio de conversion giroscopio
#define RAD_A_DEG = 57.295779 // conversion de radianes a grado 180/pi

// MPU-6050 da los valores en enteros de 16 bits, valores sin refinar (raw values)
int16_t Ax, Ay, Az;
int16_t Gx, Gy, Gz;

Madgwick filter;

void setup() {
  Wire.begin(); // empieza la comunicacion en MPU 1
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(byte(0)); // para activar la comunicación
  Wire.endTransmission(true);

  Serial.begin(115200);
  float sensorRate = 80;
  filter.begin(sensorRate);
}

void loop() {
  float roll, pitch, yaw;
  
  // MPU 1 ------------------------------------------------------------------------------
  // PARTE DEL ACELEROMETRO
  Wire.beginTransmission(MPU); //Leer los valores del Acelerometro de la IMU
  Wire.write(0x3B);            // Pedir el registro 0x3B - corresponde al AcX_1
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);         // A partir del 0x3B, se piden 6 registros
  Ax = Wire.read() << 8 | Wire.read(); // Cada valor ocupa 2 registros
  Ay = Wire.read() << 8 | Wire.read();
  Az = Wire.read() << 8 | Wire.read();

  // PARTE DEL GIROSCOPIO
  Wire.beginTransmission(MPU); //Leer los valores del Giroscopio
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 4, true); // A diferencia del Acelerometro, solo se piden 4 registros
  Gx = Wire.read() << 8 | Wire.read();
  Gy = Wire.read() << 8 | Wire.read();
  Gz = Wire.read() << 8 | Wire.read();

  Ax = Ax / A_R;
  Ay = Ay / A_R;
  Az = Az / A_R;

  Gx = Gx / G_R;
  Gy = Gy / G_R;
  Gz = Gz / G_R;

  filter.updateIMU(float(Gx), float(Gy), float(Gz), float(Ax), float(Ay), float(Az));

  roll = filter.getRoll();
  pitch = filter.getPitch();
  yaw = filter.getYaw();


  Serial.print(yaw);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.println(roll);

  delay(10);

//  Serial.print(AcX);
//  Serial.print(" ");
//  Serial.print(AcY);
//  Serial.print(" ");
//  Serial.print(AcZ);
//  Serial.print(" ");
//  Serial.print(GyX);
//  Serial.print(" ");
//  Serial.print(GyY);
//  Serial.print(" ");
//  Serial.println(GyZ);

}
