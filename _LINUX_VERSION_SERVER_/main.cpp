#include <iostream>
#include "server_windows_version.h"

int main()
{
    std::cout << "Beta Version of Win Server!\n";

	build_udp_server();
	// Server Behavior
	char key = 'a';
	while (key != 'q' || key != 'Q') {
		std::cout << "Press R to Record" << std::endl;
		std::cout << "Press T to Test Connection" << std::endl;
		std::cout << "Press C to Check Status" << std::endl;
		key = getchar();
		getchar(); // remove \n

		if (key == 'r' || key == 'R') {
			std::cout << "Start Collection" << std::endl;
			int errCode = start_collection();
			if (errCode == _SERVER_RECV_ERROR_) {
				std::cout << "Server Receive Return Command Error" << std::endl;
			}
			else if (errCode == _SERVER_SEND_ERROR_) {
				std::cout << "Server Send Return Command Error" << std::endl;
			}
			else if (errCode == _SERVER_NOT_READY_) {
				std::cout << "Server: Client Not Ready" << std::endl;
			}
		}
		else if (key == 't' || key == 'T') {
			int errCode = test_connection();
			if (errCode == _SERVER_SEND_ERROR_) {
				std::cout << "发送命令失败" << std::endl;
			}
		}
		else if (key == 'c' || key == 'C') {
			int errCode = check_connection();
			if (errCode == _SERVER_SEND_ERROR_) {
				std::cout << "发送命令失败" << std::endl;
			}
		}
	}

	return 0;
}
