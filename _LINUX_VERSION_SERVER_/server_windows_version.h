#pragma once
#ifndef _SERVER_WINDOWS_VERSION_H_
#define _SERVER_WINDOWS_VERSION_H_

#ifdef _WIN32
#include <Windows.h>
#include <winsock.h>
#pragma comment (lib,"ws2_32.lib")
#endif

#include "udp_define.h"
#include <thread>

void build_udp_server(); // 建立UDP服务端
void close_udp_server(); // 关闭UDP服务器端
int start_collection(); // 开始采集
int check_connection(); // 检测连接
int test_connection(); // 测试连接
bool send_cmd(); // 发送命令
bool recv_cmd(); // 接受命令 返回：接收到的数据长度
bool check_cmd(unsigned char cmd_return);
void print_cmd(const char * info); // 打印命令

#endif