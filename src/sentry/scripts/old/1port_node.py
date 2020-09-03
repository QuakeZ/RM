#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

import rospy
import serial
import string
import math
import sys
import json
import time
from geometry_msgs.msg import Twist 
from turtlesim.msg import Pose


global distance
twist = Twist()

rospy.init_node("chassis_port")


port='/dev/dji_board'

rospy.loginfo("Opening %s...", port)
#控制电机速度
def send_command(vx,vy,vz):
    data = { "chassis":[vx,vy,vz] }
    data_string = json.dumps(data)
    ser.write('*'+ data_string +';')
    rospy.loginfo('-----------------has send-----------')


try:
    ser = serial.Serial(port=port, baudrate=115200, timeout=5)
except serial.serialutil.SerialException:
    rospy.logerr("IMU not found at port "+port + ". Did you specify the correct port in the launch file?")
    sys.exit(0)

#ser.write('t\n')
def vel_callback(twist):
    speed_x = twist.linear.x
    speed_y = 0
    speed_z = 0
    send_command(int(speed_x),int(speed_y),int(speed_z))
    #pub.publish(twist)
    rospy.loginfo("cmd_linear_x: " + str(speed_x))

rospy.Subscriber('chassis_cmd_vel', Twist, vel_callback)

pub1 = rospy.Publisher('robot_pose', Pose, queue_size=10)

while not rospy.is_shutdown():
    line1 = ser.readline()
    #rospy.loginfo("I have got chassis velicities: " + line)
    #解析字符串为python 对象
    try:
        de_json = json.loads(line1)

        distance= de_json[1][1]
	rospy.loginfo("distance: "+ str(distance) )


	msg = Pose()
	msg.x = distance
	pub1.publish(msg)
    except:
        None


#关闭节点时停止电机
#time.sleep(1)
send_command(0,0,0)
ser.close

#关闭节点时先按control-s再按control-c
