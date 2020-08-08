#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
import rospy
import serial
import math
import sys
import json
import time
import threading
from geometry_msgs.msg import Twist
from teleopaserial.msg import angle,down
import logging
from multiprocessing import Process
from time import sleep
#from teleopaserial.msg import down
logging.basicConfig()

translation =1
port='/dev/dji_board'
rospy.loginfo("Opening %s...", port)
ser=1
'''try:
    ser = serial.Serial(port=port, baudrate=115200, timeout=5)
except serial.serialutil.SerialException:
    rospy.logerr("Dji not found at port "+port + ". Did you specify the correct port in the launch file?")
    sys.exit(0)'''


while not rospy.is_shutdown():
    #line = ser.readline()
    print("11")
    #rospy.loginfo("I have got chassis velicities: " + line)
    #pub = rospy.Publisher('currant_vel', Twist, queue_size=10)
    try:
        de_json = json.loads(line)
        #linear_x = de_json[1][0]
        #linear_y = de_json[1][1]
       # angular_z = de_json[1][2]
       # rospy.loginfo("get_feedback:[" + str(linear_x) + "," + str(linear_y)+ "," + str(angular_z) + "]\n")
        pitch1 = de_json[1][3]
        yaw2 = de_json[1][4]
        vx = de_json[1][0]
        vy = de_json[1][1]
        vz = de_json[1][2]
        a=de_json[1][5]
        b=de_json[1][6]
        c=de_json[1][7]
        d=de_json[1][8]
        SUIDONGWANCHENG1=a
        GIMBAL_FINISH_ANGLE_NINE1=b
        GIMBAL_FINISH_ANGLE_HALF1=c
        #rospy.loginfo("get_feedback:[" + str(pitch1) + "," + str(yaw2)+ ","  +  "]\n")
        #rospy.loginfo("vx:%d,vy:%d,vz:%d",vx,vy,vz)
        #twist = Twist()

        #speed_mps=(pwm-1.27)/38.61
        #x=-(linear_y-1.27)/38.61
        #y=-(linear_x-1.27)/38.61
        #z=angular_z*2.7
        
        # re   rad/s
        # 0.33 0.918
        # 0.55 1.53
        # 0.7  1.93


        #rospy.loginfo("resive base:[%.2f,%.2f,%.2f]",x,y,z)
        #twist.linear.x = float(x); twist.linear.y =float(y); twist.linear.z = 0
        #twist.angular.x = 0; twist.angular.y = 0; twist.angular.z = float(z)
        #pub.publish(twist)
    except:
        None
    #rospy.loginfo(de_json)
    #控制电机速度
    # speed_x = 0
    # speed_y = 30
    # speed_z = 0
    # send_command(speed_x,speed_y,speed_z)
#关闭节点时停止电机
#time.sleep(1)
ser.close

#关闭节点时先按control-s再按control-c
