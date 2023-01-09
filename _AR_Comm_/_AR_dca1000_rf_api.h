#pragma once

#ifndef _AR_DCA1000_RF_API_H_
#define _AR_DCA1000_RF_API_H_

// Qt的定义
#include <QFile>
#include <QList>
#include <QDebug>
#include <QDateTime>
#include <QUdpSocket>
#include <QByteArray>
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QNetworkDatagram>
// 新增库的定义
#include "../_AR_define_/_AR_actor_define_.h" 
#include "../_AR_define_/TCP_function_define.h"
#include "../_AR_define_/TCP_packet_define.h"
#include "../_AR_define_/TCP_define.h"
#include "../_AR_utility_/linked_list.h"
// 标准库定义
#include <cstring>
#include <cstdlib>
// 
#if defined _WIN32

#include <Windows.h>
#include <winsock.h>
#pragma comment (lib,"ws2_32.lib")

#else
extern radar_client * _radar_client_;
extern radar_server * _radar_server_;

using namespace std;
#endif

extern LIST_ * ClientHeader;
extern unsigned int _CLIENT_AVAILABLE_;
extern unsigned int PLAYER;
extern unsigned int _ENCODING_MODE_;

/** Success status for command                                               */
#define	STS_RFDCCARD_SUCCESS                (0)
/** Invalid input parameters status for command                              */
#define STS_RFDCCARD_INVALID_INPUT_PARAMS   (-1)
/** OS error status for command                                              */
#define STS_RFDCCARD_OS_ERR                 (-2)
/** UDP write failure status for command                                     */
#define STS_RFDCCARD_UDP_WRITE_ERR          (-3)
/** Ethernet cable connection error                                          */
#define STS_RFDCCARD_ETH_CABLE_CON_ERROR    (-4)
/** Command response timeout error                                           */
#define	STS_RFDCCARD_TIMEOUT_ERR            (-5)
/** Event timeout error                                                      */
#define	STS_RFDCCARD_EVENT_TIMEOUT_ERR      (-6)
/** Reset FPGA command code                                                  */
#define CMD_CODE_RESET_FPGA                 0x01
/** Reset AR device command code                                             */
#define CMD_CODE_RESET_AR_DEV               0x02
/** Configure FPGA data modes command code                                   */
#define CMD_CODE_CONFIG_FPGA                0x03
/** Configure EEPROM command code                                            */
#define CMD_CODE_CONFIG_EEPROM              0x04
/** Start record command code                                                */
#define CMD_CODE_START_RECORD               0x05
/** Stop record command code                                                 */
#define CMD_CODE_STOP_RECORD                0x06
/** Playback start command code                                              */
#define CMD_CODE_START_PLAYBACK             0x07
/** Playback stop command code                                               */
#define CMD_CODE_STOP_PLAYBACK              0x08
/** System aliveness check command code                                      */
#define CMD_CODE_SYSTEM_ALIVENESS           0x09
/** System status command code                                               */
#define CMD_CODE_SYSTEM_ASYNC_STATUS        0x0A
/** Configure packet data delay and size command code                        */
#define CMD_CODE_CONFIG_RECORD              0x0B
/** Configure AR device data mode command code                               */
#define CMD_CODE_CONFIG_AR_DEV              0x0C
/** Initiate FPGA playback command code                                      */
#define CMD_CODE_INIT_FPGA_PLAYBACK			0x0D
/** Read FPGA version command code                                           */
#define CMD_CODE_READ_FPGA_VERSION			0x0E
/** CLI - Read record status from shared memory command code                 */
#define CMD_CODE_CLI_PROC_STATUS_SHM        0x20
/** CLI - Read DLL version command code                                      */
#define CMD_CODE_CLI_READ_DLL_VERSION       0x21
/** CLI - Read CLI version command code                                      */
#define CMD_CODE_CLI_READ_CLI_VERSION       0x23
/** CLI - Async Record stop command code                                     */
#define CMD_CODE_CLI_ASYNC_RECORD_STOP      0x24
/** Record packet out of sequence error code                                 */
#define STS_RECORD_PKT_OUT_OF_SEQ           0xC3
/** Record progress status code                                              */
#define STS_RECORD_IS_IN_PROGRESS           0xC4
/** Playback from GUI to FPGA completed status code                          */
#define STS_GUI_PLAYBACK_COMPLETED			0xC5
/** Playback file open error code                                            */
#define STS_PLAYBACK_FILE_OPEN_ERR          0xC6
/** Playback UDP write failure error code                                    */
#define STS_PLAYBACK_UDP_WRITE_ERR          0xC7
/** Playback progress status code                                            */
#define STS_PLAYBACK_IS_IN_PROGRESS         0xC8
/** Record thread timeout error code                                         */
#define STS_CAPTURE_THREAD_TIMEOUT          0xC9
/** Command header start bytes                                               */
#define HEADER_START_BYTES                  0xA55A
/** Command footer start bytes                                               */
#define FOOTER_STOP_BYTES                   0xEEAA
/** Maximum length of a string                                               */
#define MAX_NAME_LEN                        255
/** Maximum length of version info                                           */
#define MAX_VERSION_BUF_LEN                 20
/** Number of data types for record                                          */
#define NUM_DATA_TYPES                      5
/** Maximum data bytes in command request                                    */
#define MAX_DATA_BYTES                      504

