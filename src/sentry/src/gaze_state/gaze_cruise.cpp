//加入修正后的巡航   gazebo
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <nav_msgs/Odometry.h>
#include <std_msgs/Int8.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Accel.h>
#include <stdlib.h>

float s,v,_v;
int xxx = 1;
bool recover_mode()//修正mode
{
    if(v>0&&(v*v<=2*5*(2.3-s)))       //判断从这个位置减速是否可以减到零     加速度为0.02/0.02=1
    return 0;
    else if(v<0&&(v*v<=2*5*s))
    return 0;
    else if(v==0)
    return 0;
    else return 1;
}

void chatterCallback(const std_msgs::Int8ConstPtr& msg3)
{
  xxx=msg3->data;
}

   void chatterCallback_pose(const nav_msgs::OdometryConstPtr& odom)
{
      v = odom->twist.twist.linear.x;
      s = odom->pose.pose.position.x;
      		
if (recover_mode())//
   {
        v=0;
	_v = v;
	//ROS_INFO("Recover!");
   }
else
   {     
	if(s<2.2&&s>0.2&&v>=0&&v<0.4)
	//if(x<8&&x>5&&v>=0&&v<1)
	 	{//geometry_msgs::Twist msg;
		//msg.linear.x+=0.002;
		_v = v;
		v+=0.1;
	}
	else if(s<=0.2)
	//else if(x<=5)
		{//geometry_msgs::Twist msg;
		//msg.linear.x=1;
		_v = v;
		v+=0.1;
	}
	else if(v>=0.4&&s<2.2)
	//else if(v>=1&&x<8)
		{//geometry_msgs::Twist msg;
		//msg.linear.x-=0.002;
		_v = v;
		v-=0.1;	
	}
	else if(s>=2.2)
	//else if(x>=8)
		{//geometry_msgs::Twist msg;
		//msg.linear.x-=0.002;
		_v = v;
		v-=0.1;	
	}
	else if(v<0&&v>-0.4&&s<2.2&&s>0.2)
	//else if(v<0&&v>-1&&x<8&&x>5)
		{//geometry_msgs::Twist msg;
		//msg.linear.x=-1;
		_v = v;
		v-=0.1;
	}
	else if(v<=-0.4&&s>0.2)
	//else if(v<=-1&&x>5)
		{//geometry_msgs::Twist msg;
		//msg.linear.x+=0.002;
		_v = v;
		v+=0.1;
	}
 //ROS_INFO("I heard pose x:[%f] v:[%f]",s ,v);
	
   }
}
   
int main(int argc, char **argv)
  {
      	ros::init(argc, argv, "gaze_cruise");
      	ros::NodeHandle n;
      	ros::Subscriber sub = n.subscribe("/chassis/odom",1000,chatterCallback_pose);
	ros::Subscriber sub2 = n.subscribe("order_Chassis",10,chatterCallback);
	ros::Publisher pub = n.advertise <geometry_msgs::Twist>("/chassis/cmd_vel", 1000);//此处留意一下
	
        ros::Publisher a_pub = n.advertise <geometry_msgs::Accel>("sentry/chassis/acel", 1000);
        ros::Rate rate(50);
	geometry_msgs::Twist msg;
	geometry_msgs::Accel msa;
	while(ros::ok())
     {
	if(xxx == 1)
	{
	  s=s+v/60;
	  //ROS_INFO("ZUO BIAO:%f  !!!",s);
	  msg.linear.x=v; 
	  pub.publish(msg);//向刚才锁定topic发布消息
	  msa.linear.x = (v-_v)*50;
	  a_pub.publish(msa);
	}    
        ros::spinOnce();//这句是订阅的回调点
        rate.sleep();
     }

	return 0;			 
        
  }

