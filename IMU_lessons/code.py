from vpython import *
from time import sleep
import numpy as np
import math
import serial

ad=serial.Serial('com6',115200)
#ad2=serial.Serial('com3',115200)
sleep(1)

scene.range = 5

toRad = 2*np.pi/360
toDeg = 1/toRad

# scene.forward = vector(-1,-1,-1)

board = box(length=4, width=2, height=.2, opacity=.5)
nano = box(length=2, width=.6, height=.1, pos=vector(1,.1+.05,0), opacity=.5, color=color.cyan)
xarrow = arrow(axis=vector(1,0,0), length=4, shaftwidth=.1, color=color.red)
yarrow = arrow(axis=vector(0,1,0), length=4, shaftwidth=.1, color=color.green)
zarrow = arrow(axis=vector(0,0,1), length=4, shaftwidth=.1, color=color.blue)

frontArrow = arrow(length=2, shaftwidth=.1, color=color.green, axis=vector(1,0,0))
frontArrow2 = arrow(length=2, shaftwidth=.1, color=color.green, axis=vector(1,0,0))
upArrow = arrow(length=2, shaftwidth=.1, color=color.yellow, axis=vector(0,1,0))

xlabel = label(pos=frontArrow.axis, text='Angle: ')

bread = compound([board,nano])
inicio = 0

# pitch, roll, yaw
while (True):
    while (ad.inWaiting()==0):
        pass
    
    try:
        dataPacket = ad.readline()
        dataPacket = str(dataPacket, 'utf-8')
        splitPacket = dataPacket.split(',')
        # print(splitPacket)
        # # dataPacket2 = ad2.readline()
        # # dataPacket2 = str(dataPacket2, 'utf-8')
        # # splitPacket2 = dataPacket2.split(',')
        
        pitch = float(splitPacket[0])*toRad
        roll = float(splitPacket[1])*toRad
        yaw = float(splitPacket[2])*toRad
        
        # print(f"{pitch} -> {roll} -> {yaw}")
        
        pitch2 = float(splitPacket[3])*toRad
        roll2 = float(splitPacket[4])*toRad
        yaw2 = float(splitPacket[5])*toRad
        
        rate(100)
            
        l = frontArrow.length
        k = vector(l*cos(yaw)*cos(pitch),l*sin(pitch),l*sin(yaw)*cos(pitch))
        y = vector(0,1,0)
        s = cross(k,y)
        v = cross(s,k)
        
        l2 = frontArrow2.length
        k2 = vector(l2*cos(yaw2)*cos(pitch2),l*sin(pitch2),l2*sin(yaw2)*cos(pitch2))
        
        frontArrow.axis = k
        frontArrow2.axis = k2
        upArrow.axis = v
        bread.axis = k
        bread.up = v
            
        # if(inicio==0):
        #     print('hola')
        #     xarrow.axis = k
        #     zarrow.axis = cross(k,yarrow.axis)
        #     xarrow.length=4
        #     zarrow.length=4
        #     inicio = 1
            
        xlabel.text = f"Angle: {int(diff_angle(frontArrow2.axis, frontArrow.axis)*toDeg)}"
    except:
        pass

