#pragma once

#ifndef _AR_UI_DEFINE_H_
#define _AR_UI_DEFINE_H_

// 给外部监听提供变量
unsigned char Command = 0x00; // 无指令

#define _NON_COMMAND_ 0x00
#define _RECORD_COMMAND_ 0x01
#define _EXIT_COMMAND_ 0xFF 

// 给外部监听提供回复
unsigned char Reply = 0x00; // 根据错误自动更改回复

#define _NON_REPLY_ 0x00; // 无回复

#endif 