#include "radar_define.h"
#include "drone_attitude.h"

SerialStream serial_port;
int data_socket = 0; 
int rcv_socket = 0;
int send_socket = 0;
int udp_socket = 0;
struct sockaddr_in send_addr;
struct sockaddr_in udp_addr; 
struct sockaddr_in rcv_addr;
struct sockaddr_in data_addr; // 雷达网口
int record_is_err = 0;
int record_is_over = 0;
bool DataSampleIsTrigger = false;

extern bool port_ready;
extern bool addr_ready;
extern bool data_port_ready;
extern std::string port_com;

int connect_com_port(std::string port){
    try{
        // Open the Serial Ports at the desired hardware devices.
        serial_port.Open(port.c_str());
    }
    catch (const OpenFailed&)
    {
        // std::cerr << "The serial ports did not open correctly." << std::endl ;
        return _COM_FAILURE_;
    }
    serial_port.SetBaudRate(BaudRate::BAUD_921600);
    serial_port.SetCharacterSize(CharacterSize::CHAR_SIZE_8);
    serial_port.SetParity(Parity::PARITY_NONE);
    serial_port.SetStopBits(StopBits::STOP_BITS_1);
    serial_port.SetFlowControl(FlowControl::FLOW_CONTROL_NONE);
    return _COM_NORMAL_;
} 

void close_port(){
    serial_port.Close();
}

void WritePortLD(std::string data){
    //serial_port.FlushIOBuffers();
    serial_port.write(data.c_str(), data.size());
    // Wait until the data has actually been transmitted.
    serial_port.DrainWriteBuffer();
    //usleep(100);
    //serial_port.FlushIOBuffers();
}

std::string ReadPortLD(){
    std::string buff;
    std::getline(serial_port, buff);
    return buff;
}

bool WriteCFGFile(std::string cfg_path){
    if (port_ready){
        close_port();
        usleep(100); // waiting for a while
        connect_com_port(port_com);
    }
    
    std::ifstream filestream(cfg_path.c_str(), ios::in | ios::binary);
    //判断文件是否正常打开
    if (!filestream) {
        std::cout << "WriteCFG: 无法打开文件" << std::endl;
        return false;
    }
    char _RPY_[_MAX_COM_LEN_];
    char _WRT_[_MAX_COM_LEN_];
    int reply_length = 0;
    int write_length = 0;
    
    while (!filestream.eof()){
        memset(_RPY_, 0, _MAX_COM_LEN_);
        filestream.getline(_WRT_, _MAX_COM_LEN_);
        std::string _DATA_WRT_(_WRT_);
        unsigned int found_result = 5;
        
        if (_DATA_WRT_.find("sensorStart") != std::string::npos) {
            break;
        }
        if (_DATA_WRT_.find("%") != std::string::npos) {
            continue;
        }
        
        size_t pos = _DATA_WRT_.find('\r');
        if (pos != std::string::npos) {
            _DATA_WRT_.erase(pos, 1);
        }
        std::cout << "Write Cfg: " << _DATA_WRT_ << std::endl;
        _DATA_WRT_.append(std::string("\n"));
        write_length = _DATA_WRT_.size();
        //ReadPortLD(); // clear buffer 
        //WritePortLD(_DATA_WRT_); // 20 
        while (found_result) {
            WritePortLD(_DATA_WRT_);
            usleep(10000);
            std::string _DATA_RPY_ = ReadPortLD();
            
            bool done_flag = strstr(_DATA_RPY_.c_str(), "Done") == NULL;
            bool error_flag = strstr(_DATA_RPY_.c_str(), "Error") == NULL;
            
            if (done_flag) {
                if (error_flag) {
                
                }else{
                    // 接收到了错误
                    std::cout << "Detect Error flag, received: " << _DATA_RPY_ << std::endl;
                    return false;
                }
                // 没有接收到Done提示符
                found_result--;
                continue;
            }
            else {
                std::cout << "Write Cfg: " << _DATA_RPY_ << std::endl;
                break;
            }
        }
        if (found_result == 0) {
            std::cout << "No done flag received..." << std::endl;
            port_ready = false; // connection failed
            close_port();
            std::cout << "Closing port" << std::endl;
            return false;
        }
        
        memset(_RPY_, 0, _MAX_COM_LEN_);
    }
    
    std::cout << "File Write Complete..." << std::endl;
    filestream.close();
    return true;    
}

