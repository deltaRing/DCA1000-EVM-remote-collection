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

uint16_t transmit_buff_counter = 0; // byte �ⲿ������
uint8_t transmit_buff_byte_cnt = 0; // byte �ڲ�������
uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_]; // byte����

uint8_t NYT_node_record[_MAXIMUM_BUFF_DEPTH_]; // NYT��¼���� ��¼NYT�ڵ�
uint8_t NYT_node_cnt = 0; // NYT��¼������

uint8_t transmit_byte[_MAXIMUM_BUFF_LENGTH_]; // ��¼ 0 1 ��0 �ң�1
uint8_t transmit_byte_counter = 0; // �õ�������

uint16_t decode_cnt = 0; // ������ ����
uint8_t decode_result[_MAXIMUM_BUFF_CONTAIN_];
uint8_t temp_read_buff[_MAXIMUM_BUFF_READ_];
uint8_t used_bit = 0; // �õ���λ ����
uint8_t used_byte = 0; // �õ����ֽ� ����

// ��ʼ��д�뻺��
void init_write_buff() {
	extern uint16_t decode_cnt;
	extern uint8_t decode_result[_MAXIMUM_BUFF_CONTAIN_];

	decode_cnt = 0;
	memset(decode_result, 0, _MAXIMUM_BUFF_CONTAIN_);
}

// ��ʼ����ȡ����
void init_read_buff() {
	extern uint8_t temp_read_buff[_MAXIMUM_BUFF_READ_];
	extern uint8_t used_bit; // �õ���λ ����
	extern uint8_t used_byte; // �õ����ֽ� ����


	used_bit = 0;
	used_byte = 0;
	memset(temp_read_buff, 0, _MAXIMUM_BUFF_READ_);
}

// ��ʼ������
void init_transmit_buff() {
	extern uint16_t transmit_buff_counter;
	extern uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_];

	transmit_buff_counter = 0;
	transmit_buff_byte_cnt = 0;
	memset(transmit_buff, 0, sizeof(uint8_t) * _MAXIMUM_BUFF_CONTAIN_);
}

// ��ʼ���ַ���
void init_transmit_byte() {
	extern uint8_t transmit_byte_counter;
	extern uint8_t transmit_byte[_MAXIMUM_BUFF_LENGTH_];

	transmit_byte_counter = 0;
	memset(transmit_byte, 0, sizeof(uint8_t) * _MAXIMUM_BUFF_LENGTH_);
}

// ��ʼ��NYT�ڵ�
void init_NYT_buff() {
	extern uint8_t NYT_node_record[_MAXIMUM_BUFF_DEPTH_]; // NYT��¼���� ��¼NYT�ڵ�
	extern uint8_t NYT_node_cnt; // NYT��¼������

	NYT_node_cnt = 0;
	memset(NYT_node_record, 0, sizeof(uint8_t) * _MAXIMUM_BUFF_DEPTH_);
}

#endif