/*
    Descripcion:
    La idea de este código es mejorar la medicion del angulo.
    Leer en roll de dos IMU
    El angulo es absoluto
*/

// librerias y defines
#include <Wire.h>

#define MPU 0x68              // direccion I2C: IMU 1
#define MPU2 0x69             // direccion I2C: IMU 2
#define A_R 16384.0           // ratio de conversion acelerometro
#define G_R 131.0             // ratio de conversion giroscopio
#define RAD_A_DEG = 57.295779 // conversion de radianes a grado 180/pi

// MPU-6050 da los valores en enteros de 16 bits, valores sin refinar (raw values)
int16_t AcX_1, AcY_1, AcZ_1, GyX_1, GyY_1, GyZ_1; // MPU 1
int16_t AcX_2, AcY_2, AcZ_2, GyX_2, GyY_2, GyZ_2; // MPU 2

// Angulos, X e Y del acelerometro, giroscopio y del filtro
float Acc_1[2];   // acelerometro MPU 1 [X,Y]
float Gy_1[2];    // giroscopio MPU 1 [X,Y]
float Angle_1[2]; // angulo MPU 1 [X,Y]

float Acc_2[2];   // acelerometro MPU 2 [X,Y]
float Gy_2[2];    // giroscopio MPU 2 [X,Y]
float Angle_2[2]; // angulo MPU 2 [X,Y]

// otras variables
float aumento = 180.0;
float aumento_2 = 360.0;
float angulo_final_1;
float angulo_final_2;
float angulo_total;
String dato;

void setup()
{

    Wire.begin(); // empieza la comunicacion en MPU 1
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);
    Wire.write(byte(0)); // para activar la comunicación
    Wire.endTransmission(true);

    Wire.begin(); // empieza la comunicación en MPU 2
    Wire.beginTransmission(MPU2);
    Wire.write(0x6B);
    Wire.write(byte(0)); // para activar la comunicación
    Wire.endTransmission(true);

    Serial.begin(9600); // inicializar el puerto serial
}

