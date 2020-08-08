#!/usr/bin/env python
import rospy
import smach
import smach_ros
from std_msgs.msg import Int8
from sentry.msg import Has

global hasfound
hasfound = 0
global isAttacked
isAttacked = 0
global counter
counter = 1

SAFECOUNT=5

pub = rospy.Publisher('order_Chassis', Int8, queue_size=10)

def callback(msg):    
    global hasfound
    global isAttacked
    hasfound = msg.linear
    isAttacked = msg.angular



# define state Cruise
class Cruise(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['isAttacked','noAttacked & hasFound'])
        
    
    def execute(self, userdata):
        rospy.loginfo('---------------------------------------------')
        rospy.loginfo('Executing state CRUISE')
        rospy.loginfo('isAttacked == %d hasfound == %d',isAttacked,hasfound)
        order = 1
        pub.publish(order)
        rospy.loginfo('publish order:%d',order)
        rate = rospy.Rate(1)

        while not rospy.is_shutdown():
            
            rospy.Subscriber('has_info', Has, callback)
            if (isAttacked == 1 and hasfound == 1) or (isAttacked == 1 and hasfound == 0):
                break
            if isAttacked == 0 and hasfound == 1:
                break
            global counter
            rospy.loginfo(counter)
            counter+=1
            rate.sleep()

        if (isAttacked == 1 and hasfound == 1) or (isAttacked == 1 and hasfound == 0):
            return 'isAttacked'           
        if isAttacked == 0 and hasfound == 1:
            return 'noAttacked & hasFound'
                       

            



# define state Defend
class Defend(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['isSafe & noFound','isSafe & hasFound'])
        self.safeCounter = 1

    def execute(self, userdata):
        rospy.loginfo('---------------------------------------------')
        rospy.loginfo('Executing state Defend')
        rospy.loginfo('isAttacked == %d hasfound == %d',isAttacked,hasfound)
        order = 2
        pub.publish(order)
        rospy.loginfo('publish order:%d',order)
        self.safeCounter = 1
        rate = rospy.Rate(1)

        while not rospy.is_shutdown():
            
            rospy.Subscriber('has_info', Has, callback)
            if isAttacked == 1:self.safeCounter = 0

            if isAttacked == 0 and hasfound == 0 and self.safeCounter == SAFECOUNT:
                break
            if isAttacked == 0 and hasfound == 1 and self.safeCounter == SAFECOUNT:
                break
                
            rospy.loginfo('self.safeCounter:[%d]',self.safeCounter)
            if isAttacked == 0:self.safeCounter+=1
            global counter
            rospy.loginfo(counter)
            counter+=1
            rate.sleep()

        if isAttacked == 0 and hasfound == 0 and self.safeCounter == SAFECOUNT:
            return 'isSafe & noFound'
        if isAttacked == 0 and hasfound == 1 and self.safeCounter == SAFECOUNT:
            return 'isSafe & hasFound'
        
        

# define state Attack
class Attack(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['isAttacked','noAttacked & noFound'])
    
    def execute(self, userdata):
        rospy.loginfo('---------------------------------------------')
        rospy.loginfo('Executing state ATTACK')
        rospy.loginfo('isAttacked == %d hasfound == %d',isAttacked,hasfound)
        order = 3
        pub.publish(order)
        rospy.loginfo('publish order:%d',order)
        rate = rospy.Rate(1)

        while not rospy.is_shutdown():
            
            rospy.Subscriber('has_info', Has, callback)
            if (isAttacked == 1 and hasfound == 0) or (isAttacked == 1 and hasfound == 1):               
                break
            elif isAttacked == 0 and hasfound == 0:               
                break
            global counter
            rospy.loginfo(counter)
            counter+=1
            rate.sleep()

        if (isAttacked == 1 and hasfound == 0) or (isAttacked == 1 and hasfound == 1):            
            return 'isAttacked'        
        if isAttacked == 0 and hasfound == 0:
            return 'noAttacked & noFound'               

            
# main
def main():
    rospy.init_node('smach_Chassis_state_machine')

    # Create a SMACH state machine
    sm = smach.StateMachine(outcomes=[])

    # Open the container
    with sm:
        # Add states to the container
        smach.StateMachine.add('CRUISE', Cruise(), 
                               transitions={'isAttacked':'DEFEND',
                                            'noAttacked & hasFound':'ATTACK'})
      
        smach.StateMachine.add('DEFEND', Defend(), 
                               transitions={'isSafe & noFound':'CRUISE',
                                            'isSafe & hasFound':'ATTACK'})

        smach.StateMachine.add('ATTACK', Attack(), 
                               transitions={'noAttacked & noFound':'CRUISE',
                                            'isAttacked':'DEFEND'})
    
    # Create and start the introspection server
    sis = smach_ros.IntrospectionServer('Chassis_smach_introspection_server2', sm, '/SM_ROOT2')
    sis.start()
    
    # Execute SMACH plan
    outcome = sm.execute()
    
    # Wait for ctrl-c to stop the application
    rospy.spin()
    sis.stop()

if __name__ == '__main__':
    main()
