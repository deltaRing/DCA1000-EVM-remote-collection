#pragma once
#ifndef _TCP_PACKET_DEFINE_H_
#define _TCP_PACKET_DEFINE_H_

// Ҫ���ӵ�IP
#define _SERVER_IP_ "127.0.0.1"
#define _SERVER_PORT_ 6666

#define _CON_SERVER_IP_ "127.0.0.1"
#define _CON_SERVER_PORT_ 6666

// ���ݵĳ���
#define _MIN_PACKAGE_WITHOUT_SEQ_LEN_ 48
#define _MAX_PACKAGE_WITHOUT_SEQ_LEN_ 1466
#define _PACKAGE_NUMBER_SEQ_LEN_ 10

/* ======================һЩ�����Ķ���====================== */
#define _DESCRIBE_LEN_ 1024 // ��������
#define _CLIENT_MAX_BUFFER_ 1469 // 1466 + 3 // CMD 1 + LENGTH 2 + DATA 1466

#endif