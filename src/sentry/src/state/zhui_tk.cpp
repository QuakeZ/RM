//发布自定义消息   测试专用发布器
#include "ros/ros.h"
//#include <sentry/Num.h>
#include <sentry/Aim.h>
#include <stdlib.h>
int main(int argc, char **argv){
      ros::init(argc, argv, "zhui_tk");
      ros::NodeHandle n;
      //ros::Publisher pub1 = n.advertise <sentry::Num>("num", 1000);
      ros::Publisher pub2 = n.advertise <sentry::Aim>("aim", 1000);
      ros::Rate rate(10);//所有发布者的频率
	while(ros::ok())
     {
	//sentry::Num msg1;
	//msg1.num=1;
     //   pub1.publish(msg1);
	//ROS_INFO("[%d]",msg1.num);
	sentry::Aim msg2;
	msg2.x=1;
        pub2.publish(msg2);
	ROS_INFO("aim.x[%f]",msg2.x);
        rate.sleep();
     }
	return 0;	
}
