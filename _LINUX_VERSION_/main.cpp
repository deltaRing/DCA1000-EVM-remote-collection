#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#ifndef _WIN32
    #include "include/udp_server.h"
    #include "include/udp_client.h"
#endif

extern bool port_ready;
extern bool addr_ready;
extern bool data_port_ready;

#define _SERVER_ 0
#define _CLIENT_ 1
#define _MANUAL_ 2

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: input parameter 3: server (0) or client (1), Serial Port like \"dev/tty0\", CFGfile path" << std::endl;
        return -1;
    }
    int character = atoi(argv[1]);
    if (character == _SERVER_){
        // 服务器不需要剩下的参数
    }else if(character == _MANUAL_){
        if (argc != 4){
            std::cout << "Usage: input parameter 3: server or client, Serial Port like \"dev/tty0\", CFGfile path" << std::endl;
            return 1;
        }
    }else if (character == _CLIENT_){
        if (argc != 4){
            std::cout << "Usage: input parameter 3: server or client, Serial Port like \"dev/tty0\", CFGfile path" << std::endl;
            return 1;
        }
    } // Parameter Check
    
    // These are Error Circumstance
    
    if (character == _CLIENT_){
#ifdef _WIN32
      std::cout << "Not Supposed" << std::endl;
      return 0;
#else 
        // Client Behavior
        openClient(); // initial dca1000 and radar com
        ComInit(std::string(argv[2])); // Init Com and UDP 
        
        //test_udp_client();
        
        extern unsigned char cli_buff[_PACKAGE_SIZE_];
        
        while (true){
            if (!data_port_ready || !port_ready || !addr_ready){
                std::cout << "Reconnection..." << std::endl;
                ComInit(std::string(argv[2])); // Init Com and UDP 
            }
            
            std::cout << "Receiving Command" << std::endl;
            bool valid = recvCmd();
            
            if (!valid) {
                std::cout << "Invalid Command!" << std::endl;
                continue;
            }
            
            if (cli_buff[1] == _SEND_START_RECORD_){
                auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                //转为字符串
                std::stringstream ss;
                // 可以分别以不同的形式进行显示
                ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S");
                std::string str = ss.str();
                str += std::string(".bin");
                startRecord(std::string(argv[3]), str);
            }
            else if (cli_buff[1] == _SERVER_TEST_CON_){
                // connection test
                test_udp_client();
            }
            else if (cli_buff[1] == _SERVER_CHECK_STATUS_){
                checkStatus();
            }
            else{
                // unknown command
                cli_buff[0] = _SEND_HEADER_;
                cli_buff[1] = _CLIENT_REPLY_UNKNOWN_;
                replyCmd(); // Reply Client has sampled data
            }
        }
        
        return 0;
#endif   
    }else if (character == _MANUAL_){
        ComInit(std::string(argv[2])); // Init Com and UDP 
        
        char key = 'a';
        while (key != 'q' || key != 'Q'){
            std::cout << "Press R to Record" << std::endl;
            key = getchar();
            getchar(); // remove \n
            
            if (key == 'r' || key == 'R'){
                std::cout << "Start Collection" << std::endl;
                 auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                //转为字符串
                std::stringstream ss;
                // 可以分别以不同的形式进行显示
                ss << std::put_time(std::localtime(&t), "%Y-%m-%d-%H-%M-%S");
                std::string str = ss.str();
                str += std::string(".bin");
                startRecord_local(std::string(argv[3]), str);
            }
        }
    }else{
        build_udp_server();
        // Server Behavior
        char key = 'a';
        while (key != 'q' || key != 'Q'){
            std::cout << "Press R to Record" << std::endl;
            key = getchar();
            getchar(); // remove \n
            
            if (key == 'r' || key == 'R'){
                std::cout << "Start Collection" << std::endl;
                int errCode = start_collection();
                if (errCode == _SERVER_RECV_ERROR_){
                    std::cout << "Server Receive Return Command Error" << std::endl;
                }else if (errCode == _SERVER_SEND_ERROR_){
                    std::cout << "Server Send Return Command Error" << std::endl;
                }else if (errCode == _SERVER_NOT_READY_){
                    std::cout << "Server: Client Not Ready" << std::endl;
                }else{
                    std::cout << "Server: Collection Request Send Complete!" << std::endl;
                }
            }
        }
        
        return 0;
    }
    
    return 0;
}
