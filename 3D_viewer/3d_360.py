from vpython import *
from time import *
import numpy as np
import serial
from math import acos
from math import sqrt
from math import pi

# conexion serial
ad = serial.Serial('com6', 115200)
sleep(1)

# variables
toRad = 2*np.pi/360
toDeg = 1/toRad

while (True):
    try:
        while (ad.inWaiting() == 0):
            pass

        # recibir paquete de datos
        dataPacket = ad.readline()
        dataPacket = str(dataPacket, 'utf-8')
        # print(dataPacket)
        packet = dataPacket.split(",")

        # variables del IMU MPU6050
        yaw = (float(packet[0]) * toRad)
        pitch = (float(packet[1]) * toRad)
        roll = ((float(packet[2]) - 180) / 57.29578)

        # variables del IMU LSM9DS1
        yaw2 = (float(packet[3]) * toRad)
        pitch2 = (float(packet[4]) * toRad)
        roll2 = ((float(packet[5]) - 180) / 57.29578)

        # vectores para IMU MPU9250
        k = [cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch)]

        # vectores para IMU LSM9DS1
        k_2 = [cos(yaw2)*cos(pitch2), sin(pitch2), sin(yaw2)*cos(pitch2)]

        # print(k)
        # print(k_2)

        def length(v):
            return sqrt(v[0]**2+v[1]**2)


        def dot_product(v, w):
            return v[0]*w[0]+v[1]*w[1]


        def determinant(v, w):
            return v[0]*w[1]-v[1]*w[0]


        def inner_angle(v, w):
            cosx = dot_product(v, w)/(length(v)*length(w))
            rad = acos(cosx)  # in radians
            return rad*180/pi  # returns degrees


        def angle_clockwise(A, B):
            inner = inner_angle(A, B)
            det = determinant(A, B)
            if det < 0:  # this is a property of the det. If the det < 0 then B is clockwise of A
                return inner
            else:  # if the det > 0 then A is immediately clockwise of B
                return 360-inner


        print(angle_clockwise(k, k_2))
        # print(angle_clockwise(k_2, k))

    except:
        pass
