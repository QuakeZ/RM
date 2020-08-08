 //追踪模式
#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int8.h>
#include <gazebo_msgs/ModelStates.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Accel.h>
#include "turtlesim/Pose.h"
#include <stdio.h>
#include <iostream>
#include <sentry/Aim.h>
using namespace std;

double v1=0.2;
double v2=0.6;
double v3=0.1;      
double my_x = 0;
double my_y = 0;
double my_z = 0;
double aim_x = 0;
double aim_y = 0;
double aim_z = 0;
int xxx = 0;
float s,v=0;

void orderCallback(const std_msgs::Int8ConstPtr& msg3)
{
  xxx=msg3->data;
}
void Callback_pose(const turtlesim::PoseConstPtr& msg2)
{
      s=msg2->x;//获取位置信息
}
void aimCallback(const sentry::Aim& aim)//获取相对位置，并根据上述信息作出反应
{
     if(aim.x==0||(aim.x<0&&s<=0)||(aim.x>0&&s>=10))
       	v=0;  
     else if(aim.x<0&&s>0)
	v=-1;
     else if(aim.x>0&&s<=10)
	v=1;
     ROS_INFO("I heard pose s[%f],v[%f]",s,v);
}

int main(int argc, char **argv)
{
      ros::init(argc, argv, "zhui");
      ros::NodeHandle n;

      ros::Publisher pub = n.advertise <geometry_msgs::Twist>("chassis_cmd_vel", 1000);

      ros::Subscriber sub = n.subscribe("robot_pose", 1, Callback_pose);
      ros::Subscriber sub2 = n.subscribe("order_Chassis",10,orderCallback);
      ros::Subscriber sub3 = n.subscribe("aim",1000,aimCallback);

      ros::Rate rate(10);
      double dis = 0;
      geometry_msgs::Twist mss;
      while(ros::ok())
     {
        ros::spinOnce();
        dis = aim_x - my_x;
        
	if(xxx==3)
	{
          mss.linear.x = 0;
          pub.publish(mss);////////
/*
	  if(dis > 0.01 && my_x <= 1.9)
          {
	    ROS_INFO("dis:%lf,aim_x:%lf,my_x%lf",dis,aim_x,my_x);

	    if(dis<0.15)
	    {
	        mss.linear.x = v3;      
          	pub.publish(mss);
	  	ROS_INFO("+tracing");
	    }

	    else if(dis<0.4)
	    {
		mss.linear.x = v1;      
          	pub.publish(mss);
	  	ROS_INFO("+tracing");
	    }
	    else
	    {
		mss.linear.x = v2;      
          	pub.publish(mss);
	  	ROS_INFO("+tracing");
	    }		
	  
         }

         else if(dis < -0.01 && my_x >= 0.1)
         {	
	  if(dis>-0.15)
	    {
	        mss.linear.x = -1*v3;      
          	pub.publish(mss);
	  	ROS_INFO("+tracing");
	    }

	    else if(dis>-0.4)
	    {
		mss.linear.x = -1*v1;      
          	pub.publish(mss);
	  	ROS_INFO("+tracing");
	    }
	    else
	    {
		mss.linear.x = -1*v2;      
          	pub.publish(mss);
	  	ROS_INFO("+tracing");
	    }		
         }

         else
         { 
	  mss.linear.x = 0;      
          pub.publish(mss);
	  ROS_INFO("stop");
	 }
*/
	}
     
        rate.sleep();
     }
}
