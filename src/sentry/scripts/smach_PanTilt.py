#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import rospy
import smach
import smach_ros
from std_msgs.msg import Int8
from sentry.msg import Has

global hasfound
hasfound = 0
global counter
counter = 0

pub = rospy.Publisher('order_PanTilt', Int8, queue_size=10)#pub 1=search,pub 2=attack

def callback(msg):    
    global hasfound
    hasfound=msg.linear
    #rospy.loginfo('callback')

# define state Search
class Search(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['hasFound',None])
        
    def execute(self, userdata):
        global counter
        rospy.loginfo('Executing state ||SEARCH||')
        order = 1
        pub.publish(order)
        rospy.loginfo('publish order:%d',order)
        rate = rospy.Rate(50)
        rospy.Subscriber('found_info', Has, callback)
        while not rospy.is_shutdown():
            pub.publish(order)
            if hasfound == 1:
               #order = 0
               #pub.publish(order)
               break
            if counter % 50 == 0:
                rospy.loginfo("%d",counter/50)
            #rospy.loginfo(counter)
            counter+=1
            rate.sleep()
        return 'hasFound'
        
# define state Attack
class Attack(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['noFound',None])

    def execute(self, userdata):
        global counter
        rospy.loginfo('Executing state ||ATTACK||')
        order = 2
        pub.publish(order)
        rospy.loginfo('publish order:%d',order)
        rate = rospy.Rate(50)
        rospy.Subscriber('found_info', Has, callback)
        while not rospy.is_shutdown():
            pub.publish(order)
            if hasfound == 0:
               #order = 0
               #pub.publish(order)
               break
            if counter % 50 == 0:
                rospy.loginfo("%d",counter/50)
            #rospy.loginfo(counter)
            counter+=1
            rate.sleep()
        return 'noFound'

# main
def main():
    rospy.init_node('smach_PanTilt_state_machine')

    # Create a SMACH state machine
    sm = smach.StateMachine(outcomes=['shutdown'])

    # Open the container
    with sm:
        # Add states to the container
        smach.StateMachine.add('SEARCH', Search(), 
                               transitions={'hasFound':'ATTACK',None:'shutdown'})
      
        smach.StateMachine.add('ATTACK', Attack(), 
                               transitions={'noFound':'SEARCH',None:'shutdown'})
    
    # Create and start the introspection server
    sis = smach_ros.IntrospectionServer('my_smach_introspection_server1', sm, '/SM_ROOT1')
    sis.start()
    
    # Execute SMACH plan
    outcome = sm.execute()
    
    # Wait for ctrl-c to stop the application
    rospy.spin()
    sis.stop()

if __name__ == '__main__':
    main()
