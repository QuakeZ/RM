#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <chrono>
using namespace std;

double my_px = 0;
double my_py = 0;
double my_pz = 0;
double my_vx = 99;
double my_ox,my_oy,my_oz,my_ow;


void OdomCallback(const nav_msgs::OdometryConstPtr &msg)
{   
    my_px = msg -> pose.pose.position.x;
    my_py = msg -> pose.pose.position.y;
    my_pz = msg -> pose.pose.position.z;
    //cout<<"heard my position: "<<endl;
    //cout<<my_px<<" , "<<my_py<<" , "<<my_pz<<endl;
    my_vx = msg -> twist.twist.linear.x;
}


int main(int argc, char **argv)
{
   	ros::init(argc, argv, "fake_odom");
   	ros::NodeHandle nh;
  	ros::Subscriber sub = nh.subscribe("/chassis/odom", 1, OdomCallback);
  	ros::Publisher twist_pub = nh.advertise <geometry_msgs::Twist>("/sentry/fake/twist", 1000);
   	ros::Publisher pose_pub = nh.advertise <geometry_msgs::Pose>("/sentry/fake/pose", 1000);
  
  	ros::Rate rate(50);
  	nav_msgs::Odometry msg;
  	geometry_msgs::Twist mst;
  	geometry_msgs::Pose msp;

	unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
	default_random_engine e (seed);
	normal_distribution<double> distN(0,1);

	int i = 0;
 
  while(i<1000)
  	{
      	ros::spinOnce();
    
      	mst.linear.x = my_vx + distN(e);
      	msp.position.x = my_px + distN(e);
      	mst.linear.y = my_vx;
      	msp.position.y = my_px;
 
    	twist_pub.publish(mst);
    	printf("fake vel:%lf\n",mst.linear.x);
		if(i == 20){msp.position.x = 9;}
		else if(i == 34){msp.position.x = -4;}
		else if(i == 688){msp.position.x = 0;}
    	pose_pub.publish(msp);
    	printf("pos-x:%lf\n",my_px);

		i++;

    	printf("--------\n");

    	rate.sleep();
  	}
  
  
  	//ros::spin();
  	return 0;
}