// 检查指令是否相同
int cmd_check(unsigned char * _response_, int cmd) {
	unsigned int header = (unsigned int)_response_[1] * 256 + (unsigned int)_response_[0];
	unsigned int _cmd_ = (unsigned int)_response_[3] * 256 + (unsigned int)_response_[2];
	unsigned int status = (unsigned int)_response_[5] * 256 + (unsigned int)_response_[4];
	unsigned int footer = (unsigned int)_response_[7] * 256 + (unsigned int)_response_[6];

	if (_cmd_ != cmd) return _CMD_NOT_EQUAL_;

	return status;
}

// 保存数据：输入：List头 保存文件名字 数据长度 是否删除header
int save_data(LIST_ * header, const char * name, bool delete_header, unsigned int data_length) {
	if (header == NULL) {
        printf("SAVE FILE: LIST IS NULL");
		return _LIST_IS_NULL_;
	}
	if (strlen(name) <= 0) {
		if (delete_header)
			list_delete(header);
        printf("SAVE FILE: FILE NAME IS Invalid, your name: %s", name);
		return _FILENAME_IS_EMPTY_;
	}

	FILE * fp = fopen(name, "wb");

	if (data_length == 0) {
		printf("DCA1000EVM NO data captured");
		return _NO_DATA_CAPTURED_;
	}

	unsigned char * data = (unsigned char *)malloc(sizeof(unsigned char) * data_length);
	unsigned int data_offset = 0;

	LIST_ * h = header;
	while (h != NULL) {
		memcpy(data + data_offset,
			h->data + _PACKAGE_NUMBER_SEQ_LEN_,
			sizeof(unsigned char) * (h->data_size - _PACKAGE_NUMBER_SEQ_LEN_)
		);
		data_offset += h->data_size - _PACKAGE_NUMBER_SEQ_LEN_;
		h = h->next;
	}
	fwrite(data, sizeof(unsigned char),
		data_length, fp);
	fclose(fp);
	if (delete_header)
		list_delete(header);

	free(data);

	return _NORMAL_;
}

// 开启数据接受端口
int connect_record_port(){
    data_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (data_socket == -1){
        std::cout << "record port: socket error!" << std::endl; 
        return _SOCKET_ERR_;
    }
	data_addr.sin_addr.s_addr = inet_addr("192.168.33.180");
	data_addr.sin_family = AF_INET;
	data_addr.sin_port = htons(1024);
	rcv_addr.sin_addr.s_addr = inet_addr("192.168.33.30");
	rcv_addr.sin_family = AF_INET;
	rcv_addr.sin_port = htons(4098);
    
	int ret = bind(data_socket, (struct sockaddr *)&rcv_addr, sizeof(rcv_addr));
    if (ret == -1){
        std::cout << "record port: bind error!" << std::endl;
        return _SOCKET_ERR_;
    }
    
    timeval tv = {10, 0};
    ret = setsockopt(data_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval));
    if (ret){
        std::cout << "record port: Timeout set error!" << std::endl;
        return _SOCKET_ERR_;
    }

    std::cout << "record port: bind complete!" << std::endl;
    
	return _NORMAL_;
}

// 开启系统设置端口
int addr_init() {
	udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1){
        std::cout << "addr init: socket error!" << std::endl; 
        return _SOCKET_ERR_;
    }
	send_addr.sin_addr.s_addr = inet_addr("192.168.33.180");
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(4096);
	udp_addr.sin_addr.s_addr = inet_addr("192.168.33.30");
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(4096);

	int ret = bind(udp_socket, (struct sockaddr *)&udp_addr, sizeof(udp_addr));
    if (ret < 0){
        std::cout << "addr_init: bind error!" << std::endl;
        return _SOCKET_ERR_;
    }
    
    std::cout << "addr_init: bind complete!" << std::endl;
    
    return _NORMAL_;
}

