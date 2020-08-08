 //追踪模式
#include "ros/ros.h"
#include <sentry/Aim.h>
#include <geometry_msgs/Twist.h>
#include "turtlesim/Pose.h"
#include <std_msgs/Int8.h>
float s,v=0;       //运行速度依旧设为1
int xxx;
void chatterCallback_pose(const turtlesim::PoseConstPtr& msg2)
{
      s=msg2->x;//获取位置信息
}
void chatterCallback2_pose(const std_msgs::Int8ConstPtr& msg3){
	 xxx=msg3->data;
 }
void Callback(const sentry::Aim& aim)//获取相对位置，并根据上述信息作出反应
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
      ros::init(argc, argv, "test11");
      ros::NodeHandle n;
 
      //ros::Publisher pub = n.advertise <geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
      ros::Publisher pub2 = n.advertise <geometry_msgs::Twist>("chassis_cmd_vel", 1000);
      ros::Rate rate(10);
      ros::Subscriber sub = n.subscribe("aim",1000,Callback);
      ros::Subscriber sub2 = n.subscribe("/sentry/position",1000,chatterCallback_pose);
      //ros::Subscriber sub2 = n.subscribe("robot_pose",1000,chatterCallback_pose);
      ros::Subscriber sub3 = n.subscribe("order_Chassis",10,chatterCallback2_pose);
      while(ros::ok())
     {
        ros::spinOnce();
	if(xxx==3){
	geometry_msgs::Twist msg;	
	msg.linear.x=v;      
        pub2.publish(msg);
	ROS_INFO("Zhuizong!!!");
	}
        rate.sleep();
     }
}
