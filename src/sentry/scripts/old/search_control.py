#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-
import rospy
import serial
import math
import sys
import json
import time
from teleopaserial.msg import angle
from time import sleep
from std_msgs.msg import Int8   ##order

rospy.init_node('gimbal_control_search', anonymous=True) ##init

port='/dev/dji_board'
rospy.loginfo("Opening %s...", port) ##begin

def send_gimbal(pitch,yaw):            ## rotate the gimbal
    data = { "gimbal":[pitch,yaw]}
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'?')

'''
def send_fric(fric):                   ## fire
    data ={"fric":[fric]}
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'>')

def send_trigger(trigger):           ## fire
    data ={"trigger":[trigger]}
    data_string = json.dumps(data)
    ser.write('*'+ data_string +'>')
'''

try:					## check out
    ser = serial.Serial(port=port, baudrate=115200, timeout=5)
except serial.serialutil.SerialException:
    rospy.logerr("IMU not found at port "+port + ". Did you specify the correct port in the launch file?")
    sys.exit(0)

global pitch
global yaw
global s

def controlcallback(angle):
    pitch=angle.pitch
    yaw=angle.yaw
    z=angle.s
    send_gimbal(int(pitch),int(yaw))   ##send over
    print("attack!")

def ordercallback(Int8):
    if Int8.date==1:
	  if pitch!=0:
 	     pitch=0
	  else:pitch=180
	  if yaw!=0:
	     yaw=0
	  else:yaw=180     ##这里0到180只是一个角度上下限  可调!
	  
          send_gimbal(int(pitch),int(yaw))   ##send over
          print("checking！")
          time.sleep(1)   ##时间间隔1s 可以调整

##pub 
rospy.Subscriber("control",angle, controlcallback)
rospy.Subscriber("order_PanTilt",Int8, ordercallback)
pub_ang = rospy.Publisher('robot_angle', angle, queue_size=10)

while not rospy.is_shutdown():   ##xun huan + callback1
    line = ser.readline()
   
    #rospy.loginfo("I have got chassis velicities: " + line)s
    #解析字符串为python 对象

    try:
        de_json = json.loads(line)     ##feedback
        #linear_x = de_json[1][0]
        #linear_y = de_json[1][1]
       # angular_z = de_json[1][2]
       # rospy.loginfo("get_feedback:[" + str(linear_x) + "," + str(linear_y)+ "," + str(angular_z) + "]\n")
        pitch1 = de_json[1][3]
        yaw2 = de_json[1][4]
	rospy.loginfo("get_feedback:[" + str(pitch1) + "," + str(yaw2)+ ","  +  "]\n")



        msg = angle()
        msg.pitch = pitch1
        msg.yaw = yaw2
        pub_ang.publish(msg)
        rospy.loginfo("upload_angle_yaw: %f",msg.yaw)
        rospy.loginfo("upload_angle_pitch: %f",msg.pitch)

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

