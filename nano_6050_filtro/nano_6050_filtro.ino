/*
    Descripcion:
    - Usar un IMU 6050 y un Arduino y calcular el angulo
    - IMU6050 -> IMU 2, IMU NANO -> IMU 1
    TODO:
    - Agregar BLE
*/

// librerias
#include <Arduino_LSM9DS1.h>
#include <Wire.h>

// variables para IMU 6050
#define MPU_6050 0x68              // direccion I2C: IMU 2
#define A_6050 16384.0           // ratio de conversion acelerometro
#define G_6050 131.0             // ratio de conversion giroscopio
#define RAD_A_DEG = 57.295779 // conversion de radianes a grado 180/pi
float pitch_6050_prev = 0;
float roll_6050_prev = 0;

// variables para IMU Nano
const float A_nano = 0.122; // Sensibilidad del acelerómetro ±4g -> 0.122
const float G_nano = 70.0;  // Sensibilidad del giroscopio   ±2000 dps -> 70
float pitch_nano_prev = 0;
float roll_nano_prev = 0;

// otras variables
long tiempo_prev = 0;
float aumento = 180.0;
float joint_angle;
float roll_6050_abs;
float roll_nano_abs;
float pitch_6050_abs;
float pitch_nano_abs;

void setup()
{
  Serial.begin(9600);
  while (!Serial);

  // iniciar el MPU_6050 1 (6050)
  Wire.begin(); // empieza la comunicacion en MPU_6050 1
  Wire.beginTransmission(MPU_6050);
  Wire.write(0x6B);
  Wire.write(byte(0)); // para activar la comunicación
  Wire.endTransmission(true);

  // inicial el MPU_6050 2 (Nano)
  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop()
{
  // Calcular el diferencial
  float dt;
  dt = (millis() - tiempo_prev) / 1000.0;
  tiempo_prev = millis();

  // MPU 1 (Nano) --------------------------------------------------------------------
  float Ax_nano, Ay_nano, Az_nano;
  float Gx_nano, Gy_nano, Gz_nano;
  float pitch_nano, _pitch_nano;
  float roll_nano, _roll_nano;

  // Leer datos del acelerometro y giroscopio
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
  {
    // read IMU raw value
    IMU.readAcceleration(Ax_nano, Ay_nano, Az_nano);
    IMU.readGyroscope(Gx_nano, Gy_nano, Gz_nano);
  }

  // Convertir los datos del acelerometro
  Ax_nano = Ax_nano / A_nano;
  Ay_nano = Ay_nano / A_nano;
  Az_nano = Az_nano / A_nano;

  // Convertir los datos del giroscopio
  Gx_nano = Gx_nano / G_nano;
  Gy_nano = Gy_nano / G_nano;
  Gz_nano = Gz_nano / G_nano;

  // Calcular el roll y pitch
  _pitch_nano = atan2(Ax_nano, sqrt(Ay_nano * Ay_nano + Az_nano * Az_nano)) * RAD_TO_DEG;
  _roll_nano = atan2(Ay_nano, sqrt(Ax_nano * Ax_nano + Az_nano * Az_nano)) * RAD_TO_DEG;

  // Aplicar filtro
  pitch_nano = 0.98 * (pitch_nano_prev + Gx_nano * dt) + (0.02 * _pitch_nano);
  roll_nano = 0.98 * (roll_nano_prev + Gy_nano * dt) + (0.02 * _roll_nano);

  // Actualizar con el anterior
  pitch_nano_prev = pitch_nano;
  roll_nano_prev = roll_nano;


  // MPU 2 (6050) --------------------------------------------------------------------
  // Da los valores en enteros de 16 bits, valores sin refinar (raw values)
  int16_t Ax_6050, Ay_6050, Az_6050;
  int16_t Gx_6050, Gy_6050, Gz_6050;
  float pitch_6050, _pitch_6050;
  float roll_6050, _roll_6050;


  // Leer datos del acelerometro
  Wire.beginTransmission(MPU_6050);
  Wire.write(0x3B);   // Pedir el registro 0x3B - corresponde al Ax_6050
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_6050, 6, true);    // A partir del 0x3B, se piden 6 registros
  Ax_6050 = Wire.read() << 8 | Wire.read();   // Cada valor ocupa 2 registros
  Ay_6050 = Wire.read() << 8 | Wire.read();
  Az_6050 = Wire.read() << 8 | Wire.read();

  // Leer los datos del giroscopio
  Wire.beginTransmission(MPU_6050);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_6050, 4, true); // A diferencia del Acelerometro, solo se piden 4 registros
  Gx_6050 = Wire.read() << 8 | Wire.read();
  Gy_6050 = Wire.read() << 8 | Wire.read();

  // Convertir los valores del acelerometro
  Ax_6050 = Ax_6050 / A_6050;
  Ay_6050 = Ay_6050 / A_6050;
  Az_6050 = Az_6050 / A_6050;

  // Convertir los valores del giroscopio
  Gx_6050 = Gx_6050 / G_6050;
  Gy_6050 = Gy_6050 / G_6050;

  // Calcular el roll y pitch
  _pitch_6050 = atan2(Ax_6050, sqrt(Ay_6050 * Ay_6050 + Az_6050 * Az_6050)) * RAD_TO_DEG;
  _roll_6050 = atan2(Ay_6050, sqrt(Ax_6050 * Ax_6050 + Az_6050 * Az_6050)) * RAD_TO_DEG;

  // Aplicar filtro
  pitch_6050 = 0.98 * (pitch_6050_prev + Gx_6050 * dt) + (0.02 * _pitch_6050);
  roll_6050 = 0.98 * (roll_6050_prev + Gy_6050 * dt) + (0.02 * _roll_6050);

  // Actualizar con el anterior
  pitch_6050_prev = pitch_6050;
  roll_6050_prev = roll_6050;

  // Joint Angle ----------------------------------------------
  pitch_nano_abs = abs(pitch_nano);
  roll_6050_abs = abs(roll_6050);

  joint_angle = aumento - (roll_6050_abs + pitch_nano_abs);
  Serial.print(joint_angle);
  Serial.print(" ");
  Serial.print(pitch_nano_abs);
  Serial.print(" ");
  Serial.println(roll_6050_abs);

}
