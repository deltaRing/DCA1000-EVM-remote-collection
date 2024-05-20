#include "drone_attitude.h"
#include <fstream>

bool test_callback = false;
bool record_data   = false;
bool request_record_data = false;
std::string attitude_file_name;

void FC_posture_callback(const geometry_msgs::PoseStamped::ConstPtr & msg){
	float x = msg->pose.position.x;
 	float y = msg->pose.position.y;
 	float z = msg->pose.position.z;
	float qx = msg->pose.orientation.x;
	float qy = msg->pose.orientation.y;
	float qz = msg->pose.orientation.z;
	float qw = msg->pose.orientation.w;

    if (request_record_data) {
        std::ofstream file(attitude_file_name.c_str(), std::ios::app);
        file << x << " " << y << " " << z << " " << qx << " " << qy << " " << qz << " " << qw << std::endl;
        file.close();
    }

	ROS_INFO("POSTURE_INFO");
	if (test_callback) {
		std::cout << "Posture Info:" << std::endl <<
			"x: " << x << std::endl <<
			"y: " << y << std::endl << 
			"z: " << z << std::endl <<
			"qx: " << qx << std::endl <<
			"qy: " << qy << std::endl <<
			"qz: " << qz << std::endl <<
			"qw: " << qw << std::endl;
	}
	else {
	
	}
}

void FC_velocity_callback(const geometry_msgs::TwistStamped::ConstPtr & msg){
	float vx = msg->twist.linear.x;
	float vy = msg->twist.linear.y;
	float vz = msg->twist.linear.z;
	float agx = msg->twist.angular.x;
	float agy = msg->twist.angular.y;
	float agz = msg->twist.angular.z;

	ROS_INFO("VELOCITY_INFO");
	if (test_callback) {
		std::cout << "Velocity Info:" << std::endl <<
			"vx: " << vx << std::endl <<
			"vy: " << vy << std::endl << 
			"vz: " << vz << std::endl <<
			"agx: " << agx << std::endl <<
			"agy: " << agy << std::endl <<
			"agz: " << agz << std::endl;
	}
	else {
	
	}
}