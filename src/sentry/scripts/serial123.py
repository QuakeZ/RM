#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
import rospy
import serial
from std_msgs.msg import String
import math
import sys
import json
import time
from geometry_msgs.msg import Twist
import logging
logging.basicConfig()

from time import sleep


port='/dev/dji_board'
rospy.loginfo("Opening %s...", port)
#控制电机速度


def send_gimbal(pitch,yaw):
    data = { "gimbal":[pitch,yaw]}
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'?')

def send_chassis(vx,vy,vz):
    data = { "chassis":[vx,vy,vz] }
    data_string = json.dumps(data)
    ser.write('*'+ data_string +';')

def send_translation(translation):
    data ={"translation":[translation]}
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'!')
    
def send_fric(fric):
    data ={"fric":[fric]}
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'>')

def send_trigger(trigger):
    data ={"trigger":[trigger]}
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'>')
def send_gimbal_hand_angle(hand_pitch,hand_yaw):
    data = {"hand_angle":[hand_pitch,hand_yaw]}
    data_string = json.dumps(data)
    ser.write('*' + data_string + '<')




try:
    ser = serial.Serial(port=port, baudrate=115200, timeout=5)
except serial.serialutil.SerialException:
    rospy.logerr("Dji not found at port "+port + ". Did you specify the correct port in the launch file?")
    sys.exit(0)




rospy.init_node('serial', anonymous=True)


while 1 :   
  speed_x = 3
  speed_y = 0
  speed_z = 0
  send_chassis(int(speed_x),int(speed_y),int(speed_z))
  rospy.loginfo("linear_x: %f",speed_x)


        



  


while not rospy.is_shutdown():
    line = ser.readline()
   
    #rospy.loginfo("I have got chassis velicities: " + line)
    #解析字符串为python 对象
    #rospy.loginfo("hello")
    #pub = rospy.Publisher('currant_vel', Twist, queue_size=10)
    #sub1= rospy.Subscriber("teleop_cmd_vel", Twist, callback)
    try:
        de_json = json.loads(line)
        #linear_x = de_json[1][0]
        #linear_y = de_json[1][1]
       # angular_z = de_json[1][2]
       # rospy.loginfo("get_feedback:[" + str(linear_x) + "," + str(linear_y)+ "," + str(angular_z) + "]\n")

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
send_command(0,0,0)
ser.close

#关闭节点时先按control-s再按control-c
