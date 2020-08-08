#include "ros/ros.h"
#include "turtlesim/Pose.h"
#include <std_msgs/Int8.h>
#include <geometry_msgs/Twist.h>
#include <stdlib.h>

#define begin 150     //all   0~195 0-164
#define end 20
#define v_max 50

float s,v,flag=1;
int xxx=1;
void chatterCallback(const std_msgs::Int8ConstPtr& msg1)
{
  xxx=msg1->data;
}
void chatterCallback_pose(const turtlesim::PoseConstPtr& msg2)
{
      s=msg2->x;
      ROS_INFO(" location now  %f",s);
      if((s>=begin)) flag=1;
      else if(s<=end) flag=-1;
      if(flag==1)
      {
       v=v_max;
      }
      if(flag==-1)
      {
       v=-v_max;
      }
      //if((s>=begin)) v=v_max;
      //else if(s<=end) v=-v_max;
      //else if((s<begin)&&(s>end)&&(s<=s_last)) v=v_max;
      //else if((s<begin)&&(s>end)&&(s>s_last)) v=-v_max;
}
 
      		

int main(int argc, char **argv)
  {
      	ros::init(argc, argv, "cruise");
      	ros::NodeHandle n;
      	ros::Subscriber sub = n.subscribe("robot_pose",1000,chatterCallback_pose);
	ros::Subscriber sub2 = n.subscribe("order_Chassis",10,chatterCallback);
	ros::Publisher pub = n.advertise <geometry_msgs::Twist>("chassis_cmd_vel", 1000);

        ros::Rate rate(50);
	geometry_msgs::Twist msg;
	//geometry_msgs::Accel msa;
	while(ros::ok())
     {
	
	  ros::spinOnce();
	  msg.linear.x=v;
        if(xxx == 1)
	{ 
	  pub.publish(msg);
          ROS_INFO("i send v:%f ",v);

	}   
          ROS_INFO("flag:%f ",flag);
        
          rate.sleep();
     }

	return 0;			 
        
  }

