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

class TeleopTurtle//定义了一个类
{
public:
  TeleopTurtle();//定义 函数? 作用？
  void keyLoop();//定义 函数 作用？

private:

  
  ros::NodeHandle nh_;//含有句柄
  int linear, angular, l_scale_, a_scale_;//
  ros::Publisher has_pub_;// 定义发布者
  
};

TeleopTurtle::TeleopTurtle():  //这里的格式？
  linear(1),
  angular(1),
  l_scale_(1),//线速度
  a_scale_(1)//角速度
{
  nh_.param("scale_angular", a_scale_, a_scale_);//“参数名”，参数值，默认值 作用？
  nh_.param("scale_linear", l_scale_, l_scale_);

  has_pub_ = nh_.advertise<sentry::Has>("has_info", 1);//发布的消息 固定格式
}

int kfd = 0;//定义 kfd 作用：文件描述符
struct termios cooked, raw;//定义 接口cooked 接口raw 
//termios 函数族提供了一个常规的终端接口，用于控制非同步通信端口 
//这个变量被用来提供一个健全的线路设置集合

void quit(int sig)//定义 quit函数 作用是处理SIGINT信号
{
  (void)sig;// （void）为什么这样写？sig是一个形参 这里代表SIGINT信号
  tcsetattr(kfd, TCSANOW, &cooked);//tcsetattr函数用于设置终端的相关参数。
                                   //如果试图改变波特率，应用程序应该调用tcsetattr()，然后调用tcgetattr()，以确定实际选择了什么波特率。
                                   //波特率：表示每秒钟传送的码元符号的个数，是衡量数据传送速率的指标，它用单位时间内载波调制状态改变的次数来表示。
                                   //参数kfd为终端的文件描述符，返回的结果保存在termios 结构体中
                                   //TCSANOW：不等数据传输完毕就立即改变属性 函数返回值：成功返回0，失败返回－1
                                   //文件描述符：在linux系统中打开文件就会获得文件描述符，它是个很小的非负整数。
                                   //每个进程在PCB（Process Control Block）中保存着一份文件描述符表，文件描述符就是这个表的索引，每个表项都有一个指向已打开文件的指针。
                                   //&cooked 指向结构体termios
  
  ros::shutdown();//关闭节点
  exit(0);//终止进程
}

//主函数//
int main(int argc, char** argv)
{
  ros::init(argc, argv, "key2att");
  TeleopTurtle teleop_turtle;

  signal(SIGINT,quit);

  teleop_turtle.keyLoop();
  
  return(0);
}


void TeleopTurtle::keyLoop()//定义 用于键盘控制的函数
{
  char c;
  bool dirty=false;
                                        
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);           
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");//puts()向标准输出设备（屏幕）输出字符串并换行 需要<stdio.h>
  puts("---------------------------");
  puts("Use arrow keys to move the turtle. 'q' to quit.");


  for(;;)
  {
    
    if(read(kfd, &c, 1) < 0)
    {
      perror("read():");
      exit(-1);
    }


    ROS_DEBUG("value: 0x%02X\n", c);
  
    switch(c)//如果是规定的按键 就开启dirty 使发布者发布消息
    {
      case KEYCODE_L:
        ROS_DEBUG("isAttacked");//LEFT
        angular = 1;//isAttacked
        //linear = 1;
        dirty = true;
        break;
      case KEYCODE_R:
        ROS_DEBUG("notAttacked");//RIGHT
        angular = 0;
        //linear = 1;
        dirty = true;
        break;
      case KEYCODE_U:
        ROS_DEBUG("UP");
        //angular = 1;
        //linear = 0;
        //linear = 1;//hasfound
        dirty = true;
        break;
      case KEYCODE_D:
        ROS_DEBUG("DOWN");
        //angular = 0;
        //linear = 0;
        dirty = true;
        break;
      case KEYCODE_Q:
        ROS_DEBUG("quit");
        return;
    }
   

    sentry::Has mss;
    mss.angular=angular;
    //mss.linear=linear;
    if(dirty ==true)//dirty开启则 发布信息 然后dirty关闭
    {
      has_pub_.publish(mss);  //发布消息  
      dirty=false;//发布后关闭消息
    }
  }


  return;
}



