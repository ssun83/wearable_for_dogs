from vpython import *
from time import *
import numpy as np
import math
import serial

# conexion serial
ad = serial.Serial('com6',115200)
sleep(1)

# variables
toRad = 2*np.pi/360
toDeg = 1/toRad

# configuracion de la escena
scene.range=5
scene.background=color.black
scene.forward=vector(-1,-1,-1)
scene.width=600
scene.height=600

# flechas x, y, z
xarrow=arrow(lenght=2, shaftwidth=.1, color=color.red,axis=vector(1,0,0))
yarrow=arrow(lenght=2, shaftwidth=.1, color=color.green,axis=vector(0,1,0))
zarrow=arrow(lenght=2, shaftwidth=.1, color=color.blue,axis=vector(0,0,1))

# flechas para IMU MPU9250
frontArrow=arrow(length=4,shaftwidth=.1,color=color.purple,axis=vector(1,0,0))
# upArrow=arrow(length=4,shaftwidth=.1,color=color.magenta,axis=vector(0,1,0))
# sideArrow=arrow(length=4,shaftwidth=.1,color=color.orange,axis=vector(0,0,1))

# xlabel = label(pos=frontArrow.axis, text='Angle 1: ')
ylabel = label(pos=yarrow.axis, text='Angle 2: ')
zlabel = label(pos=zarrow.axis, text='Angle 3: ')

# flechas para IMU LSM9DS1
frontArrow_2=arrow(length=4,shaftwidth=.1,color=vector(154/255, 125/255, 10/255),axis=vector(1,0,0))
# upArrow_2=arrow(length=4,shaftwidth=.1,color=vector(29/255, 131/255, 72/255),axis=vector(0,1,0))
# sideArrow_2=arrow(length=4,shaftwidth=.1,color=vector(33/255, 97/255, 140/255),axis=vector(0,0,1))
# xlabel_2 = label(pos=frontArrow.axis, text='LSM9DS1 y')
# ylabel_2 = label(pos=sideArrow.axis, text='LSM9DS1 x')
# zlabel_2 = label(pos=upArrow.axis, text='LSM9DS1 z')

while (True):
    try:
        while (ad.inWaiting()==0):
            pass
        
        # recibir paquete de datos
        dataPacket=ad.readline()
        dataPacket=str(dataPacket,'utf-8')
        print(dataPacket)
        packet=dataPacket.split(",")

        # variables del IMU MPU6050
        yaw = (float(packet[0]) * toRad)
        pitch = (float(packet[1]) * toRad)
        roll = ((float(packet[2]) - 180) / 57.29578)
    
        # variables del IMU LSM9DS1
        yaw2 = (float(packet[3]) * toRad)
        pitch2 = (float(packet[4]) * toRad)
        roll2 = ((float(packet[5]) - 180) / 57.29578)
                
        # rate
        rate(100)

        # vectores para IMU MPU9250
        k=vector(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch))
        # y=vector(0,1,0)
        # s=cross(k,y)
        # v=cross(s,k)
        # vrot=v*cos(roll)+cross(k,v)*sin(roll)

        # vectores para IMU LSM9DS1
        k_2=vector(cos(yaw2)*cos(pitch2), sin(pitch2),sin(yaw2)*cos(pitch2))
        # y_2=vector(0,1,0)
        # s_2=cross(k_2,y_2)
        # v_2=cross(s_2,k_2)
        # vrot_2=v_2*cos(roll2)+cross(k_2,v_2)*sin(roll2)
        
        print(k)
        print(k_2)
        
        # sistema de referencia del MPU6050
        frontArrow.axis=k
        # sideArrow.axis=cross(k,vrot)
        # upArrow.axis=vrot

        # sistema de referencia del LSM9DS1
        frontArrow_2.axis=k_2
        # sideArrow_2.axis=cross(k_2,vrot_2)
        # upArrow_2.axis=vrot_2

        # ajuste de flechas para MPU6050
        # sideArrow.length=4
        frontArrow.length=4
        # upArrow.length=4
        
        # ajuste de flechas para LSM9DS1
        # sideArrow_2.length=4
        frontArrow_2.length=4
        # upArrow_2.length=4
        
        # xlabel.text = f"Angle 1: {diff_angle(sideArrow.axis, sideArrow_2.axis)*toDeg}"
        ylabel.text = f"Angle 2: {diff_angle(frontArrow_2.axis, frontArrow.axis)*toDeg}"
        zlabel.text = f"Angle 3: {diff_angle(frontArrow.axis, frontArrow_2.axis)*toDeg}"
        
        # xlabel_2.pos = frontArrow_2.pos
        # ylabel_2.pos = sideArrow_2.pos
        # zlabel_2.pos = upArrow_2.pos
    
    except:
        pass
