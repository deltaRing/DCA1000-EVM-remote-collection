#pragma once
#ifndef _A_CODER_DEFINE_H_
#define _A_CODER_DEFINE_H_

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <stdlib.h>
#include <iostream>

#define _MAXIMUM_BUFF_CONTAIN_ 1469
#define _MAXIMUM_BUFF_LENGTH_ 256
#define _MAXIMUM_BUFF_DEPTH_ 256
#define _MAXIMUM_BUFF_READ_ 4

uint16_t transmit_buff_counter = 0; // byte 外部计数器
uint8_t transmit_buff_byte_cnt = 0; // byte 内部计数器
uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_]; // byte数组

uint8_t NYT_node_record[_MAXIMUM_BUFF_DEPTH_]; // NYT记录缓存 记录NYT节点
uint8_t NYT_node_cnt = 0; // NYT记录计数器

uint8_t transmit_byte[_MAXIMUM_BUFF_LENGTH_]; // 记录 0 1 左：0 右：1
uint8_t transmit_byte_counter = 0; // 用到的数据

uint16_t decode_cnt = 0; // 解码器 计数
uint8_t decode_result[_MAXIMUM_BUFF_CONTAIN_];
uint8_t temp_read_buff[_MAXIMUM_BUFF_READ_];
uint8_t used_bit = 0; // 用到的位 计数
uint8_t used_byte = 0; // 用到的字节 计数

// 初始化写入缓存
void init_write_buff() {
	extern uint16_t decode_cnt;
	extern uint8_t decode_result[_MAXIMUM_BUFF_CONTAIN_];

	decode_cnt = 0;
	memset(decode_result, 0, _MAXIMUM_BUFF_CONTAIN_);
}

// 初始化读取缓存
void init_read_buff() {
	extern uint8_t temp_read_buff[_MAXIMUM_BUFF_READ_];
	extern uint8_t used_bit; // 用到的位 计数
	extern uint8_t used_byte; // 用到的字节 计数


	used_bit = 0;
	used_byte = 0;
	memset(temp_read_buff, 0, _MAXIMUM_BUFF_READ_);
}

// 初始化数据
void init_transmit_buff() {
	extern uint16_t transmit_buff_counter;
	extern uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_];

	transmit_buff_counter = 0;
	transmit_buff_byte_cnt = 0;
	memset(transmit_buff, 0, sizeof(uint8_t) * _MAXIMUM_BUFF_CONTAIN_);
}

// 初始化字符串
void init_transmit_byte() {
	extern uint8_t transmit_byte_counter;
	extern uint8_t transmit_byte[_MAXIMUM_BUFF_LENGTH_];

	transmit_byte_counter = 0;
	memset(transmit_byte, 0, sizeof(uint8_t) * _MAXIMUM_BUFF_LENGTH_);
}

// 初始化NYT节点
void init_NYT_buff() {
	extern uint8_t NYT_node_record[_MAXIMUM_BUFF_DEPTH_]; // NYT记录缓存 记录NYT节点
	extern uint8_t NYT_node_cnt; // NYT记录计数器

	NYT_node_cnt = 0;
	memset(NYT_node_record, 0, sizeof(uint8_t) * _MAXIMUM_BUFF_DEPTH_);
}

#endif