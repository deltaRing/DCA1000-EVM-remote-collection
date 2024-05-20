#include <ros/ros.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <chrono>
#include "dca1000ctrl/instruction.h"
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/RCIn.h>
#include "udp_client.h" 							// init com port and udp port
#include "drone_attitude.h"						

#define _RC_HIGH_ 1750
#define _RC_MIDDLE_ 1500
#define _RC_LOW_ 1250
#define _RC_CONTROL_DCA1000_CHANNEL_ 9
mavros_msgs::State current_state; // current status of flight controller
std::string ld_port = "/dev/ttyUSB0";
std::string cfg_file = "/home/tx1_user/ros_package/catkin_workspace/src/mavros_dca1000_control/src/dca1000ctrl/profile_monitor_xwr18xx.cfg";

// connnection flags 
extern bool port_ready;
extern bool addr_ready;
extern bool data_port_ready;
extern bool request_record_data;
extern std::string attitude_file_name;

void FC_callback(const mavros_msgs::RCIn::ConstPtr & msg){
	if (msg->rssi == 0) {
		return;
	}

	if (msg->channels[_RC_CONTROL_DCA1000_CHANNEL_] >= _RC_HIGH_){
		// try to get radar
		auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		//转为字符串
 		std::stringstream ss;
	        // 可以分别以不同的形式进行显示
	        ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S");
	        std::string str = ss.str();
			attitude_file_name = str + std::string(".txt");
	        str += std::string(".bin");
			std::thread _record_thread_ = std::thread(startRecord, cfg_file, str);
	        _record_thread_.detach();
			//startRecord(cfg_file, str);
			ROS_INFO("[node] Send request for recording data");
	}
}

void FC_status_callback(const mavros_msgs::State::ConstPtr & msg){
	current_state = *msg;
}

int main(int argc, char ** argv){
	ros::init(argc, argv, "radar_sample_main_program");
	ros::NodeHandle nh;
	
	ROS_INFO("Initializing Flight Controller Callback");
	ros::Subscriber state_sub    = nh.subscribe<mavros_msgs::State>("mavros/state", 10, FC_status_callback);
	ros::Subscriber rc_sub       = nh.subscribe<mavros_msgs::RCIn>("mavros/rc/in", 10, FC_callback);
	ros::Publisher sample_status = nh.advertise<dca1000ctrl::instruction>("sample/status", 10);

	ros::Rate rate(10);
	
	// wait for fc's connection
	while (ros::ok() && !current_state.connected){
		ROS_INFO("Waiting for flight controller");
		ros::spinOnce();
		rate.sleep();
	}

	// wait for radar's connection
	while (ros::ok() && (!port_ready || !data_port_ready || !addr_ready)){
		ros::spinOnce();
		rate.sleep();
		ComInit(ld_port);
		if (!port_ready){
			ROS_ERROR("Radar USB port not available");
		}
		if (!data_port_ready){
			ROS_ERROR("DCA1000EVM data port not available");
		}
		if (!addr_ready){
			ROS_ERROR("DCA1000EVM config port not available");
		}
	}
	ROS_INFO("All Radar Ports are available!");

	std::thread posture_record_thread;
	ros::Subscriber pose_sub     = nh.subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose", 1, FC_posture_callback);
	// ros::Subscriber velo_sub     = nh.subscribe<geometry_msgs::TwistStamped>("/mavros/local_position/velocity", 1, FC_velocity_callback);

	// if fc is connected
	// waiting RC
	while (ros::ok()){

		ros::spinOnce();
		rate.sleep();
	}	
	return 0;
}