// 重置 DCA1000
int reset_dca1000() {
	unsigned char data[_LEN_RESET_CMD_];
	unsigned char buff[_MAX_RESPONSE_LEN_];

	data[0] = HEADER_START_BYTES & 0x00FF;
	data[1] = HEADER_START_BYTES >> 8;
	data[2] = CMD_CODE_RESET_FPGA & 0x00FF;
	data[3] = CMD_CODE_RESET_FPGA >> 8;
	data[4] = 0 & 0x00FF;
	data[5] = 0 >> 8;
	data[6] = FOOTER_STOP_BYTES & 0x00FF;
	data[7] = FOOTER_STOP_BYTES >> 8;

	int len = sendto(udp_socket, (char *)data, _LEN_RESET_CMD_, 0, (sockaddr *)&send_addr, sizeof(send_addr));
    if (len < 0){
        std::cout << "reset dca1000: send error!" << std::endl;
        return _SOCKET_SEND_ERR_;
    }
    
	struct sockaddr_in s;
	int ret;
    socklen_t addrlen = sizeof(sockaddr);
	addrlen = sizeof(s);
    
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_, 0, (sockaddr *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_RESET_FPGA);
        std::cout << "DCA1000 reset: return " << status << std::endl;
		if (status) 
            return status;
	}

	printf("DCA1000 重置完成\n");
	return _NORMAL_;
}

