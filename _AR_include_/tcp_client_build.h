#pragma once

#ifndef _TCP_CLIENT_BUILD_H_
#define _TCP_CLIENT_BUILD_H_

#include "../_AR_decode/_wave_AR_decode_.h"
#include "../_AR_include_/_AR_UI_INFO_SUBSYSTEM_.h"
#include "../_AR_define_/TCP_define.h"
#include "../_AR_define_/_AR_UI_define_.h"
#include "../_AR_define_/_net_work_define_.h"
#include "../_AR_define_/TCP_packet_define.h"
#include "../_AR_define_/TCP_compress_define.h"
#include "../_AR_define_/TCP_function_define.h"
#include "../_AR_utility_/linked_list.h"
#include <cstdlib>
#include <mutex>
#include <cmath>

/* =======================功能定義========================= */
extern unsigned int _TELE_ENABLE_;

/* ======================IP定义========================== */
extern QString IP;
extern QString Port;

/* =====================Info定义====================== */
extern std::vector <QByteArray> info; // 全局定义
extern std::vector <LEVEL> warn; // 警告程度
extern std::mutex info_mutex; // 消息互斥锁

/* =====================链表申明====================== */
LIST_ * ClientHeader = NULL; // 需要在 RF_API_DIY 中赋值

/* ====================错误码定义===================== */
#define _CLIENT_NORMAL_ 0 // 正常情况
#define _CLIENT_WSA_ERROR_ -1 // Windows库问题
#define _CLIENT_SOCKET_ERROR_ -2 // Socket相关的问题
#define _CLIENT_CONNECT_ERROR_ -3
#define _CLIENT_IS_NOT_INIT_ -4
#define _CLIENT_COMMAND_RECV_ERR_ -5
#define _CLIENT_COMMAND_CMD_IS_NOT_EXIST_ -6
#define _CLIENT_COMMAND_CMD_IS_ERR_ -7
#define _CLIENT_DATA_IS_NULL_ -8
#define _CLIENT_DATA_SEND_ERR_ -9
#define _CLIENT_CMD_SEND_ERR_ -10
#define _CLIENT_REPLY_SEND_ERR_ -11
#define _CLIENT_REPLY_SEND_DATA_LENGTH_ERR_ -12
#define _CLIENT_CONNECT_IS_CLOSED_ -13
#define _CLIENT_REPLY_SEND_ERR_ -14
#define _CLIENT_REPLY_RECV_ERR_ -15
#define _CLIENT_REPLY_RECV_NOT_CORRECT_ -16
#define _CLIENT_REPLY_UNKNOWN_STATUS_ -17
#define _CLIENT_REPLY_WAIT_TOO_LONG_ -18
#define _CLIENT_RETRY_TIMES_ 10

/* ====================命令码定义===================== */
#define _CLIENT_COMMAND_CLOSE_SOCKET_ 1

/* ===================自定义PrintFunc=================== */
#define PRINT_FUNC(d, t) printf("回传进度 %d =====> %d\n",d,t);
#define PRINT_BYTE(d) printf("传输中... 字节数：%d", d);

/* =======================界面指令交互定义============================ */
extern unsigned char Command;
extern unsigned char Reply; // UI <============> Client <===========> Server <============> UI

#if defined _WIN32
#include <winsock.h>
#pragma comment (lib,"ws2_32.lib")

SOCKET clientSocket;
SOCKADDR_IN client_in;
BOOL clientsocket_is_built = 0;
BOOL clientsocket_is_ready = 0;
BOOL client_collection_finish;
BOOL client_collection_enable;
INT8 Reply_Succ = _REPLY_NOT_INIT_;
UINT32 Use_Coding = _NO_COMPRESS_CODING_;
unsigned char command_buff[_CLIENT_MAX_BUFFER_];
unsigned char sending_buff[_CLIENT_MAX_BUFFER_];

// 回复报错码
int WINAPI reply_code(unsigned char code) {
	if (!clientsocket_is_built)
		return _CLIENT_IS_NOT_INIT_;

	memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
	sending_buff[0] = _DATA_TYPE_REPLY_;
	sending_buff[1] = code;
	int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
	if (send_result <= 0) {
		return _CLIENT_REPLY_SEND_ERR_;
	}
	else {
		return _CLIENT_NORMAL_;
	}
}

