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
global ifBurst
ifBurst = 0
SAFECOUNT=5*50

pubo = rospy.Publisher('order_Chassis', Int8, queue_size=10)
pubb = rospy.Publisher('order_Burst', Int8, queue_size=10)

def foundcallback(msgf):    
    global hasfound
    hasfound = msgf.linear
    #rospy.loginfo('hasfound == %d',hasfound)
    #rospy.loginfo("--------------------------")
def attcallback(msga):    
    global isAttacked
    isAttacked = msga.angular
    #rospy.loginfo('isAttacked == %d',isAttacked)
    #rospy.loginfo("============================")
def burstcallback(msgb):    
    global ifBurst
    ifBurst = msgb.data
    #rospy.loginfo('ifBurst == %d',ifBurst)
    #rospy.loginfo("++++++++++++++++++++++++++++")


# define state Cruise
class Cruise(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['isAttacked','noAttacked & hasFound',None])
        
    
    def execute(self, userdata):
        global counter
        rospy.loginfo('---------------------------------------------')
        rospy.loginfo('Executing state ||CRUISE||')
        rospy.loginfo('isAttacked == %d hasfound == %d',isAttacked,hasfound)
        order = 1
        pubo.publish(order)
        rospy.loginfo('publish [Order:%d]',order)
        rate = rospy.Rate(50)
        rospy.Subscriber('found_info', Has, foundcallback)
        rospy.Subscriber('att_info', Has, attcallback)
        while not rospy.is_shutdown():
            if (isAttacked == 1 and hasfound == 1) or (isAttacked == 1 and hasfound == 0):
                break
            if isAttacked == 0 and hasfound == 1:
                break
            #rospy.loginfo(counter)
            if counter % 50 == 0:
                rospy.loginfo("%d",counter/50)
            counter+=1
            rate.sleep()

        if (isAttacked == 1 and hasfound == 1) or (isAttacked == 1 and hasfound == 0):
            return 'isAttacked'           
        if isAttacked == 0 and hasfound == 1:
            return 'noAttacked & hasFound'
                       

            



# define state Defend
class Defend(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['isSafe & noFound','isSafe & hasFound',None])
        self.safeCounter = 1

    def execute(self, userdata):
        global counter
        global _Burst
        rospy.loginfo('---------------------------------------------')
        rospy.loginfo('Executing state ||Defend||')
        rospy.loginfo('isAttacked : %d|hasfound : %d|Burst : %d',isAttacked,hasfound,ifBurst)
        order = 2
        _Burst = 0
        pubo.publish(order)
        pubb.publish(ifBurst)
        rospy.loginfo('publish [order:%d]',order)
        self.safeCounter = 1
        rate = rospy.Rate(50)
        rospy.Subscriber('found_info', Has, foundcallback)
        rospy.Subscriber('burst_info', Has, burstcallback)
        rospy.Subscriber('att_info', Has, attcallback)
        while not rospy.is_shutdown():
            if ifBurst == 1 and _Burst == 0:
                rospy.loginfo('Burst Mode ON !!!')
            if ifBurst == 0 and _Burst == 1:
                rospy.loginfo('Burst Mode OFF!')
            if isAttacked == 1:self.safeCounter = 0

            if isAttacked == 0 and hasfound == 0 and self.safeCounter == SAFECOUNT:
                break
            if isAttacked == 0 and hasfound == 1 and self.safeCounter == SAFECOUNT:
                break
            if counter % 50 == 0:    
                rospy.loginfo('self.safeCounter:[%d]',self.safeCounter/50)
            if isAttacked == 0:self.safeCounter+=1
            #rospy.loginfo(counter)
            if counter % 50 == 0:
                rospy.loginfo("%d",counter/50)
            _Burst = ifBurst
            counter+=1
            rate.sleep()

        if isAttacked == 0 and hasfound == 0 and self.safeCounter == SAFECOUNT:
            return 'isSafe & noFound'
        if isAttacked == 0 and hasfound == 1 and self.safeCounter == SAFECOUNT:
            return 'isSafe & hasFound'
        
        

# define state Attack
class Attack(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['isAttacked','noAttacked & noFound',None])
    
    def execute(self, userdata):
        global counter
        rospy.loginfo('---------------------------------------------')
        rospy.loginfo('Executing state ||ATTACK||')
        rospy.loginfo('isAttacked == %d hasfound == %d',isAttacked,hasfound)
        order = 3
        pubo.publish(order)
        rospy.loginfo('publish [order:%d]',order)
        rate = rospy.Rate(50)
        rospy.Subscriber('found_info', Has, foundcallback)
        rospy.Subscriber('att_info', Has, attcallback)
        while not rospy.is_shutdown():
            if (isAttacked == 1 and hasfound == 0) or (isAttacked == 1 and hasfound == 1):               
                break
            elif isAttacked == 0 and hasfound == 0:               
                break
            #rospy.loginfo(counter)
            if counter % 50 == 0:
                rospy.loginfo("%d",counter/50)
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
    sm = smach.StateMachine(outcomes=['shutdown'])

    # Open the container
    with sm:
        # Add states to the container
        smach.StateMachine.add('CRUISE', Cruise(), 
                               transitions={'isAttacked':'DEFEND',
                                            'noAttacked & hasFound':'ATTACK',
                                            None:'shutdown'})
      
        smach.StateMachine.add('DEFEND', Defend(), 
                               transitions={'isSafe & noFound':'CRUISE',
                                            'isSafe & hasFound':'ATTACK',
                                            None:'shutdown'})

        smach.StateMachine.add('ATTACK', Attack(), 
                               transitions={'noAttacked & noFound':'CRUISE',
                                            'isAttacked':'DEFEND',
                                            None:'shutdown'})
    
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
