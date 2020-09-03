#include <boost/bind.hpp>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Int8.h>
#include <turtlesim/Pose.h>
//#include <std_srvs/Empty.h>
//#include <random_move/test_start.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>//用于取随机数

#define g 9.81

const float nol_vel = 0.8;//set nomal speed
const float rev_vel = 0.9;//set recover speed
float burst_x = 1;//设置暴走模式下的增大倍率
float energy_gate = 0.7;//暴走模式的期望速度缩减比率


float energy = 200;

struct robot
{
	float pose_x;
	float speed;
	float accelarate;
	float weight;
	float frict;//摩擦系数,didnt use
};
struct robot robot;
float Goal_x;
float set_speed = 0.01;//速度 以X轴正方向为正
float re_set_speed;//设置恢复速度，使如果超功率后机器成功降功率了，再把速度设置回去
float power;//功率
float gate_1 = 0.7;//设立阈值系数一
float gate_2 = 0.3;//同理

bool fist_goal_set = false;
int STart = 0;//开关：是否启动随机运动模式
int Burst = 0;//开关：是否进入暴走模式

void poseCallback(const turtlesim::PoseConstPtr& pose)
{
	float test=0;
	test =  2.05-(pose->x /100 +0.2);
	if (0 <= test && test < 2.2)       //在这里加入了只接受这个范围内的数据
		robot.pose_x = 2.05-(pose->x /100 +0.2);
}

//*******************************判断行*************************//
bool HasReach_goal()
{
	return fabsf(Goal_x - robot.pose_x) < 0.2;//设置目标达成的区间，同时也是
}

bool InTrack()//判断是否在规定范围之内，不在范围内，则设目标为另一限制上限
{
	return 0.3 <= robot.pose_x && robot.pose_x <=1.8;
}

bool out_energy()  //给暴走模式加的一个阈值
{
	return energy < 50;
}
//*******************************判断行*************************//

//*********************************设置目标行*******************************//
void recover()//如果哨兵位置超可移动范围，则设置目标为远一段的边界
{
	if (robot.pose_x < 0.3)//此时与右端相距1.5~1.8米
	{
		Goal_x = 1.8;
		set_speed = rev_vel;
	}
	else
	{
		Goal_x = 0.3;
		set_speed = -rev_vel;
	}
}

float SuiJiShu()//这是根据时间的伪随机数，只能取0~100的数
{
	float n = 0.0;
	srand((int)time(0));
	n = rand() % (100 - 1 + 1) + 1;
	n = n * 0.01;
	return n;
}

void set_goal_set_speed()			//半随机运动，目标取点总是取大范围的一边。因此，小车总是会先减速。
{						//又因为会减速，所以我想在达到目标的判定条件中加一个大一些的区间，作为一个刹车区间。
	float left_side;			//但这样做有两个弊端：1.小车第一次必定减速，增加了被攻击的可能性。
	float right_side;			//2.小车在中点的时候可能会加速，但貌似影响不大
	left_side = robot.pose_x;
	right_side = 1.8 - robot.pose_x;//轨道的长度为2.1米，为了防止超轨道，先设置为1.8M

	if (left_side < right_side)// if robot is on the left side,then the goal will be set on the right side
	{
		Goal_x = right_side * SuiJiShu()+ robot.pose_x;
		set_speed = nol_vel;
	}
	else if (left_side >= right_side)//这里设置了，若哨兵在中间的位置，则取哨兵左边的点
	{
		Goal_x = left_side * SuiJiShu();
		set_speed = - nol_vel;
	}
	if (Goal_x < 0.3)
		Goal_x += 0.3;//防止随机出来的目标超出可移动范围
}

void whole_goal_speed()//将②整合为一个函数
{
	if (InTrack())
		set_goal_set_speed();
	else 
		recover();
	re_set_speed = set_speed;
}

void Fist_Set_Goal()		//使循环函数第一次设置目标，且只执行一次。（未验证）
{
	if (!fist_goal_set)
		whole_goal_speed();
	fist_goal_set = true;
}
//*********************************设置目标行*******************************//
void commandTurtle(ros::Publisher vel_pub, float linear)
{
  geometry_msgs::Twist twist;
  twist.linear.x = linear;
  vel_pub.publish(twist);
}

void timerCallback(const ros::TimerEvent&, ros::Publisher vel_pub)
{	
	Fist_Set_Goal();
	if (HasReach_goal())
		whole_goal_speed();
	else 
		set_speed = re_set_speed;
	robot.speed = set_speed;

	if (STart == 2)
        {
	   if(Burst == 1)//判断是否接收到要进入暴走模式的指令  
	   {	
		set_speed = set_speed*burst_x;
		if(out_energy())
	{
			set_speed = set_speed*energy_gate;
			//set_speed *=100;
			commandTurtle(vel_pub,set_speed*100);//发布处1。
		}
		else 
		{
			//set_speed *=100;
			commandTurtle(vel_pub,set_speed*100);//发布处2
		}
   	   }	
	   else if(Burst == 0)
	   {
		//set_speed *=100;
		commandTurtle(vel_pub,set_speed*100);  //发布处3。
	   }
           ROS_INFO("publish linear_x :%f",set_speed);
        };

}

void STart__Callback(const std_msgs::Int8ConstPtr& xiaoming)
{
	STart = xiaoming->data;
	//ROS_INFO("[%d]",STart);
}

void Burst__Callback(const std_msgs::Int8ConstPtr& xiaohua)
{
	Burst = xiaohua->data;
	//ROS_INFO("[%d]",Burst);
}

int main(int argc, char *argv[])
{
 ros::init(argc, argv, "random_move");
 ros::NodeHandle nh;
 ros::Subscriber pose_sub = nh.subscribe("robot_pose", 1, poseCallback);
 ros::Publisher vel_pub = nh.advertise<geometry_msgs::Twist>("chassis_cmd_vel", 1);
 //ros::ServiceClient reset = nh.serviceClient<std_srvs::Empty>("reset");
 
 ros::Subscriber STart_sub = nh.subscribe("order_Chassis", 10, STart__Callback);
 ros::Timer timer = nh.createTimer(ros::Duration(0.025), boost::bind(timerCallback, _1, vel_pub));
 //std_srvs::Empty empty;
 //reset.call(empty);
 ros::spin();

}
