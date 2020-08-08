//发布坐标坐标信息  用pose消息类  订阅daji发送给底盘节点

 #include "ros/ros.h"
 #include "turtlesim/Pose.h"

float s=0,v=0;//初始坐标为零  默认最左端

void coordinatesCallback(const turtlesim::PoseConstPtr& in_msg)
 {
	v=in_msg->x;
	ROS_INFO("dipan shiji v : %f !!",v);
 }

int main(int argc, char **argv)
 {
	ros::init(argc, argv, "coordinates");
      	ros::NodeHandle n;
      	ros::Subscriber sub = n.subscribe("robot_vel",10,coordinatesCallback);//这个是先将坐标发给电机之后在进行解算
	ros::Publisher pub = n.advertise <turtlesim::Pose>("robot_pose", 10);
	ros::Rate rate(60);//这个频率上位机和下位机一定要保持一致！！！这里先用1/60s
	while(ros::ok())
	 {
		ros::spinOnce();//假设速度为零时也可以反馈
		turtlesim::Pose out_msg;

		ROS_INFO("the coordinates is: %f !!",s);
		out_msg.x=s;
		pub.publish(out_msg);
		//ros::spinOnce();
		rate.sleep();//60hz  保持一致
	 }
		return 0;

 }
//当然用timer会更精确