// Data log mode Raw Mode = 1; Multi Mode = 2 default =1
// Data LVDS mode Four Lane = 1; Two Lane = 2 default =2
// Data transfer mode Capture = 1; PlayBack = 2 default=1
// Data capture mode SD_STORAGE = 1; ETH = 2 default=2
// Data format mode Bit12 = 1 Bit14 = 2 Bit16 = 3 default=3
// Timeout value for LVDS data default = 30 default=30
int configure_dca1000(DCA_FPGA_CONFIG config) {

	unsigned char data[_LEN_CON_FPGA_CMD_];
	unsigned char buff[_MAX_RESPONSE_LEN_];

	data[0] = HEADER_START_BYTES & 0x00FF;
	data[1] = HEADER_START_BYTES >> 8;
	data[2] = CMD_CODE_CONFIG_FPGA & 0x00FF;
	data[3] = CMD_CODE_CONFIG_FPGA >> 8;
	data[4] = 6; // data length = 6
	data[5] = 0;
	data[6] = config.eLogMode;
	data[7] = config.eLvdsMode;
	data[8] = config.eDataXferMode;
	data[9] = config.eDataCaptureMode;
	data[10] = config.eDataFormatMode;
	data[11] = config.u8Timer;
	data[12] = FOOTER_STOP_BYTES & 0x00FF;
	data[13] = FOOTER_STOP_BYTES >> 8;

	sendto(udp_socket, (char *)data, _LEN_CON_FPGA_CMD_, 0, (sockaddr *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret;
    socklen_t addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_, 0, (sockaddr *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_CONFIG_FPGA);
        std::cout << "DCA1000 configure: return " << status << std::endl;
		if (status)
            return status;
	}

	std::cout << "DCA1000 FPGA设置完成 模式为：\nLogging 模式： " << (config.eLogMode == 1 ? "Multi Mode" : "Raw Mode" ) << "\nLVDS 模式： " << (config.eLvdsMode == 1 ? "4 Lane" : "2 Lane") << "\n数据保存模式： " << (config.eDataCaptureMode == 1 ? "SD卡保存" : "以太网传输") << std::endl;
	return _NORMAL_;
}

// 保持100us以下 // 100us 100Mbps
// 10us 600Mbps
int set_packet_delay_dca1000(unsigned int us) {

	unsigned char data[_LEN_SET_PACKET_DELAY_CMD_];
	unsigned char buff[_MAX_RESPONSE_LEN_];

	unsigned int _1us_offset = 125;
	unsigned int _final_cnt_ = us * _1us_offset; // 40MHz?

	data[0] = HEADER_START_BYTES & 0x00FF;
	data[1] = HEADER_START_BYTES >> 8;
	data[2] = CMD_CODE_CONFIG_RECORD & 0x00FF;
	data[3] = CMD_CODE_CONFIG_RECORD >> 8;
	data[4] = 6; // data length = 6
	data[5] = 0;
	data[6] = 0xbe;
	data[7] = 0x05;
	data[8] = (_final_cnt_ & 0x000000FF);
	data[9] = (_final_cnt_ & 0x0000FF00) >> 8;
	data[10] = (_final_cnt_ & 0x00FF0000) >> 16;
	data[11] = (_final_cnt_ & 0xFF000000) >> 24;
	data[12] = FOOTER_STOP_BYTES & 0x00FF;
	data[13] = FOOTER_STOP_BYTES >> 8;

	sendto(udp_socket, (char *)data, _LEN_SET_PACKET_DELAY_CMD_,
		0, (sockaddr *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret;
	socklen_t addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_, 0, (sockaddr *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_CONFIG_RECORD);
        std::cout << "DCA1000 Packet Delay set: return " << status << std::endl;
		if (status) return status;
	}

	std::cout << "DCA1000 Packet Delay设置完成 延时为： "<< us << std::endl;
	return _NORMAL_;
}

int handshake_dca1000() {
	unsigned char data[_LEN_HANDSHAKE_CMD_];
	unsigned char buff[_MAX_RESPONSE_LEN_];

	data[0] = HEADER_START_BYTES & 0x00FF;
	data[1] = HEADER_START_BYTES >> 8;
	data[2] = CMD_CODE_SYSTEM_ALIVENESS & 0x00FF;
	data[3] = CMD_CODE_SYSTEM_ALIVENESS >> 8;
	data[4] = 0 & 0x00FF;
	data[5] = 0 >> 8;
	data[6] = FOOTER_STOP_BYTES & 0x00FF;
	data[7] = FOOTER_STOP_BYTES >> 8;

	sendto(udp_socket, (char *)data, _LEN_HANDSHAKE_CMD_, 0, (sockaddr *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret;
	socklen_t addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_, 0, (sockaddr *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_SYSTEM_ALIVENESS);
		if (status) return status;
	}

	std::cout << ("DCA1000 握手完成\n");
	return _NORMAL_;
}

void data_record_dca1000_finish_detect_thread() {

	unsigned char buff[_MAX_RESPONSE_LEN_];
	unsigned char data[_LEN_DATA_START_RECORD_CMD_];

	data[0] = HEADER_START_BYTES & 0x00FF;
	data[1] = HEADER_START_BYTES >> 8;
	data[2] = CMD_CODE_START_RECORD & 0x00FF;
	data[3] = CMD_CODE_START_RECORD >> 8;
	data[4] = 0 & 0x00FF;
	data[5] = 0 >> 8;
	data[6] = FOOTER_STOP_BYTES & 0x00FF;
	data[7] = FOOTER_STOP_BYTES >> 8;
    
	sendto(udp_socket, (char *)data, _LEN_DATA_START_RECORD_CMD_,
           0, (sockaddr *)&send_addr, sizeof(send_addr));
    
	struct sockaddr_in s;
	int ret;
	socklen_t addrlen = sizeof(s);

	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_,
                   0, (sockaddr *)&s, &addrlen);
    
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_START_RECORD);
		if (status) {
            std::cout << "Record Thread: Record Error" << std::endl; 
            record_is_err = 1;
        }
	}else{
        std::cout << "Record Thread: Sending Record Request Error" << std::endl;
    }

    std::cout << "Record Thread: Sent Record Request" << std::endl;
	DataSampleIsTrigger = true;

	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_,
		0, (sockaddr *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_SYSTEM_ASYNC_STATUS);
		if (status) {
			record_is_over = 1;
		}
		else {
			record_is_over = 1;
		}
		std::cout << "DCA1000 发来命令： CMD_CODE_SYSTEM_ASYNC_STATUS\n返回值为: " << record_is_over << std::endl;
	}
	
	return;
}

