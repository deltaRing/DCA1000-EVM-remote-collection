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
// 0 : �����ɼ� 
// 1 : Server
// 2 : Client

unsigned int _CLIENT_AVAILABLE_ = 0;
// 1 ��ʵʱģʽ
// 2 �����ߴ���ģʽ
#define _REALTIME_MODE_ 1
#define _OFFLINE_MODE_ 2

unsigned int _STRICT_MODE_OPEN_ = 2;
#define _STRICT_CLIENT_MODE_ 1
#define _TURN_INTO_NORMAL_IF_FAILED_ 2
// 1�� �ϸ�Ҫ��ͻ�����Ч
// 2�� Ҫ��ͻ���δ���ӵ�ʱ������ɼ���

// ���������ʱ��ִ�е���Ϊ
unsigned int _ERROR_BEHAVIOR_ = 1;
#define _ERROR_QUIT_ 1 // ����ֱ���˳�
#define _ERROR_CONTINUE_ 2 // �������
#define _ERROR_TRY_RECON_ 5 // �������Ӵ��������������Ŀ
#define _ERROR_TRY_RECON_TIME_ 10000 // ʮ�� 10000���� ��������һ��

// ����ģʽ
unsigned int _ENCODING_MODE_ = 0;
#define _NONE_ENCODING_ 0 // �ޱ���
#define _DHUFFMAN_ENCODING_ 1 // ��̬������������
#define _DHUFFMAN_DPCM_ENCODING_ 2 // ���Ԥ�����

// ��ǰ����״̬
unsigned int _OPEN_ = 0;
#define _LISTEN_CON_ 1
#define _DISABLE_CLOSE_DISCON_ 0
#define _OPEN_MAX_STATUS_ 2

// �Ƿ��_���؂�
unsigned int _TELE_ENABLE_ = 1; // Ĭ�J�_���؂�
#define _TELE_IS_ENABLED_ 1 // �؂��_��
#define _TELE_IS_DISABLED_ 0 // ����Ҫ�؂�

#if defined _WIN32

#endif

#endif