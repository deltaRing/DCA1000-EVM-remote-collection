#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_

#include "udp_define.h"
#include "radar_define.h"

void checkStatus(); // check connection
void openClient(); //
void closeClient(); // 
bool recvCmd(); //
void test_udp_client();
bool replyCmd(); //
void startRecord(std::string cfg, std::string save_path); //
void startRecord_local(std::string cfg, std::string save_path); //
void ComInit(std::string port);

#endif
