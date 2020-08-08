#!/usr/bin/env python
import rospy
from std_msgs.msg import Int8
def callback(a):
    rospy.loginfo('I heard order:%d', a.data)
    rospy.loginfo('-------------------------')


rospy.init_node('listener', anonymous=True) 
rospy.Subscriber('order_PanTilt', Int8, callback)
rospy.spin()
