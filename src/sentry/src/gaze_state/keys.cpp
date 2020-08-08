#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>

#define KEYCODE_R 0x43 //R：方向键right    0x-> 16进制 应该是对应键码 但是这个值和我查到的不一样？
#define KEYCODE_L 0x44 //L：方向键left     我查到的是 0x43:C 0x44:D 0x42:B 0x41:A 0x71:F2
#define KEYCODE_U 0x41 //U：方向键up       0x71 是q的ASCII码值
#define KEYCODE_D 0x42 //D：方向键down     输入A B C D 可以得到等同上下左右的效果
#define KEYCODE_Q 0x71 //Q

class TeleopTurtle//定义了一个类
{
public:
  TeleopTurtle();//定义 函数? 作用？
  void keyLoop();//定义 函数 作用？

private:

  
  ros::NodeHandle nh_;//含有句柄
  double linear_, angular_, l_scale_, a_scale_;//线速度等变量
  ros::Publisher twist_pub_;//含有发布者
  
};

TeleopTurtle::TeleopTurtle():  //这里的格式？
  linear_(0),
  angular_(0),
  l_scale_(1.0),//线速度
  a_scale_(1.0)//角速度
{
  nh_.param("scale_angular", a_scale_, a_scale_);//“参数名”，参数值，默认值 作用？
  nh_.param("scale_linear", l_scale_, l_scale_);

  twist_pub_ = nh_.advertise<geometry_msgs::Twist>("/chassis/cmd_vel", 1);//发布的消息 固定格式
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

//主函数
int main(int argc, char** argv)
{
  ros::init(argc, argv, "teleop_turtle");//ROS初始化
  TeleopTurtle teleop_turtle;//定义 TeleopTurtle类 的 teleop_turtle

  signal(SIGINT,quit);//相当于规定Ctrl+C的退出方式
                      //前一个参数表示要处理的信号 后一个参数表示信号的处理方式
                      //SIGINT /* interrupt */程序终止(interrupt)信号, 通常是Ctrl+C，用于通知前台进程组终止进程。
                      //当收到 SIGINT 信号 就调用quit（）函数

  teleop_turtle.keyLoop();//调用 keyLoop()函数
  quit(0);//调用quit函数 结束进程
  
  return(0);
}
//

void TeleopTurtle::keyLoop()//定义 用于键盘控制的函数 以下全是
{
  char c;//定义 字符型变量c 作用？
  bool dirty=false;//定义 布尔型dirty 作用：控制在对应按键被按下后发布消息


  // get the console in raw mode    让控制台处于原始模式？                                                          
  tcgetattr(kfd, &cooked);//tcgetattr()与tcsetattr()一起使用，以确定实际数据传送速率
  memcpy(&raw, &cooked, sizeof(struct termios));//memcpy指的是C和C++使用的内存拷贝函数
                                                //头文件：<string.h> 
                                                //从源src（&cooked）所指的内存地址的起始位置开始，拷贝n个字节的数据到目标dest（&raw）所指的内存地址的起始位置中。
                                                //sizeof()返回一个对象或者类型所占的内存字节数
  raw.c_lflag &=~ (ICANON | ECHO);// /* local mode flags */
  // Setting a new line, then end of file                         
  raw.c_cc[VEOL] = 1;//c_cc control characters VEOL 11
  raw.c_cc[VEOF] = 2;//VEOF 4
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");//puts()向标准输出设备（屏幕）输出字符串并换行 需要<stdio.h>
  puts("---------------------------");
  puts("Use arrow keys to move the turtle. 'q' to quit.");


  for(;;)//无条件循环 
  {
    // get the next event from the keyboard  
    if(read(kfd, &c, 1) < 0)//如果已经读取到末尾，报错并退出
                            //read（）：函数从打开的设备或文件中读取数据。
                            //#include <unistd.h>    
                            //ssize_t read(int fd, void *buf, size_t count);
                            //count（这里是1）是请求读取的字节数，读上来的数据保存在缓冲区buf中，同时文件的当前读写位置向后移。     
                            //返回值：成功返回读取的字节数，出错返回-1并设置errno，如果在调read之前已到达文件末尾，则这次read返回0
    {
      perror("read():");//perror（）：输出括号内的内容并且输出系统错误信息 <stdio.h>
      exit(-1);//退出程序 exit(0)表示程序正常退出，非0表示非正常退出
    }

    linear_=angular_=0;//线速度和角速度变量的初始化
    ROS_DEBUG("value: 0x%02X\n", c);
  
    switch(c)//如果是规定的按键 就开启dirty 使发布者发布消息
    {
      case KEYCODE_L:
        ROS_DEBUG("LEFT");
        linear_ = 0.001;
        dirty = true;
        break;
      case KEYCODE_R:
        ROS_DEBUG("RIGHT");
        linear_ = 0.001;
        dirty = true;
        break;
      case KEYCODE_U:
        ROS_DEBUG("UP");
        linear_ = 0.5;
        dirty = true;
        break;
      case KEYCODE_D:
        ROS_DEBUG("DOWN");
        linear_ = -0.5;
        dirty = true;
        break;
      case KEYCODE_Q:
        ROS_DEBUG("quit");
        return;
    }
   

    geometry_msgs::Twist twist;//定义速度消息
    twist.angular.z = a_scale_*angular_;//设置 角速度信息
    twist.linear.x = l_scale_*linear_;//设置 线速度信息
    
    if(dirty ==true)//dirty开启则 发布信息 然后dirty关闭
    {
      twist_pub_.publish(twist);  //发布消息  
      dirty=false;//发布后关闭消息
    }
  }


  return;
}



