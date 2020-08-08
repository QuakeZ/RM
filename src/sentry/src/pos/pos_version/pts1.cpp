#include<ros/ros.h>
#include<cmath>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <cstdlib>
using namespace std;
using namespace Eigen;

double psn = 1.234;//position
double vel = 1.001;//velocity
double delt = 0.01;//delta t
double u = 0;//controlled variable = aceleration
double y;
int main(int argc, char **argv)
{	
	
//
	MatrixXd A(2,2);
	A<< 0,1,0,0;
	MatrixXd B(2,1);
	B<<0,1;
	MatrixXd F(2,2);
	F<<1,delt,0,1;
	MatrixXd G(2,1);
	G<<0.5*delt*delt,delt;
	MatrixXd H(1,2);
	H<<0,1;
//noise
	MatrixXd W(2,1);//x noise
	W<<2,3;
	MatrixXd Q(2,2);//p noise
	Q << 0,0,0,0;//= (W*W.transpose());
//	
	MatrixXd inputU(2,1);
	inputU = u*MatrixXd::Identity(1,1);

	MatrixXd z(1,1);
	z << 1;
	//
	MatrixXd stateX(2,1);
	stateX << psn,vel;
	MatrixXd stateX_(2,1);//gujizhi
	stateX_ << 0,0;
	MatrixXd state_X(2,1);//guoquzhi
	state_X = stateX;
	//
	MatrixXd outputY(1,1);
	MatrixXd tmp1(1,2);tmp1<<1,0;	
	outputY = tmp1*stateX + inputU*0;
	//
	 MatrixXd P(2,2);
	 P << 1,0,0,1;	
	 MatrixXd _P(2,1);
	 _P = P;
	 MatrixXd P_(2,1);
	 MatrixXd K(2,1);
	 MatrixXd R(1,1);
	 R << 1;
	
	// MatrixXd G(2,1);
	// G<<;

	cout << "stateX=\n" << stateX << endl;
//function
	stateX_ = F*state_X+G*inputU;
	cout << "stateX_=\n" << stateX_ << endl;	

	P_ = F*_P*F.transpose()+Q;
	cout << "P_=\n" << P_ << endl;

	K = P_*H.transpose()*(H*P_*H.transpose()+R).inverse();
	cout << "K=\n" <<  K << endl;
	
	stateX = stateX_+K*(z-H*stateX_);
	cout << "stateX=\n" << stateX << endl;	

	P = (MatrixXd::Identity(2,2)-K*H)*P_;
	cout << "P =\n" << P << endl;

 

/*
    cout << "stateX=\n" << stateX << endl;
    cout << "A=\n" << A << endl;
    cout << "B=\n" << B << endl;
    cout << "inputU=\n" << inputU << endl;
	cout << "K=\n" <<  K << endl;
	cout << "F=\n" <<  F << endl;
	cout << "G=\n" <<  G << endl;
	cout << "W=\n" <<  W << endl;
	cout << "Q=\n" <<  Q << endl;
	cout << "H=\n" <<  H << endl;
	
*/
	y = outputY.determinant();
	
	cout << "outputY=\n" <<  outputY << endl;
	cout << "y=\n" <<  y << endl;
	

	return 0;
}