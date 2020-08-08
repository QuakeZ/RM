#!/usr/bin/env python

import rospy
import smach
import smach_ros


global found
found = 0
global hasAttacked
hasAttacked = 0


# define state RegularDFS
class RegularDFS(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['oc1','oc2','oc3'])
        self.counter = 0
        self.safeTimer = 0
    def execute(self, userdata):
        rospy.loginfo('Executing state RegularDFS')

        if self.safeTimer == 5:
            if found == 0:
                return 'oc1'
            elif found != 0:
                return 'oc2'
        elif  self.counter == 8:
            return 'oc3'
            

# define state RandomDFS
class RandomDFS(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['oc1','oc2','oc3'])
        self.counter = 0
        self.safeTimer = 0

    def execute(self, userdata):
        rospy.loginfo('Executing state RandomDFS')

        if self.safeTimer == 5:
            if found == 0:
                return 'oc1'
            elif found != 0:
                return 'oc2'
        elif self.safeTimer != 5: 
            if self.counter == 8:
                return 'oc3'

# define state Cruise
class Cruise(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['is attacked','hasfound'])
        self.counter = 0
    
    def execute(self, userdata):
        rospy.loginfo('Executing state CRUISE')
        if hasAttacked == 1:
            return 'is attacked'
        elif found == 1:
            return 'hasfound'


# define state Attack
class Attack(smach.State):
    def __init__(self):
        smach.State.__init__(self, outcomes=['is attacked','nofound'])
        self.counter = 0
    
    def execute(self, userdata):
        rospy.loginfo('Executing state ATTACK')
        if hasAttacked == 1:
            return 'is attacked'
        elif found == 0:
            return 'nofound'


def main():
    rospy.init_node('smach_example_state_machine')

    # Create the top level SMACH state machine
    sm_top = smach.StateMachine(outcomes=['stop'])

    # Open the container
    with sm_top:

        smach.StateMachine.add('CRUISE', Cruise(),
                               transitions={'is attacked':'DEFENSE',
                                            'hasfound':'ATTACK'})
        smach.StateMachine.add('ATTACK', Attack(),
                               transitions={'is attacked':'DEFENSE',
                                            'nofound':'CRUISE'})

        # Create the sub SMACH state machine
        sm_sub = smach.StateMachine(outcomes=['to CRUISE','to ATTACK'])

        # Open the container
        with sm_sub:

            # Add states to the container
            smach.StateMachine.add('RegularDFS', RegularDFS(),
                                   transitions={'oc1':'to CRUISE',
                                                'oc2':'to ATTACK',
                                                'oc3':'RandomDFS'})
            smach.StateMachine.add('RandomDFS', RandomDFS(),
                                   transitions={'oc1':'to CRUISE',
                                                'oc2':'to ATTACK',
                                                'oc3':'RegularDFS'})

        smach.StateMachine.add('DEFENSE', sm_sub,
                               transitions={'to CRUISE':'CRUISE',
                                            'to ATTACK':'ATTACK'})

    # Create and start the introspection server
    sis = smach_ros.IntrospectionServer('my_smach_introspection_server', sm_top, '/SM_ROOT')
    sis.start()

    # Execute SMACH plan
    outcome = sm_top.execute()

    # Wait for ctrl-c to stop the application
    rospy.spin()
    sis.stop()

if __name__ == '__main__':
    main()

