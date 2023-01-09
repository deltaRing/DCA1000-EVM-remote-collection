#pragma once

#ifndef _TCP_FUNCTION_DEFINE_H_
#define _TCP_FUNCTION_DEFINE_H_

#include <cstdio>
#include <cstdlib>

#define _NORMAL_PLAYER_ 0
#define _SERVER_PLAYER_ 1
#define _CLIENT_PLAYER_ 2

#define _HELP_ printf(" ===================================== Help ===================================== \n");
#define _ERROR_ printf(" ===================================== Error ===================================== \n");
#define _WARNING_ printf(" ===================================== WARNING ===================================== \n");
#define _NOTIFICATION_ printf(" ===================================== NOTIFICATION ===================================== \n");

unsigned int PLAYER = 0;
#define _NORMAL_COLLECTOR_ 0
#define _SERVER_RECEIVER_ 1
#define _CLIENT_TRANSMITTER 2
#define _PLAYER_MAX_STATUS_ 3
// 0 : 正常采集 
// 1 : Server
// 2 : Client

unsigned int _CLIENT_AVAILABLE_ = 0;
// 1 ：实时模式
// 2 ：离线传输模式
#define _REALTIME_MODE_ 1
#define _OFFLINE_MODE_ 2

unsigned int _STRICT_MODE_OPEN_ = 2;
#define _STRICT_CLIENT_MODE_ 1
#define _TURN_INTO_NORMAL_IF_FAILED_ 2
// 1： 严格要求客户端有效
// 2： 要求客户端未连接的时候继续采集！

// 发生错误的时候执行的行为
unsigned int _ERROR_BEHAVIOR_ = 1;
#define _ERROR_QUIT_ 1 // 错误直接退出
#define _ERROR_CONTINUE_ 2 // 错误继续
#define _ERROR_TRY_RECON_ 5 // 发生连接错误后重试连接数目
#define _ERROR_TRY_RECON_TIME_ 10000 // 十秒 10000毫秒 重试连接一次

// 编码模式
unsigned int _ENCODING_MODE_ = 0;
#define _NONE_ENCODING_ 0 // 无编码
#define _DHUFFMAN_ENCODING_ 1 // 动态哈夫曼树编码
#define _DHUFFMAN_DPCM_ENCODING_ 2 // 差分预测编码

// 当前工作状态
unsigned int _OPEN_ = 0;
#define _LISTEN_CON_ 1
#define _DISABLE_CLOSE_DISCON_ 0
#define _OPEN_MAX_STATUS_ 2

// 是否開啓回傳
unsigned int _TELE_ENABLE_ = 1; // 默認開啓回傳
#define _TELE_IS_ENABLED_ 1 // 回傳開啓
#define _TELE_IS_DISABLED_ 0 // 不需要回傳

#if defined _WIN32

#endif

#endif