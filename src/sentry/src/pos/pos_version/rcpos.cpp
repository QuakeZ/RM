#include <ros/ros.h>
#include <dynamic_reconfigure/server.h>
#include <dynamic_kf/posKFConfig.h>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include<geometry_msgs/Twist.h>
#include<geometry_msgs/Accel.h>
#include<geometry_msgs/Pose.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
using namespace std;
using namespace Eigen;
 
double pos = -99,vel = -88;//pos初始位置 vel测量速度
double t = 0.02,y = 0;//t间隔时间 y输出的位置

void print();//输出信息到终端的函数
bool flag = 0,flaginit = 0;//flag判断是否获得初始位置、初始速度，flaginit判断是否初始化

MatrixXd X(2,1),_X(2,1),X_(2,1);//t t-1 t+1 时刻的状态量 
MatrixXd P(2,2),_P(2,2),P_(2,2);//t t-1 t+1 时刻的状态量的协方差
MatrixXd Z(1,1),R(1,1),U(1,1),K(2,1),Y(1,1);//测量量矩阵，状态更新的参数，控制量矩阵（加速度），卡尔曼系数，输出矩阵
MatrixXd F(2,2),G(2,1),H(1,2);//F!G!H!协方差外推
MatrixXd Vn(1,1),Q(2,2),W(2,1);//，协方差外推的噪声，状态外推的噪声
//MatrixXd A(2,2),B(2,1);

void dynamic_callback(dynamic_kf::posKFConfig &config)
{
    ROS_INFO("Reconfigure Request: %s %d %f %f %s %d",
            config.cmd_topic.c_str(),
            config.cmd_pub_rate,
            config.linear_x,
            config.angular_z,
            config.move_flag?"True":"False",
            config.log_level);
 
    cmd_topic = config.cmd_topic;
    loop_rate = config.cmd_pub_rate;
    linear_x  = config.linear_x;
    angular_z = config.angular_z;
    move_flag = config.move_flag;
    log_level = config.log_level;
}
 
 
int main(int argc, char **argv)
{
    ros::init(argc, argv, "dynamic_kf_node");
    ros::NodeHandle handle;
 
    dynamic_reconfigure::Server<dynamic_kf::posKFConfig> server;
    dynamic_reconfigure::Server<dynamic_kf::posKFConfig>::CallbackType callback;
 
    callback = boost::bind(&dynamic_callback, _1, _2);
    server.setCallback(callback);
 
    ros::Publisher cmdVelPub;
    geometry_msgs::Twist speed;
 
    while(ros::ok())
    {
        cmdVelPub = handle.advertise<geometry_msgs::Twist>(cmd_topic, 1);
        ros::Rate rate(loop_rate);
        if(move_flag)
        {
            speed.linear.x  = linear_x;
            speed.angular.z = angular_z;
            cmdVelPub.publish(speed);
 
            switch(log_level)
            {
                case 0:
                    ROS_INFO("log level: INFO, cmd_pub_rate: %d", loop_rate);
                    break;
                case 1:
                    ROS_WARN("log level: WARN, cmd_pub_rate: %d", loop_rate);
                    break;
                case 2:
                    ROS_ERROR("log level: ERROR, cmd_pub_rate: %d", loop_rate);
                    break;
                default:
                    break;
            }
        }
        ros::spinOnce();
        rate.sleep();
    }
}
