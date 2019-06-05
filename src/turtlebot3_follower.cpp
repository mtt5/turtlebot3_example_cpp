#include <ros/ros.h>
#include <math.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <vector>
#include <algorithm>


class Follower{
public:
    Follower(){ROS_ASSERT(init());}

    bool init(){
        cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel",5);
        return true;
    }

    void run(){
        while(ros::ok()) {
            std::vector<float> lidar_distance = get_scan();
            min_distance_ = *min_element(lidar_distance.begin(),lidar_distance.end());
            min_index_ = std::min_element(lidar_distance.begin(),lidar_distance.end())
                           -lidar_distance.begin();
            //min_distance_ = lidar_distance[min_index_];
            ROS_INFO("%f, %d",min_distance_,min_index_);
            if(min_index_>=10 && min_index_<350)
                turn();
            else
                move();
        }
        command_.linear.x = 0.0;
        command_.angular.z = 0.0;
        cmd_pub_.publish(command_);
    }

    void move(){
        if(min_distance_<min_) {
            command_.linear.x = 0.0;
            command_.angular.z = 0.0;
            cmd_pub_.publish(command_);
            ROS_INFO("stop!");
        }
        else if(min_distance_>max_) {
            command_.linear.x = linear_vel_;
            command_.angular.z = 0.0;
            cmd_pub_.publish(command_);
        }
        else {
            command_.linear.x = 0.0;
            command_.angular.z = 0.0;
            cmd_pub_.publish(command_);
        }

    }

    void turn(){
        if(min_index_<180) {
            command_.linear.x = 0.0;
            command_.angular.z = 0.5;
            cmd_pub_.publish(command_);
        }
        else {
            command_.linear.x = 0.0;
            command_.angular.z = -0.5;
            cmd_pub_.publish(command_);
        }
    }

    std::vector<float> get_scan(){
        std::vector<float> scan_filter;
        boost::shared_ptr<sensor_msgs::LaserScan const> scan =
                ros::topic::waitForMessage<sensor_msgs::LaserScan>("scan",nh_);
        for(const auto &data:scan->ranges)
            scan_filter.push_back(data);
        //for(auto iter = scan->ranges.begin(); iter != scan->ranges.end(); ++iter)
        //    scan_filter.push_back(*iter);
        for(int i = 0;i<scan_filter.size();++i){
            if(scan_filter[i] >3.5)
                scan_filter[i] = 3.5;
            else if(isnanf(scan_filter[i]) || scan_filter[i] == 0)
                scan_filter[i] = 3.5;
        }
        return scan_filter;
    }

private:
    ros::NodeHandle nh_;
    ros::Publisher cmd_pub_;
    geometry_msgs::Twist command_;
    float const linear_vel_ = 0.1;
    float const min_ = 0.12;
    float const max_ = 0.30;
    float min_distance_ = 0.0;
    int min_index_ = 0;

};


int main(int argc, char** argv) {
    ros::init(argc, argv, "Turtlebot3_follower");
    Follower turtlebot3_follower;
    turtlebot3_follower.run();
    return 0;
}