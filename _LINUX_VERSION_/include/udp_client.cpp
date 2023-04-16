#include "udp_client.h"

int clientSocket;
struct sockaddr_in server_;
struct sockaddr_in client_;
unsigned char cli_buff[_PACKAGE_SIZE_];

bool port_ready = false;
bool addr_ready = false;
bool data_port_ready = false;

void checkStatus()
{
    cli_buff[0] = _SEND_HEADER_;
    cli_buff[1] = _CLIENT_REPLY_STATUS_REPORT_;
    cli_buff[2] = _CLIENT_REPLY_COM_CON_ERR_;
    cli_buff[3] = _CLIENT_REPLY_UDP_CON_ERR_;
    cli_buff[4] = _CLIENT_REPLY_UDP_ADR_ERR_;
    
    if (port_ready) cli_buff[2] = _CLIENT_REPLY_COM_CON_SUC_;
    if (data_port_ready) cli_buff[3] = _CLIENT_REPLY_UDP_CON_SUC_;
    if (addr_ready) cli_buff[4] = _CLIENT_REPLY_UDP_ADR_SUC_;
    
    replyCmd();
}


void openClient() //
{
    //建立一个数据报类型的UDP套接字  ******************//
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0); //配置模式，
	//设置服务器地址addrSrv和监听端口
	server_.sin_family = AF_INET;
	server_.sin_addr.s_addr = inet_addr(_SERVER_IP_); //设置服务器主机ip地址（与接收方客户端的IP对应）
	server_.sin_port = htons(_SERVER_PORT_);					 //发送用的端口，可以根据需要更改
    client_.sin_family = AF_INET;
	client_.sin_addr.s_addr = inet_addr(_CLIENT_IP_); //设置接收ip地址
	client_.sin_port = htons(_CLIENT_PORT_);					 //接收用的端口
    while (true) {
        // connect UDP port
        int ret = bind(clientSocket, (sockaddr *)&client_, sizeof(client_));
        if (ret == -1){
            std::cout << "Build UDP Client: Client Build failure" << std::endl;
        }else{
            std::cout << "Build UDP Client: Client Build Success" << std::endl;
            break;
        }
        sleep(1); // try re-connect after 1 second
    }
}

void closeClient() //
{
    close(clientSocket);
}

bool recvCmd() //
{
    socklen_t len = sizeof(sockaddr);
    memset(cli_buff, 0, _PACKAGE_SIZE_);
    int ret = recvfrom(clientSocket, cli_buff,
             _PACKAGE_SIZE_, 0,
             (sockaddr *)&client_,  &len);
    if (ret < 0){
        std::cout << "recvCmd: Not received" << std::endl;
        return false;
    }
    return true;
}

bool replyCmd() //
{
    int t = sendto(clientSocket, cli_buff,
                   _PACKAGE_SIZE_, 0,
                   (sockaddr *)&server_, sizeof(server_));
	if (t < 0) {
        std::cout << "Send Reply Failure" << std::endl;
        memset(cli_buff, 0, _PACKAGE_SIZE_);
        return false;
    }
    std::cout << "Send Reply Success, Reply is " << int(cli_buff[1]) << std::endl;
    memset(cli_buff, 0, _PACKAGE_SIZE_);
    return true;
}

void startRecord(std::string cfg, std::string save_path) //
{
    cli_buff[0] = _SEND_HEADER_;
    cli_buff[1] = _CLIENT_REPLY_OK_;
    replyCmd(); // Reply Send Command is Received 
    
    bool writeNormal = WriteCFGFile(cfg);
    if (!writeNormal) {
        cli_buff[0] = _SEND_HEADER_;
        cli_buff[1] = _CLIENT_REPLY_COM_ERR_;
        replyCmd(); // Reply Send Command is Received 
        std::cout << "Write Config Failure..." << std::endl;
        return;
    }
    
    reset_dca1000();
    DCA_FPGA_CONFIG config;
	config.eLogMode = 1;
	config.eLvdsMode = 2;
	config.eDataXferMode = 1;
	config.eDataCaptureMode = 2;
	config.eDataFormatMode = 3;
	config.u8Timer = 30;
    configure_dca1000(config);
    set_packet_delay_dca1000(30);
    handshake_dca1000();
    
    // 链表数据定义
	LIST_ * header = NULL;
    header = list_init();
    unsigned int data_length = 0;
    int res = data_record_dca1000(header, &data_length);
    
    if (res == _SOCKET_TIMEOUT_ERR_){
        std::cout << "Collection Failure: Socket timeout" << std::endl;
        cli_buff[0] = _SEND_HEADER_;
        cli_buff[1] = _CLIENT_REPLY_ERR_;
        replyCmd(); // Reply Client has sampled data
        return;
    }
    
    save_data(header, save_path.c_str(), 0, data_length);
    list_delete(header);
    
    std::cout << "Client: Collection Complete. Bytes: " << data_length << std::endl;
    
    cli_buff[0] = _SEND_HEADER_;
    cli_buff[1] = _CLIENT_REPLY_RECORD_OVER_;
    replyCmd(); // Reply Client has sampled data
}

