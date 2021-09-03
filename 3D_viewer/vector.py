from vpython import *
from time import *
import numpy as np
import math
from vpython import *
from time import *
import serial

ad=serial.Serial('com6',115200)
sleep(1)

scene.range=5
#scene.forward=vector(-1,-1,-1)

scene.width=600
scene.height=600

xarrow=arrow(lenght=2, shaftwidth=.1, color=color.red,axis=vector(1,0,0))
yarrow=arrow(lenght=2, shaftwidth=.1, color=color.green,axis=vector(0,1,0))
zarrow=arrow(lenght=4, shaftwidth=.1, color=color.blue,axis=vector(0,0,1))
upArrow=arrow(length=1,shaftwidth=.1,color=color.magenta,axis=vector(0,1,0))

v1 = [0,0,1]
_v1 = [0,0,0]

while (True):
    try:
        while (ad.inWaiting()==0):
            pass

        dataPacket=ad.readline()
        dataPacket=str(dataPacket,'utf-8')
        splitPacket=dataPacket.split(",")

        #print(dataPacket)

        q10 = float(splitPacket[0])
        q11 = float(splitPacket[1])
        q12 = float(splitPacket[2])
        q13 = float(splitPacket[3])

        _v1[0] = (1-2*(q12*q12)-2*(q13*q13))*v1[0] + (2*(q11*q12+q10*q13))*v1[1] + (2*(q11*q13-q10*q12))*v1[2]
        _v1[1] = (2*(q11*q12-q10*q13))*v1[0] + (1-2*(q11*q11)-2*(q13*q13))*v1[1] + (2*(q12*q13+q10*q11))*v1[2]
        _v1[2] =(2*(q11*q13+q10*q12))*v1[0] + (2*(q12*q13-q10*q11))*v1[1] + (1-2*(q11*q11)-2*(q12*q12))*v1[2]

        # v1 = _v1

        rate(100)

        upArrow.axis = vector(_v1[0],_v1[1],_v1[2])
        upArrow.length=2

    except:
        pass