

/*
    Descripcion: Muestra los valores raw del IMU Nano
*/

#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <MadgwickAHRS.h>

Madgwick filter;

// #define gscale 70e-3; // Angular rate FS =+-2000
#define gscale 1;

void setup()
{

  Serial.begin(115200);
  while (!Serial);
  //  Serial.println("Started");

  if (!IMU.begin())
  {
    //    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  delay(1000);

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

void loop()
{
  // raw values
  float Ax, Ay, Az;
  float Gx, Gy, Gz;
  float Mx, My, Mz;
  float roll, pitch, yaw;

  if (IMU.accelAvailable() && IMU.gyroAvailable() && IMU.magnetAvailable())
  {
    // read IMU raw value
    IMU.readAccel(Ax, Ay, Az);
    IMU.readGyro(Gx, Gy, Gz);
    IMU.readMagnet(Mx, My, Mz);
  }

  //  Gx = Gx * gscale;
  //  Gy = Gy * gscale;
  //  Gz = Gz * gscale;


  // opcion original con acelerometro y giro (tiene drift, la punta está para abajo y no está alineado x ni y)
  filter.updateIMU(Gx, Gy, Gz, -Ax, Ay, Az);

  // opcion original con los tres sensores (no tiene drift, la punta está para abajo y no está alineado x ni y)
  // filter.update(Gx, Gy, Gz, Ax, Ay, Az, Mx, My, Mz);

  // opcion con x cambiado con y con simbolos positivos (tiene drift, la punta está para abajo y no está alineado)
  // filter.updateIMU(Gy, Gx, Gz, Ay, Ax, Az);

  // opcion con x cambiado con y con simbolos negativos (tiene drift)
  //filter.updateIMU(Gy, -Gx, Gz, Ay, -Ax, Az);

  // opcion con solo x cambiado negativo
  //filter.updateIMU(-Gy, Gx, Gz, -Ay, Ax, Az);
  
  // opcion con solo y cambiado negativo
  //filter.updateIMU(Gy, Gx, -Gz, Ay, Ax, -Az);
  
  // opcion con solo z cambiado
  // opcion con todo cambiado a negativo


  // OPCION
  //filter.updateIMU(Gx, Gy, -Gz, Ax, Ay, -Az);

  // OPCION 2 ---------------------------------
  // filter.updateIMU(Gy, Gx, Gz, Ay, Ax, Az);

  // OPCION 3 -------------------------------
  //filter.update(Gx, Gy, -Gz, Ax, Ay, -Az, -Mx, My, -Mz);

  // OPCION 4 (donatello) -------------------------------------
  //filter.update(Gx, Gy, Gz, Ax, Ay, Az, -Mx, My, Mz);

  // OPCION 4.5 (mathlete) -------------------------------------
  //filter.update(Gx, Gy, Gz, Ax, Ay, Az, -My, -Mx, Mz);

  // OPCION 4.6 (jremington) -------------------------------------
  //filter.update(-Gy, -Gx, Gz, -Ay, -Ax, Az, Mx, My, Mz);

  // OPCION 5 (yapibot) -------------------------------------
  //filter.update(-Gy, -Gx, Gz, -Ay, -Ax, Az, My, -Mx, Mz);
  //filter.updateIMU(Gy, Gx, Gz, Ay, Ax, Az);


  // OPCION 5.5 con ejes invertidos Gy <-> Gx 2.0 -------
  //filter.update(Gy, Gx, -Gz, Ay, Ax, -Az, My, -Mx, -Mz);

  // siguiendo los NED de la foto de arduino
  // filter.update(Gx, Gy, -Gz, Ax, Ay, -Az, -Mx, My, -Mz);

  // print the heading, pitch and roll
  roll = filter.getRoll();
  pitch = filter.getPitch();
  yaw = filter.getYaw();


  Serial.print(yaw);
  Serial.print(",");
  Serial.print(pitch);
  Serial.print(",");
  Serial.println(roll);

//  Serial.print(Ax);
//  Serial.print(",");
//  Serial.print(Ay);
//  Serial.print(",");
//  Serial.println(Az);

  delay(10);


}
