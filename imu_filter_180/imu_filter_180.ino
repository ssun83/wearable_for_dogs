/*
  Descripcion: Este código lee los valores de un MPU6050 con aumento de 180
 */

#include <Wire.h>

// Direccion I2C de la IMU
#define MPU 0x68

// Ratios de conversion según la documentación
#define A_R 16384.0
#define G_R 131.0

// Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779

// MPU-6050 da los valores en enteros de 16 bits, valores sin refinar (raw values)
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;

// Angulos: X e Y
float Acc[2];   // del acelerometro
float Gy[2];    // del giroscopio
float Angle[2]; // del filtro

// otras variables
float aumento = 180.0;
float aumento_2 = 360.0;
float angulo_final;

void setup()
{
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(byte(0)); // para activar la comunicación
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop()
{
  // PARTE DEL ACELEROMETRO ------------------------------------------------------------
  // Leer los valores del Acelerometro de la IMU
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Pedir el registro 0x3B - corresponde al AcX
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);       // A partir del 0x3B, se piden 6 registros
  AcX = Wire.read() << 8 | Wire.read(); // Cada valor ocupa 2 registros
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // A partir de los valores del acelerometro, se calculan los angulos Y, X
  // respectivamente, con la formula de la tangente.
  Acc[1] = atan(-1 * (AcX / A_R) / sqrt(pow((AcY / A_R), 2) + pow((AcZ / A_R), 2))) * RAD_TO_DEG;
  Acc[0] = atan((AcY / A_R) / sqrt(pow((AcX / A_R), 2) + pow((AcZ / A_R), 2))) * RAD_TO_DEG;

  // PARTE DEL GIROSCOPIO -------------------------------------------------------------
  // Leer los valores del Giroscopio
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 4, true); // A diferencia del Acelerometro, solo se piden 4 registros
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();

  // Calculo del angulo del Giroscopio
  Gy[0] = GyX / G_R;
  Gy[1] = GyY / G_R;

  // PARTE DEL FILTRO -------------------------------------------------------------------
  // Aplicar el Filtro Complementario
  Angle[0] = 0.98 * (Angle[0] + Gy[0] * 0.010) + 0.02 * Acc[0];
  Angle[1] = 0.98 * (Angle[1] + Gy[1] * 0.010) + 0.02 * Acc[1];

  // para angulos de 360º
  if (AcZ < 0){

    if(Angle[0] < 0){
      angulo_final = Angle[0]*-1 + aumento;
    }

    else{
      angulo_final = aumento - Angle[0];
    }
    
  }
  else
  {
    if(Angle[0] < 0){
      angulo_final = aumento_2 + Angle[0];
    }

    else{
      angulo_final = Angle[0];
    }
  }

  //Mostrar los valores por consola
  //Serial.print("Angle X: "); Serial.print(Angle[0]); Serial.print("\n");
  Serial.print("Angle X: ");
  Serial.print(angulo_final);
  Serial.print("\n");
  Serial.print("Orientation Z: ");
  Serial.print(AcZ);
  Serial.print("\n");
  Serial.print("Angle Y: ");
  Serial.print(Angle[1]);
  Serial.print("\n------------\n");

  delay(50); //Nuestra dt sera, pues, 0.010, que es el intervalo de tiempo en cada medida
}