void TriggerDataSample() {
    while(!DataSampleIsTrigger) usleep(100); // 0.1ms 
    std::cout << "Trigger: Writing Radar" << std::endl;
	std::string _sensorStart_cmd_ = std::string("sensorStart\n");
	WritePortLD(_sensorStart_cmd_);
	std::string _recv_ = ReadPortLD();
    bool done_flag = strstr(_recv_.c_str(), "Done") == NULL;
    while (done_flag) {
        _recv_ = ReadPortLD();
        done_flag = strstr(_recv_.c_str(), "Done") == NULL;
    }
    std::cout << _recv_ << std::endl;
}

int data_record_dca1000(
    LIST_ * header, unsigned int * data_length
) {
	DataSampleIsTrigger = false;

	record_is_err = 0;
	record_is_over = 0;

	std::thread _thread_trigger_sample_ = std::thread(&TriggerDataSample);
	_thread_trigger_sample_.detach();
    
	std::thread _thread_detect_finish_ = std::thread(&data_record_dca1000_finish_detect_thread);
	_thread_detect_finish_.detach();


	*data_length = 0;
	unsigned int total_length = 0;
	LIST_ * h = header;

	struct sockaddr_in s;
	unsigned int TIMEOUT_TIMES = 0;
	int ret = 0;
	socklen_t addrlen = sizeof(s);
	unsigned char data_buff[_MAX_PACKAGE_WITHOUT_SEQ_LEN_];

	while (1) {
		memset(data_buff, 0, _MAX_PACKAGE_WITHOUT_SEQ_LEN_);
		if (record_is_over){
            std::cout << ("DCA1000EVM Record is Over！\n");
			break;
        }
		if (record_is_err) {
			std::cout << ("DCA1000EVM Record is Error！\n");
			return _RECORD_IS_ERR_;
		}

		int bytes = recvfrom(data_socket, (char *)data_buff, _MAX_PACKAGE_WITHOUT_SEQ_LEN_,
			0, (sockaddr *)&s, &addrlen);
		if (bytes < 0) {
			std::cout << ("DCA1000EVM Packet Timeout！\n");
			TIMEOUT_TIMES++;
			if (TIMEOUT_TIMES >= _MAX_TIMEOUT_TIMES_) {
				return _SOCKET_TIMEOUT_ERR_;
			}
			continue;
		}else{
            
        }
		if (bytes < _MIN_PACKAGE_WITHOUT_SEQ_LEN_) {
			std::cout << ("DCA1000EVM Packet Not Healthy， Byte: %d！\n", bytes);
		}
		if (header->data_size == 0) {
			memcpy(header->data, data_buff, bytes);
			header->data_size = bytes;
			h = header;
		}
		else {
			LIST_ * node = list_init();
			memcpy(node->data, data_buff, bytes);
			node->data_size = bytes;
			h = list_append(h, node);
		}
		*data_length += (bytes - _PACKAGE_NUMBER_SEQ_LEN_);
		total_length += bytes;
	}

	std::cout << "DCA1000EVM: Collection Complete!" << std::endl;
	return _NORMAL_;
}

int close_dca1000() {

	unsigned char data[_LEN_DATA_STOP_RECORD_CMD_];
	unsigned char buff[_MAX_RESPONSE_LEN_];

	data[0] = HEADER_START_BYTES & 0x00FF;
	data[1] = HEADER_START_BYTES >> 8;
	data[2] = CMD_CODE_STOP_RECORD & 0x00FF;
	data[3] = CMD_CODE_STOP_RECORD >> 8;
	data[4] = 0 & 0x00FF;
	data[5] = 0 >> 8;
	data[6] = FOOTER_STOP_BYTES & 0x00FF;
	data[7] = FOOTER_STOP_BYTES >> 8;

	sendto(udp_socket, (char *)data,
           _LEN_DATA_STOP_RECORD_CMD_, 0,
        (sockaddr *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret = 0;
	socklen_t addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_,
		0, (sockaddr *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_STOP_RECORD);
		if (status) return status;
	}

	close(udp_socket);
	close(data_socket);
    
    std::cout << "Closing Socket..." << std::endl;
    
	return _NORMAL_;
}
