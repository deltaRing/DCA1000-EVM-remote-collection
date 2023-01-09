#pragma once

#ifndef _TCP_SERVER_BUILD_H_
#define _TCP_SERVER_BUILD_H_

#define _TIMEOUT_TIME_ 100
#define _BUFF_LEN_ 1024

#include <mutex>
#include <cstdio>
#include "../_AR_utility_/server_list.h"
#include "../_AR_utility_/linked_list.h"
#include "../_AR_define_/TCP_define.h"
#include "../_AR_define_/TCP_packet_define.h"
#include "../_AR_define_/_net_work_define_.h"
#include "../_AR_define_/TCP_function_define.h"
#include "../_AR_define_/TCP_compress_define.h"
#include "../_AR_include_/_AR_UI_INFO_SUBSYSTEM_.h"
#include "../_AR_decode/_wave_AR_decode_.h"

/* ==========================IP定义============================= */
extern QString IP;
extern QString Port;

/* =========================INFO定义=========================== */
extern std::vector <QByteArray> info; // 全局定义
extern std::vector <LEVEL> warn; // 警告程度
extern std::mutex info_mutex; // 互斥锁

/* ========================服务器错误码========================== */
#define _NORMAL_ 0
#define _WSA_ERROR_ -1
#define _SOCKET_ERROR_ -2
#define _SOCKET_BIND_ERROR_ -3
#define _SOCKET_LISTEN_ERROR_ -4
#define _SOCKET_TIMEOUT_ERROR_ -5
#define _SOCKET_DATA_HEADER_IS_NULL_ -6
#define _SOCKET_IS_NOT_INIT_ -7
#define _SOCKET_DATA_IS_NOT_READY_ -8
#define _SOCKET_DATA_TYPE_UNKNOWN_ -9
#define _SOCKET_DATA_REPLY_UNKNOWN_ -10
#define _SOCKET_DATA_LENGTH_INVALID_ -11
#define _SOCKET_DATA_DESCRIBE_INVALID_ -12
#define _SOCKET_DATA_LENGTH_IS_NOT_EQUAL_ -13
#define _SOCKET_DATA_TRANSMITTING_REPEAT_ -14 // 重复发送
#define _SOCKET_SAVE_FILENAME_IS_EMPTY_ -15

/* =======================服务器其他定义======================== */
#define _FILENAME_MAX_LEN_ 1024
#define _LOSS_PKG_MAX_TIMES_ 10 // 最大丢包次数

#include <thread>
#include <QTimer>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QThread>
#include <QDebug>

