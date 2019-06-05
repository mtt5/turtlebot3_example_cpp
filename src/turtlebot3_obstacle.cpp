#include <ros/ros.h>
#include <math.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <vector>
#include <algorithm>


class Obstacle{
public:
    Obstacle(){ROS_ASSERT(init());}

    bool init(){
        cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel",5);
        turtlebot3_moving = true;
        return true;
    }

    void run(){
        while(ros::ok()){
            std::vector<float> lidar_distance = get_scan();
            //ROS_INFO("%f",lidar_distance[0]);
            float min_distance = *min_element(lidar_distance.begin(),lidar_distance.end());
            //int min_index = std::min_element(lidar_distance.begin(),lidar_distance.end())
            //               -lidar_distance.end();
            if(min_distance < SAFE_STOP_DISTANCE){
                if(turtlebot3_moving){
                    command_.linear.x = 0.0;
                    command_.angular.z = 0.0;
                    cmd_pub_.publish(command_);
                    turtlebot3_moving = false;
                    ROS_INFO("stop!");
                }
            }
            else{
                command_.linear.x = LINEAR_VEL;
                command_.angular.z = 0.0;
                cmd_pub_.publish(command_);
                turtlebot3_moving = true;
                ROS_INFO("distance of obstacle is : %f", min_distance);
            }
        }
    }

    std::vector<float> get_scan(){
        std::vector<float> scan_filter;
        boost::shared_ptr<sensor_msgs::LaserScan const> scan =
                ros::topic::waitForMessage<sensor_msgs::LaserScan>("scan",nh_);
        ROS_INFO("%f",scan->ranges[0]);
        int samples = scan->ranges.size();
        int samples_view = 1;
        if(samples_view>samples){
            samples_view = samples;
        }
        else if(samples_view == 1){
            scan_filter.push_back(scan->ranges[0]);
        }
        else{
            int left_index = samples-samples_view/2;
            int right_index = samples_view/2;
            for(int i = left_index; i<samples; i++){
                scan_filter.push_back(scan->ranges[i]);
            }
            for(int i = 0; i< right_index; ++i){
                scan_filter.push_back((scan->ranges[i]));
            }
        }

        for(int i =0;i<samples_view;++i){
            if(scan_filter[i] >3.5)
                scan_filter[i] = 3.5;
            else if(isnanf(scan_filter[i]))
                scan_filter[i] = 0.0;
        }
        return scan_filter;
    }

private:
    ros::NodeHandle nh_;
    ros::Publisher cmd_pub_;
    //boost::shared_ptr<sensor_msgs::LaserScan const> scan;
    geometry_msgs::Twist command_;
    bool turtlebot3_moving = true;
    float const LINEAR_VEL = 0.22;
    float const STOP_DISTANCE = 0.2;
    float const LIDAR_ERROR = 0.05;
    float SAFE_STOP_DISTANCE = STOP_DISTANCE + LIDAR_ERROR;

};


int main(int argc, char** argv){
    ros::init(argc,argv,"Turtlebot3_obstacle");
    Obstacle turtlebot_obstacle;
    turtlebot_obstacle.run();
    }