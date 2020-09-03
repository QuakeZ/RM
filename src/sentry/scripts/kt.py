#!/usr/bin/env python
import rospy
from sentry.msg import Has
import sys, select, termios, tty

banner = """
Reading from the keyboard  and Publishing
---------------------------
send:
        w
   a    s    d
     z         c
anything else : stop
CTRL-C to quit
"""

keyad = {
  'a':(1,0),
  'd':(0,0),
}

keyws = {
  'w':(1,0),
  's':(0,0),
}

keyzc = {
  'z':(1,0),
  'c':(0,0),
}

def getKey():
   tty.setraw(sys.stdin.fileno())
   select.select([sys.stdin], [], [], 0)
   key = sys.stdin.read(1)
   termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
   return key


if __name__=="__main__":
    settings = termios.tcgetattr(sys.stdin)
    pub = rospy.Publisher('/att_info', Has, queue_size=1)
    pub2 = rospy.Publisher('/found_info', Has, queue_size=1)
    pub3 = rospy.Publisher('/burst_info', Has, queue_size=1)
    rospy.init_node('keykt')
    print(banner)
    msg = Has()
    mss = Has()
    mgg = Has()
    while(1):
       key = getKey()
       if key in keyad.keys():
           msg.angular = keyad[key][0]  
           pub.publish(msg)

       if key in keyws.keys():
           mss.linear = keyws[key][0]
           pub2.publish(mss)

       if key in keyzc.keys():
           mgg.data = keyzc[key][0]
           pub3.publish(mgg)

       if (key == '\x03'):
           break

       termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)



















