#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <math.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Quaternion.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/JointState.h>
#include <turtlebot3_msgs/SensorState.h>
#include "turtlebot3_example/Turtlebot3Action.h"
#include <vector>

class Turtlebot3_Action{
public:
    Turtlebot3_Action(std::string name):action_name_(name),
    action_server_(nh_,name,boost::bind(&Turtlebot3_Action::executeCB,this,_1),false){
        //stats_sub_ = nh_.subscribe("joint_states",1,&Turtlebot3_Action::stats_CB,this);
        //odom_sub_ = nh_.subscribe("odom",1,&Turtlebot3_Action::odom_CB,this);
        //init_state_ = true;
        action_server_.start();
        ROS_INFO("server on");
    }

    void executeCB(const turtlebot3_example::Turtlebot3GoalConstPtr &goal){
        int mode = goal->goal.x;
        float length = goal->goal.y;
        int count = goal->goal.z;
        bool success = true;
        bool circle_mode = true;
        bool half_patrol = false;
        int circle_count = 0;
        //ROS_INFO("%d,%f,%d",mode,length,count);
        //position_ = geometry_msgs::Point();
        //r_ = 15;
        //r1_ = 1;
        stats_sub_ = nh_.subscribe("joint_states",1000,&Turtlebot3_Action::stats_CB,this);
        odom_sub_ = nh_.subscribe("odom",1000,&Turtlebot3_Action::odom_CB,this);
        cmd_vel_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel",5);

        for(int i = 0;i<count;++i){
            if(mode == 1){
                std::vector<float> area(4,0);
                area[0] = area[1] = length;
                for(int j = 0;j<=3;++j){
                    go_front(area[j],j);
                    r1_.sleep();
                    turn(-90);
                }
            }
            else if(mode == 2){
                std::vector<float> area(3,0);
                area[0] = area[1] = length;
                for(int j =0;j<=2;++j){
                    go_front(area[j],j);
                    r1_.sleep();
                    turn(-120);
                }
            }
            else if(mode == 3){
                //later
            }
        }

        if(success){
            result_.result.result = "";
            ROS_INFO("%s:Succeeded", action_name_.c_str());
            action_server_.setSucceeded(result_.result);
        }
    }

    void stats_CB(const sensor_msgs::JointState::ConstPtr &msg){
        float TICK2RAD = 0.001533981;
        float last_pos = 0.0;
        float diff_pos = 0.0;
        float cur_pos = 0.0;
        int encoder = 0;

        cur_pos = msg->position[0];
        diff_pos = cur_pos - last_pos;
        encoder = encoder + int(diff_pos / TICK2RAD);
        right_encoder_ = encoder;
    }

    void odom_CB(const nav_msgs::Odometry::ConstPtr &msg){
        position_.x = msg->pose.pose.position.x;
        position_.y = msg->pose.pose.position.y;
        position_.z = msg->pose.pose.position.z;
        //ROS_INFO("received position");
    };

    void go_front(const float length,const int count){
        if(count == 0){
            while(position_.x<length){
                //ROS_INFO("%f",position_.x);
                command_.linear.x = 0.1;
                command_.angular.z = 0.0;
                cmd_vel_pub_.publish(command_);
                r_.sleep();
            }
        }
        else if(count == 1){
            while(position_.y<length){
                command_.linear.x = 0.1;
                command_.angular.z = 0.0;
                cmd_vel_pub_.publish(command_);
                r_.sleep();
            }
        }
        else if(count == 2){
            while(position_.x>length){
                command_.linear.x = 0.1;
                command_.angular.z = 0.0;
                cmd_vel_pub_.publish(command_);
                r_.sleep();
            }
        }
        else{
            while(position_.y>length){
                command_.linear.x = 0.1;
                command_.angular.z = 0.0;
                cmd_vel_pub_.publish(command_);
                r_.sleep();
            }
        }
        command_.linear.x = 0.0;
        cmd_vel_pub_.publish(command_);
        r_.sleep();
    }

    void turn(const int degree){
        if(init_state_){
            init_right_encoder_ = right_encoder_;
            init_state_ = false;
        }
        float diff_encoder = ((M_PI/180)*degree*0.080)/(0.207/4096);
        //ROS_INFO("%f",diff_encoder);
        while(abs(init_right_encoder_ - right_encoder_)<abs(diff_encoder)){
            if(diff_encoder >= 0 )
                command_.angular.z = -0.5;
            else
                command_.angular.z = 0.5;
            cmd_vel_pub_.publish(command_);
            r_.sleep();
        }
        init_state_ = true;
        command_.angular.z = 0.0;
        cmd_vel_pub_.publish(command_);
        r_.sleep();
    }

private:
    ros::NodeHandle nh_;
    actionlib::SimpleActionServer<turtlebot3_example::Turtlebot3Action> action_server_;
    std::string action_name_;
    ros::Subscriber stats_sub_;
    ros::Subscriber odom_sub_;
    ros::Publisher cmd_vel_pub_;
    turtlebot3_example::Turtlebot3ActionFeedback feedback_;
    turtlebot3_example::Turtlebot3ActionResult result_;
    //geometry_msgs::Point init_Position_;
    geometry_msgs::Point position_;
    geometry_msgs::Twist command_;

    bool init_state_ = true;
    //float TICK2RAD = 0.001533981;
    //float last_pos = 0.0;
    //float diff_pos = 0.0;
    //float cur_pos = 0.0;
    //int encoder = 0;
    int right_encoder_ = 0;
    int init_right_encoder_ = 0;
    ros::Rate r_ = 15;
    ros::Rate r1_ = 1 ;




};

int main(int argc, char** argv){
    ros::init(argc,argv,"turtlebot3");
    Turtlebot3_Action server(ros::this_node::getName());
    ros::spin();
    return 0;
}