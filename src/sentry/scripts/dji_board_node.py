#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
import rospy
import serial
from std_msgs.msg import String
import math
import sys
import json
import time
import threading
from geometry_msgs.msg import Twist
import logging
import smach
import smach_ros
from multiprocessing import Process
from time import sleep


#串口信息初始化
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

def send_chassis(vx,vz):
    data = { "chassis":[vx,vz] }
    data_string = json.dumps(data)
    ser.write('*'+ data_string +';')

def cmdCB(twist):
        print("callback")
        speed_x = twist.linear.x
        speed_z = twist.angular.z
	print("speed_x%f,speed_z%f"%(speed_x,speed_z))
        send_chassis(int(1000*speed_x),int(1000*speed_z))

#主循环
rospy.init_node('serial')

rospy.Subscriber('teleop_twist', Twist, cmdCB)
while not rospy.is_shutdown():
    	time.sleep(0.1)
    	#line = ser.readline()
    	try:
        	de_json = json.loads(line)
		current_left_front_wheel = de_json[1][0]
		current_right_front_wheel = de_json[1][1]
		current_left_rear_wheel = de_json[1][2]
		current_right_rear_wheel = de_json[1][3]
		current_vx = de_json[1][4]
		current_vy = de_json[1][5]
		current_angular_z = de_json[1][6]
		current_pitch = de_json[1][7]
		current_yaw = de_json[1][8]
		downyawpub.publish(current_yaw)
		print("i get wheel speed zs,rs,zx,yx:{},{},{},{}".format(current_left_front_wheel,current_right_front_wheel,current_left_rear_wheel,current_right_rear_wheel))
		print("i get current vel:{},{},{}".format(current_vx,current_vy,current_angular_z))
		print("i get current angle:{},{}".format(current_pitch,current_yaw))
    	except:
		pass
        	#print("get data false!!! please check for errors [qsy]")

#rospy.spin()
print("end")
send_chassis(0,0)
ser.close

