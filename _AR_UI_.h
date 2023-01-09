#pragma once

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QValueAxis>
#include <QtWidgets/QMainWindow>
#include <QPalette>
#include <QDesktopWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QTimer>
#include "ui__AR_UI_.h"
#include "_AR_include_/tcp_server_build.h" // 服务器定义部分
#include "_AR_include_/tcp_client_build.h" // 客户端定义部分
#include "_AR_include_/_AR_UI_INFO_SUBSYSTEM_.h" // UI显示部分
#include "_AR_define_/TCP_function_define.h" // 本程序功能定义部分
#include "_AR_define_/_net_work_define_.h" // 网络IP、端口定义
#include "_AR_define_/_AR_actor_define_.h" // 全局服务器与客户端定义
#include "_AR_define_/_AR_UI_define_.h" // 界面指令定义
#include "_AR_Comm_/_AR_dca1000_rf_api.h" // DCA1000通信部分
#include "_AR_decode/_wave_AR_decode_.h" // 解释波形
#include "_AR_ADDONs_/IPSetWidget.h" // 设置IP
#include <mutex> // 互斥锁定义

/* =====================================全局的定义============================================== */
#if not defined _WIN32
extern radar_client * _radar_client_; // 全局预先定义
extern radar_server * _radar_server_; // 全局预先定义
#endif

/* =============================字符显示区域变量定义============================= */
extern QColor _f_info; // 信息的颜色
extern QColor _f_warn; // 警告的颜色
extern QColor _f_err; // 错误的颜色
extern QColor _f_background; //  背景的颜色
extern std::vector <QByteArray> info; // 全局定义
extern std::vector <LEVEL> warn; // 警告程度

/* ============================INFO互斥锁定义=============================== */
std::mutex info_mutex;

/* ==============================Qt 表格定义=============================== */
QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

/* =================================主页面定义=================================== */
extern _AR_radar_buff_ * _AR_RADAR_WAVE_; // 解析到的波形
BOOL chan_data_stop = 1;
BOOL new_wave_data = 0;
extern BOOL chan_data_is_ready;
extern unsigned int _CHIRP_NUMBER_;
extern unsigned int _ADC_NUMBER_;
extern unsigned int _antenna_num_;

/* ================================指令定义===================================== */
extern unsigned char Command;
extern unsigned char Reply;

/* =================================客户端列表==================================== */
extern SERV_CON_LIST * header;

/* =================================功能部分====================================== */
extern unsigned int _TELE_ENABLE_; // 是否開啓回傳

class _AR_UI_ : public QMainWindow
{
    Q_OBJECT

signals:
	void start_record();

public:
    _AR_UI_(QWidget *parent = Q_NULLPTR);
	void update_info_box(QColor backgroundColor=_f_background);
	void listen_extern_command();

public:
	radar_com * _radar_com_ = NULL; // 雷达通信部分
	QChart * _wave_chart_ = NULL;
	QChartView * _chart_view_ = NULL; // 显示波形
	QSplineSeries * _wave_form_ = NULL;
	QValueAxis * axisX = NULL;
	QValueAxis * axisY = NULL;
	QTimer * _timer_wave_update_ = NULL;
	IPSetWidget * _ip_set_ = NULL;
	bool axis_is_initialized = false;

private:
	// 链表数据定义
	LIST_ * header = NULL;
	// 收到的字节数目
	UINT32 data_length = 0;

/* =================================界面的定义=================================== */
private:
	int _original_window_x_ = 1920; // 窗口的宽度
	int _original_window_y_ = 1080; // 窗口的高度

	int _exit_original_x_ = 1753; // 原始的X坐标
	int _exit_original_y_ = 716; // 原始的Y坐标
	int _exit_button_w_ = 150; // 原始退出按钮的宽度
	int _exit_button_h_ = 350; // 原始退出按钮的高度

	int _con_original_x_ = 1255; // 原始服务器连接按钮X坐标
	int _con_original_y_ = 985; // 原始服务器连接按钮Y坐标
	int _con_button_w_ = 215; // 原始服务器连接按钮宽度
	int _con_button_h_ = 60; // 原始服务器连接按钮高度

	int _ser_original_x_ = 1500; // 原始界面工作模式按钮X坐标
	int _ser_original_y_ = 985; // 原始界面工作模式按钮Y坐标
	int _ser_button_w_ = 215; // 原始界面工作模式按钮宽度
	int _ser_button_h_ = 60; // 原始界面工作模式按钮高度

	int _l_con_original_x_ = 1250; // 连接按钮X坐标
	int _l_con_original_y_ = 980; // 连接按钮Y坐标
	int _l_con_button_w_ = 225; // 连接按钮宽度
	int _l_con_button_h_ = 70; // 连接按钮高度

	int _l_ser_original_x_ = 1495; // 工作模式按钮X坐标
	int _l_ser_original_y_ = 980; // 工作模式按钮Y坐标
	int _l_ser_button_w_ = 225; // 工作模式按钮宽度
	int _l_ser_button_h_ = 70; // 工作模式按钮高度

	int _info_original_x_ = 40; // 原始界面提示框X坐标
	int _info_original_y_ = 800; // 原始界面提示框Y坐标
	int _info_width_ = 510; // 原始界面提示框宽度
	int _info_height_ = 240; // 原始界面提示框高度