void loop()
{

    // MPU 1 ------------------------------------------------------------------------------
    // PARTE DEL ACELEROMETRO
    Wire.beginTransmission(MPU); //Leer los valores del Acelerometro de la IMU
    Wire.write(0x3B);            // Pedir el registro 0x3B - corresponde al AcX_1
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);         // A partir del 0x3B, se piden 6 registros
    AcX_1 = Wire.read() << 8 | Wire.read(); // Cada valor ocupa 2 registros
    AcY_1 = Wire.read() << 8 | Wire.read();
    AcZ_1 = Wire.read() << 8 | Wire.read();

    // A partir de los valores del acelerometro, se calculan los angulos Y, X
    //respectivamente, con la formula de la tangente.
    Acc_1[1] = atan(-1 * (AcX_1 / A_R) / sqrt(pow((AcY_1 / A_R), 2) + pow((AcZ_1 / A_R), 2))) * RAD_TO_DEG;
    Acc_1[0] = atan((AcY_1 / A_R) / sqrt(pow((AcX_1 / A_R), 2) + pow((AcZ_1 / A_R), 2))) * RAD_TO_DEG;

    // PARTE DEL GIROSCOPIO
    Wire.beginTransmission(MPU); //Leer los valores del Giroscopio
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 4, true); // A diferencia del Acelerometro, solo se piden 4 registros
    GyX_1 = Wire.read() << 8 | Wire.read();
    GyY_1 = Wire.read() << 8 | Wire.read();
    // Calculo del angulo del Giroscopio
    Gy_1[0] = GyX_1 / G_R;
    Gy_1[1] = GyY_1 / G_R;

    // PARTE DEL FILTRO
    // Aplicar el Filtro Complementario
    Angle_1[0] = 0.98 * (Angle_1[0] + Gy_1[0] * 0.010) + 0.02 * Acc_1[0];
    Angle_1[1] = 0.98 * (Angle_1[1] + Gy_1[1] * 0.010) + 0.02 * Acc_1[1];

    // acondicionamiento para que le angulo vaya hasta 360
    // if (AcZ_1 < 0)
    // {

    //     if (Angle_1[0] < 0)
    //     {
    //         angulo_final_1 = Angle_1[0] * -1 + aumento;
    //     }

    //     else
    //     {
    //         angulo_final_1 = aumento - Angle_1[0];
    //     }
    // }
    // else
    // {

    //     if (Angle_1[0] < 0)
    //     {
    //         angulo_final_1 = aumento_2 + Angle_1[0];
    //     }

    //     else
    //     {
    //         angulo_final_1 = Angle_1[0];
    //     }
    // }

    // MPU2 ------------------------------------------------------------------------------
    // PARTE DEL ACELEROMETRO
    Wire.beginTransmission(MPU2); //Leer los valores del Acelerometro de la IMU
    Wire.write(0x3B);             //Pedir el registro 0x3B - corresponde al AcX_1
    Wire.endTransmission(false);
    Wire.requestFrom(MPU2, 6, true);        //A partir del 0x3B, se piden 6 registros
    AcX_2 = Wire.read() << 8 | Wire.read(); //Cada valor ocupa 2 registros
    AcY_2 = Wire.read() << 8 | Wire.read();
    AcZ_2 = Wire.read() << 8 | Wire.read();

    // A partir de los valores del acelerometro, se calculan los angulos Y, X
    // respectivamente, con la formula de la tangente.
    Acc_2[1] = atan(-1 * (AcX_2 / A_R) / sqrt(pow((AcY_2 / A_R), 2) + pow((AcZ_2 / A_R), 2))) * RAD_TO_DEG;
    Acc_2[0] = atan((AcY_2 / A_R) / sqrt(pow((AcX_2 / A_R), 2) + pow((AcZ_2 / A_R), 2))) * RAD_TO_DEG;

    // PARTE DEL GIROSCOPIO -------------------------------------------------------------
    // Leer los valores del Giroscopio
    Wire.beginTransmission(MPU2);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU2, 4, true); //A diferencia del Acelerometro, solo se piden 4 registros
    GyX_2 = Wire.read() << 8 | Wire.read();
    GyY_2 = Wire.read() << 8 | Wire.read();
    // Calculo del angulo del Giroscopio
    Gy_2[0] = GyX_2 / G_R;
    Gy_2[1] = GyY_2 / G_R;

    // PARTE DEL FILTRO -------------------------------------------------------------------
    // Aplicar el Filtro Complementario
    Angle_2[0] = 0.98 * (Angle_2[0] + Gy_2[0] * 0.010) + 0.02 * Acc_2[0];
    Angle_2[1] = 0.98 * (Angle_2[1] + Gy_2[1] * 0.010) + 0.02 * Acc_2[1];

    // acondicionamiento para que le angulo vaya hasta 360
    // if (AcZ_2 < 0)
    // {

    //     if (Angle_2[0] < 0)
    //     {
    //         angulo_final_2 = Angle_2[0] * -1 + aumento;
    //     }

    //     else
    //     {
    //         angulo_final_2 = aumento - Angle_2[0];
    //     }
    // }
    // else
    // {

    //     if (Angle_2[0] < 0)
    //     {
    //         angulo_final_2 = aumento_2 + Angle_2[0];
    //     }

    //     else
    //     {
    //         angulo_final_2 = Angle_2[0];
    //     }
    // }

    delay(10); //Nuestra dt sera, pues, 0.010, que es el intervalo de tiempo en cada medida
    
    angulo_final_1 = abs(Angle_1[1]);
    angulo_final_2 = abs(Angle_2[1]);
    angulo_total = aumento - (angulo_final_1 + angulo_final_2);

    // Mostrar los valores por consola
    Serial.print(angulo_final_1);
    Serial.print(" ");
    Serial.print(angulo_final_2);
    Serial.print(" ");
    Serial.println(angulo_total);
}
