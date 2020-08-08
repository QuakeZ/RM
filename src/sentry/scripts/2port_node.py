#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
import rospy
from std_msgs.msg import Int8
import serial
from std_msgs.msg import String
import math
import sys
import json
import time
import threading
from geometry_msgs.msg import Twist
from hkvs.msg import vel
import logging
import smach
import smach_ros
from multiprocessing import Process
from time import sleep
#交互头文件
#from dynamic_reconfigure.server import Server
#from teleopaserial.cfg import serialConfig

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

global distance
twist = Twist()


#云台yaw速度
def send_gimbal(cv):
    data = { "gimbal1":[cv]}
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'?')

#decide search or attack
def send_state_order(order):
    data = {"gimbal":[order]}
    data_string = json.dumps(data)
    ser.write('*' + data_string + '<')

def send_command(vx,vy,vz):
    data = { "chassis":[vx,vy,vz] }
    data_string = json.dumps(data)
    ser.write('*'+ data_string +';')
    rospy.loginfo('-----has send chassis-----')


def callback1(vel_visual):
	cv = vel_visual.vel
	print("i heard visual_vel",cv)
        send_gimbal(int(cv))

def ordercallback(order_state):
	order = order_state.data
	print("i heard order_state",order)
        send_state_order(int(order))

def vel_callback(twist):
    speed_x = twist.linear.s
    speed_y = 0
    speed_z = 0
    send_command(int(speed_x),int(speed_y),int(speed_z))
    rospy.loginfo("cmd_linear_x: "+ str(speed_x))

#控制下位机kpki
#def control_chassis_kpki(chassis_kp,chassis_ki,gimbal_kp,gimbal_ki,bullet_kp,bullet_ki):
#    data = {"Pid_param":[chassis_kp,chassis_ki,gimbal_kp,gimbal_ki,bullet_kp,bullet_ki]}
#    data_string = json.dumps(data)
#    ser.write('*' + data_string + '>')


#人机交互
#def uptodowncontrol(config, level):
#    print("chassis kp,ki  is:{},{}".format(config.chassis_kp,config.chassis_ki))
#    print("gimbal  kp,ki  is:{},{}".format(config.gimbal_kp,config.gimbal_ki))
#    print("bullet  kp,ki  is:{},{}".format(config.bullet_kp,config.bullet_ki))
#    print('*'*30)
    #control_chassis_kpki(int(100*config.chassis_kp),int(100*config.chassis_ki),int(100*config.gimbal_kp),int(100*config.gimbal_ki),int(100*config.bullet_kp),int(100*config.bullet_ki))
#    return config

#def callback2(translate):
#        transform =translate.data
#        transform=transform.strip()
#        if transform=="up": 
#             send_gimbal_hand_angle(1,0)
#        if transform=="down": 
#             send_gimbal_hand_angle(-1,0)
#        if transform=="left": 
#             send_gimbal_hand_angle(0,1)
#        if transform=="right": 
#             send_gimbal_hand_angle(0,-1)

#def callback3(twist):
#        speed_x = twist.linear.x
#        speed_y = twist.linear.y
#        speed_z = twist.angular.z
#        rospy.loginfo("linear_x: %f",speed_x)
#        send_chassis(int(speed_x),int(speed_y),int(speed_z))
#'''

#主循环

rospy.init_node('port_node')

#srv = Server(serialConfig, uptodowncontrol)
rospy.Subscriber("vel_linear",vel, callback1)
rospy.Subscriber("order_PanTilt",Int8, ordercallback)
rospy.Subscriber("chassis_cmd_vel",Twist ,vel_callback)
#rospy.Subscriber("schemaTranslation",String, callback2)
#rospy.Subscriber('teleop_twist', Twist, callback3)

pub_vel = rospy.Publisher('robot_pose', Pose, queue_size=10)

while not rospy.is_shutdown():
    #while True:
    	#time.sleep(0.1)
    	#print("我在接受下位机消息")
    	line = ser.readline()
        try:
        	de_json = json.loads(line)
                distance = de_json[1][1]
                rospy.loginfo("distance : "+str(distance))
                msg = Pose()
                msg.x = distance
                pub_vel.publish(msg)

                current_pitch = de_json[1][7]
		current_yaw = de_json[1][8]
                rospy.loginfo("current angle:{},{}".format(current_pitch,current_yaw)
        except
                None
		#pass
        	#print("get data false!!! please check for errors [qsy]")

#rospy.spin()
send_chassis(0,0,0)
ser.close

#关闭节点时先按control-s再按control-c
