#include "server_windows_version.h"

int serverSocket;
SOCKADDR_IN server;
SOCKADDR_IN client;
unsigned char ser_buff[_PACKAGE_SIZE_];
bool serverCanStartSample = true;

void build_udp_server() // ����UDP�����
{
	WSADATA wsadata;//wsa ��windows socket async �첽�׽���
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		printf("�׽���δ��\n");
		WSACleanup();
		exit(1);
	}
	else
	{
		printf("�Ѵ��׽���\n");
	}
	serverCanStartSample = true;
	//����һ�����ݱ����͵�UDP�׽���  ******************//
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //����ģʽ��
	//���÷�������ַaddrSrv�ͼ����˿�
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

void close_udp_server() // �ر�UDP��������
{
	closesocket(serverSocket);
}

void wait_sample_complete() {
	while (true) {
		recv_cmd();
		if (check_cmd(_CLIENT_REPLY_RECORD_OVER_)) {
			std::cout << "Client is Ready Now" << std::endl;
			break;
		}
		if (check_cmd(_CLIENT_REPLY_COM_ERR_)) {
			std::cout << "Client: Com Write Failure Need Re-capture" << std::endl;
			break;
		}
	}
	// serverCanStartSample = true;
}

int check_connection() // �������
{
	memset(ser_buff, 0, _PACKAGE_SIZE_);
	ser_buff[0] = _SEND_HEADER_;
	ser_buff[1] = _SERVER_CHECK_STATUS_;

	bool send_result = send_cmd();
	recv_cmd();

	if (send_result) {
		if (!check_cmd(_CLIENT_REPLY_STATUS_REPORT_)) {
			std::cout << "�ظ�ʧ�ܻ��߶�����" << std::endl;
			return _SERVER_RECV_ERROR_;
		}
		else
		{
			unsigned char COM = ser_buff[2];
			unsigned char UDP = ser_buff[3];
			unsigned char ADR = ser_buff[4];
			if (UDP == _CLIENT_REPLY_UDP_CON_SUC_) {
				std::cout << "DCA1000 ���ӳɹ� ";
			}
			else {
				std::cout << "DCA1000 ����ʧ�� ";
			}

			if (COM == _CLIENT_REPLY_COM_CON_SUC_) {
				std::cout << "�״����߰� ���ӳɹ� ";
			}
			else {
				std::cout << "�״����߰� ����ʧ�� ";
			}

			if (ADR == _CLIENT_REPLY_UDP_ADR_SUC_) {
				std::cout << "DCA1000 ���ö˿� ���ӳɹ�" << std::endl;
			}
			else {
				std::cout << "DCA1000 ���ö˿� ����ʧ��" << std::endl;
			}
		}
	}
	else {
		return _SERVER_SEND_ERROR_;
	}

	return _SERVER_SEND_OK_;
}

int test_connection() // ��������
{
	memset(ser_buff, 0, _PACKAGE_SIZE_);
	ser_buff[0] = _SEND_HEADER_;
	ser_buff[1] = _SERVER_TEST_CON_;

	bool send_result = send_cmd();
	recv_cmd();

	if (send_result) {
		if (!check_cmd(_CLIENT_REPLY_TEST_CON_)) {
			std::cout << "����ʧ��" << std::endl;
			return _SERVER_RECV_ERROR_;
		}
		else {
			std::cout << "���ӳɹ�" << std::endl;
		}
	}
	else {
		return _SERVER_SEND_ERROR_;
	}

	return _SERVER_SEND_OK_;
}

int start_collection() // ��ʼ�ɼ�
{
/*	if (!serverCanStartSample)
		return _SERVER_NOT_READY_;
	serverCanStartSample = false;*/

	memset(ser_buff, 0, _PACKAGE_SIZE_);
	ser_buff[0] = _SEND_HEADER_;
	ser_buff[1] = _SEND_START_RECORD_;

	bool send_result = send_cmd();
	recv_cmd();
	if (send_result) {
		if (!check_cmd(_CLIENT_REPLY_OK_)) {
			// serverCanStartSample = true;
			return _SERVER_RECV_ERROR_;
		}
		else
		{
			std::cout << "��ʼ�ɼ�" << std::endl;
		}
	}
	else {
		return _SERVER_SEND_ERROR_;
	}

	recv_cmd();
	if (check_cmd(_CLIENT_REPLY_RECORD_OVER_)) {
		std::cout << "�ɼ����" << std::endl;
	}
	else {
		std::cout << "�ɼ�ʧ��" << std::endl;
		return _SERVER_RECV_ERROR_;
	}

	return _SERVER_SEND_OK_;
}

// ���룺Client�ͻ��� clientSocket�ͻ��� 
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
	if (ser_buff[0] != _SEND_HEADER_) return false;
	if (ser_buff[1] == cmd_return) return true;
	return false;
}

bool recv_cmd() {
	int len = sizeof(SOCKADDR_IN);
	memset(ser_buff, 0, _PACKAGE_SIZE_);
	int t = recvfrom(serverSocket, (char *)ser_buff,
		_PACKAGE_SIZE_, 0,
		(SOCKADDR *)&server, &len);
	if (t < 0)
		return false;
	return true;
}

void print_cmd(const char * info) {
	std::cout << info << std::endl;
}
