import numpy as np
import math
import serial
from time import sleep

ad=serial.Serial('com6',115200)
sleep(2)

v1 = np.array([0,0,1])
v2 = np.array([0,0,1])
_v1 = np.array([0,0,0])
_v2 = np.array([0,0,0])

while (True):
    try:
        while (ad.inWaiting()==0):
            pass
        dataPacket=ad.readline()
        dataPacket=str(dataPacket,'utf-8')
        packet=dataPacket.split(",")

        q1 = np.array([float(packet[0]), float(packet[1]), float(packet[2]),float(packet[3])])
        q2 = np.array([float(packet[4]), float(packet[5]), float(packet[6]),float(packet[7])])

        print(f'q1 = {q1} - q2 = {q2}')
    
        # _v1[0] = (1-2*(q12*q12)-2*(q13*q13))*v1[0] + (2*(q11*q12+q10*q13))*v1[1] + (2*(q11*q13-q10*q12))*v1[2]
        # _v1[1] = (2*(q11*q12-q10*q13))*v1[0] + (1-2*(q11*q11)-2*(q13*q13))*v1[1] + (2*(q12*q13+q10*q11))*v1[2]
        # _v1[2] =(2*(q11*q13+q10*q12))*v1[0] + (2*(q12*q13-q10*q11))*v1[1] + (1-2*(q11*q11)-2*(q12*q12))*v1[2]

        # _v2[0] = (1-2*(q22*q22)-2*(q23*q23))*v2[0] + (2*(q21*q22+q20*q23))*v2[1] + (2*(q21*q23-q20*q22))*v2[2]
        # _v2[1] = (2*(q21*q22-q20*q23))*v2[0] + (1-2*(q21*q21)-2*(q23*q23))*v2[1] + (2*(q22*q23+q20*q21))*v2[2]
        # _v2[2] =(2*(q21*q23+q20*q22))*v2[0] + (2*(q22*q23-q20*q21))*v2[1] + (1-2*(q21*q21)-2*(q22*q22))*v2[2]
    except:
        pass