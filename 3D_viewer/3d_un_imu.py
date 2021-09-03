from vpython import *
from time import *
import numpy as np
import math
import serial

ad=serial.Serial('com6',115200)
sleep(2)

scene.range=5
scene.background=color.black
toRad=2*np.pi/360
toDeg=1/toRad
scene.forward=vector(-1,-1,-1)

scene.width=600
scene.height=600

xarrow=arrow(lenght=2, shaftwidth=.1, color=color.red,axis=vector(1,0,0))
yarrow=arrow(lenght=2, shaftwidth=.1, color=color.green,axis=vector(0,1,0))
zarrow=arrow(lenght=2, shaftwidth=.1, color=color.blue,axis=vector(0,0,1))

frontArrow=arrow(length=4,shaftwidth=.1,color=color.purple,axis=vector(1,0,0))
upArrow=arrow(length=4,shaftwidth=.1,color=color.magenta,axis=vector(0,1,0))
sideArrow=arrow(length=4,shaftwidth=.1,color=color.orange,axis=vector(0,0,1))
xlabel = label(pos=frontArrow.axis, text='Angle: ')
ylabel = label(pos=sideArrow.axis, text='Angle-o: ')
# zlabel = label(pos=upArrow.axis, text='z1')

# frontArrow_2=arrow(length=4,shaftwidth=.1,color=color.purple,axis=vector(1,0,0))
# upArrow_2=arrow(length=4,shaftwidth=.1,color=color.magenta,axis=vector(0,1,0))
# sideArrow_2=arrow(length=4,shaftwidth=.1,color=color.orange,axis=vector(0,0,1))
# xlabel_2 = label(pos=frontArrow.axis, text='x1')
# ylabel_2 = label(pos=sideArrow.axis, text='y1')
# zlabel_2 = label(pos=upArrow.axis, text='z1')

while (True):
    try:
        while (ad.inWaiting()==0):
            pass
        dataPacket=ad.readline()
        dataPacket=str(dataPacket,'utf-8')
        splitPacket=dataPacket.split(",")

        # dataPacket2=ad2.readline()
        # dataPacket2=str(dataPacket2,'utf-8')
        # splitPacket2=dataPacket2.split(",")

        yaw = -(float(splitPacket[0]) / 57.29578)
        pitch = (float(splitPacket[1]) / 57.29578)
        roll = ((float(splitPacket[2]) - 180) / 57.29578)
        
        # yaw2 = (float(splitPacket2[0]) / 57.29578)
        # pitch2 = -(float(splitPacket2[1]) / 57.29578)
        # roll2 = -((float(splitPacket2[2]) - 180) / 57.29578)
        

        # q0=float(splitPacket[0])
        # q1=float(splitPacket[1])
        # q2=float(splitPacket[2])
        # q3=float(splitPacket[3])

        # roll=-math.atan2(2*(q0*q1+q2*q3),1-2*(q1*q1+q2*q2))
        # pitch=math.asin(2*(q0*q2-q3*q1))
        # yaw=-math.atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3))-np.pi/2

        #print(f"Roll: {roll}, Pitch: {pitch}, Yaw: {yaw}")
        
        rate(100)


        # ylabel.text = f"y: {pitch}"
        # zlabel.text = f"z: {yaw}"

        k=vector(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch))
        y=vector(0,1,0)
        s=cross(k,y)
        v=cross(s,k)
        vrot=v*cos(roll)+cross(k,v)*sin(roll)
        
        print(k)

        # k_2=vector(cos(yaw2)*cos(pitch2), sin(pitch2),sin(yaw2)*cos(pitch2))
        # y_2=vector(0,1,0)
        # s_2=cross(k_2,y_2)
        # v_2=cross(s_2,k_2)
        # vrot_2=v_2*cos(roll2)+cross(k_2,v_2)*sin(roll2)

        # frontArrow_2.axis=k_2
        # sideArrow_2.axis=cross(k_2,vrot_2)
        # upArrow_2.axis=vrot_2

        frontArrow.axis=k
        sideArrow.axis=cross(k,vrot)
        upArrow.axis=vrot

        # sideArrow.length=4
        # frontArrow.length=4
        # upArrow.length=4
        # sideArrow_2.length=4
        # frontArrow_2.length=4
        # upArrow_2.length=4
        xlabel.text = f"Angle: {int(diff_angle(frontArrow.axis, xarrow.axis)*toDeg)}"
        
        x1 = k.x
        y1 = k.z
        z1 = k.y
        x2 = 1
        y2 = 0
        z2 = 0
        
        dote = x1 * x2 + y1 * y2 + z1 * z2
        cross_x = (y1 * z2) - (z1 * y2)
        cross_y = (z1 * x2) - (x1 * z2)
        cross_z = (x1 * y2) - (y1 * x2)
        dete = sqrt(cross_x * cross_x + cross_y * cross_y + cross_z * cross_z)
        angle_o = atan2(dete, dote)
        
        theta1 = atan2(y1,x1)
        theta2 = atan2(y2,x2)
        angle_o = theta1 - theta2
        if angle_o < 0:
            angle_o = angle_o + 2*pi
        
        
        ylabel.text = f"Angle_o: {int(angle_o* toDeg)}"
    
    except:
        pass

    