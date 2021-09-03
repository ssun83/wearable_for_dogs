from vpython import *
from time import *
import numpy as np
import math
import serial

# configuracion del serial
ad=serial.Serial('com6',115200)
ad2=serial.Serial('com3',115200)
sleep(1)

# variables
toRad=2*np.pi/360
toDeg=1/toRad

# configuracion de la escena
scene.range=5
scene.background=color.black
scene.forward=vector(-1,-1,-1)
scene.width=1200
scene.height=1080

# flechas x, y, z
xarrow=arrow(lenght=2, shaftwidth=.1, color=color.red,axis=vector(1,0,0))
yarrow=arrow(lenght=2, shaftwidth=.1, color=color.green,axis=vector(0,1,0))
zarrow=arrow(lenght=2, shaftwidth=.1, color=color.blue,axis=vector(0,0,1))

# flechas para IMU MPU6050
frontArrow=arrow(length=4,shaftwidth=.1,color=vector(252/255, 243/255, 207/255),axis=vector(1,0,0))
upArrow=arrow(length=4,shaftwidth=.1,color=vector(171/255, 235/255, 198/255),axis=vector(0,1,0))
sideArrow=arrow(length=4,shaftwidth=.1,color=vector(169/255, 204/255, 227/255),axis=vector(0,0,1))
xlabel = label(pos=frontArrow.axis, text='Angle 1: ')
ylabel = label(pos=sideArrow.axis, text='Angle 2: ')
zlabel = label(pos=upArrow.axis, text='Angle 3: ')

# flechas para IMU LSM9DS1
frontArrow_2=arrow(length=4,shaftwidth=.1,color=vector(154/255, 125/255, 10/255),axis=vector(1,0,0))
upArrow_2=arrow(length=4,shaftwidth=.1,color=vector(29/255, 131/255, 72/255),axis=vector(0,1,0))
sideArrow_2=arrow(length=4,shaftwidth=.1,color=vector(33/255, 97/255, 140/255),axis=vector(0,0,1))
# xlabel_2 = label(pos=frontArrow.axis, text='x1')
# ylabel_2 = label(pos=sideArrow.axis, text='y1')
# zlabel_2 = label(pos=upArrow.axis, text='z1')

while (True):
    try:
        while (ad.inWaiting()==0 and ad2.in_waiting()==0):
            pass
        
        # paquete primer serial
        dataPacket=ad.readline()
        dataPacket=str(dataPacket,'utf-8')
        packet_1=dataPacket.split(",")

        # paquete segundo serial
        dataPacket2=ad2.readline()
        dataPacket2=str(dataPacket2,'utf-8')
        packet_2=dataPacket2.split(",")

        # variables del serial com6
        yaw = (float(packet_1[0]) / 57.29578)
        pitch = (float(packet_1[1]) / 57.29578)
        roll = ((float(packet_1[2]) - 180) / 57.29578)
        
        # variables del serial com3
        yaw2 = (float(packet_2[0]) / 57.29578)
        pitch2 = (float(packet_2[1]) / 57.29578)
        roll2 = ((float(packet_2[2]) - 180) / 57.29578)
                
        rate(100)

        # vectores para IMU MPU6050
        k=vector(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch))
        y=vector(0,1,0)
        s=cross(k,y)
        v=cross(s,k)
        vrot=v*cos(roll)+cross(k,v)*sin(roll)

        # vectores para IMU LSM9DS1
        k_2=vector(cos(yaw2)*cos(pitch2), sin(pitch2),sin(yaw2)*cos(pitch2))
        y_2=vector(0,1,0)
        s_2=cross(k_2,y_2)
        v_2=cross(s_2,k_2)
        vrot_2=v_2*cos(roll2)+cross(k_2,v_2)*sin(roll2)

        # sistema de referencia del MPU6050
        frontArrow.axis=k
        sideArrow.axis=cross(k,vrot)
        upArrow.axis=vrot

        # sistema de referencia del LSM9DS1
        frontArrow_2.axis=k_2
        sideArrow_2.axis=cross(k_2,vrot_2)
        upArrow_2.axis=vrot_2

        # ajuste de flechas para MPU6050
        sideArrow.length=4
        frontArrow.length=4
        upArrow.length=4
        
        # ajuste de flechas para LSM9DS1
        sideArrow_2.length=4
        frontArrow_2.length=4
        upArrow_2.length=4
        
        xlabel.text = f"Angle 1: {diff_angle(sideArrow.axis, sideArrow_2.axis)*toDeg}"
        ylabel.text = f"Angle 2: {diff_angle(upArrow.axis, upArrow_2.axis)*toDeg}"
        zlabel.text = f"Angle 3: {diff_angle(frontArrow.axis, frontArrow_2.axis)*toDeg}"
    
    except:
        pass

    