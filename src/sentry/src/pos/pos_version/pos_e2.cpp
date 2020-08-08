#include<ros/ros.h>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include<geometry_msgs/Twist.h>
#include<geometry_msgs/Accel.h>
using namespace std;
using namespace Eigen;

double pos = 0,vel = 0.78;//position,velocity
double t = 0.1,u = 0,y = 0;//time,controlled variable/acel, output
double v;
void print();

MatrixXd X(2,1),X_(2,1),_X(2,1);
MatrixXd P(2,2),_P(2,2),P_(2,2);
MatrixXd Z(1,1),R(1,1),U(2,1),K(2,1),Y(1,1);
MatrixXd F(2,2),G(2,1),H(1,2);
MatrixXd Nz(1,1),Q(2,2);
MatrixXd A(2,2),B(2,1);

void velCallback(const geometry_msgs::Twist::ConstPtr &msg)
{  
  geometry_msgs::Twist twist;
  vel = msg->linear.x ;
  ROS_INFO("heard vel:%lf",vel);
}
void accelCallback(const geometry_msgs::Accel::ConstPtr &mss)
{  
  geometry_msgs::Accel accel;
  u = mss->linear.x;
  ROS_INFO("heard u(accel):%lf",u);
}


int main(int argc, char **argv)
{	

	X << pos,vel;
    P << 1,0,0,1;
	F << 1,t,0,1;
	G << 0.5*t*t ,t;
	H << 0,1;
    R << 0;
    
    A << 0,1,0,0;
	B << 0,1;
    
//temp
    R << 0.1;
    Nz << 0;
    Q << 1,0,0,1;
    
////////////////////////////////////////////////////////////////////////
	//0.初始化
    _X = X;
    _P = P;
    print();


    ros::init(argc,argv,"positioning");
	ros::NodeHandle n;
	ros::Rate rate(10);
    ros::Subscriber sub = n.subscribe("vel_callback", 100, velCallback);
    ros::Subscriber sub2 = n.subscribe("accel_callback", 100, accelCallback);
    
    while(ros::ok())
	{
        //1.测量
        Z = vel * MatrixXd::Identity(1,1) + Nz;
        //R = ?

    
        //2.更新
        Z = vel * MatrixXd::Identity(1,1) + Nz;
        //R =  ?
        K = _P * H.transpose() * ( ( H * _P * H.transpose() + R ).inverse() );
        X = _X + K * (Z - H * _X);
        P = ( MatrixXd::Identity(2,2) - K * H ) * P_; 
        print();
        
        //3.预测
        U = u * MatrixXd::Identity(1,1);
        X_ = F * X + G * U;
        P_ = F * P * F.transpose() + Q;

        
        //(4.变量更替 n->n+1)
        _X = X_ ;
        _P = P_ ;
        ros::spinOnce();        
        rate.sleep();
	}

    return 0;
}

void print()
{
    MatrixXd tmp1(1,2);tmp1<<1,0;
    U = u * MatrixXd::Identity(1,1);
    Y = tmp1 * X + U * 0;
    y = Y.determinant();
    v = (H * X).determinant();
    ROS_INFO("y = %lf",y);
    ROS_INFO("v = %lf",v);
    ROS_INFO("-----------------------------------------------");
}

