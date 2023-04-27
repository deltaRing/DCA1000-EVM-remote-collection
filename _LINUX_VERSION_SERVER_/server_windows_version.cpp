#include "server_windows_version.h"

int serverSocket;
SOCKADDR_IN server;
SOCKADDR_IN client;
unsigned char ser_buff[_PACKAGE_SIZE_];
unsigned char ser_recv[_PACKAGE_SIZE_];
bool serverCanStartSample = true;
int _reply_ = 0;

void recv_queue() {
	while (true) {
		recv_cmd();
		if (ser_recv[0] != _SEND_HEADER_) {
			std::cout << "Anomaly Data Received" << std::endl;
			continue; // anomaly buff
		}

		if (ser_recv[1] == _CLIENT_REPLY_STATUS_REPORT_) {
			unsigned char COM = ser_recv[2];
			unsigned char UDP = ser_recv[3];
			unsigned char ADR = ser_recv[4];
			if (UDP == _CLIENT_REPLY_UDP_CON_SUC_) {
				std::cout << "DCA1000 连接成功 ";
			}
			else {
				std::cout << "DCA1000 连接失败 ";
			}
			if (COM == _CLIENT_REPLY_COM_CON_SUC_) {
				std::cout << "雷达天线板 连接成功 ";
			}
			else {
				std::cout << "雷达天线板 连接失败 ";
			}

			if (ADR == _CLIENT_REPLY_UDP_ADR_SUC_) {
				std::cout << "DCA1000 配置端口 连接成功" << std::endl;
			}
			else {
				std::cout << "DCA1000 配置端口 连接失败" << std::endl;
			}
		}
		else if (ser_recv[1] == _CLIENT_REPLY_TEST_CON_) {
			std::cout << "链接成功" << std::endl;
		}
		else if (ser_recv[1] == _CLIENT_REPLY_OK_) {
			std::cout << "开始采集" << std::endl;
		}
		else if (ser_recv[1] == _CLIENT_REPLY_RECORD_OVER_) {
			std::cout << "采集完成" << std::endl;
		}
	}
}

void build_udp_server() // 建立UDP服务端
{
	WSADATA wsadata;//wsa 即windows socket async 异步套接字
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		printf("套接字未打开\n");
		WSACleanup();
		exit(1);
	}
	else
	{
		printf("已打开套接字\n");
	}
	serverCanStartSample = true;
	//建立一个数据报类型的UDP套接字  ******************//
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //配置模式，
	//设置服务器地址addrSrv和监听端口
	server.sin_addr.S_un.S_addr = inet_addr(_SERVER_IP_);
	server.sin_family = AF_INET;
	server.sin_port = htons(_SERVER_PORT_);
	client.sin_addr.S_un.S_addr = inet_addr(_CLIENT_IP_);
	client.sin_family = AF_INET;
	client.sin_port = htons(_CLIENT_PORT_);

	if (bind(serverSocket, (LPSOCKADDR)&server, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		std::cout << "Build UDP Server: Server Build failure" << std::endl;
		std::cout << GetLastError() << std::endl;
		exit(1);
	}
	else {
		std::cout << "Build UDP Server: Server Build success" << std::endl;
	}
}

void close_udp_server() // 关闭UDP服务器端
{
	closesocket(serverSocket);
}

int check_connection() // 检测连接
{
	memset(ser_buff, 0, _PACKAGE_SIZE_);
	ser_buff[0] = _SEND_HEADER_;
	ser_buff[1] = _SERVER_CHECK_STATUS_;

	bool send_result = send_cmd();
	
	return _SERVER_SEND_OK_;
}

int test_connection() // 测试连接
{
	memset(ser_buff, 0, _PACKAGE_SIZE_);
	ser_buff[0] = _SEND_HEADER_;
	ser_buff[1] = _SERVER_TEST_CON_;

	bool send_result = send_cmd();

	return _SERVER_SEND_OK_;
}

int start_collection() // 开始采集
{
/*	if (!serverCanStartSample)
		return _SERVER_NOT_READY_;
	serverCanStartSample = false;*/

	memset(ser_buff, 0, _PACKAGE_SIZE_);
	ser_buff[0] = _SEND_HEADER_;
	ser_buff[1] = _SEND_START_RECORD_;

	bool send_result = send_cmd();

	return _SERVER_SEND_OK_;
}

// 输入：Client客户端 clientSocket客户端 
bool send_cmd() {
	int t = sendto(serverSocket, (char *)ser_buff,
		_PACKAGE_SIZE_, 0,
		(SOCKADDR *)&client, sizeof(client));
	memset(ser_buff, 0, _PACKAGE_SIZE_);
	if (t < 0)
		return false;
	return true;
}

bool check_cmd(unsigned char cmd_return) {
	if (ser_recv[0] != _SEND_HEADER_) return false;
	if (ser_recv[1] == cmd_return) return true;
	return false;
}

bool recv_cmd() {
	int len = sizeof(SOCKADDR_IN);
	memset(ser_recv, 0, _PACKAGE_SIZE_);
	int t = recvfrom(serverSocket, (char *)ser_recv,
		_PACKAGE_SIZE_, 0,
		(SOCKADDR *)&server, &len);
	if (t < 0)
		return false;
	return true;
}

void print_cmd(const char * info) {
	std::cout << info << std::endl;
}

