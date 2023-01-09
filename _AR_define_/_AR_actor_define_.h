#pragma once

#ifndef _AR_ACTOR_DEFINE_H_
#define _AR_ACTOR_DEFINE_H_

#include "_AR_include_/tcp_client_build.h"
#include "_AR_include_/tcp_server_build.h"

#if not defined _WIN32
radar_client * _radar_client_ = NULL; // 全局预先定义
radar_server * _radar_server_ = NULL; // 全局预先定义
#endif

#endif
