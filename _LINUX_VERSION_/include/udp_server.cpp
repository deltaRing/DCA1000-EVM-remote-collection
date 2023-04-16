#include "udp_server.h"

int serverSocket;
struct sockaddr_in server;
struct sockaddr_in client;
unsigned char ser_buff[_PACKAGE_SIZE_];
bool serverCanStartSample = true;

void build_udp_server() // 建立UDP服务端
{
    serverCanStartSample = true;
	//建立一个数据报类型的UDP套接字  ******************//
    serverSocket = socket(PF_INET, SOCK_DGRAM, 0); //配置模式，
	//设置服务器地址addrSrv和监听端口
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(_SERVER_IP_); //设置服务器主机ip地址（与接收方客户端的IP对应）
	server.sin_port = htons(_SERVER_PORT_);					 //发送用的端口，可以根据需要更改
    client.sin_family = AF_INET;
	client.sin_addr.s_addr = inet_addr(_CLIENT_IP_); //设置接收ip地址
	client.sin_port = htons(_CLIENT_PORT_);					 //接收用的端口
	if (bind(serverSocket, (sockaddr *)&server, sizeof(sockaddr_in)) < 0){
        std::cout << "Build UDP Server: Server Build failure" << std::endl;
        return;
    }
}

void close_udp_server() // 关闭UDP服务器端
{
    close(serverSocket);
}

void wait_sample_complete(){
    while (true){
        recv_cmd();
        if (check_cmd(_CLIENT_REPLY_RECORD_OVER_)) {
            break;
        }
    }
    std::cout << "Client is Ready Now" << std::endl;
    serverCanStartSample = true;
}

int start_collection() // 开始采集
{
    if (!serverCanStartSample) 
        return _SERVER_NOT_READY_;
    serverCanStartSample = false;
    
    memset(ser_buff, 0, _PACKAGE_SIZE_);
    ser_buff[0] = _SEND_HEADER_;
    ser_buff[1] = _SEND_START_RECORD_;
    
    bool send_result = send_cmd();
    recv_cmd();
    if (send_result){
        if (!check_cmd(_CLIENT_REPLY_OK_)) { // if reply is not OK 
            return _SERVER_RECV_ERROR_;
        }
    }else{
        return _SERVER_SEND_ERROR_;
    }
    
    std::thread _wait_thread_ = std::thread(&wait_sample_complete);
    _wait_thread_.detach(); // wait udp client send back 
    
    return _SERVER_SEND_OK_;
}

// 输入：Client客户端 clientSocket客户端 
bool send_cmd(){
    int t = sendto(serverSocket, ser_buff,
                   _PACKAGE_SIZE_, 0,
                   (sockaddr*)&client, sizeof(client));
    memset(ser_buff, 0, _PACKAGE_SIZE_);
	if (t < 0) 
        return false;
    return true;
}


bool check_cmd(unsigned char cmd_return){
    if (ser_buff[0] == _SEND_HEADER_) return false;
    if (ser_buff[1] == cmd_return) return true;
    return false;
}

bool recv_cmd(){
    socklen_t len = sizeof(sockaddr);
    memset(ser_buff, 0, _PACKAGE_SIZE_);
    int t = recvfrom(serverSocket, ser_buff,
             _PACKAGE_SIZE_, 0,
             (sockaddr *)&server,  &len);
    if (t < 0) 
        return false;
    return true;
}

void print_cmd(const char * info){
    std::cout << info << std::endl;
}
