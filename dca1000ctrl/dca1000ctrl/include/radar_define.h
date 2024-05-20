#ifndef _RADAR_DEFINE_H_
#define _RADAR_DEFINE_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>
#include <thread>
#include "linked_list.h"
#include "udp_define.h"

using namespace std;

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
#define _MAX_TIMEOUT_TIMES_ 3
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
#define _SOCKET_SEND_ERR_ -13
#define _MAX_COM_LEN_ 512

#define _COM_NORMAL_ 0
#define _COM_FAILURE_ 1

// 数据的长度
#define _MIN_PACKAGE_WITHOUT_SEQ_LEN_ 48
#define _MAX_PACKAGE_WITHOUT_SEQ_LEN_ 1466
#define _PACKAGE_NUMBER_SEQ_LEN_ 10

// 定义的DCA1000EVM 配置的结构体
typedef struct DCA1000EVM_fpga_configure {
	unsigned char eLogMode; // Data log mode Raw Mode = 1; Multi Mode = 2
	unsigned char eLvdsMode; // Data LVDS mode Four Lane = 1; Two Lane = 2
	unsigned char eDataXferMode; // Data transfer mode Capture = 1; PlayBack = 2
	unsigned char eDataCaptureMode; // Data capture mode SD_STORAGE = 1; ETH = 2
	unsigned char eDataFormatMode; // Data format mode Bit12 = 1 Bit14 = 2 Bit16 = 3
	unsigned char u8Timer; // Timeout value for LVDS data default = 30;
} DCA_FPGA_CONFIG;

using namespace LibSerial;

void close_port();
int connect_com_port(std::string port); // 连接串口
void WritePortLD(std::string data); // 写入雷达串口
std::string ReadPortLD(); // 读取雷达串口
bool WriteCFGFile(std::string cfg_path); // 读取配置文件
// 检查指令是否相同
int cmd_check(unsigned char * _response_, int cmd); // 检查命令
// 保存数据：输入：List头 保存文件名字 数据长度 是否删除header
int save_data(LIST_ * header, const char * name, bool delete_header, unsigned int data_length);
// 开启数据接受端口
int connect_record_port();
// 开启系统设置端口
int addr_init();
// 重置 DCA1000
int reset_dca1000();
// Data log mode Raw Mode = 1; Multi Mode = 2 default =1
// Data LVDS mode Four Lane = 1; Two Lane = 2 default =2
// Data transfer mode Capture = 1; PlayBack = 2 default=1
// Data capture mode SD_STORAGE = 1; ETH = 2 default=2
// Data format mode Bit12 = 1 Bit14 = 2 Bit16 = 3 default=3
// Timeout value for LVDS data default = 30 default=30
int configure_dca1000(DCA_FPGA_CONFIG config);
// 保持100us以下 // 100us 100Mbps
// 10us 600Mbps
int set_packet_delay_dca1000(unsigned int us);
int handshake_dca1000();
void data_record_dca1000_finish_detect_thread();
void TriggerDataSample();
int data_record_dca1000(LIST_ * header, unsigned int * data_length);
int close_dca1000();

#endif
