 //追踪模式
#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int8.h>
#include <gazebo_msgs/ModelStates.h>
#include <stdio.h>
#include <iostream>
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
int xxx;

void chatterCallback(const std_msgs::Int8ConstPtr& msg3)
{
  xxx=msg3->data;
}

void modelStatesCallback(const gazebo_msgs::ModelStatesConstPtr &msg)
{
    int modelCount = msg->name.size();

    for(int modelInd = 0; modelInd < modelCount; ++modelInd)
    { 
        if(msg->name[modelInd] == "mrobot")
        {
             geometry_msgs::Pose pose = msg->pose[modelInd];
             
             
             my_x = pose.position.x;
             my_y = pose.position.y;
             my_z = pose.position.z;
             //cout<<"heard my position: "<<endl;
             //cout<<my_x<<" , "<<my_y<<" , "<<my_z<<endl;
             //cout<<"have heard my pos"<<endl;
             
        }

        if(msg->name[modelInd] == "Mailbox")
        {
             geometry_msgs::Pose pose = msg->pose[modelInd];
             aim_x = pose.position.x;
             aim_y = pose.position.y;
             aim_z = pose.position.z;
             //cout<<"heard my position: "<<endl;
             //cout<<aim_x<<" , "<<aim_y<<" , "<<aim_z<<endl;
             //cout<<"have heard my aim"<<endl;
             break;
        }
        //break;
    }
}

int main(int argc, char **argv)
{
      ros::init(argc, argv, "zhui");
      ros::NodeHandle n;
      ros::Publisher pub = n.advertise <geometry_msgs::Twist>("cmd_vel", 1000);
      ros::Subscriber sub = n.subscribe("/gazebo/model_states", 1, modelStatesCallback);
      ros::Subscriber sub2 = n.subscribe("order_Chassis",10,chatterCallback);
      ros::Rate rate(10);
      double dis = 0;
      geometry_msgs::Twist mss;
      while(ros::ok())
     {
        ros::spinOnce();
        dis = aim_x - my_x;
        
	if(xxx==3)
	{
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

	}
     
        rate.sleep();
     }
}
