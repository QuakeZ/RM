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
  ros::Publisher found_pub_;
  ros::Publisher att_pub_;
  
};

TeleopTurtle::TeleopTurtle():
  linear(0),
  angular(0),
  l_scale_(1),
  a_scale_(1)
{
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);

  found_pub_ = nh_.advertise<sentry::Has>("found_info", 1);//发布的消息 固定格式
  att_pub_ = nh_.advertise<sentry::Has>("att_info", 1);
}

int kfd = 0;//定义 kfd 作用：文件描述符
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
  ros::init(argc, argv, "key2c");//ROS初始化
  TeleopTurtle teleop_turtle;//定义 TeleopTurtle类 的 teleop_turtle

  signal(SIGINT,quit);//相当于规定Ctrl+C的退出方式
                      //当收到 SIGINT 信号 就调用quit（）函数

  teleop_turtle.keyLoop();//调用 keyLoop()函数
  quit(0);//调用quit函数 结束进程
  
  return(0);
}

void TeleopTurtle::keyLoop()//定义 用于键盘控制的函数
{
  char c;
  bool dirty=false;//定义 布尔型dirty 作用：控制在对应按键被按下后发布消息
    
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);// /* local mode flags */
  // Setting a new line, then end of file                         
  raw.c_cc[VEOL] = 1;//c_cc control characters VEOL 11
  raw.c_cc[VEOF] = 2;//VEOF 4
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");//puts()向标准输出设备（屏幕）输出字符串并换行 需要<stdio.h>
  puts("---------------------------");
  puts("Use arrow keys to move the turtle. 'q' to quit.");


  for(;;)
  {
    // get the next event from the keyboard  
    if(read(kfd, &c, 1) < 0)//如果已经读取到末尾，报错并退出
                            
    {
      perror("read():");//perror（）：输出括号内的内容并且输出系统错误信息 <stdio.h>
      exit(-1);//退出程序 exit(0)表示程序正常退出，非0表示非正常退出
    }


    ROS_DEBUG("value: 0x%02X\n", c);
  
    switch(c)//如果是规定的按键 就开启dirty 使发布者发布消息
    {
      case KEYCODE_L:
        ROS_DEBUG("LEFT");
        angular = 1;//isAttacked
        //linear = 1;
        dirty = true;
        break;
      case KEYCODE_R:
        ROS_DEBUG("RIGHT");
        angular = 0;
        //linear = 1;
        dirty = true;
        break;
      case KEYCODE_U:
        ROS_DEBUG("UP");
        //angular = 1;
        //linear = 0;
        linear = 1;//hasfound
        dirty = true;
        break;
      case KEYCODE_D:
        ROS_DEBUG("DOWN");
        //angular = 0;
        linear = 0;
        dirty = true;
        break;
      case KEYCODE_Q:
        ROS_DEBUG("quit");
        return;
    }
   

    sentry::Has mss;//
    mss.linear=linear;//
    sentry::Has mgg;
    mgg.angular=angular;
    if(dirty == true)//dirty开启则 发布信息 然后dirty关闭
    {
      found_pub_.publish(mss);//发布消息
      att_pub_.publish(mgg);  
      dirty=false;//发布后关闭消息
    }
  }


  return;
}



