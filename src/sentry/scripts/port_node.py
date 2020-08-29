#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
import rospy
import serial
from std_msgs.msg import String
from std_msgs.msg import Int8
import math
import sys
import json
import time
import threading
from hkvs.msg import vel
from geometry_msgs.msg import Twist
import logging
import smach
import smach_ros
from multiprocessing import Process
from time import sleep
from turtlesim.msg import Pose

logging.basicConfig()
port='/dev/dji_board'
rospy.loginfo("Opening %s...", port)
ser=1

try:
    ser = serial.Serial(port=port, baudrate=115200, timeout=5)
except serial.serialutil.SerialException:
    rospy.logerr("Dji not found at port "+port + ". Did you specify the correct port in the launch file?")
    sys.exit(0)
print("串口初始化完成")

#==========================================================#

global distance


#smach state switch
def send_state_order(order):
    data={"state":[order]}##!!state
    data_string=json.dumps(data)
    ser.write('*'+data_string+'<')

def send_gimbal(cv):
    data = { "gimbal":[cv]}##!!gimbal
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'?')

#底盘
def send_command(vx,vy,vz):
    data = { "chassis":[vx,vy,vz] }
    data_string = json.dumps(data)
    ser.write('*'+ data_string +';')
    rospy.loginfo('-----------------has send-----------')



#在这里发布视觉linear
def callback1(vel_visual):
	cv = vel_visual.vel
	print("i heard visual_vel",cv)
        send_gimbal(int(cv))


def callback2(translate):
        transform =translate.data
        transform=transform.strip()
        if transform=="up": 
           	send_gimbal_hand_angle(1,0)
        if transform=="down": 
           	send_gimbal_hand_angle(-1,0)
        if transform=="left": 
        	send_gimbal_hand_angle(0,1)
        if transform=="right": 
             	send_gimbal_hand_angle(0,-1)

def ordercallback(order_state):
        order = order_state.data
        print("i heard order_state",order)
        send_state_order(int(order))


def vel_callback(twist):
        speed_x = twist.linear.x
        speed_y = 0
        speed_z = 0
        send_command(int(speed_x),int(speed_y),int(speed_z))
        rospy.loginfo("cmd_linear_x: " + str(speed_x))
        #pub.publish(twist)

#===============================================================#


rospy.init_node('port_node', anonymous=True)
rospy.Subscriber("vel_linear",vel, callback1)
rospy.Subscriber('chassis_cmd_vel', Twist, vel_callback)
rospy.Subscriber("schemaTranslation",String, callback2)
rospy.Subscriber('order_PanTilt', Int8, ordercallback)

pub1 = rospy.Publisher('robot_pose', Pose, queue_size=10)

while not rospy.is_shutdown():
    line = ser.readline()
    try:
        de_json = json.loads(line)
        distance= de_json[1][1]
	rospy.loginfo("distance: "+ str(distance) )
	msg = Pose()
	msg.x = distance
	pub1.publish(msg)
        #twist = Twist()
    except:
        None

send_command(0,0,0)
send_gimbal(0)
send_state_order(0)
ser.close

#关闭节点时先按control-s再按control-c
