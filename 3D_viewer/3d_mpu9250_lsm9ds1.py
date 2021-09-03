# importar librerias
from os import kill
from vpython import *
from time import *
import numpy as np
import math
import serial

# variables de conversion
toRad = 2*np.pi/360
toDeg = 1/toRad

# conexion serial
ad = serial.Serial('com6',115200)
sleep(1)

# configuracion de la escena
scene.range=5
scene.background=color.black
scene.forward=vector(-1,-1,-1)
scene.width=600
scene.height=600

# flechas del sistema de referencia
xarrow=arrow(lenght=2, shaftwidth=.1, color=color.red,axis=vector(1,0,0))
yarrow=arrow(lenght=2, shaftwidth=.1, color=color.green,axis=vector(0,1,0))
zarrow=arrow(lenght=2, shaftwidth=.1, color=color.blue,axis=vector(0,0,1))

# flechas para el MPU9250
frontArrow=arrow(length=4,shaftwidth=.1,color=color.purple,axis=vector(1,0,0))
# upArrow=arrow(length=4,shaftwidth=.1,color=color.magenta,axis=vector(0,1,0))
# sideArrow=arrow(length=4,shaftwidth=.1,color=color.orange,axis=vector(0,0,1))

# flechas para el LSM9DS1
frontArrow_2=arrow(length=4,shaftwidth=.1,color=color.cyan,axis=vector(1,0,0))
# upArrow_2=arrow(length=4,shaftwidth=.1,color=color.yellow,axis=vector(0,1,0))
# sideArrow_2=arrow(length=4,shaftwidth=.1,color=color.white,axis=vector(0,0,1))

# textos
xlabel = label(pos=xarrow.axis, text='')
ylabel = label(pos=yarrow.axis, text='')
zlabel = label(pos=zarrow.axis, text='')

# programa de iteracion
while (True):
    try:
        while (ad.inWaiting()==0):
            pass
        
        # recibir paquete de datos
        dataPacket=ad.readline()
        dataPacket=str(dataPacket,'utf-8')
        # print(dataPacket)
        packet=dataPacket.split(",")

        # euler del IMU MPU9250
        yawm = (float(packet[0]) * toRad)
        pitchm = (float(packet[1]) * toRad)
        # pitchm = 0.0
        rollm = ((float(packet[2]) - 180) / 57.29578)
        # rollm = 0.0
    
        # euler del LSM9DS1
        yawl = -(float(packet[3]) * toRad)
        pitchl = -(float(packet[4]) * toRad)
        # pitchl = 0.0
        rolll = ((float(packet[5]) - 180) / 57.29578)
        # rolll = 0.0
                
        # rate
        rate(100)

        # vectores para IMU MPU9250 ---------------------------------------
        k=vector(-cos(yawm)*cos(pitchm),sin(pitchm),sin(yawm)*cos(pitchm))
        # k=vector(-cos(yawm)*cos(pitchm),0,sin(yawm)*cos(pitchm))
        # y=vector(0,1,0)
        # s=cross(k,y)
        # v=cross(s,k)
        # vrot=v*cos(rollm)+cross(k,v)*sin(rollm)
        
        # sistema de referencia del MPU9250
        frontArrow.axis=k
        # sideArrow.axis=cross(k,vrot)
        # upArrow.axis=vrot
        
        # ajuste de flechas para MPU6050
        # sideArrow.length=4
        frontArrow.length=4
        # upArrow.length=4

        # vectores para IMU LSM9DS1 ----------------------------------------
        kl=vector(cos(yawl)*cos(pitchl),sin(pitchl),sin(yawl)*cos(pitchl))
        # kl=vector(cos(yawl)*cos(pitchl),0,sin(yawl)*cos(pitchl))
        # y_2=vector(0,1,0)
        # s_2=cross(k_2,y_2)
        # v_2=cross(s_2,k_2)
        # vrot_2=v_2*cos(rollm2)+cross(k_2,v_2)*sin(rollm2)
        
        # sistema de referencia del LSM9DS1
        frontArrow_2.axis=kl
        # sideArrow_2.axis=cross(k_2,vrot_2)
        # upArrow_2.axis=vrot_2

        # ajuste de flechas para LSM9DS1
        # sideArrow_2.length=4
        frontArrow_2.length=4
        # upArrow_2.length=4
        
        # print(k)
        # print(kl)
        
        # obtener el angulo ---------------------------------------------------
        x1 = k.x
        y1 = k.z
        z1 = k.y
        x2 = kl.x
        y2 = kl.z
        z2 = kl.y
        
        theta1 = atan2(y1,x1)
        theta2 = atan2(y2,x2)
        angle_o = theta1 - theta2
        if angle_o < 0:
            angle_o = angle_o + 2*pi
        
        
        xlabel.text = f"Angle (360): {int(angle_o* toDeg)}"
        ylabel.text = f"Angle (): {360 - int(angle_o* toDeg)}"
        zlabel.text = f"Angle: {int(diff_angle(frontArrow_2.axis, frontArrow.axis)*toDeg)}"
    
    except:
        pass