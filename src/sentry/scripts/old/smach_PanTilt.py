#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import rospy
import smach
import smach_ros
from std_msgs.msg import Int8

global counter
counter = 0
pub = rospy.Publisher('chatter', Int8, queue_size=10)
# define state Search
class Search(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['outcome1'])
        

    def execute(self, userdata):
        rospy.loginfo('Executing state SEARCH')
        global counter
        counter = 0
        counter += 1
        if counter == 1:
            pub.publish(counter)
            counter += 1
            return 'outcome1'
        

# define state Target
class Target(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['outcome2','outcome3'])

    def execute(self, userdata):
        rospy.loginfo('Executing state TARGET')
        global counter
        if counter == 2:
            pub.publish(counter)
            counter += 1
            return 'outcome3'
        elif counter == 4:
            pub.publish(counter)
            counter += 1
            return 'outcome2'

# define state Attack
class Attack(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['outcome4'])

    def execute(self, userdata):
        rospy.loginfo('Executing state ATTACK')
        global counter
        if counter == 3:
            pub.publish(counter)
            counter += 1
            return 'outcome4'

# main
def main():
    rospy.init_node('smach_example_state_machine')

    # Create a SMACH state machine
    sm = smach.StateMachine(outcomes=[])

    # Open the container
    with sm:
        # Add states to the container
        smach.StateMachine.add('SEARCH', Search(), 
                               transitions={'outcome1':'TARGET'})
        smach.StateMachine.add('TARGET', Target(), 
                               transitions={'outcome2':'SEARCH',
                                            'outcome3':'ATTACK'})
        smach.StateMachine.add('ATTACK', Attack(), 
                               transitions={'outcome4':'TARGET'})
    
    # Create and start the introspection server
    sis = smach_ros.IntrospectionServer('my_smach_introspection_server', sm, '/SM_ROOT')
    sis.start()
    
    # Execute SMACH plan
    outcome = sm.execute()
    
    # Wait for ctrl-c to stop the application
    rospy.spin()
    sis.stop()

if __name__ == '__main__':
    main()
