#include <ros/ros.h>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Accel.h>
#include <turtlesim/Pose.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;
using namespace Eigen;

double pos = 0,vel = 0;//pos初始位置 vel测量速度 acel 
double acel = 0;
double t = 0.01;//t间隔时间 
float y = 0;//y输出的位置

void print();//输出信息到终端的函数
bool flag = 0,flaginit = 0;//flag判断是否获得初始位置、初始速度，flaginit判断是否初始化

MatrixXd X(2,1),_X(2,1),X_(2,1);//t t-1 t+1 时刻的状态量 
MatrixXd P(2,2),_P(2,2),P_(2,2);//t t-1 t+1 时刻的状态量的协方差
MatrixXd Z(1,1),R(1,1),U(1,1),K(2,1),Y(1,1);//测量量矩阵，状态更新的参数，控制量矩阵（加速度），卡尔曼系数，输出矩阵
MatrixXd F(2,2),G(2,1),H(1,2);//F!G!H!协方差外推
MatrixXd Q(2,2),W(2,1);//协方差外推的噪声，状态外推的噪声
//MatrixXd A(2,2),B(2,1);

void velCallback(const geometry_msgs::Twist::ConstPtr &msg)//获取观测速度的回调函数 从有误差的伪里程计订阅
{  
  geometry_msgs::Twist twist;
  vel = msg->linear.x ;//vel状态量X中的速度
  printf("==1==getV==\n");
}
/*
void acelCallback(const geometry_msgs::Accel::ConstPtr &msa)
{  
  geometry_msgs::Accel accel;
  acel = msa->linear.x ;
  printf("==2==getA==\n");
}
*/


int main(int argc, char **argv)
{	

    F << 1,t,0,1;//状态转换矩阵
    U << 0;//控制量
    G << 0.5*t*t ,t;//控制矩阵
    P << 10,0,0,10;//估计不确定度 初值
///////////////////////////
    H << 0,1;//观察矩阵
///////////////////////////
    
//temp
    R << 10000;//测量不确定度
    Q << 15,0,0,15;//过程噪声方差
    W << 0,0;//过程噪声向量

////////////////////////////////////////////////////////////////////////

    ros::init(argc,argv,"pos_v1");
    ros::NodeHandle n;
    ros::Rate rate(50);

    //ros::Subscriber a_sub = n.subscribe("/sentry/chassis/acel", 10, acelCallback);
    ros::Subscriber t_sub = n.subscribe("/robot_vel", 10, velCallback);
    ros::Publisher po_pub = n.advertise <turtlesim::Pose>("/sentry/position", 1000);
    turtlesim::Pose msgp;

    while(ros::ok())
    {
        ros::spinOnce();
        
        if( flaginit == 0)
	    {
            //0.初始化
    	    _X = X;
    	    _P = P;
            flaginit = 1;
            printf("init!\n");
	    }
	    else
	    {
	        //1.观测
            Z << vel;
	    U << acel;

            //2.更新
            K = _P * H.transpose() * ( ( H * _P * H.transpose() + R ).inverse() );//计算此刻卡尔曼系数

            X = _X + K * (Z - H * _X);
            P = ( MatrixXd::Identity(2,2) - K * H ) * _P 
            * ( MatrixXd::Identity(2,2) - K * H ).transpose()
            + K * R * K.transpose(); 
            
            //publish 
            MatrixXd tmp1(1,2);tmp1<<1,0;
            Y = tmp1 * X + U * 0;//提取位置矩阵
            y = Y.determinant();//转换位置数值
            msgp.x = y;//位置数值
            //msgp.y = 0;//0作为误差的基准线，便于plot观察
            
            po_pub.publish(msgp);//发布

            print();//数据打印到终端

            //3.预测
            X_ = F * X + G * U ;
            P_ = F * P * F.transpose() + Q;

            
            //(4.变量更替 n->n+1)
            _X = X_ ;
            _P = P_ ;
	    }

        rate.sleep();
    }

    return 0;
}

void print()
{
    double v;
    MatrixXd tmp1(1,2);//把状态量转换成位置的矩阵
    tmp1<<1,0;
    Y = tmp1 * X + U * 0;
    y = Y.determinant();
    v = (H * X).determinant();
    double u = U.determinant();
    cout<<"pos_x:"<<y<<endl;
    //cout<<"vel:"<<vel<<endl;
    //cout<<"v:"<<v<<endl;

    //cout<<"acel:"<<acel<<endl;
    cout<<"--------"<<endl;
}

