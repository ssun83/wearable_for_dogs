from vpython import *
from time import *
import serial


ad=serial.Serial('com6',115200)
sleep(2)

scene.range=5
scene.background=color.black
# scene.forward=vector(-1,-1,-1)

scene.width=1200
scene.height=800

roll_imu1 = label(text = "Roll IMU 1: ", pos = vector(0, 2, 0), height = 60, color = vector(0.5, 1, 1), box=False)
roll_imu2 = label(text = "Roll IMU 2: ", pos = vector(0, 1, 0), height = 60, color = vector(0.8,1, 1), box=False)
rom = label(text = "ROM angle: ", pos = vector(0, -1, 0), height = 100, color = vector(1, 0.5, 0), box=False)


while (True):
    while (ad.inWaiting()==0):
        pass

    dataPacket=ad.readline()
    dataPacket=str(dataPacket,'utf-8')
    splitPacket=dataPacket.split(",")

    data_1 = splitPacket[0]
    data_2 = splitPacket[1]
    data_3 = splitPacket[2]

    print(f"{data_1} - {data_2} - {data_3}")

    rate(100)

    roll_imu1.text = f"Roll IMU 1: {data_1} º"
    roll_imu2.text = f"Roll IMU 2: {data_2} º"
    rom.text = f"ROM angle: {data_3} º"


    