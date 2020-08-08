#include <boost/bind.hpp>
#include <ros/ros.h>
#include <turtlesim/Pose.h>
#include <geometry_msgs/Twist.h>
#include <std_srvs/Empty.h>
 #include "std_msgs/String.h"
 #include <nav_msgs/Odometry.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>//用于取随机数

#define g 9.81

struct robot
{
	float pose_x;
	float speed;
	float accelarate;
	float weight;
	float frict;//摩擦系数
};
struct robot robot;
float Goal_x = 400;//预防位置数据不正常而导致的到达目标的误判
float set_speed = 0.01;//速度 以X轴正方向为正
float set_accelarate;//加速度
float re_set_speed;//设置恢复速度，使如果超功率后机器成功降功率了，再把速度设置回去
float power;//功率
float gate_1 = 0.7;//设立阈值系数一
float gate_2 = 0.3;//同理

float err = 0.0; //定义偏差值
float err_last = 0.0; //定义上一个偏差值
float Kp = 0.2, Ki = 0.015, Kd = 0.005; //定义比例、积分、微分系数，现在的是瞎填的
float integral = 0.0; //定义积分值/test_start

bool fist_goal_set = false;
int STart = 1;

void poseCallback(const  nav_msgs::OdometryConstPtr& odom)
{
  robot.pose_x = odom->pose.pose.position.x-0.17;
  robot.speed  = odom->twist.twist.linear.x;
}

//*******************************判断行*************************//
bool HasReach_goal()
{
	return fabsf(Goal_x - robot.pose_x) < 0.05;//设置目标达成的区间，同时也是
}

bool InTrack()//判断是否在规定范围之内，不在范围内，则设目标为另一限制上限
{
	return 0.3 <= robot.pose_x && robot.pose_x <=1.8;
}

bool out_power_1()	//没有具体数据，未考虑好到底设多少为阈值一，先把框架写出来
{
	return 0;
}

bool out_power_2()  //设置功率阈值二，赛规虽然有缓冲能量，但上限功率为30W
{
	power = 0;
	return power - 30 < 0;
}
//*******************************判断行*************************//

//*********************************设置目标行*******************************//
void recover()//如果哨兵位置超可移动范围，则设置目标为远一段的边界
{
	if (0 < robot.pose_x && robot.pose_x < 0.3)//
	{
		Goal_x = 1.8;
		set_speed = 1.1;
	}
	else 
	{
		Goal_x = 0.3;
		set_speed = -1.1;
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

void set_goal_set_speed()				//半随机运动，目标取点总是取大范围的一边。因此，小车总是会先减速。
{								//又因为会减速，所以我想在达到目标的判定条件中加一个大一些的区间，作为一个刹车区间。
	float left_side;			//但这样做有两个弊端：1.小车第一次必定减速，增加了被攻击的可能性。
	float right_side;			//2.小车在中点的时候可能会加速，但貌似影响不大
	float goal_x;
	left_side = robot.pose_x;
	right_side = 1.8 - robot.pose_x;//轨道的长度为2.1米，为了防止超轨道，先设置为1.8M
	if (left_side < right_side)
	{
		Goal_x = right_side * SuiJiShu()+ robot.pose_x;
		goal_x = right_side * SuiJiShu() + robot.pose_x;
		set_speed = 0.9;
	}
		else if (left_side >= right_side)//这里设置了，若哨兵在中间的位置，则取哨兵左边的点
		{
			Goal_x = left_side * SuiJiShu();
			goal_x = left_side * SuiJiShu();
			set_speed = - 0.9;
		}

	if (Goal_x < 0.3)
	{
		Goal_x = Goal_x + 0.3;//防止随机出来的目标超出可移动范围
	}
	else Goal_x = goal_x;	
}

void whole_goal_speed()//将②整合为一个函数
{
	if (InTrack())
		set_goal_set_speed();
	else 
		recover();
	re_set_speed = set_speed;
}

void Fist_Set_Goal()		//使循环函数能够成功循环，因为在仿真中，在前几次接收消息的时候，机器人的位置和速度都为0，导致目标一直不达成，最终程序卡死
{
	if (!fist_goal_set)
	{
		
		if (0 < robot.pose_x && robot.pose_x < 2.2)//这里我假定，当传感器节点数据异常或者接收不到的时候，这里的robot.pose_x为0;所以，只有当这个数据正常的时候才开始运行节点。我认为传感器的异常信息应该交由传感器信息发布节点来排查。
		{       
			whole_goal_speed();
			fist_goal_set = true;
		}
	}
}
//*********************************设置目标行*******************************//

void Set_Acclerate()
{		//ROS_INFO("%f",set_speed);	
	if (set_speed != 0.01)  //预防目标并没有设置成功,  疑问： !set_speed == 0.01 不正确，set_speed != 0.01正确
	{	
		err = set_speed - robot.speed;
		integral += err;
		set_accelarate = Kp*err+Ki*integral+ Kd * (err - err_last);//效仿PID控制来设置加速度，不一定对，也可以用其他方法
		err_last = err;
	}
}

void commandTurtle(ros::Publisher vel_pub, float linear)
{
  geometry_msgs::Twist twist;
  twist.linear.x = linear;
  vel_pub.publish(twist);
}

void timerCallback(const ros::TimerEvent&, ros::Publisher vel_pub)
{	
	ROS_INFO(" %d/n",fist_goal_set);
	ROS_INFO(" %f, %f, %f, %f",robot.pose_x,robot.speed,set_speed,Goal_x);//robot.speed
	Fist_Set_Goal();
	if (HasReach_goal())
		whole_goal_speed();
	if (out_power_1())//判断是否超功率，通过降低期望速度来降低功率
	{									//此处超功率判断因为没有具体参数，并没有启用和调试。
		if (out_power_2())
			set_speed = set_speed * gate_2;
		else 
			set_speed = set_speed * gate_1;
	}

	else set_speed = re_set_speed;
	//Set_Acclerate();
        //robot.speed = robot.speed+set_accelarate;
        //robot.speed = set_speed;

	if (STart == 1)//判断是否发布信息的条件,若不满足，则不发送消息，即该模式不起用
        {
           commandTurtle(vel_pub,set_speed);//发布处。
	//ROS_INFO(" %f, %f, %f, %f",robot.pose_x,robot.speed,set_speed,Goal_x);//robot.speed
        };

}

//void STart__Callback(const random_move::test_startConstPtr& xiaoming)//缺少接受的消息类型
//{
//	STart = xiaoming->if_start;
//	ROS_INFO("[%d]",STart);
//}

int main(int argc, char *argv[])
{
 ros::init(argc, argv, "random_move");
 ros::NodeHandle nh;
 ros::Subscriber pose_sub = nh.subscribe("/chassis/position", 1, poseCallback);
 ros::Publisher vel_pub = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 10);//geometry_msgs::Twist这个就是发布的消息类型，若自定义，可以改为 功能包名::自定义消息文件名称。10是缓存区，表示最多储存的消息内容的数量。
 //ros::ServiceClient reset = nh.serviceClient<std_srvs::Empty>("reset");
 
// ros::Subscriber STart_sub = nh.subscribe("test_start", 10, STart__Callback);//接收状态机的指令
 ros::Timer timer = nh.createTimer(ros::Duration(0.025), boost::bind(timerCallback, _1, vel_pub));
 //std_srvs::Empty empty;
 //reset.call(empty);
 ros::spin();

}
