#include<ros/ros.h>
#include<geometry_msgs/Twist.h>
#include<geometry_msgs/Accel.h>
using namespace std;
 
int main(int argc, char **argv)
{
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("vel_callback", 1);
  ros::Publisher accel_pub = n.advertise<geometry_msgs::Accel>("accel_callback", 1);
  ros::Rate loop_rate(10);
  double count = 1;
  while (ros::ok())
  {
    geometry_msgs::Twist msg;
    msg.linear.x = count;
    geometry_msgs::Accel (mss);
    mss.linear.x = 1;
    
    
    vel_pub.publish(msg);
    ROS_INFO("talk:vel = %lf", msg.linear.x);

    accel_pub.publish(mss);
    ROS_INFO("talk:accel = %lf", mss.linear.x);
    
    loop_rate.sleep();
    ++count;
  }
  return 0;
}