	int _ip_origin_x_ = 1420; // IP提示框X坐标
	int _ip_origin_y_ = 805; // IP提示框Y坐标
	int _ip_width_ = 290; // IP提示框宽度
	int _ip_height_ = 60; // IP提示框高度

	int _port_origin_x_ = 1420; // 端口提示框X坐标
	int _port_origin_y_ = 885; // 端口提示框Y坐标
	int _port_width_ = 290; // 端口提示框宽度
	int _port_height_ = 60; // 端口提示框高度

	int _record_origin_x_ = 1730; // 录制的按钮
	int _record_origin_y_ = 630;
	int _record_width_ = 160;
	int _record_height_ = 60;

	int _save_path_origin_x_ = 1240; // 保存的路径
	int _save_path_origin_y_ = 630;
	int _save_path_width_ = 480;
	int _save_path_height_ = 60;

	int _path_origin_x_ = 1250; // 位置显示区
	int _path_origin_y_ = 640;
	int _path_width_ = 460;
	int _path_height_ = 40;

	int _TX1_ENABLE_x_ = 1325;
	int _TX1_ENABLE_y_ = 100;
	int _TX1_ENABLE_w_ = 70;
	int _TX1_ENABLE_h_ = 40;

	int _TX2_ENABLE_x_ = 1475;
	int _TX2_ENABLE_y_ = 100;
	int _TX2_ENABLE_w_ = 70;
	int _TX2_ENABLE_h_ = 40;

	int _TX3_ENABLE_x_ = 1625;
	int _TX3_ENABLE_y_ = 100;
	int _TX3_ENABLE_w_ = 70;
	int _TX3_ENABLE_h_ = 40;

	int _ADC_input_x_ = 1405; // ADC采样输入框位置
	int _ADC_input_y_ = 155;
	int _ADC_input_w_ = 75;
	int _ADC_input_h_ = 55;

	int _CHIRP_input_x_ = 1588; // Chirps输入框位置
	int _CHIRP_input_y_ = 155;
	int _CHIRP_input_w_ = 75;
	int _CHIRP_input_h_ = 55;

	int _FRAME_input_x_ = 1782; // Frame输入框位置
	int _FRAME_input_y_ = 155;
	int _FRAME_input_w_ = 75;
	int _FRAME_input_h_ = 55;

	int _AR_CURV_x_ = 35;  // 曲线
	int _AR_CURV_y_ = 90;
	int _AR_CURV_w_ = 1160;
	int _AR_CURV_h_ = 620;

	int _AR_CURV_offsetX_ = 10; // 按钮偏移
	int _AR_CURV_offsetY_ = 10;
	int _AR_CURV_offsetW_ = -20;
	int _AR_CURV_offsetH_ = -20;

	int _AR_ENABLE_TELE_x_ = 1635; // 回傳啓用定義
	int _AR_ENABLE_TELE_y_ = 290;
	int _AR_ENABLE_TELE_w_ = 240;
	int _AR_ENABLE_TELE_h_ = 110;

private:
    Ui::_AR_UI_Class ui;
	void _AR_UI_init(); // 初始化
	void _Connect_COM_(); // 连接串口
	void _Connect_UDP_(); // 连接UDP端口
	void _DisConnect_UDP_(); // 断开UDP端口
	unsigned int _AR_UI_CNT_ = 0;
	bool disable_tx1_ = false;
	bool disable_tx2_ = false;
	bool disable_tx3_ = false; // 不要使用TX1、TX2、TX3 true的时候为不使用

	bool _com_connected_ = false;
	bool _udp_connected_ = false;
	bool _tcp_connected_ = false; // 连接是否被建立

	QString save_dir = "";
	QString file_dir = "0.bin";
	unsigned int record_num = 0;
	QString file_addr = ".bin";

	bool com_write_initialized = false;
	void _write_cfg_();
	bool _wait_done_();
	
signals:
	void _info_is_coming_(); // 准备显示数据

public slots:
	void _AR_UI_exit(); // 退出界面
	void _AR_UI_::resizeEvent(QResizeEvent * event); // 重载窗口大小的子函数

	// 切换模式
	void _AR_UI_CHANGE_MODE_(); // 如果是0：0：DISABLED 正常采集 如果是1：监听 LISTEN 1 或者停止监听 0 如果是2：连接或者断开
	// 切换工作单位
	void _AR_UI_CHANGE_ACTOR_(); // 0: 正常采集 1：服务器 2：客户端
	void _Set_Save_Dir_(); // 设置保存文件夹路径
	void _AR_UI_START_RECORD_(); // 开始录制雷达数据
	void _AR_UI_CHANGE_TX1_ENABLE_(); // 改变TX1状态
	void _AR_UI_CHANGE_TX2_ENABLE_(); // 改变TX1状态
	void _AR_UI_CHANGE_TX3_ENABLE_(); // 改变TX1状态
	void _AR_UI_Get_data_and_Draw(); // 绘制数据
	void _AR_UI_Show_IP_Config(); // 改变IP地址
	void _AR_UI_Change_IP_Button(); // 改变IP地址
	void _AR_UI_Show_Info(); // 准备显示数据
	void _AR_UI_Change_TELE_MODE(); // 調整是否接受回傳
	void _AR_UI_UPDATE_TELE_MODE(); // 上傳回傳狀態
};
