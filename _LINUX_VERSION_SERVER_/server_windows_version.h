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

void build_udp_server(); // ����UDP�����
void close_udp_server(); // �ر�UDP��������
int start_collection(); // ��ʼ�ɼ�
int check_connection(); // �������
int test_connection(); // ��������
bool send_cmd(); // ��������
bool recv_cmd(); // �������� ���أ����յ������ݳ���
bool check_cmd(unsigned char cmd_return);
void print_cmd(const char * info); // ��ӡ����

#endif