#if defined _WIN32
#include <Windows.h>
#include <winsock.h>
#pragma comment (lib,"ws2_32.lib")
	SOCKET server;
	SOCKADDR_IN server_addr;
	BOOL server_is_init = 0;
	BOOL DATA_IS_READY = 0;
	BOOL server_is_closed = 1; // 服务器默认关闭
	extern SERV_CON_LIST * header;
	extern SERV_CON_LIST * footer;
	extern UINT32 CONNECT_NUM;
	LIST_ * ServerHeader = NULL;
	LIST_ * FOOTERHEADER = NULL; // 做Append使用
	UINT32 DATA_LENGTH = 0; // 实际的数据长度
	UINT32 RECV_DATA_LENGTH = 0; // 记录接受到的
	UINT32 WRITE_LENGTH = 0; // 实际要写入的包长度
	UINT32 DATA_INDEX = 1; // 验证重复的包
	UINT32 Current_Coding = _NO_COMPRESS_CODING_; // 現在所使用的編碼

	// 向端口写入数据
	void WINAPI tcp_server_send_data(const char data[], SERV_CON_LIST * client) {
		if (!server_is_init) {
			info_mutex.lock();
			info.push_back("TCP Server: TCP write data: Server is not initialized\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}

		if (client == NULL) {
			info_mutex.lock();
			info.push_back("TCP Server: TCP write data: client is NULL\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}

		UINT32 size = send(client->client, data, _CLIENT_MAX_BUFFER_, 0);
		if (size == SOCKET_ERROR) {
			info_mutex.lock();
			info.push_back("TCP Server: TCP write data: _SOCKET_ERROR_!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			client->ALIVE = 0;
			closesocket(client->client);
		}
		else if (size == 0) {
			info_mutex.lock();
			info.push_back("TCP Server: TCP write data: _Connection_is_closed_!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			client->ALIVE = 0;
			closesocket(client->client);
		}
	}

	// 更新TCP服務器 查看有哪些客戶端不在綫
	void tcp_server_check_client(SERV_CON_LIST * h, unsigned int & loss_pkg_times) {
		if (h == NULL) {
			return;
		}
		unsigned char command[_CLIENT_MAX_BUFFER_];
		command[0] = _DATA_TYPE_COMMAND_;
		command[1] = _CMD_ARE_U_ALIVE_;
		tcp_server_send_data((char *)command, h);
		Sleep(100); // 等待100ms
		// 接收數據
		memset(command, 0, _CLIENT_MAX_BUFFER_);
		int bytes = recv(h->client, (char *)command, _CLIENT_MAX_BUFFER_, 0);
		if (bytes < 0 && loss_pkg_times >= _LOSS_PKG_MAX_TIMES_) {
			h->ALIVE = 0; // 後面的函數會自動刪除
		}
	}

	// TCP 服务器重置
	void tcp_server_reset() {
		if (ServerHeader != NULL) {
			list_delete(ServerHeader);
			ServerHeader = NULL;
			FOOTERHEADER = NULL;
		}

		UINT32 DATA_INDEX = 1;
		DATA_IS_READY = 0;
		DATA_LENGTH = 0;
		WRITE_LENGTH = 0;
		RECV_DATA_LENGTH = 0;

		info_mutex.lock();
		info.push_back("TCP Server: Server is Reset\n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
	}

	// 保存数据
	int WINAPI tcp_server_save_data(const char * filename) {
		extern LIST_ * ServerHeader;
		extern UINT32 DATA_LENGTH;
		extern UINT32 RECV_DATA_LENGTH;

		if (ServerHeader == NULL) {
			info_mutex.lock();
			info.push_back("TCP Server: Header is NULL\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_DATA_HEADER_IS_NULL_;
		}

		if (!DATA_IS_READY) {
			info_mutex.lock();
			info.push_back("TCP Server: Data is not Ready\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_DATA_IS_NOT_READY_;
		}

		if (DATA_LENGTH <= 0) {
			info_mutex.lock();
			info.push_back("TCP Server: Data length is empty\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_DATA_LENGTH_INVALID_;
		}

		if (strlen(filename) <= 0) {
			info_mutex.lock();
			info.push_back("TCP Server: File name is empty\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_SAVE_FILENAME_IS_EMPTY_;
		}

		FILE * fp = fopen(filename, "wb");

		unsigned char * data = (unsigned char *)malloc(sizeof(unsigned char) * WRITE_LENGTH);
		unsigned int data_offset = 0;

		LIST_ * h = ServerHeader;
		while (h != NULL) {
			memcpy(data + data_offset,
				h->data + _PACKAGE_NUMBER_SEQ_LEN_,
				sizeof(unsigned char) * (h->data_size - _PACKAGE_NUMBER_SEQ_LEN_)
			);
			data_offset += h->data_size - _PACKAGE_NUMBER_SEQ_LEN_;
			h = h->next;
		}
		fwrite(data, sizeof(unsigned char),
			DATA_LENGTH, fp);
		fclose(fp);

		list_delete(ServerHeader);
		ServerHeader = NULL;
		FOOTERHEADER = NULL;

		free(data);
		DATA_IS_READY = 0;
		DATA_LENGTH = 0;
		WRITE_LENGTH = 0;
		RECV_DATA_LENGTH = 0;
		return _NORMAL_;
	}

	// 检查是否重复传输数据
	BOOL WINAPI check_current_packet_repeat(UINT32 expect_num, UINT32 current_num) {
		if (expect_num == current_num) return 1;
		else return 0;
	}

	// 分析命令
	int WINAPI tcp_server_analysis_command(char buff[_CLIENT_MAX_BUFFER_]) {
		if (!server_is_init) {
			info_mutex.lock();
			info.push_back("TCP Server: TCP Analysis Command, Server is not Initialized\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_IS_NOT_INIT_;
		}

		UINT8 DATA_TYPE = buff[0];

		switch (DATA_TYPE) {
		case _DATA_TYPE_COMMAND_: {

			break;
		}
		case _DATA_TYPE_REPLY_: {
			if (buff[1] == _REPLY_DATA_IS_TRANSFERRED_) {
				DATA_IS_READY = 1;
			}
			else if (buff[1] == _REPLY_SUCCESS_) {
				info_mutex.lock();
				info.push_back("TCP Server: TCP Analysis Command, Server received Success reply\n");
				warn.push_back(_UI_INFO_);
				info_mutex.unlock();
			}
			else if (buff[1] == _REPLY_FAILED_) {
				info_mutex.lock();
				info.push_back("TCP Server: TCP Analysis Command, Server received Failure reply\n");
				warn.push_back(_UI_WARN_);
				info_mutex.unlock();
			}
			else if (buff[1] == _REPLY_YEAP_RA9_) {

			}
			else {
				info_mutex.lock();
				info.push_back("TCP Server: Unknown Response...\n");
				warn.push_back(_UI_ERR_);
				info_mutex.unlock();
				return _SOCKET_DATA_REPLY_UNKNOWN_;
			}

			break;
		}
		case _DATA_TYPE_DATA_: {
			// 2023年 加入了 動態哈夫曼編解碼 要求：先解碼
			if (Current_Coding == _DDHUFF_COMPRESS_CODING_) {
				extern uint8_t decode_result[_MAXIMUM_BUFF_CONTAIN_]; // 解碼結果
				uint16_t _external_counter_ = (UINT8)buff[1] +
					(UINT8)buff[2] * 256; // 外部的字節計數
				uint8_t _internal_counter_ = (UINT8)buff[3]; // 内部的計數
				
				UINT8 buff_temp[_CLIENT_MAX_BUFFER_]; // 臨時的緩存
				memset(buff_temp, 0, _CLIENT_MAX_BUFFER_);
				memcpy(buff_temp, buff + 4, _CLIENT_MAX_BUFFER_ - 4);
				_get_D_DHuff_code_(buff_temp, _internal_counter_, _external_counter_);
				memcpy(buff + 3, decode_result, _CLIENT_MAX_BUFFER_ - 3); // 複製内存
			}
			else {
			 // 默認無編碼 忽略
			}

			// 检验包
			UINT32 packet_index = (UINT8)buff[3] +
				(UINT8)buff[4] * 256 +
				(UINT8)buff[5] * 65536 +
				(UINT8)buff[6] * 16777216;

			if (check_current_packet_repeat(DATA_INDEX, packet_index)) {
				// 数据重复了
				info_mutex.lock();
				info.push_back("TCP Server: TCP Analysis Data: Detect Loop Data Skipping...\n");
				warn.push_back(_UI_WARN_);
				info_mutex.unlock();
				return _SOCKET_DATA_TRANSMITTING_REPEAT_;
			}

			UINT16 datasize = 0;
			if (ServerHeader == NULL) {
				ServerHeader = list_init();
				UINT8 buff1 = buff[1];
				UINT8 buff2 = buff[2];
				datasize = buff1 + buff2 * 256;
				memcpy(ServerHeader->data, buff + 3, datasize);
				ServerHeader->data_size = datasize;
				FOOTERHEADER = ServerHeader;
			}
			else {
				LIST_ * node = list_init();
				UINT8 buff1 = buff[1];
				UINT8 buff2 = buff[2];
				datasize = buff1 + buff2 * 256;
				memcpy(node->data, buff + 3, datasize);
				node->data_size = datasize;
				FOOTERHEADER = list_append(FOOTERHEADER, node);
			}
			WRITE_LENGTH += datasize - _PACKAGE_NUMBER_SEQ_LEN_;
			RECV_DATA_LENGTH += datasize;
			DATA_INDEX += 1;
			break;
		}
		case _DATA_TYPE_DATA_LENGTH_: {
			DATA_INDEX = 1;
			DATA_LENGTH = 0;
			UINT8 buff1 = buff[1];
			UINT8 buff2 = buff[2];
			UINT8 buff3 = buff[3];
			UINT8 buff4 = buff[4];
			DATA_LENGTH = buff1 +
				buff2 * 256 +
				buff3 * 65536 +
				buff4 * 16777216;
			// printf("TCP Server: TCP Get Length %d bytes\n", DATA_LENGTH);
			break;
		}
		case _DATA_TYPE_CODING_: {
			
			break; // 服務器會接收到編碼類型請求的數據？
		}
		default: {
			// printf("TCP Server: TCP分析命令：未知命令\n");
			return _SOCKET_DATA_TYPE_UNKNOWN_;
		}
		}

		return _NORMAL_;
	}

	// 接受请求数据
	void WINAPI tcp_server_recv_data(SERV_CON_LIST * lpParameter) {
		extern LIST_ * ServerHeader;
		if (!server_is_init) {
			info_mutex.lock();
			info.push_back("TCP Server: TCP Receive Data: Server is not initialized\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}

		if (ServerHeader != NULL) {
			list_delete(ServerHeader);
			ServerHeader = NULL;
		}

		unsigned char buff[_CLIENT_MAX_BUFFER_];
		unsigned int _loss_pkg_times_ = 0;

		SERV_CON_LIST * CLIENT = lpParameter;
		while (1) {
			if (!CLIENT->ALIVE) {
				if (CLIENT->forward != NULL) {
					CLIENT->forward = CLIENT->next;
				}
				else if (CLIENT->forward == NULL && CLIENT->next == NULL) {
					header = NULL;
					footer = NULL;
				}

				QByteArray temp = QByteArray("TCP Server: Disconnected: ");
				temp.append(QString::fromStdString(std::to_string((int)CLIENT->ip_addr.sin_addr.S_un.S_un_b.s_b1)));
				temp.append('.');
				temp.append(QString::fromStdString(std::to_string((int)CLIENT->ip_addr.sin_addr.S_un.S_un_b.s_b2)));
				temp.append('.');
				temp.append(QString::fromStdString(std::to_string((int)CLIENT->ip_addr.sin_addr.S_un.S_un_b.s_b3)));
				temp.append('.');
				temp.append(QString::fromStdString(std::to_string((int)CLIENT->ip_addr.sin_addr.S_un.S_un_b.s_b4)));
				temp.append(" Port: ");
				temp.append(QString::fromStdString(std::to_string((int)CLIENT->ip_addr.sin_port)));
				temp.append('\n');
				info_mutex.lock();
				info.push_back(temp);
				warn.push_back(_UI_WARN_);
				info_mutex.unlock();

				free(CLIENT);
				CONNECT_NUM -= 1;
				return; // 断开链接的话 返回
			}
			memset(buff, 0, _BUFF_LEN_);
			int bytes = recv(CLIENT->client, (char *)buff, _CLIENT_MAX_BUFFER_, 0);
			if (bytes < 0) {
				// no data time out
				tcp_server_check_client(CLIENT, _loss_pkg_times_);
				Sleep(100); // 等待100毫秒
				continue;
			}
			_loss_pkg_times_ = 0;
			int Result = tcp_server_analysis_command((char*)buff); 
			int Result_Reply = 0; // 回复发送字节数目
			memset(buff, 0, _BUFF_LEN_);
			if (Result && Result != _SOCKET_DATA_TRANSMITTING_REPEAT_) {
				QByteArray temp = QByteArray("TCP Server: TCP Receive data: Detect Error in Loop code: ");
				temp.append(QString::fromStdString(std::to_string(Result)));
				temp.append('\n');
				info_mutex.lock();
				info.push_back(temp);
				warn.push_back(_UI_WARN_);
				info_mutex.unlock();

				// 同样回复失败 要求重传
				buff[0] = _DATA_TYPE_REPLY_;
				buff[1] = _REPLY_FAILED_;
				Result_Reply = send(CLIENT->client, (char*)buff, _CLIENT_MAX_BUFFER_, 0);
			}
			else {
				// 加入回复模块 表示正常接收
				buff[0] = _DATA_TYPE_REPLY_;
				buff[1] = _REPLY_SUCCESS_;
				Result_Reply = send(CLIENT->client, (char*)buff, _CLIENT_MAX_BUFFER_, 0);
			}

			if (Result_Reply <= 0) {
				int retry_time = 5;
				bool sucessful_send = false;
				for (int retry_times = 0; retry_times < retry_time; retry_times++) {
					Result_Reply = send(CLIENT->client, (char*)buff, _CLIENT_MAX_BUFFER_, 0);
					if (Result_Reply) {
						sucessful_send = true;
						break;
					}
				}
				if (!sucessful_send) {
					QByteArray temp = QByteArray("TCP Server: Reply Progress Error!\n");
					info_mutex.lock();
					info.push_back(temp);
					warn.push_back(_UI_ERR_);
					info_mutex.unlock();
				}
			}

			if (DATA_IS_READY) {
				if (DATA_LENGTH != RECV_DATA_LENGTH) {
					// _WARNING_
					QByteArray temp = QByteArray("TCP Server: TCP Receive data: Detect Different Length, Expect: ");
					temp.append(QString::fromStdString(std::to_string(DATA_LENGTH)));
					temp.append(" Received: ");
					temp.append(QString::fromStdString(std::to_string(RECV_DATA_LENGTH)));
					temp.append(' \n');
					info_mutex.lock();
					info.push_back(temp);
					warn.push_back(_UI_WARN_);
					info_mutex.unlock();
				}
				info_mutex.lock();
				info.push_back("TCP Server: Saving data...\n");
				warn.push_back(_UI_INFO_);
				info_mutex.unlock();
				tcp_server_save_data("ADC_DATA_BUFF.bin");
				info_mutex.lock();
				info.push_back("TCP Server: Data is saved...\n");
				warn.push_back(_UI_INFO_);
				info_mutex.unlock();
			}
		}
	}

	// 等待连接
	void WINAPI tcp_server_waiting_connect() {
		if (!server_is_init) {
			info_mutex.lock();
			info.push_back("TCP Server: TCP wait connection: server is not initialized!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
		while (!server_is_closed) {
			SOCKET client;
			SOCKADDR_IN remoteAddr;//sockaddr_in常用于socket定义和赋值,sockaddr用于函数参数
			int nAddrlen = sizeof(remoteAddr);
			char describe[_DESCRIBE_MAX_LEN_];
			client = accept(server, (SOCKADDR *)&remoteAddr, &nAddrlen);
			if (client == INVALID_SOCKET) {
				info_mutex.lock();
				info.push_back("TCP Server: Socket is Closed! \n");
				warn.push_back(_UI_ERR_);
				info_mutex.unlock();
				continue;
			}

			QByteArray temp = QByteArray("TCP Server: TCP Receive Data: Connected: ");
			temp.append(QString::fromStdString(std::to_string((int)remoteAddr.sin_addr.S_un.S_un_b.s_b1)));
			temp.append('.');
			temp.append(QString::fromStdString(std::to_string((int)remoteAddr.sin_addr.S_un.S_un_b.s_b2)));
			temp.append('.');
			temp.append(QString::fromStdString(std::to_string((int)remoteAddr.sin_addr.S_un.S_un_b.s_b3)));
			temp.append('.');
			temp.append(QString::fromStdString(std::to_string((int)remoteAddr.sin_addr.S_un.S_un_b.s_b4)));
			temp.append(" Port: ");
			temp.append(QString::fromStdString(std::to_string((int)remoteAddr.sin_port)));
			temp.append('\n');
			info_mutex.lock();
			info.push_back(temp);
			warn.push_back(_UI_INFO_);
			info_mutex.unlock();

			int ret = recv(client, describe, _DESCRIBE_MAX_LEN_, 0);
			if (ret <= 0) {
				info_mutex.lock();
				info.push_back("TCP Server: Describe is invalid!\n");
				warn.push_back(_UI_WARN_);
				info_mutex.unlock();
			}
			SERV_CON_LIST * node = server_client_node_init();
			node->client = client;
			node->ip_addr = remoteAddr;
			node->index = 0;
			strcpy(node->describe, describe);
			CONNECT_NUM += 1;
			// 如果初始化后 header 与 footer 同时为空
			if (header == NULL && footer == NULL) {
				header = node;
				footer = node;
			}
			else {
				server_client_append_node(header, node);
			}

			std::thread _thread_recv_data_thread_ = std::thread(tcp_server_recv_data, node);
			_thread_recv_data_thread_.detach(); // 线程自动接受
		}
	}

	// 建立 TCP_SERVER
	int build_tcp_server() {
		//初始化WSA
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA wsaData;//WSADATA结构体变量的地址值

		//成功时会返回0，失败时返回非零的错误代码值
		if (WSAStartup(sockVersion, &wsaData) != 0)
		{
			info_mutex.lock();
			info.push_back("TCP Server: WSAStartup Error \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _WSA_ERROR_;
		}

		server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (server == INVALID_SOCKET) {
			info_mutex.lock();
			info.push_back("TCP Server: Socket Error \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_ERROR_;
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(Port.toInt());//设置端口。
		server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // 允许所有地址

		DWORD lBuffer[2] = { 0, 0 };
		int lSize;
		lBuffer[0] = (DWORD)_TIMEOUT_TIME_;
		lSize = sizeof(DWORD);
		if (setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (char*)lBuffer, sizeof(lBuffer))) {
			info_mutex.lock();
			info.push_back("TCP Server: Timeout set failure\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_TIMEOUT_ERROR_;
		}

		if (bind(server, (SOCKADDR*)&server_addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
		{
			info_mutex.lock();
			info.push_back("TCP Server: Socket Bind Error! \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_BIND_ERROR_;
		}

		if (listen(server, 5) == SOCKET_ERROR)
		{
			info_mutex.lock();
			info.push_back("TCP Server: Socket Listen Error! \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _SOCKET_LISTEN_ERROR_;
		}

		server_is_init = 1;

		return _NORMAL_;
	}
#else

class radar_server : public QObject {
	Q_OBJECT
public:
	explicit radar_server(QObject * parent = nullptr);
	void start_listen(int port); // QT 编写的 TCP 通信 // 开始监听
	void decrypt_data(); //  解密数据
	void write_data(QByteArray data);  //  写入数据
	bool check_is_new_data(UINT32 expect_num, UINT32 current_num); // 检查包是不是重复了
	int tcp_server_save_data(const char * filename); // 保存数据
	void recv_data_subfunc(); // 接受数据子程序

private:
	QTcpServer * p_server = NULL;
	QTcpSocket * p_socket = NULL;
	char describe[_DESCRIBE_MAX_LEN_];
	char * buff = NULL; //  先指向空指针
	int bytes; // 读取了多少字节
	QByteArray buf;
	BOOL server_is_init = 0;
	BOOL DATA_IS_READY = 0;
	LIST_ * ServerHeader = NULL;
	LIST_ * FOOTERHEADER = NULL; // 做Append使用
	UINT32 DATA_LENGTH = 0; // 实际的数据长度
	UINT32 RECV_DATA_LENGTH = 0; // 记录接受到的
	UINT32 WRITE_LENGTH = 0; // 实际要写入的包长度
	UINT32 DATA_INDEX = 1; // 验证重复的包

public slots:
	void new_connection_subfunc();  // 新建连接 
	void disconnection_subfunc(); // 一旦失联 请重新链接
	void ready_read_data();
};

// 雷达服务器初始化
radar_server::radar_server(QObject * parent) :
	QObject(parent) {
	p_server = new QTcpServer();
	p_socket = new QTcpSocket();

	start_listen(_SERVER_PORT_);
}

// 保存数据
int radar_server::tcp_server_save_data(const char * filename) {
	if (!DATA_IS_READY) {
		printf("TCP Server: 数据还没有准备完毕\n");
		return _SOCKET_DATA_IS_NOT_READY_;
	}

	if (DATA_LENGTH <= 0) {
		printf("TCP Server: 数据长度非法\n");
		return _SOCKET_DATA_LENGTH_INVALID_;
	}

	FILE * fp = fopen(filename, "wb");

	unsigned char * data = (unsigned char *)malloc(sizeof(unsigned char) * WRITE_LENGTH);
	unsigned int data_offset = 0;

	LIST_ * h = ServerHeader;
	while (h != NULL) {
		memcpy(data + data_offset,
			h->data + _PACKAGE_NUMBER_SEQ_LEN_,
			sizeof(unsigned char) * (h->data_size - _PACKAGE_NUMBER_SEQ_LEN_)
		);
		data_offset += h->data_size - _PACKAGE_NUMBER_SEQ_LEN_;
		h = h->next;
	}
	fwrite(data, sizeof(unsigned char),
		DATA_LENGTH, fp);
	fclose(fp);

	list_delete(ServerHeader);
	ServerHeader = NULL;
	FOOTERHEADER = NULL;

	free(data);
	DATA_IS_READY = 0;
	DATA_LENGTH = 0;
	WRITE_LENGTH = 0;
	RECV_DATA_LENGTH = 0;
	return _NORMAL_;
}

// 检查数据是不是重复了
bool radar_server::check_is_new_data(UINT32 expect_num, UINT32 current_num) {
	if (expect_num == current_num) return 1;
	else return 0;
}

// 写入数据到本地
void radar_server::write_data(QByteArray data) {
	if (!server_is_init) return;
	p_socket->write(data);
}

// 检查有无新连接
void radar_server::new_connection_subfunc() {
	if (p_socket == NULL) return;
	if (server_is_init) return; // 如果连接上了 不允许其他连接进来
	p_socket = p_server->nextPendingConnection();
	connect(p_socket, SIGNAL(readyRead()), this, SLOT(recv_data()));
	connect(p_socket, SIGNAL(disconnected()), this, SLOT(disconnection_subfunc()));
}

// 断开 子程序
void radar_server::disconnection_subfunc() {
	server_is_init = 0;
	start_listen(_SERVER_PORT_); // 尝试重新链接
}

// Qt 开始监听
void radar_server::start_listen(int port) {
	if (p_server->listen(QHostAddress::Any, port)) { // 有新的连接进来
		server_is_init = 1;
		connect(p_server, SIGNAL(newConnection()), this, SLOT(new_connection_subfunc()));
	}
}

// 解码数据
void radar_server::decrypt_data() {
	if (bytes <= 0) {
		// 没啥好解压的
		return;
	}

	if (!server_is_init) {
		printf("TCP Server: TCP分析命令：服务器尚未初始化\n");
		return;
	}

	UINT8 DATA_TYPE = buff[0];

	switch (DATA_TYPE) {
	case _DATA_TYPE_COMMAND_: {

		break;
	}
	case _DATA_TYPE_REPLY_: {
		if (buff[1] == _REPLY_DATA_IS_TRANSFERRED_) {
			DATA_IS_READY = 1;
		}
		else if (buff[1] == _REPLY_SUCCESS_) {

		}
		else if (buff[1] == _REPLY_FAILED_) {

		}
		else if (buff[1] == _REPLY_YEAP_RA9_) {

		}
		else {
			printf("TCP Server: 未知回复\n");
			return;
		}

		break;
	}
	case _DATA_TYPE_DATA_: {
		// 检验包
		UINT32 packet_index = (UINT8)buff[3] +
			(UINT8)buff[4] * 256 +
			(UINT8)buff[5] * 65536 +
			(UINT8)buff[6] * 16777216;

		if (!check_is_new_data(DATA_INDEX, packet_index)) {
			// 数据重复了
			return;
		}

		UINT16 datasize = 0;
		if (ServerHeader == NULL) {
			ServerHeader = list_init();
			UINT8 buff1 = buff[1];
			UINT8 buff2 = buff[2];
			datasize = buff1 + buff2 * 256;
			memcpy(ServerHeader->data, buff + 3, datasize);
			ServerHeader->data_size = datasize;
			FOOTERHEADER = ServerHeader;
		}
		else {
			LIST_ * node = list_init();
			UINT8 buff1 = buff[1];
			UINT8 buff2 = buff[2];
			datasize = buff1 + buff2 * 256;
			memcpy(node->data, buff + 3, datasize);
			node->data_size = datasize;
			FOOTERHEADER = list_append(FOOTERHEADER, node);
		}
		WRITE_LENGTH += datasize - _PACKAGE_NUMBER_SEQ_LEN_;
		RECV_DATA_LENGTH += datasize;
		DATA_INDEX += 1;
		break;
	}
	case _DATA_TYPE_DATA_LENGTH_: {
		DATA_INDEX = 1;
		DATA_LENGTH = 0;
		UINT8 buff1 = buff[1];
		UINT8 buff2 = buff[2];
		UINT8 buff3 = buff[3];
		UINT8 buff4 = buff[4];
		DATA_LENGTH = buff1 +
			buff2 * 256 +
			buff3 * 65536 +
			buff4 * 16777216;
		printf("TCP Server: TCP Get Length %d bytes\n", DATA_LENGTH);
		break;
	}
	default: {
		printf("TCP Server: TCP分析命令：未知命令\n");
		return;
	}
	}

	return;
}

// 准备接收数据
void radar_server::ready_read_data() {
	if (!server_is_init) return;
	std::thread recv_data_sub_thread = std::thread(&radar_server::recv_data_subfunc, this);
	recv_data_sub_thread.detach();
}

// 接受数据子线程
void radar_server::recv_data_subfunc() {
	QString key = p_socket->peerAddress().toString() + p_socket->peerPort();
	bool loop = true;
	connect(p_socket, &QTcpSocket::disconnected, this, [&] {
		loop = false;
		}); // 断开自动退出
	connect(p_socket, &QIODevice::readyRead, this, [&]() {
		buf = p_socket->readAll(); // 会给出一段描述？
		if (buf.size() == 0) {
			return;
		}
		while (loop) {
			QByteArray buf = p_socket->read(_CLIENT_MAX_BUFFER_);
			if (buf.size() == 0) {
				QThread::msleep(10);
				continue;
			}
			else {
				buff = buf.data();
				bytes = buf.size();
				decrypt_data(); // 解码数据
				buf.clear();
			}
		}
		}); // 
}

#endif

#endif