// 时间的长度
#define _MAX_TIMEOUT_TIMES_ 10
#define _TIMEOUT_OUT_UDP_US_ 2000
#define _DATA_PRE_LENGTH_ 1073741824

// 指令的长度
#define _MAX_RESPONSE_LEN_ 1024
#define _LEN_RESET_CMD_ 8
#define _LEN_CON_FPGA_CMD_ 14
#define _LEN_SET_PACKET_DELAY_CMD_ 14
#define _LEN_HANDSHAKE_CMD_ 8
#define _LEN_DATA_START_RECORD_CMD_ 8
#define _LEN_DATA_STOP_RECORD_CMD_ 8

// 返回的指令码
#define _NORMAL_ 0
#define _SOCKET_ERR_ -1
#define _WSA_ERR_ -2
#define _SOCKET_BIND_ERR_ -3
#define _SOCKET_NOT_INIT_ -4
#define _CMD_NOT_EQUAL_ -5
#define _RECORD_IS_ERR_ -6
#define _LIST_IS_NULL_ -7
#define _SOCKET_TIMEOUT_ERR_ -8
#define _NO_DATA_CAPTURED_ -9
#define _CLIENT_NOT_INITIALIZED_ -10
#define _SOCKET_NO_RESPONSE_ -11
#define _FILENAME_IS_EMPTY_ -12