// 接收回复编码
// 沒有用了這個
int WINAPI recv_reply_code() {
	if (!clientsocket_is_built)
		return _CLIENT_IS_NOT_INIT_;
	memset(command_buff, 0, _CLIENT_MAX_BUFFER_);

	int recv_result = recv(clientSocket, (char *)command_buff, _CLIENT_MAX_BUFFER_, 0);
	if (recv_result > 0) {
		if (command_buff[0] == _DATA_TYPE_REPLY_) {
			if (command_buff[1] == _REPLY_SUCCESS_) {
				return _REPLY_SUCCESS_;
			}
			else if (command_buff[1] == _REPLY_FAILED_){
				return _REPLY_FAILED_;
			}
			else {
				// 未知回复的情况
				return _CLIENT_REPLY_UNKNOWN_STATUS_;
			}
		}
		else {
			return _CLIENT_REPLY_RECV_NOT_CORRECT_;
		}
	}
	else {
		return _CLIENT_REPLY_RECV_ERR_;
	}
}

// 实时传输数据
int WINAPI send_real_time_data() {
	extern LIST_ * ClientHeader;

	if (!clientsocket_is_built) {
		info_mutex.lock();
		info.push_back("TCP Client Error: Client is not initialized!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_IS_NOT_INIT_;
	}

	if (ClientHeader == NULL) {
		info_mutex.lock();
		info.push_back("TCP Client Error: List Header is NULL!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_DATA_IS_NULL_;
	}

	memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
	sending_buff[0] = _DATA_TYPE_DATA_;
	sending_buff[1] = ClientHeader->data_size & 0x00FF;
	sending_buff[2] = _rotr(ClientHeader->data_size, 8);
	memcpy(sending_buff + 3, ClientHeader->data, ClientHeader->data_size);
	int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
	if (send_result < 0) {
		info_mutex.lock();
		info.push_back("TCP Client Error: Data Send Failed!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_DATA_SEND_ERR_;
	}
	else if (send_result == 0) {
		info_mutex.lock();
		info.push_back("TCP Client Error: Connection is closed!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_CONNECT_IS_CLOSED_;
	}

	return _CLIENT_NORMAL_;
}

// 传输数据最后一部分长度
int WINAPI send_real_time_data_length(UINT32 data_length) {
	memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
	sending_buff[0] = _DATA_TYPE_DATA_LENGTH_;
	sending_buff[1] = data_length & 0x00FF;
	sending_buff[2] = _rotr(data_length, 8);
	sending_buff[3] = _rotr(data_length, 16);
	sending_buff[4] = _rotr(data_length, 24);
	int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
	if (send_result < 0) {
		info_mutex.lock();
		info.push_back("TCP Client: Data Length Send Failed\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_REPLY_SEND_DATA_LENGTH_ERR_;
	}
	else if (send_result == 0) {
		info_mutex.lock();
		info.push_back("TCP Client: Connection is closed!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_CONNECT_IS_CLOSED_;
	}
	return _CLIENT_NORMAL_;
}

// 传输完成标志位发布
int WINAPI send_real_time_data_complete() {
	memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
	sending_buff[0] = _DATA_TYPE_REPLY_;
	sending_buff[1] = _REPLY_DATA_IS_TRANSFERRED_;
	int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
	if (send_result < 0) {
		info_mutex.lock();
		info.push_back("TCP Client: Data Finish Flag Send Failed\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_REPLY_SEND_ERR_;
	}
	return _CLIENT_NORMAL_;
}

// 发送数据
int WINAPI send_collected_data(UINT32 data_length, UINT32 _use_compress_= _NO_COMPRESS_CODING_) {
	extern LIST_ * ClientHeader;
	extern uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_]; // byte数组
	extern uint8_t transmit_buff_byte_cnt;
	extern uint16_t transmit_buff_counter;

	if (!clientsocket_is_built) {
		info_mutex.lock();
		info.push_back("TCP Client Error: Client is not initialized\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_IS_NOT_INIT_;
	}

	if (ClientHeader == NULL) {
		info_mutex.lock();
		info.push_back("TCP Client Error: List is NULL\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_DATA_IS_NULL_;
	}

	info_mutex.lock();
	// 离线采集完毕
	info.push_back("TCP Client Info: Tranmitting...\n");
	warn.push_back(_UI_INFO_);
	info_mutex.unlock();

	Reply_Succ = _REPLY_NOT_INIT_;
	LIST_ * n = ClientHeader;
	UINT32 DATA_SENT = 0;
	while (n != NULL) {
		// 2023年1月1日
		// 加入動態霍夫曼編碼
		if (_use_compress_ == _DDHUFF_COMPRESS_CODING_) {
			transmit_buff_byte_cnt = 0; // 比特計數
			transmit_buff_counter = 0; // 字節計數
			_get_DHuff_code_(n->data, n->data_size);
			memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
			sending_buff[0] = _DATA_TYPE_DATA_;
			sending_buff[1] = transmit_buff_counter & 0x00FF;
			sending_buff[2] = _rotr(transmit_buff_counter, 8);
			sending_buff[3] = transmit_buff_byte_cnt; // 比特計數
			uint16_t total_length = transmit_buff_counter + 1; // 我估計最後一個傳輸過去應該沒事
			memcpy(sending_buff + 4, transmit_buff, total_length);
		}
		else {
			// 默認不編碼
			memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
			sending_buff[0] = _DATA_TYPE_DATA_;
			sending_buff[1] = n->data_size & 0x00FF;
			sending_buff[2] = _rotr(n->data_size, 8);
			memcpy(sending_buff + 3, n->data, n->data_size);
		}

		int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
		if (send_result < 0) {
			info_mutex.lock();
			info.push_back("TCP Client: Data Send Failed!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _CLIENT_DATA_SEND_ERR_;
		}
		else if (send_result == 0) {
			info_mutex.lock();
			info.push_back("TCP Client: Connection is closed!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _CLIENT_CONNECT_IS_CLOSED_;
		}
		// 等待回复
		UINT8 WAIT_TIME = 0;
		while (Reply_Succ == _REPLY_NOT_INIT_) {
			Sleep(10); 
			if (WAIT_TIME > _CLIENT_RETRY_TIMES_) {
				info_mutex.lock();
				info.push_back("TCP Client: Connection is closed!\n");
				warn.push_back(_UI_ERR_);
				info_mutex.unlock();
				return _CLIENT_REPLY_WAIT_TOO_LONG_; // 回复失败
			}
			WAIT_TIME++;
		}
		if (Reply_Succ == _REPLY_FAILED_) {
			continue;
		}
		Reply_Succ = _REPLY_NOT_INIT_;

		DATA_SENT += n->data_size;
		// PRINT_FUNC(DATA_SENT, data_length);
		n = n->next;
	}

	memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
	sending_buff[0] = _DATA_TYPE_DATA_LENGTH_;
	sending_buff[1] = DATA_SENT & 0x00FF;
	sending_buff[2] = _rotr(DATA_SENT, 8);
	sending_buff[3] = _rotr(DATA_SENT, 16);
	sending_buff[4] = _rotr(DATA_SENT, 24);
	int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
	if (send_result < 0) {
		info_mutex.lock();
		info.push_back("TCP Client: Data Length Send Failed!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_REPLY_SEND_DATA_LENGTH_ERR_;
	}
	else if (send_result == 0) {
		info_mutex.lock();
		info.push_back("TCP Client: Connection is closed!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_CONNECT_IS_CLOSED_;
	}

	memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
	sending_buff[0] = _DATA_TYPE_REPLY_;
	sending_buff[1] = _REPLY_DATA_IS_TRANSFERRED_;
	send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
	if (send_result < 0) {
		info_mutex.lock();
		info.push_back("TCP Client: Data Send Failed!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_REPLY_SEND_ERR_;
	}
	else if (send_result == 0) {
		info_mutex.lock();
		info.push_back("TCP Client: Connection is closed!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_CONNECT_IS_CLOSED_;
	}

	QByteArray temp("TCP Client: Data Send Success! Total Bytes: ");
	temp.append(QString::fromStdString(std::to_string(DATA_SENT)));
	temp.append('\n');
	info_mutex.lock();
	info.push_back(temp);
	warn.push_back(_UI_INFO_);
	info_mutex.unlock();
	return _CLIENT_NORMAL_;
}

// 分析命令
int command_analysis_client() {
	extern unsigned char command_buff[_CLIENT_MAX_BUFFER_];
	extern BOOL clientsocket_is_ready;

	if (!clientsocket_is_built) {
		info_mutex.lock();
		info.push_back("TCP Client Error: Client is not initialized!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_IS_NOT_INIT_;
	}

	if (!clientsocket_is_ready) {
		info_mutex.lock();
		info.push_back("TCP Client Error: TCP Command Analysis not Ready!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_COMMAND_CMD_IS_ERR_;
	}

	unsigned char DATA_TYPE = (unsigned char)command_buff[0];
	switch (DATA_TYPE) {
	case _DATA_TYPE_COMMAND_: {
		// 接受命令
		unsigned char DATA_CMD = (unsigned char)command_buff[1]; // 接受命令

		// 如果是录制命令
		if (DATA_CMD == _CMD_RECORD_) {
			info_mutex.lock();
			info.push_back("TCP Client: Start Recording!\n");
			warn.push_back(_UI_INFO_);
			info_mutex.unlock();
			_TELE_ENABLE_ = _TELE_IS_ENABLED_;

			Command = _RECORD_COMMAND_;
		}
		else if (DATA_CMD == _CMD_RECORD_DONOT_TRANSFER_) {
			info_mutex.lock();
			info.push_back("TCP Client: Start Recording with no telemetry!\n");
			warn.push_back(_UI_INFO_);
			info_mutex.unlock();
			_TELE_ENABLE_ = _TELE_IS_DISABLED_;

			Command = _RECORD_COMMAND_;
		}
		else if (DATA_CMD == _CMD_ARE_U_ALIVE_) {
			// 詢問客戶端是否還在綫
			memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
			sending_buff[0] = _DATA_TYPE_REPLY_;
			sending_buff[1] = _REPLY_YEAP_RA9_;
			int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
		}
		else if (DATA_CMD == _CMD_CLOSE_CLIENT_) {
			memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
			sending_buff[0] = _DATA_TYPE_REPLY_;
			sending_buff[1] = _REPLY_SUCCESS_;
			int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
			Sleep(100); // 等待100ms后自動關閉
			exit(0);
		}
		else {
			info_mutex.lock();
			info.push_back("TCP Client Error: Unknown Command!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
			sending_buff[0] = _DATA_TYPE_REPLY_;
			sending_buff[1] = _REPLY_FAILED_;
			int send_result = send(clientSocket, (char *)sending_buff, _CLIENT_MAX_BUFFER_, 0);
		}

		break;
	}
	case _DATA_TYPE_REPLY_: {
		Reply_Succ = command_buff[1];
		break;
	}
	case _DATA_TYPE_DATA_: {

		break;
	}
	case _DATA_TYPE_CODING_: {
		Use_Coding = command_buff[1];
		break;
	}
	default: {
		info_mutex.lock();
		info.push_back("TCP Client Error: Unknown Command is Received\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_COMMAND_CMD_IS_NOT_EXIST_;
	}
	}
	clientsocket_is_ready = 0;
	return _CLIENT_NORMAL_;
}

// 接受命令
int WINAPI recv_command_client() {
	extern BOOL clientsocket_is_ready; // 接收到数据了
	extern BOOL clientsocket_is_built; // 服务器是否建立好了
	clientsocket_is_ready = 0;
	while (1) {
		if (!clientsocket_is_built) {
			info_mutex.lock();
			info.push_back("TCP Client Error: Client is not initialized!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return _CLIENT_IS_NOT_INIT_;
		}
		memset(command_buff, 0, _CLIENT_MAX_BUFFER_);
		int bytes = recv(clientSocket, (char *)command_buff, _CLIENT_MAX_BUFFER_, 0);
		if (bytes < 0) {
			info_mutex.lock();
			info.push_back("TCP Client Error: Client Received minus Bytes Check Connection\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			clientsocket_is_built = 0;
			return _CLIENT_COMMAND_RECV_ERR_;
		}
		else if (bytes == 0) {
			info_mutex.lock();
			info.push_back("TCP Client Error: Client Connection is Closed\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			clientsocket_is_built = 0;
			return _CLIENT_CONNECT_IS_CLOSED_;
		}
		clientsocket_is_ready = 1;
		int _cmd_ret_ = command_analysis_client();
		if (_cmd_ret_) {
			// 命令出错的情况
			info_mutex.lock();
			info.push_back("TCP Client Warning: Command is Error!\n");
			warn.push_back(_UI_WARN_);
			info_mutex.unlock();
			continue;
		}
	}
	return _CLIENT_NORMAL_;
}

int build_radar_data_transfer_client() {
	if (clientsocket_is_built) {
		closesocket(clientSocket); // 先关闭连接
	}
	WSADATA wsadata;
	if (0 == WSAStartup(MAKEWORD(2, 2), &wsadata))
	{

	}
	else
	{
		info_mutex.lock();
		info.push_back("TCP Radar Client: WSA open failed!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_WSA_ERROR_;
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (clientSocket == INVALID_SOCKET) {
		info_mutex.lock();
		info.push_back("TCP Radar Client: Invalid Socket!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_SOCKET_ERROR_;
	}

	client_in.sin_addr.S_un.S_addr = inet_addr(IP.toStdString().data()); //将网络地址字符串转换成二进制形式
	//client_in.sin_addr.S_un.S_addr = inet_addr(_CON_SERVER_IP_);
	client_in.sin_family = AF_INET;
	// client_in.sin_port = htons(_CON_SERVER_PORT_);
	client_in.sin_port = htons(Port.toInt());

	if (connect(clientSocket, (SOCKADDR*)&client_in, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		info_mutex.lock();
		info.push_back("TCP Radar Client: Connection Error!\n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return _CLIENT_CONNECT_ERROR_;
	}

	char describe[_DESCRIBE_LEN_];
	memcpy(describe, "Radar Collection Client\0", strlen("Radar Collection Client\0"));
	send(clientSocket, describe, _DESCRIBE_LEN_, 0);

	clientsocket_is_built = 1;

	return _CLIENT_NORMAL_;
}

// 还原Client
void WINAPI ResetClient() {
	if (clientsocket_is_built)
		closesocket(clientSocket);
	while (!build_radar_data_transfer_client()) {
		info_mutex.lock();
		info.push_back("TCP Client: Reset Client Failure, reconnection at 1 second later\n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
		Sleep(1000);
	}
	clientsocket_is_ready = 0;
	memset(command_buff, 0, _CLIENT_MAX_BUFFER_);
	memset(sending_buff, 0, _CLIENT_MAX_BUFFER_);
	if (ClientHeader != NULL) {
		list_delete(ClientHeader);
		ClientHeader = NULL;
	}
}

#else
#include <thread>
#include <QTimer>
#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QDebug>
#include <QHostAddress>

class radar_client : public QObject {
	Q_OBJECT
public:
	explicit radar_client(QObject * parent = nullptr);
	void start_connect(QString ip, int port); // 开始连接
	void analysis_command(); // 分析命令
	int write_data(); // 发送数据
	int send_realtime_data(); // 发送实时数据
	int send_offine_data(); // 采集完毕后发送数据
	int send_collection_complete(); // 发送采集完毕标志
	int send_collection_length(unsigned int); // 发送采集数据长度标志
	
public:
	unsigned char send_data[_CLIENT_MAX_BUFFER_]; // 要发送的数据
	QByteArray send_buff; // 数据 适应QByteArray
	unsigned char * recv_data; // 要接受的数据
	int recv_data_length = 0; // 接收数据 长度
	int send_data_length = 0; // 发送数据 长度
	QByteArray recv_buff; // 数据 适应 Qt
	QString ip = _CON_SERVER_IP_;
	int port = _CON_SERVER_PORT_;

private:
	QTcpSocket * client = NULL;
	bool status = 0; // 链接状态

public slots:
	void read_data();
	
};

// 发送实时数据
int radar_client::send_realtime_data() {
	extern LIST_ * ClientHeader;

	if (client == NULL) {
		printf("TCP Client Error: Client is not initialized!\n");
		return _CLIENT_IS_NOT_INIT_;
	}

	if (ClientHeader == NULL) {
		printf("TCP Client Error: List is NULL!\n");
		return _CLIENT_DATA_IS_NULL_;
	}

	memset(send_data, 0, _CLIENT_MAX_BUFFER_);
	send_data[0] = _DATA_TYPE_DATA_;
	send_data[1] = ClientHeader->data_size & 0x00FF;
	send_data[2] = _rotr(ClientHeader->data_size, 8);
	memcpy(send_data + 3, ClientHeader->data, ClientHeader->data_size);

	return write_data();
}

// 采集完毕后发送数据
int radar_client::send_offine_data() {
	extern LIST_ * ClientHeader;

	if (!client == NULL) {
		printf("TCP Client Error: Client is not initialized!\n");
		return _CLIENT_IS_NOT_INIT_;
	}

	if (ClientHeader == NULL) {
		printf("TCP Client Error: List is NULL!\n");
		return _CLIENT_DATA_IS_NULL_;
	}

	// 离线采集完毕
	printf("TCP Client Info: 离线采集完毕，正在回传...\n");

	LIST_ * n = ClientHeader;
	unsigned int DATA_SENT = 0;
	while (n != NULL) {
		memset(send_data, 0, _CLIENT_MAX_BUFFER_);
		send_data[0] = _DATA_TYPE_DATA_;
		send_data[1] = n->data_size & 0x00FF;
		send_data[2] = _rotr(n->data_size, 8);
		memcpy(send_data + 3, n->data, n->data_size);
		int send_result = write_data();
		if (send_result != _CLIENT_NORMAL_) {
			printf("TCP Client: Data Send Failed!\n");
			return send_result;
		}
		DATA_SENT += n->data_size;
		n = n->next;
	}

	memset(send_data, 0, _CLIENT_MAX_BUFFER_);
	send_data[0] = _DATA_TYPE_DATA_LENGTH_;
	send_data[1] = DATA_SENT & 0x00FF;
	send_data[2] = _rotr(DATA_SENT, 8);
	send_data[3] = _rotr(DATA_SENT, 16);
	send_data[4] = _rotr(DATA_SENT, 24);
	int send_result = write_data();
	if (send_result != _CLIENT_NORMAL_) {
		printf("TCP Client: Data Length Send Failed!");
		return send_result;
	}

	memset(send_data, 0, _CLIENT_MAX_BUFFER_);
	send_data[0] = _DATA_TYPE_REPLY_;
	send_data[1] = _REPLY_DATA_IS_TRANSFERRED_;
	send_result = write_data();
	if (send_result != _CLIENT_NORMAL_) {
		printf("TCP Client: Data Length Send Failed!");
		return send_result;
	}

	printf("TCP Client: Data Send Success! Total Bytes: %d\n", DATA_SENT);
	return _CLIENT_NORMAL_;
}

// 发送采集完毕标志
int radar_client::send_collection_complete() {
	memset(send_data, 0, _CLIENT_MAX_BUFFER_);
	send_data[0] = _DATA_TYPE_REPLY_;
	send_data[1] = _REPLY_DATA_IS_TRANSFERRED_;

	return write_data();
}

// 发送采集数据长度标志
int radar_client::send_collection_length(unsigned int data_length) {
	memset(send_data, 0, _CLIENT_MAX_BUFFER_);
	send_data[0] = _DATA_TYPE_DATA_LENGTH_;
	send_data[1] = data_length & 0x00FF;
	send_data[2] = _rotr(data_length, 8);
	send_data[3] = _rotr(data_length, 16);
	send_data[4] = _rotr(data_length, 24);

	return write_data();
}

void radar_client::analysis_command() {
	char DATA_TYPE = recv_data[0];
	switch (DATA_TYPE) {
	case _DATA_TYPE_COMMAND_: {

		break;
	}
	case _DATA_TYPE_REPLY_: {

		break;
	}
	case _DATA_TYPE_DATA_: {

		break;
	}
	default: {
		printf("TCP Client Error: 接收到了不存在的命令\n");
		return;
	}
	}
	return;
}

// 开始连接
void radar_client::start_connect(QString ip, int port) {
	if (client == NULL) {
		return; // client 没有初始化
	}

	while (status == 0) {
		// 重复尝试连接
		client->connectToHost(QHostAddress(ip), port);
		if (client->waitForConnected(3000)) {
			status = 1; // 连接上了
		}
		else {
			status = 0; // 连接失败
		}
	}
}

radar_client::radar_client(QObject * parent) :
	QObject(parent) {
	client = new QTcpSocket();

	connect(client, &QTcpSocket::disconnected, this, [=]()
	{
		status = 0;
		client->close();
		// 尝试重新连接
		std::thread recon_thread = std::thread(&radar_client::start_connect,
			this,
			ip,
			port);
		recon_thread.detach();
	});
	// 关联数据接受
	connect(client, &QTcpSocket::readyRead, this, &radar_client::read_data);
}

int radar_client::write_data() {
	if (client == NULL) return _CLIENT_IS_NOT_INIT_; // 客户端没有初始化 
	if (status == 0) return _CLIENT_CONNECT_ERROR_; // 没有连接

	client->write((char *)send_data, _CLIENT_MAX_BUFFER_);
	return _CLIENT_NORMAL_;
}

void radar_client::read_data() {
	if (client == NULL) return; // 客户端没有初始化 
	if (status == 0) return; // 没有连接

	recv_buff = client->read(_CLIENT_MAX_BUFFER_);
	if (recv_buff.size() <= 0) {
		return; // 连接可能存在问题
	}

	recv_data = (unsigned char *)recv_buff.data();
	recv_data_length = recv_buff.size();
	// 读取数据后 对数据进行分析
	analysis_command();
}

#endif

#endif