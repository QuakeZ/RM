#include <ros/ros.h>
//#include <geometry_msgs/Twist.h>
#include <sentry/Has.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>

#define KEYCODE_R 0x43 //R：方向键right    0x-> 16进制 应该是对应键码 但是这个值和我查到的不一样？
#define KEYCODE_L 0x44 //L：方向键left     我查到的是 0x43:C 0x44:D 0x42:B 0x41:A 0x71:F2
#define KEYCODE_U 0x41 //U：方向键up
#define KEYCODE_D 0x42 //D：方向键down
#define KEYCODE_Q 0x71 //Q

class TeleopTurtle
{
public:
  TeleopTurtle();
  void keyLoop();

private:

  
  ros::NodeHandle nh_;
  int linear, angular, l_scale_, a_scale_;
  ros::Publisher has_pub_;
  
};

TeleopTurtle::TeleopTurtle():  
  linear(1),
  angular(1),
  l_scale_(1),
  a_scale_(1)
{
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);

  has_pub_ = nh_.advertise<sentry::Has>("has_info", 1);
}

int kfd = 0;
struct termios cooked, raw;


void quit(int sig)
{
  (void)sig;
  tcsetattr(kfd, TCSANOW, &cooked);

  
  ros::shutdown();//关闭节点
  exit(0);//终止进程
}

//主函数//
int main(int argc, char** argv)
{
  ros::init(argc, argv, "key1");
  TeleopTurtle teleop_turtle;

  signal(SIGINT,quit);

  teleop_turtle.keyLoop();
  quit(0);
  
  return(0);
}


void TeleopTurtle::keyLoop()
{
  char c;
  bool dirty=false;


                                                 
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);   
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");
  puts("---------------------------");
  puts("Use arrow keys to move the turtle. 'q' to quit.");


  for(;;)//无条件循环 
  {
    
    if(read(kfd, &c, 1) < 0)
    {
      perror("read():");
      exit(-1);//退出程序 
    }

    //linear=angular=0;//线速度和角速度变量的初始化
    ROS_DEBUG("value: 0x%02X\n", c);
  
    switch(c)
    {
      case KEYCODE_L:
        ROS_DEBUG("LEFT");
        angular = 1;
        linear = 1;
        dirty = true;
        break;
      case KEYCODE_R:
        ROS_DEBUG("RIGHT");
        angular = 0;
        linear = 1;
        dirty = true;
        break;
      case KEYCODE_U:
        ROS_DEBUG("UP");
        angular = 1;
        linear = 0;
        dirty = true;
        break;
      case KEYCODE_D:
        ROS_DEBUG("DOWN");
        angular = 0;
        linear = 0;
        dirty = true;
        break;
      case KEYCODE_Q:
        ROS_DEBUG("quit");
        return;
    }
   

    sentry::Has mss;//定义速度消息
    mss.angular=angular;//设置 角速度信息
    mss.linear=linear;//设置 线速度信息
    if(dirty ==true)//dirty开启则 发布信息 然后dirty关闭
    {
      has_pub_.publish(mss);  //发布消息  
      dirty=false;//发布后关闭消息
    }
  }


  return;
}