// 定义的DCA1000EVM 配置的结构体
typedef struct DCA1000EVM_fpga_configure {
	unsigned char eLogMode; // Data log mode Raw Mode = 1; Multi Mode = 2
	unsigned char eLvdsMode; // Data LVDS mode Four Lane = 1; Two Lane = 2
	unsigned char eDataXferMode; // Data transfer mode Capture = 1; PlayBack = 2
	unsigned char eDataCaptureMode; // Data capture mode SD_STORAGE = 1; ETH = 2
	unsigned char eDataFormatMode; // Data format mode Bit12 = 1 Bit14 = 2 Bit16 = 3
	unsigned char u8Timer; // Timeout value for LVDS data default = 30;
} DCA_FPGA_CONFIG;

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
		// printf("SAVE FILE: LIST IS NULL");
		info.push_back("File Save Service: LIST IS NULL!\n");
		warn.push_back(_UI_ERR_);
		return _LIST_IS_NULL_;
	}
	if (strlen(name) <= 0) {
		info.push_back("File Save Service: File name is empty!\n");
		warn.push_back(_UI_ERR_);

		if (delete_header)
			list_delete(header);

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

class radar_com : public QObject {
	Q_OBJECT

public:
#if defined _WIN32

	SOCKET udp_socket;  // system config socket
	SOCKET data_socket; // data transport socket
	SOCKADDR_IN send_addr; // client ADDR
	SOCKADDR_IN data_addr; // client ADDR
	SOCKADDR_IN udp_addr;
	SOCKADDR_IN rcv_addr;
	BOOL udp_is_init = 0;
	BOOL data_port_is_open = 0;
	BOOL record_is_over = 0;
	BOOL record_is_err = 0;

#endif

public:
	explicit radar_com(QObject * parent = nullptr);
	//串口设置部分
	bool GetEnhancePort();
	void connect_com_port();
	void WritePortLD(QByteArray cmd);
	void ReadPortLD();
	void OpenCfgFile();
	void WriteCfgFile(); // 写入命令
	bool ComIsOpen = false; // 串口是否被打开
	QByteArray com_port_data; // 串口读取到的数据

#if defined _WIN32
	int connect_record_port(); // 开启数据接收端口
	int addr_init(); // 开启系统设置端口
	int reset_dca1000(); // 重置DCA1000
	int configure_dca1000(DCA_FPGA_CONFIG config); // 配置DCA1000
	int set_packet_delay_dca1000(unsigned int us); // 设置包延时
	int handshake_dca1000();
	void data_record_dca1000_finish_detect_thread();
	// 实时数据传输线程 如果将其用单线程 会导致UDP丢包 选择使用多线程 防止丢包
	void data_record_dca1000_real_time_transfer_thread(LIST_ * lpParameter);
	int data_record_dca1000(LIST_ * header, UINT32 * data_length);
	int close_dca1000(); // 关闭DCA1000
#else
	// UDP连接部分
	// 数据接受部分
	QUdpSocket * data_socket = NULL;   //udp发送端
	QUdpSocket * recv_socket = NULL;   //udp接受端
	// 系统设置部分
	QUdpSocket * send_socket = NULL;   //udp命令请求接受端
	QUdpSocket * udp_socket = NULL;   //udp命令请求发送端
	// UDP发送数据端口 以及数据接受端口
	void connect_udp_port();
	// 连接数据串口以及网口
	int reset_dca1000();
	int configure_dca1000(DCA_FPGA_CONFIG config);
	int set_packet_delay_dca1000(unsigned int us);
	int handshake_dca1000();
	int close_dca1000();
	void data_record_dca1000_real_time_transfer_thread(LIST_ * header);
	int data_record_dca1000(LIST_ * header, unsigned int * data_length);
#endif
	bool offline_sample = true;
	bool sensorCanStart = false; // DCA1000可以触发了

private:
	std::vector <QByteArray> cfg; // cfg命令
	QHostAddress send_address = QHostAddress("192.168.33.180");
	int send_port = 4096;
	QHostAddress udp_address = QHostAddress("192.168.33.30");
	int udp_port = 4096;
	QHostAddress data_address = QHostAddress("192.168.33.180");
	int data_port = 1024;
	QHostAddress recv_address = QHostAddress("192.168.33.30");
	int recv_port = 4098;

	// 串口连接部分
	QSerialPort * SerialPort = NULL;
	QSerialPortInfo * SerialPortInfo = NULL;
	QList<QSerialPortInfo> lists; // 找到Enhance端口

	// cfg文件读取
	QFile * cfg_file = NULL;
	QByteArray cfg_files_bytes;
	bool FileIsOpen = false; // 文件是否被打开

	// 串口配置完成
	bool ComSettingComplete = false;
	bool UdpSettingComplete = false;
	bool DataSampleIsTrigger = false;
	bool DataIsReadyToTrigger = false;
	QByteArray _udp_recv_;
	QByteArray _dat_recv_;

	void TriggerDataSample();
public slots:

};

extern BOOL clientsocket_is_built;
extern BOOL clientsocket_is_ready;
extern BOOL client_collection_finish;
extern BOOL client_collection_enable;

radar_com::radar_com(QObject * parent) :
	QObject(parent) {

#if defined _WIN32
	
#else
	data_socket = new QUdpSocket();   //udp发送端
	recv_socket = new QUdpSocket();   //udp接受端
	// 系统设置部分
	send_socket = new QUdpSocket();   //udp命令请求接受端
	udp_socket = new QUdpSocket();   //udp命令请求发送端

	connect_udp_port(); // 连接UDP端口
#endif

	SerialPort = new QSerialPort();
	SerialPortInfo = new QSerialPortInfo();

}

void radar_com::OpenCfgFile() {
	if (cfg_file != NULL) delete cfg_file;
	cfg_file = new QFile("C:/Users/E1ysium/Desktop/AirPlane/ARPlaneSLAM/CLI/_AR_UI_/x64/Debug/profile_monitor_xwr18xx.cfg");
	if (cfg_file->exists()) 
		FileIsOpen = true;
}

void radar_com::WriteCfgFile() {
	if (!ComIsOpen) return; // 串口被打开
	if (!FileIsOpen) return; // 没有文件被打开
	ReadPortLD(); // 读取几个字节
	if (cfg_file->open(QFile::ReadOnly)) {
		while (!cfg_file->atEnd()) {
			unsigned int found_result = 100;
			cfg_files_bytes = cfg_file->readLine();
			cfg_files_bytes = cfg_files_bytes.simplified();
			if (cfg_files_bytes.contains("sensorStart")) {
				sensorCanStart = true;
				break;
			}
			if (cfg_files_bytes.contains("%"))
				continue;
			cfg_files_bytes.append('\n');
			ReadPortLD(); // clear buffer 
			WritePortLD(cfg_files_bytes); // 20 
			while (found_result) {
				Sleep(100);
				ReadPortLD();
				if (!com_port_data.contains("Done")) {
					// 没有接收到Done提示符
					found_result--;
					continue;
				}
				else {
					break;
				}
				if (!com_port_data.contains("Error")) {
					// 接收到了错误
					cfg_file->close();
					FileIsOpen = false;
					delete cfg_file; // 删除内存
					cfg_file = NULL;
					ComSettingComplete = false;
					return;
				}
			}
			if (found_result == 0) {
				cfg_file->close();
				FileIsOpen = false;
				delete cfg_file; // 删除内存
				cfg_file = NULL;
				ComSettingComplete = false;
				return;
			}
		}
	}
	cfg_file->close();
	FileIsOpen = false;
	delete cfg_file; // 删除内存
	cfg_file = NULL;
	ComSettingComplete = true;
}

// 自动连接Enhance Port COM
void radar_com::connect_com_port() {
	if (GetEnhancePort()) {
		this->SerialPort->setPortName(SerialPortInfo->portName());
		this->SerialPort->setBaudRate(921600);
		this->SerialPort->setDataBits(QSerialPort::Data8);//设置数据位（8）
		this->SerialPort->setParity(QSerialPort::NoParity); //设置奇偶校检（无）
		this->SerialPort->setStopBits(QSerialPort::OneStop);//设置停止位(一位)
		this->SerialPort->setFlowControl(QSerialPort::NoFlowControl);//设置流控制（无）
		this->SerialPort->setReadBufferSize(1024);
		if (!SerialPort->open(QIODevice::ReadWrite)) return;
		this->ComIsOpen = true;
	}
	else {
		this->ComIsOpen = false;
		return;
	}
}

// Com 写入数据
void radar_com::WritePortLD(QByteArray cmd) {
	if (!this->ComIsOpen) return;
	if (cmd.isEmpty()) return; // cmd是空的
	this->SerialPort->clear();
	this->SerialPort->write(cmd);
	this->SerialPort->flush();    //尽可能多地从内部写缓冲区写入底层串口而不阻塞
	this->SerialPort->waitForBytesWritten(100);
}

// Com 读取数据
void radar_com::ReadPortLD() {
	com_port_data.clear();
	if (!this->ComIsOpen) return;
	this->SerialPort->waitForReadyRead(100);
	com_port_data.append(this->SerialPort->readAll());
}

// 寻找增强端口
bool radar_com::GetEnhancePort() {
	this->lists = QSerialPortInfo::availablePorts();
	bool find = false;
	for (int ii = 0; ii < lists.length(); ii++) {
		if (lists[ii].description() == "Silicon Labs Dual CP2105 USB to UART Bridge: Enhanced COM Port") {
			if (this->SerialPortInfo != NULL) {
				delete this->SerialPortInfo;
			}
			this->SerialPortInfo = new QSerialPortInfo(lists[ii].portName());
			find = true;
		}
	}
	return find;
}

#if defined _WIN32
// 开启数据接收端口
int radar_com::connect_record_port() {
	WSADATA wsadata;//wsa 即windows socket async 异步套接字
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		//printf("套接字未打开\n");
		WSACleanup();
		return _WSA_ERR_;
	}
	else
	{
		//printf("已打开套接字\n");
	}
	data_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (data_socket == INVALID_SOCKET)
	{
		//printf("invalid socket!");
		return _SOCKET_ERR_;
	}

	data_addr.sin_addr.S_un.S_addr = inet_addr("192.168.33.180");
	data_addr.sin_family = AF_INET;
	data_addr.sin_port = htons(1024);

	rcv_addr.sin_addr.S_un.S_addr = inet_addr("192.168.33.30");
	rcv_addr.sin_family = AF_INET;
	rcv_addr.sin_port = htons(4098);

	// recv time out
	DWORD lBuffer[2] = { 0, 0 };
	int lSize;
	lBuffer[0] = (DWORD)_TIMEOUT_OUT_UDP_US_;
	lSize = sizeof(DWORD);
	if (setsockopt(data_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)lBuffer, sizeof(lBuffer))) {
		//printf("DCA1000EVM connect: 无法设置超时!\n");
		return _SOCKET_TIMEOUT_ERR_;
	}

	if (bind(data_socket, (LPSOCKADDR)&rcv_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {//绑定完成
		//printf("bind error !\n");
		return _SOCKET_BIND_ERR_;
	}
	data_port_is_open = 1;

	return _NORMAL_;
}

// 开启系统设置端口
int radar_com::addr_init() {
	WSADATA wsadata;//wsa 即windows socket async 异步套接字
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		//printf("套接字未打开\n");
		WSACleanup();
		return _WSA_ERR_;
	}
	else
	{
		//printf("已打开套接字\n");
	}
	udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_socket == INVALID_SOCKET)
	{
		//printf("invalid socket!");
		return _SOCKET_ERR_;
	}
	send_addr.sin_addr.S_un.S_addr = inet_addr("192.168.33.180");
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(4096);

	udp_addr.sin_addr.S_un.S_addr = inet_addr("192.168.33.30");
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(4096);

	if (bind(udp_socket, (LPSOCKADDR)&udp_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {//绑定完成
		//printf("bind error !\n");
		return _SOCKET_BIND_ERR_;
	}
	udp_is_init = 1;

	return _NORMAL_;
}

// 重置DCA1000EVM
int radar_com::reset_dca1000() {
	if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

	sendto(udp_socket, (char *)data, _LEN_RESET_CMD_, 0, (SOCKADDR *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret, addrlen;
	addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_, 0, (SOCKADDR *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_RESET_FPGA);
		if (status) return status;
	}

	//printf("DCA1000 重置完成\n");
	return _NORMAL_;
}

// Data log mode Raw Mode = 1; Multi Mode = 2 default =1
// Data LVDS mode Four Lane = 1; Two Lane = 2 default =2
// Data transfer mode Capture = 1; PlayBack = 2 default=1
// Data capture mode SD_STORAGE = 1; ETH = 2 default=2
// Data format mode Bit12 = 1 Bit14 = 2 Bit16 = 3 default=3
// Timeout value for LVDS data default = 30 default=30
int radar_com::configure_dca1000(DCA_FPGA_CONFIG config) {
	if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

	sendto(udp_socket, (char *)data, _LEN_CON_FPGA_CMD_, 0, (SOCKADDR *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret, addrlen;
	addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_, 0, (SOCKADDR *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_CONFIG_FPGA);
		if (status) return status;
	}

	//printf("DCA1000 FPGA设置完成 模式为：\nLogging 模式： %s\nLVDS 模式： %s\n数据保存模式： %s\n",
	//	config.eLogMode == 1 ? "Multi Mode" : "Raw Mode",
	//	config.eLvdsMode == 1 ? "4 Lane" : "2 Lane",
	//	config.eDataCaptureMode == 1 ? "SD卡保存" : "以太网传输"
	//);
	return _NORMAL_;
}

// 保持100us以下 // 100us 100Mbps
// 10us 600Mbps
int radar_com::set_packet_delay_dca1000(unsigned int us) {
	if (!udp_is_init) return _SOCKET_NOT_INIT_;

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
		0, (SOCKADDR *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret, addrlen;
	addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_, 0, (SOCKADDR *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_CONFIG_RECORD);
		if (status) return status;
	}

	//printf("DCA1000 Packet Delay设置完成 延时为： %d us\n", us);
	return _NORMAL_;
}

int radar_com::handshake_dca1000() {
	if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

	sendto(udp_socket, (char *)data, _LEN_HANDSHAKE_CMD_, 0, (SOCKADDR *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret, addrlen;
	addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_, 0, (SOCKADDR *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_SYSTEM_ALIVENESS);
		if (status) return status;
	}

	//printf("DCA1000 握手完成\n");
	return _NORMAL_;
}

void radar_com::data_record_dca1000_finish_detect_thread() {
	if (!udp_is_init || !data_port_is_open) return;

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

	sendto(udp_socket, (char *)data, _LEN_DATA_START_RECORD_CMD_, 0,
		(SOCKADDR *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret, addrlen;
	addrlen = sizeof(s);

	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_,
		0, (SOCKADDR *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_START_RECORD);
		if (status) record_is_err = 1;
	}

	if (recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_,
		0, (SOCKADDR *)&s, &addrlen)) {
		int status = cmd_check(buff, CMD_CODE_SYSTEM_ASYNC_STATUS);
		if (status) {
			record_is_over = 1;
			//printf("DCA1000 发来命令： CMD_CODE_SYSTEM_ASYNC_STATUS\n返回值为: %d\n",
			//	record_is_over);
		}
		else {
			record_is_over = 1;
		}
	}
	return;
}

// 实时数据传输线程 如果将其用单线程 会导致UDP丢包 选择使用多线程 防止丢包
void radar_com::data_record_dca1000_real_time_transfer_thread(LIST_ * lpParameter) {
	if (!udp_is_init || !data_port_is_open) {
		//printf("RealTime Data Record Client: 数据端口没有打开\n");
		return;
	}

	extern BOOL client_collection_finish;
	extern BOOL client_collection_enable;

	LIST_ * h = (LIST_ *)lpParameter;

	while (client_collection_enable) {
		if (client_collection_finish && h->next == NULL) {
			break;
		}
		if (h != NULL) {
			ClientHeader = h;
			if (h->data_size == 0) {
				continue;
			}
			int ret_code = send_real_time_data();
			if (ret_code) {
			}
		}
		else {
			return;
		}

		if (h->next == NULL) {
			continue;
		}
		else {
			h = h->next;
		}
	}

	client_collection_enable = 0;
}

void radar_com::TriggerDataSample() {
	if (DataSampleIsTrigger) return;
	while (!DataIsReadyToTrigger) Sleep(1);
	QByteArray _sensorStart_cmd_ = QByteArray("sensorStart");
	_sensorStart_cmd_.append('\n');
	WritePortLD(_sensorStart_cmd_);
	ReadPortLD();
	DataSampleIsTrigger = false;
}

int radar_com::data_record_dca1000(LIST_ * header, UINT32 * data_length) {
	if (!udp_is_init || !data_port_is_open) return _SOCKET_NOT_INIT_;

	extern BOOL client_collection_finish;
	extern BOOL client_collection_enable;

	DataIsReadyToTrigger = false;
	DataSampleIsTrigger = false;
	client_collection_finish = 0; // 清空标志位
	client_collection_enable = 0; // 初始化标志位

	record_is_err = 0;
	record_is_over = 0;

	std::thread _thread_detect_finish_ = std::thread(&radar_com::data_record_dca1000_finish_detect_thread, this);
	_thread_detect_finish_.detach();

	std::thread _thread_trigger_sample_ = std::thread(&radar_com::TriggerDataSample, this);
	_thread_trigger_sample_.detach();
	DataSampleIsTrigger = true;

	if (PLAYER == _CLIENT_TRANSMITTER && _CLIENT_AVAILABLE_ == _REALTIME_MODE_ && _TELE_ENABLE_ == _TELE_IS_ENABLED_) {
		std::thread _thread_transmit_realtime_ = std::thread(&radar_com::data_record_dca1000_real_time_transfer_thread,
			this, header);
		_thread_transmit_realtime_.detach();
	}

	*data_length = 0;
	UINT32 total_length = 0;
	LIST_ * h = header;

	struct sockaddr_in s;
	UINT32 TIMEOUT_TIMES = 0;
	int ret, addrlen;
	addrlen = sizeof(s);
	unsigned char data_buff[_MAX_PACKAGE_WITHOUT_SEQ_LEN_];

	while (1) {
		client_collection_enable = 1;
		memset(data_buff, 0, _MAX_PACKAGE_WITHOUT_SEQ_LEN_);
		if (record_is_over)
			break;
		if (record_is_err) {
			//printf("DCA1000EVM Record is Error！\n");
			return _RECORD_IS_ERR_;
		}

		DataIsReadyToTrigger = true;
		int bytes = recvfrom(data_socket, (char *)data_buff, _MAX_PACKAGE_WITHOUT_SEQ_LEN_,
			0, (SOCKADDR *)&s, &addrlen);
		if (bytes < 0) {
			//printf("DCA1000EVM Packet Timeout！\n");
			TIMEOUT_TIMES++;
			if (TIMEOUT_TIMES >= _MAX_TIMEOUT_TIMES_) {
				info_mutex.lock();
				info.push_back("DCA1000EVM Start Record: Timeout!\n");
				warn.push_back(_UI_ERR_);
				info_mutex.unlock();
				return _SOCKET_TIMEOUT_ERR_;
			}
			continue;
		}
		if (bytes < _MIN_PACKAGE_WITHOUT_SEQ_LEN_) {
			//printf("DCA1000EVM Packet Not Healthy， Byte: %d！\n", bytes);
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
	client_collection_finish = 1; // 传输完毕

	if (_CLIENT_AVAILABLE_ == _REALTIME_MODE_ && _TELE_ENABLE_ == _TELE_IS_ENABLED_) {
		while (client_collection_enable);
	}


	if (_CLIENT_AVAILABLE_ == _OFFLINE_MODE_ && _TELE_ENABLE_ == _TELE_IS_ENABLED_) {
		if (total_length <= 0) {
			info_mutex.lock();
			info.push_back("DCA1000EVM Start Record: Data length is zero!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
		}
		else {
			ClientHeader = header;
			send_collected_data(total_length); // 离线模式
		}
	}
	else if (_CLIENT_AVAILABLE_ == _REALTIME_MODE_ && _TELE_ENABLE_ == _TELE_IS_ENABLED_) {
		// 发送完成与数据量
		int ret_code = send_real_time_data_length(total_length);
		if (ret_code) {
			return ret_code;
		}
		ret_code = send_real_time_data_complete();
		if (ret_code) {
			return ret_code;
		}
	}

	DataIsReadyToTrigger = false;

	return _NORMAL_;
}

int radar_com::close_dca1000() {
	if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

	sendto(udp_socket, (char *)data, _LEN_DATA_STOP_RECORD_CMD_, 0, (SOCKADDR *)&send_addr, sizeof(send_addr));

	struct sockaddr_in s;
	int ret, addrlen;
	addrlen = sizeof(s);
	ret = recvfrom(udp_socket, (char *)buff, _MAX_RESPONSE_LEN_,
		0, (SOCKADDR *)&s, &addrlen);
	if (ret > 0) {
		int status = cmd_check(buff, CMD_CODE_STOP_RECORD);
		if (status) return status;
	}

	closesocket(udp_socket);
	closesocket(data_socket);

	return _NORMAL_;
}
#else
	void radar_com::connect_udp_port() {
		// 固定好的地址
		data_socket->bind(QHostAddress("192.168.33.180"), 1024);
		recv_socket->bind(QHostAddress("192.168.33.30"), 4098);

		send_socket->bind(QHostAddress("192.168.33.180"), 4096);
		udp_socket->bind(QHostAddress("192.168.33.30"), 4096);
		udp_is_init = true;
	}

	// 重置 DCA1000
	int radar_com::reset_dca1000() {
		if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

		udp_socket->writeDatagram((char *)data, _LEN_RESET_CMD_, send_address, send_port); // 写入数据
		udp_socket->waitForReadyRead(100);//结束发送，等待读取
		QByteArray dataGram = udp_socket->receiveDatagram().data(); // 接收数据
		int ret = dataGram.size();
		memcpy(buff, dataGram.data(), ret);

		if (ret > 0) {
			int status = cmd_check(buff, CMD_CODE_RESET_FPGA);
			if (status) return status;
		}
		else {
			return _SOCKET_NO_RESPONSE_;
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
	int radar_com::configure_dca1000(DCA_FPGA_CONFIG config) {
		if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

		udp_socket->writeDatagram((char *)data, _LEN_CON_FPGA_CMD_, send_address, send_port); // 写入数据
		udp_socket->waitForReadyRead(100);
		QByteArray dataGram = udp_socket->receiveDatagram().data(); // 接收数据
		int ret = dataGram.size();
		memcpy(buff, dataGram.data(), ret);

		if (ret > 0) {
			int status = cmd_check(buff, CMD_CODE_CONFIG_FPGA);
			if (status) return status;
		}
		else {
			return _SOCKET_NO_RESPONSE_;
		}

		printf("DCA1000 FPGA设置完成 模式为：\nLogging 模式： %s\nLVDS 模式： %s\n数据保存模式： %s\n",
			config.eLogMode == 1 ? "Multi Mode" : "Raw Mode",
			config.eLvdsMode == 1 ? "4 Lane" : "2 Lane",
			config.eDataCaptureMode == 1 ? "SD卡保存" : "以太网传输"
		);
		return _NORMAL_;
	}

	// 保持100us以下 // 100us 100Mbps
	// 10us 600Mbps
	int radar_com::set_packet_delay_dca1000(unsigned int us) {
		if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

		udp_socket->writeDatagram((char *)data, _LEN_SET_PACKET_DELAY_CMD_, send_address, send_port); // 写入数据
		udp_socket->waitForReadyRead(100);//结束发送，等待读取
		QByteArray dataGram = udp_socket->receiveDatagram().data(); // 接收数据
		int ret = dataGram.size();
		memcpy(buff, dataGram.data(), ret);

		if (ret > 0) {
			int status = cmd_check(buff, CMD_CODE_CONFIG_RECORD);
			if (status) return status;
		}
		else {
			return _SOCKET_NO_RESPONSE_;
		}

		printf("DCA1000 Packet Delay设置完成 延时为： %d us\n", us);
		return _NORMAL_;
	}

	int radar_com::handshake_dca1000() {
		if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

		udp_socket->writeDatagram((char *)data, _LEN_HANDSHAKE_CMD_, send_address, send_port); // 写入数据
		udp_socket->waitForReadyRead(100);//结束发送，等待读取
		QByteArray dataGram = udp_socket->receiveDatagram().data(); // 接收数据

		int ret = dataGram.size();
		memcpy(buff, dataGram.data(), ret);

		if (ret > 0) {
			int status = cmd_check(buff, CMD_CODE_SYSTEM_ALIVENESS);
			if (status) return status;
		}
		else {
			return _SOCKET_NO_RESPONSE_;
		}

		printf("DCA1000 握手完成\n");
		return _NORMAL_;
	}

	// 实时数据传输线程 如果将其用单线程 会导致UDP丢包 选择使用多线程 防止丢包
	void radar_com::data_record_dca1000_real_time_transfer_thread(LIST_ * header) {
		if (!udp_is_init) {
			printf("RealTime Data Record Client: 数据端口没有打开\n");
			return;
		}
		if (_radar_client_ == NULL) {
			printf("RealTime Data Record Client: 数据端口客户端是NULL指针\n");
			return;
		}

		LIST_ * h = header;

		while (client_collection_enable) {
			if (client_collection_finish && h->next == NULL) {
				break;
			}
			if (h != NULL) {
				ClientHeader = h;
				if (h->data_size == 0) {
					// printf("RealTime Data Record Client: Waiting data...\n");
					continue;
				}
				int ret_code = _radar_client_->send_realtime_data();
				if (ret_code) {
					printf("RealTime Data Record Client: Error at Data Transferring with %d\n",
						ret_code);
				}
			}
			else {
				//_ERROR_
				//printf("RealTime Data Record Client: Error: 数据为空\n");
				return;
			}

			if (h->next == NULL) {
				// printf("RealTime Data Record Client: 下一个所连接的数据为空\n");
				continue;
			}
			else {
				h = h->next;
			}
		}

		client_collection_enable = 0;
		printf("RealTime Data Record Client: 数据传输完成...\n");
	}

	int radar_com::data_record_dca1000(LIST_ * header, unsigned int * data_length) {
		if (!udp_is_init) return _SOCKET_NOT_INIT_;

		DataIsReadyToTrigger = false;
		DataSampleIsTrigger = false;
		client_collection_finish = 0; // 清空标志位
		client_collection_enable = 0; // 初始化标志位

		record_is_err = 0;
		record_is_over = 0;

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
		// 下令开始采集数据

		udp_socket->writeDatagram((char *)data, _LEN_DATA_START_RECORD_CMD_, send_address, send_port); // 写入数据
		udp_socket->waitForReadyRead(100);//结束发送，等待读取
		QByteArray dataGram = udp_socket->receiveDatagram().data(); // 接收数据
		int ret = dataGram.size();
		memcpy(buff, dataGram.data(), ret);

		if (ret > 0) {
			int status = cmd_check(buff, CMD_CODE_START_RECORD);
			if (status)
				record_is_err = 1;
			if (record_is_err)
				return _RECORD_IS_ERR_;
		}
		else {
			header = NULL;
			return _SOCKET_NO_RESPONSE_;
		}

		if (PLAYER == _CLIENT_TRANSMITTER && _CLIENT_AVAILABLE_ == _REALTIME_MODE_) {
			std::thread _thread_transmit_realtime_ = std::thread(&radar_com::data_record_dca1000_real_time_transfer_thread,
				this, header);
			_thread_transmit_realtime_.detach();
		}

		std::thread _thread_trigger_sample_ = std::thread(&radar_com::TriggerDataSample, this);
		_thread_trigger_sample_.detach();
		DataSampleIsTrigger = true;

		*data_length = 0;
		unsigned int total_length = 0;
		LIST_ * h = header;

		unsigned int TIMEOUT_TIMES = 0;
		unsigned char data_buff[_MAX_PACKAGE_WITHOUT_SEQ_LEN_];

		// 准备开始录制
		recv_socket->waitForReadyRead(100); // 100ms

		// 循环等待
		while (1) {
			client_collection_enable = 1;
			DataIsReadyToTrigger = true;

			QByteArray dataGram = recv_socket->receiveDatagram().data(); // 接收数据
			int bytes = dataGram.size();
			if (bytes < 0) {
				break;
			}
			else if (bytes == 0) {
				QByteArray udp_data = udp_socket->receiveDatagram().data(); // 接收数据
				if (udp_data.size() > 0) {
					int status = cmd_check((unsigned char *)udp_data.data(), CMD_CODE_SYSTEM_ASYNC_STATUS);
					if (status) {
						record_is_over = 1;
							record_is_over);
					}
					break;
				}
				continue; // 没有数据到来
			}
			if (bytes < _MIN_PACKAGE_WITHOUT_SEQ_LEN_) {
				printf("DCA1000EVM Packet Not Healthy， Byte: %d！\n", bytes);
			}
			else if (bytes > _MAX_PACKAGE_WITHOUT_SEQ_LEN_) {
				printf("DCA1000EVM Packet Not Healthy， Byte: %d！\n", bytes);
			}

			memcpy(data_buff, dataGram.data(), bytes);
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
		client_collection_finish = 1; // 传输完毕

		if (_CLIENT_AVAILABLE_ == _REALTIME_MODE_) {
			while (client_collection_enable);
		}

		if (_CLIENT_AVAILABLE_ == _OFFLINE_MODE_ && PLAYER == _CLIENT_TRANSMITTER) {
			ClientHeader = header;
			if (_radar_client_ == NULL) {
				return _CLIENT_NOT_INITIALIZED_;
			}
			_radar_client_->send_offine_data(); // 离线模式
		}
		else if (_CLIENT_AVAILABLE_ == _REALTIME_MODE_ && PLAYER == _CLIENT_TRANSMITTER) {
			// 发送完成与数据量
			if (_radar_client_ == NULL) {
				return _CLIENT_NOT_INITIALIZED_;
			}
			int ret_code = _radar_client_->send_collection_length(total_length);
			if (ret_code) {
				return ret_code;
			}
			ret_code = _radar_client_->send_collection_complete();
			if (ret_code) {
				return ret_code;
			}
		}

		return _NORMAL_;
	}

	int radar_com::close_dca1000() {
		if (!udp_is_init) return _SOCKET_NOT_INIT_;

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

		udp_socket->writeDatagram((char *)data, _LEN_HANDSHAKE_CMD_, send_address, send_port); // 写入数据
		udp_socket->waitForReadyRead(100);
		QByteArray dataGram = udp_socket->receiveDatagram().data(); // 接收数据

		int ret = dataGram.size();
		memcpy(buff, dataGram.data(), ret);

		if (ret > 0) {
			int status = cmd_check(buff, CMD_CODE_STOP_RECORD);
			if (status) return status;
		}
		else {
			return _SOCKET_NO_RESPONSE_;
		}

		return _NORMAL_;
	}
#endif

#endif
