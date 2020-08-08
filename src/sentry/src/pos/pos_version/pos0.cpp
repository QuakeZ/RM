#include<ros/ros.h>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include<geometry_msgs/Twist.h>
#include<geometry_msgs/Accel.h>
#include<geometry_msgs/Pose.h>
#include <iostream>
#include <stdlib.h>
using namespace std;
using namespace Eigen;

double pos = -99,vel = -88;//position,velocity
double t = 0.02,y = 0;//time,output
double v;
void print();
bool flag = 0,flaginit = 0;

MatrixXd X(2,1),X_(2,1),_X(2,1);
MatrixXd P(2,2),_P(2,2),P_(2,2);
MatrixXd Z(1,1),R(1,1),U(1,1),K(2,1),Y(1,1);
MatrixXd F(2,2),G(2,1),H(1,2);
MatrixXd Nz(1,1),Q(2,2),W(2,1);
MatrixXd A(2,2),B(2,1);

void velCallback(const geometry_msgs::Twist::ConstPtr &msg)
{  
  geometry_msgs::Twist twist;
  vel = msg->linear.x ;
  printf("==1==getV==\n");
}

void posCallback(const geometry_msgs::Pose::ConstPtr &mss)
{  
  geometry_msgs::Pose pose;
  pos = mss->position.x ;
  if (flag == 0)
  {
    X << pos,vel;
    flag = 1;
    printf("flag!\n");
  }
  printf("==2==getX==\n");


}


int main(int argc, char **argv)
{	

	//X << pos,vel;
    P << 100,0,0,1;
	F << 1,t,0,1;
	G << 0.5*t*t ,t;
	H << 0,1;
    U << 0;
    A << 0,1,0,0;
	B << 0,1;
    
//temp
    R << 0.1;
    Nz << -0.0515;
    Q << 1,0,0,1;
    W << 0,0;

////////////////////////////////////////////////////////////////////////

    ros::init(argc,argv,"positioning0");
    ros::NodeHandle n;
    ros::Rate rate(50);

    ros::Subscriber t_sub = n.subscribe("/sentry/fake/twist", 10, velCallback);
    ros::Subscriber p_sub = n.subscribe("/sentry/fake/pose", 10, posCallback);
    ros::Publisher po_pub = n.advertise <geometry_msgs::Pose>("/sentry/positionZero", 1000);
    geometry_msgs::Pose msgp;

    while(ros::ok())
    {
       ros::spinOnce();
	if(pos == -99)
	continue;	
	
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
	  //1.测量
          Z = vel * MatrixXd::Identity(1,1) + Nz;
          //R = ?

    
          //2.更新
          K = _P * H.transpose() * ( ( H * _P * H.transpose() + R ).inverse() );
          X = _X + K * (Z - H * _X);
          P = ( MatrixXd::Identity(2,2) - K * H ) * P_; 
          //publish 
	  MatrixXd tmp1(1,2);tmp1<<1,0;
          Y = tmp1 * X + U * 0;
          y = Y.determinant();
          msgp.position.x = y;
	  msgp.position.y = 0;
	  msgp.position.z = pos-y;
          po_pub.publish(msgp);

          print();

          //3.预测
          U = G.transpose() * (_X - W - F * X);
          X_ = F * X + G * U + W;
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

    MatrixXd tmp1(1,2);tmp1<<1,0;
    Y = tmp1 * X + U * 0;
    y = Y.determinant();
    v = (H * X).determinant();
    double u = U.determinant();
    cout<<"position.x:"<<y<<endl;
    cout<<"vel:"<<vel<<endl;
    cout<<"v:"<<v<<endl;
    cout<<"dx:"<< pos-y <<endl;
    //cout<<"acel:"<<u<<endl;
    cout<<"--------"<<endl;
}

