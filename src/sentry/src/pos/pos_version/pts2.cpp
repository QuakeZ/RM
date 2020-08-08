#include<ros/ros.h>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
using namespace std;
using namespace Eigen;

double pos = 0,vel = 1;//position,velocity
double t = 1,u = 0,y = 0;//time,controlled variable/acel, output

int main(int argc, char **argv)
{	
    //定义
    MatrixXd X(2,1),X_(2,1),_X(2,1);
    MatrixXd P(2,2),_P(2,2),P_(2,2);
    MatrixXd Z(1,1),R(1,1),U(2,1),K(2,1),Y(1,1);
	MatrixXd F(2,2),G(2,1),H(1,2);
    MatrixXd Nz(1,1),Q(2,2);
    MatrixXd A(2,2),B(2,1);
	
	F << 1,t,0,1;
	G << 0.5*t*t ,t;
	H << 0,1;
    R << 0;
    
    A << 0,1,0,0;
	B << 0,1;
    
//temp
    R << 1;
    Nz << 0;
    Q << 0,0,0,0;
     
////////////////////////////////////////////////////////////////////////
	//0.初始化
    _X << 0,0.78;
    _P << 1,0,0,1;


    //1.测量
    Z = vel * MatrixXd::Identity(1,1) + Nz;
    //R = 

    
    //2.更新
    Z = vel * MatrixXd::Identity(1,1) + Nz;
    //R =  
    K = _P * H.transpose() * ( ( H * _P * H.transpose() + R ).inverse() );
    X = X_ + K * (Z - H * X_);
    P = ( MatrixXd::Identity(2,2) - K * H ) * P_; 

    MatrixXd tmp1(1,2);tmp1<<1,0;
    U = u * MatrixXd::Identity(1,1);
    Y = tmp1 * X + U * 0;
    y = Y.determinant();
    cout << "y=\n" <<  y << endl;


    //3.预测
    //U = u * MatrixXd::Identity(1,1);
    X_ = F * _X + G * U;
    P_ = F * _P * F.transpose() + Q;

    
    //(4.变量更替 n->n+1)
    _X = X_ ;
    _P = P_ ;

    

    return 0;
}