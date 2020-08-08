//pub to smach_pantilt :whether has found the target
//has -> pub 1 ; not has-> pub 0
#include <ros/ros.h>
#include <sentry/Has.h>
#include <hkvs/square.h>

bool foundflag = 0;//
bool key_pri = 0;
void pointCallback(const hkvs::squareConstPtr& msgp)//
{
  foundflag = msgp->square_num;
}

void priCallback(const sentry::HasConstPtr& mspr)//
{
  key_pri = mspr->linear;
}

int main(int argc, char **argv)
{
      ros::init(argc, argv, "found_tk");
      ros::NodeHandle n;
      ros::Publisher haspub = n.advertise <sentry::Has>("has_info", 1);
      ros::Subscriber sub = n.subscribe("point2d",1,pointCallback);
      ros::Subscriber subpri = n.subscribe("has_pri",1,priCallback);
      ros::Rate rate(1);
      sentry::Has hasmsg;
      while(key_pri==0)
     {
        ros::spinOnce();
        if (foundflag == 1)
        {hasmsg.linear = 1;
         ROS_INFO("hasfound!");}
        else
        {hasmsg.linear = 0;
	ROS_INFO("none...");}

	haspub.publish(hasmsg);
        foundflag = 0;
        ROS_INFO("-------");

        
        rate.sleep();
     }
  return 0;
}