void startRecord_local(std::string cfg, std::string save_path) //
{
    bool writeNormal = WriteCFGFile(cfg);
    if (!writeNormal) {
        std::cout << "Write Config Failure..." << std::endl;
        return;
    }
    
    reset_dca1000();
    DCA_FPGA_CONFIG config;
	config.eLogMode = 1;
	config.eLvdsMode = 2;
	config.eDataXferMode = 1;
	config.eDataCaptureMode = 2;
	config.eDataFormatMode = 3;
	config.u8Timer = 30;
    configure_dca1000(config);
    set_packet_delay_dca1000(30);
    handshake_dca1000();
    
    // 链表数据定义
	LIST_ * header = NULL;
    header = list_init();
    unsigned int data_length = 0;
    data_record_dca1000(header, &data_length);
    
    save_data(header, save_path.c_str(), 0, data_length);
    list_delete(header);
    
    std::cout << "Client: Collection Complete. Bytes: " << data_length << std::endl;
}

void test_udp_client(){
    cli_buff[0] = _SEND_HEADER_;
    cli_buff[1] = _CLIENT_REPLY_TEST_CON_;
    replyCmd();
}

void ComInit(std::string port)
{
    int max_retry_time = 10;
    int retry_time = 0;
    while (retry_time < max_retry_time){
        if (port_ready && addr_ready && data_port_ready){
            cli_buff[0] = _SEND_HEADER_;
            cli_buff[1] = _CLIENT_REPLY_CON_SUC_;
            replyCmd();
            std::cout << "all components are avaliable now" << std::endl;
            break;
        }
        retry_time++;
        sleep(1);
        int ret = -1;
        if (!port_ready){
            std::cout << "try connecting usb port" << std::endl;
            ret = connect_com_port(port);
            if (ret == _COM_NORMAL_){
                port_ready = true;
                cli_buff[0] = _SEND_HEADER_;
                cli_buff[1] = _CLIENT_REPLY_COM_CON_SUC_;
                replyCmd();
                std::cout << "Connected to Com" << std::endl;
            }else{
                port_ready = false;
                cli_buff[0] = _SEND_HEADER_;
                cli_buff[1] = _CLIENT_REPLY_COM_CON_ERR_;
                replyCmd();
                std::cout << "Retry to Connect Radar..." << std::endl;
                continue;
            }
        }
        
        if (!data_port_ready){
            ret = connect_record_port();
            if (ret != _NORMAL_) {
                cli_buff[0] = _SEND_HEADER_;
                cli_buff[1] = _CLIENT_REPLY_UDP_CON_ERR_;
                replyCmd();
                data_port_ready = false;
                std::cout << "Error at connect port: " << ret << std::endl;
                continue;
            }else{
                cli_buff[0] = _SEND_HEADER_;
                cli_buff[1] = _CLIENT_REPLY_UDP_CON_SUC_;
                replyCmd();
                data_port_ready = true;
                std::cout << "Connected to Record Port" << std::endl;
            }
        }
        
        if (!addr_ready){
            ret = addr_init();
            // 连接udp端口
            if (ret != _NORMAL_) {
                cli_buff[0] = _SEND_HEADER_;
                cli_buff[1] = _CLIENT_REPLY_UDP_ADR_ERR_;
                replyCmd();
                std::cout << "Error at addr port: " << ret << std::endl;
                continue;
            }else{
                cli_buff[0] = _SEND_HEADER_;
                cli_buff[1] = _CLIENT_REPLY_UDP_ADR_SUC_;
                replyCmd();
                addr_ready = true;
                std::cout << "Connected to Command Port" << std::endl; 
            }
        }
    }
    if (retry_time >= max_retry_time){
        cli_buff[0] = _SEND_HEADER_;
        cli_buff[1] = _CLIENT_REPLY_CON_ERR_;
        replyCmd();
    }
}

