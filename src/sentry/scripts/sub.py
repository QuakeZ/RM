#!/usr/bin/env python
import rospy
from sentry.msg import Has
global linear
global flag
flag = 1
linear = -100
if __name__=="__main__":

    pub = rospy.Publisher('/found_info', Has, queue_size=1)
    rospy.init_node('TKt')
    msg = Has()
    rate = rospy.Rate(50)

    while not rospy.is_shutdown():
        msg.linear = linear
        pub.publish(msg)
        rospy.loginfo("pub %d",msg.linear)

        if linear == 100:
            flag = -1
        if linear == -100:
            flag = 1

        if flag == 1:
            linear = linear + 1
        if flag == -1:
            linear = linear -1

        rate.sleep()



















