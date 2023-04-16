#ifndef _UDP_DEFINE_H_
#define _UDP_DEFINE_H_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <typeinfo>
#include <iostream>

#define _PACKAGE_SIZE_ 32
#define _RADAR_PACKAGE_SIZE_ 1466

#define _RECORD_IP_ "192.168.33.180"
#define _RECORD_PORT_ 1024
#define _RECV_IP_ "192.168.33.30"
#define _RECV_PORT_ 4098

#define _SEND_IP_ "192.168.33.180"
#define _SEND_PORT_ 4096
#define _UDP_IP_ "192.168.33.30"
#define _UDP_PORT_ 4096

#define _SERVER_IP_ "192.168.1.100"
#define _SERVER_PORT_ 103
#define _CLIENT_IP_ "192.168.1.101"
#define _CLIENT_PORT_ 721

#define _SEND_HEADER_ 0x17
#define _SEND_START_RECORD_ 0x01

#define _SERVER_SEND_OK_ 0x00
#define _SERVER_SEND_ERROR_ 0x01
#define _SERVER_RECV_ERROR_ 0x02
#define _SERVER_NOT_READY_ 0x03
#define _SERVER_TEST_CON_ 0x04
#define _SERVER_CHECK_STATUS_ 0x05
#define _SERVER_ALLOW_RECORD_ 0x06

#define _CLIENT_REPLY_OK_ 0x00
#define _CLIENT_REPLY_ERR_ 0x01
#define _CLIENT_REPLY_RECORD_OVER_ 0x02
#define _CLIENT_REPLY_UNKNOWN_ 0x03 // Unknown Instruction
#define _CLIENT_REPLY_TEST_CON_ 0x04
#define _CLIENT_REPLY_COM_ERR_ 0x05
#define _CLIENT_REPLY_UDP_CON_ERR_ 0x06
#define _CLIENT_REPLY_UDP_CON_SUC_ 0x07
#define _CLIENT_REPLY_COM_CON_ERR_ 0x08
#define _CLIENT_REPLY_COM_CON_SUC_ 0x09
#define _CLIENT_REPLY_UDP_ADR_ERR_ 0x10
#define _CLIENT_REPLY_UDP_ADR_SUC_ 0x11
#define _CLIENT_REPLY_CON_ERR_ 0x12
#define _CLIENT_REPLY_CON_SUC_ 0x13
#define _CLIENT_REPLY_STATUS_REPORT_ 0x14

#endif
