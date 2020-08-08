#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import rospy
from geometry_msgs.msg import Twist
from turtlesim.msg import Pose



def test2():
	rospy.init_node('test2', anonymous=True)

	pub = rospy.Publisher('robot_vel', Pose, queue_size=10)	
	##pub = rospy.Publisher('chassis_cmd_vel', Twist, queue_size=10)
	rate = rospy.Rate(10)

        while not rospy.is_shutdown():        ##保证在关闭节点时主函数一直循环！！

##	 msg = Twist()
##	 msg.linear.x = 20
##	 msg.angular.z = 0
##	 pub.publish(msg)
##       rospy.loginfo('-----------------has send-----------')
##	 rate.sleep()

	 msg = Pose()
	 msg.x = 1
	 pub.publish(msg)
         rospy.loginfo('-----------------has send-----------')
	 rate.sleep()

if __name__ == '__main__':
    try:
        test2()
    except rospy.ROSInterruptException:
        pass                           
