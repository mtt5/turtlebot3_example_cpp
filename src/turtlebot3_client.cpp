#include "ros/ros.h"
#include <actionlib/client/simple_action_client.h>
#include "turtlebot3_example/Turtlebot3Action.h"
#include "turtlebot3_example/Turtlebot3Goal.h"
#include <vector>
#include <iostream>


int main(int argc, char** argv){
    ros::init(argc,argv,"turtlebot3_client");
    actionlib::SimpleActionClient<turtlebot3_example::Turtlebot3Action>
            action_client("turtlebot3",true);
    ROS_INFO("input mode, area, count");
    std::vector<int> vector1;
    int a;
    while(vector1.size()<3 && std::cin>>a){
        vector1.push_back(a);
    }

    action_client.waitForServer();
    turtlebot3_example::Turtlebot3Goal goal;
    goal.goal.x = vector1[0];
    goal.goal.y = vector1[1];
    goal.goal.z = vector1[2];
    action_client.sendGoal(goal);
    ROS_INFO("send to goal");
    action_client.waitForResult();
    action_client.getResult();
    return 0;
}