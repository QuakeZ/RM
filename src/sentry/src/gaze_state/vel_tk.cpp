#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int8.h>
#include <gazebo_msgs/ModelStates.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char **argv)
{
      ros::init(argc, argv, "vel_tk");
      ros::NodeHandle n;
      ros::Publisher pub = n.advertise <geometry_msgs::Twist>("/chassis/cmd_vel", 1000);
      ros::Rate rate(2);
   
      geometry_msgs::Twist mss;
      while(ros::ok())
     {
        ros::spinOnce();
        
	mss.linear.x = 0.1;      
        pub.publish(mss);
	//ROS_INFO("talking!");    
        
     
        rate.sleep();
     }
}
