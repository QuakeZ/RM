//加入修正后的巡航
#include "ros/ros.h"
 #include "std_msgs/String.h"
 #include "turtlesim/Pose.h"
 #include <geometry_msgs/Twist.h>
 #include <stdlib.h>
 #include <std_msgs/Int8.h>
// #include <boost/bind.hpp>
 
/*#include<tf/transform_broadcaster.h>
#include<nav_msgs/Odometry.h>
#include<geometry_msgs/Twist.h>
#include<iostream>*/
    //#include <sstream>
 
  
  float x, y, theta,v,s=5.544445;
  int xxx=1;
bool recover_mode()//修正mode
{
    if(v>0&&(v*v<=2*1*(9-x)))       //判断从这个位置减速是否可以减到零     加速度为0.02/0.02=1
    return 0;
    else if(v<0&&(v*v<=2*1*x))
    return 0;
    else if(v==0)
    return 0;
    else return 1;
}

 void chatterCallback2_pose(const std_msgs::Int8ConstPtr& msg3){
	 xxx=msg3->data;
 }

   void chatterCallback_pose(const turtlesim::PoseConstPtr& msg2)
{
     // float x, y, theta,v;
      x=msg2->x;
      y=msg2->y;
      theta=msg2->theta;
      v=msg2->linear_velocity;
      //ROS_INFO("I heard pose x:[%f] y:[%f] z:[%f] v[%f]",x, y, theta,v);
      		
if (recover_mode())//
   {
        v=0;
	ROS_INFO("Recover!");
   }
else
   {     
	if(s<8&&s>5&&v>=0&&v<1)
	//if(x<8&&x>5&&v>=0&&v<1)
	 	{//geometry_msgs::Twist msg;
		//msg.linear.x+=0.002;
		v+=0.02;
	}
	else if(s<=5)
	//else if(x<=5)
		{//geometry_msgs::Twist msg;
		//msg.linear.x=1;
		v+=0.02;
	}
	else if(v>=1&&s<8)
	//else if(v>=1&&x<8)
		{//geometry_msgs::Twist msg;
		//msg.linear.x-=0.002;
		v-=0.02;	
	}
	else if(s>=8)
	//else if(x>=8)
		{//geometry_msgs::Twist msg;
		//msg.linear.x-=0.002;
		v-=0.02;	
	}
	else if(v<0&&v>-1&&s<8&&s>5)
	//else if(v<0&&v>-1&&x<8&&x>5)
		{//geometry_msgs::Twist msg;
		//msg.linear.x=-1;
		v-=0.02;
	}
	else if(v<=-1&&s>5)
	//else if(v<=-1&&x>5)
		{//geometry_msgs::Twist msg;
		//msg.linear.x+=0.002;
		v+=0.02;
	}
 ROS_INFO("I heard pose x:[%f] y:[%f] z:[%f] v[%f]",x, y, theta,v);
	
   }
}
   
 /*void callback(const geometry_msgs::Twist& cmd_vel)
{
	ROS_INFO("Received a /cmd_vel message!");
	ROS_INFO("Linear Components:[%f,%f,%f]",cmd_vel.linear.x,cmd_vel.linear.y,cmd_vel.linear.z);
	ROS_INFO("Angular Components:[%f,%f,%f]",cmd_vel.angular.x,cmd_vel.angular.y,cmd_vel.angular.z);
	// Do velocity processing here:  
}**/
//在test1的基础上加了另一个发布者控制它启动。
int main(int argc, char **argv)
  {
      	ros::init(argc, argv, "test1");
      	ros::NodeHandle n;
      	ros::Subscriber sub = n.subscribe("/sentry/position",1000,chatterCallback_pose);
      	//ros::Subscriber sub = n.subscribe("robot_pose",1000,chatterCallback_pose);
     	ros::Subscriber sub2 = n.subscribe("order_Chassis",10,chatterCallback2_pose);
		//ros::Publisher pub = n.advertise <geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
		ros::Publisher pub2 = n.advertise <geometry_msgs::Twist>("chassis_cmd_vel", 10);
	ros::Rate rate(60);
	while(ros::ok())
     {
	if(xxx==1)
	{
	geometry_msgs::Twist msg;
	s=s+v/60;
	ROS_INFO("ZUO BIAO:%f  !!!",s);
	msg.linear.x=v;     
        //pub.publish(msg);
        pub2.publish(msg);
	}
        ros::spinOnce();//这句就是同时发布节点和订阅节点的关键了
        rate.sleep();
     }
	return 0;			 
        
	//ros::Subscriber sub = n.subscribe("turtle1/pose",1000,boost::bind(chatterCallback_pose,_1,pub));
	//ros::Subscriber sub2 = n.subscribe("/turtle1/cmd_vel", 1000, callback);
      //std::cout<<" for test1"<<std::endl;
      //ros::spin();
 
      //return 0;
  }

