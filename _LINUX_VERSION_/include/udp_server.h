#ifndef _UDP_SERVER_H_
#define _UDP_SERVER_H_

#include "udp_define.h"
#include <thread>

void build_udp_server(); // 建立UDP服务端
void close_udp_server(); // 关闭UDP服务器端
int start_collection(); // 开始采集
bool send_cmd(); // 发送命令
bool recv_cmd(); // 接受命令 返回：接收到的数据长度
bool check_cmd(unsigned char cmd_return);
void print_cmd(const char * info); // 打印命令

#endif
