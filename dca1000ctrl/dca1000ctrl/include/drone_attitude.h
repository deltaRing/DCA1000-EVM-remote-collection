#ifndef _DRONE_ATTITUDE_H_
#define _DRONE_ATTITUDE_H_

#include <iostream>
#include <ostream>
#include <vector>
#include <string>

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>

void FC_posture_callback(const geometry_msgs::PoseStamped::ConstPtr & msg);
void FC_velocity_callback(const geometry_msgs::TwistStamped::ConstPtr & msg);

#endif
