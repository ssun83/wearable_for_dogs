/*
    Descripcion: Saber la inclinacion del IMU Build-in Nano 33 BLE
*/

#include <Arduino_LSM9DS1.h>

long tiempo_prev = 0;
float pitch_prev = 0;
float roll_prev = 0;

// Sensibilidad del giroscopio   ±2000 dps -> 70
// Sensibilidad del acelerómetro ±4g -> 0.122
const float A_S = 0.122;
const float G_S = 70.0;


void setup()
{

  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin())
  {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  // Serial.println("Acceleration in G's");
  // Serial.println("AX\tAY\tAZ\tGX\tGY\tGZ");

  // Serial.println("Gyroscope in degrees/second");

}

void loop()
{
  // raw values
  float Ax, Ay, Az;
  float Gx, Gy, Gz;
  float pitch, _pitch;
  float roll, _roll;
  float dt;
  String _command = "";


  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
  {
    // read IMU raw value
    IMU.readAcceleration(Ax, Ay, Az);
    IMU.readGyroscope(Gx, Gy, Gz);
  }

  dt = (millis() - tiempo_prev) / 1000.0;
  tiempo_prev = millis();

  // Ratios
  Ax = Ax / A_S;
  Ay = Ay / A_S;
  Az = Az / A_S;
  Gx = Gx / G_S;
  Gy = Gy / G_S;
  Gz = Gz / G_S;

  // Calcular los ángulos con el acelerometro
  _pitch = atan2(Ax, sqrt(Ay * Ay + Az * Az));
  _roll =  atan2(Ay, sqrt(Ax * Ax + Az * Az));

  // Convertimos de radianes a grados
  _pitch *= (180.0 / PI);
  _roll  *= (180.0 / PI);

  // aplicar filtro
  pitch = 0.98 * (pitch_prev + Gx * dt) + (0.02 * _pitch);
  roll =  0.98 * (roll_prev + Gy * dt) + (0.02 * _roll);

  pitch_prev = pitch;
  roll_prev = roll;

  Serial.print(pitch);
  Serial.print('\t');
  Serial.println(roll);

  // print raw data
  //    Serial.print(Ax);
  //    Serial.print('\t');
  //    Serial.print(Ay);
  //    Serial.print('\t');
  //    Serial.print(Az);
  //    Serial.print('\t');
  //    Serial.print(Gx);
  //    Serial.print('\t');
  //    Serial.print(Gy);
  //    Serial.print('\t');
  //    Serial.println(Gz);

  delay(10);

}
