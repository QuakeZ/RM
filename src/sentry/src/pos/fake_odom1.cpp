#include <ros/ros.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;

double my_px = 0;
double my_py = 0;
double my_pz = 0;
double my_vx = 99;
double my_ox,my_oy,my_oz,my_ow;

//Callback 函数定义如下，根据模型的名称，可得对应模型的状态：
void OdomCallback(const nav_msgs::OdometryConstPtr &msg)
{   
    
    my_px = msg -> pose.pose.position.x;
    my_py = msg -> pose.pose.position.y;
    my_pz = msg -> pose.pose.position.z;
    //cout<<"heard my position: "<<endl;
    //cout<<my_px<<" , "<<my_py<<" , "<<my_pz<<endl;
    my_vx = msg -> twist.twist.linear.x;
}

double sampleNormal() 
{
    double u = ((double) rand() / (RAND_MAX)) * 2 - 1;
    double v = ((double) rand() / (RAND_MAX)) * 2 - 1;
    double r = u * u + v * v;
    if (r == 0 || r > 1 ) 
    return sampleNormal();
    double c = sqrt(-2 * log(r) / r);
    return u * c;
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
  while(ros::ok())
  {
    ros::spinOnce();
    
    mst.linear.x = my_vx; //+ sampleNormal();
    msp.position.x = my_px ;
 
    twist_pub.publish(mst);
    printf("fake vel:%lf\n",mst.linear.x);

    pose_pub.publish(msp);
    printf("pos-x:%lf\n",my_px);

    printf("--------\n");
    rate.sleep();
  }
  
  
  //ros::spin();

  return 0;
}

