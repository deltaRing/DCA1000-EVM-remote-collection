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
#include "_AR_include_/tcp_server_build.h" // ���������岿��
#include "_AR_include_/tcp_client_build.h" // �ͻ��˶��岿��
#include "_AR_include_/_AR_UI_INFO_SUBSYSTEM_.h" // UI��ʾ����
#include "_AR_define_/TCP_function_define.h" // �������ܶ��岿��
#include "_AR_define_/_net_work_define_.h" // ����IP���˿ڶ���
#include "_AR_define_/_AR_actor_define_.h" // ȫ�ַ�������ͻ��˶���
#include "_AR_define_/_AR_UI_define_.h" // ����ָ���
#include "_AR_Comm_/_AR_dca1000_rf_api.h" // DCA1000ͨ�Ų���
#include "_AR_decode/_wave_AR_decode_.h" // ���Ͳ���
#include "_AR_ADDONs_/IPSetWidget.h" // ����IP
#include <mutex> // ����������

/* =====================================ȫ�ֵĶ���============================================== */
#if not defined _WIN32
extern radar_client * _radar_client_; // ȫ��Ԥ�ȶ���
extern radar_server * _radar_server_; // ȫ��Ԥ�ȶ���
#endif

/* =============================�ַ���ʾ�����������============================= */
extern QColor _f_info; // ��Ϣ����ɫ
extern QColor _f_warn; // �������ɫ
extern QColor _f_err; // �������ɫ
extern QColor _f_background; //  ��������ɫ
extern std::vector <QByteArray> info; // ȫ�ֶ���
extern std::vector <LEVEL> warn; // ����̶�

/* ============================INFO����������=============================== */
std::mutex info_mutex;

/* ==============================Qt �����=============================== */
QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

/* =================================��ҳ�涨��=================================== */
extern _AR_radar_buff_ * _AR_RADAR_WAVE_; // �������Ĳ���
BOOL chan_data_stop = 1;
BOOL new_wave_data = 0;
extern BOOL chan_data_is_ready;
extern unsigned int _CHIRP_NUMBER_;
extern unsigned int _ADC_NUMBER_;
extern unsigned int _antenna_num_;

/* ================================ָ���===================================== */
extern unsigned char Command;
extern unsigned char Reply;

/* =================================�ͻ����б�==================================== */
extern SERV_CON_LIST * header;

/* =================================���ܲ���====================================== */
extern unsigned int _TELE_ENABLE_; // �Ƿ��_���؂�

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
	radar_com * _radar_com_ = NULL; // �״�ͨ�Ų���
	QChart * _wave_chart_ = NULL;
	QChartView * _chart_view_ = NULL; // ��ʾ����
	QSplineSeries * _wave_form_ = NULL;
	QValueAxis * axisX = NULL;
	QValueAxis * axisY = NULL;
	QTimer * _timer_wave_update_ = NULL;
	IPSetWidget * _ip_set_ = NULL;
	bool axis_is_initialized = false;

private:
	// �������ݶ���
	LIST_ * header = NULL;
	// �յ����ֽ���Ŀ
	UINT32 data_length = 0;

/* =================================����Ķ���=================================== */
private:
	int _original_window_x_ = 1920; // ���ڵĿ��
	int _original_window_y_ = 1080; // ���ڵĸ߶�

	int _exit_original_x_ = 1753; // ԭʼ��X����
	int _exit_original_y_ = 716; // ԭʼ��Y����
	int _exit_button_w_ = 150; // ԭʼ�˳���ť�Ŀ��
	int _exit_button_h_ = 350; // ԭʼ�˳���ť�ĸ߶�

	int _con_original_x_ = 1255; // ԭʼ���������Ӱ�ťX����
	int _con_original_y_ = 985; // ԭʼ���������Ӱ�ťY����
	int _con_button_w_ = 215; // ԭʼ���������Ӱ�ť���
	int _con_button_h_ = 60; // ԭʼ���������Ӱ�ť�߶�

	int _ser_original_x_ = 1500; // ԭʼ���湤��ģʽ��ťX����
	int _ser_original_y_ = 985; // ԭʼ���湤��ģʽ��ťY����
	int _ser_button_w_ = 215; // ԭʼ���湤��ģʽ��ť���
	int _ser_button_h_ = 60; // ԭʼ���湤��ģʽ��ť�߶�

	int _l_con_original_x_ = 1250; // ���Ӱ�ťX����
	int _l_con_original_y_ = 980; // ���Ӱ�ťY����
	int _l_con_button_w_ = 225; // ���Ӱ�ť���
	int _l_con_button_h_ = 70; // ���Ӱ�ť�߶�

	int _l_ser_original_x_ = 1495; // ����ģʽ��ťX����
	int _l_ser_original_y_ = 980; // ����ģʽ��ťY����
	int _l_ser_button_w_ = 225; // ����ģʽ��ť���
	int _l_ser_button_h_ = 70; // ����ģʽ��ť�߶�

	int _info_original_x_ = 40; // ԭʼ������ʾ��X����
	int _info_original_y_ = 800; // ԭʼ������ʾ��Y����
	int _info_width_ = 510; // ԭʼ������ʾ����
	int _info_height_ = 240; // ԭʼ������ʾ��߶�

	int _ip_origin_x_ = 1420; // IP��ʾ��X����
	int _ip_origin_y_ = 805; // IP��ʾ��Y����
	int _ip_width_ = 290; // IP��ʾ����
	int _ip_height_ = 60; // IP��ʾ��߶�

	int _port_origin_x_ = 1420; // �˿���ʾ��X����
	int _port_origin_y_ = 885; // �˿���ʾ��Y����
	int _port_width_ = 290; // �˿���ʾ����
	int _port_height_ = 60; // �˿���ʾ��߶�

	int _record_origin_x_ = 1730; // ¼�Ƶİ�ť
	int _record_origin_y_ = 630;
	int _record_width_ = 160;
	int _record_height_ = 60;

	int _save_path_origin_x_ = 1240; // �����·��
	int _save_path_origin_y_ = 630;
	int _save_path_width_ = 480;
	int _save_path_height_ = 60;

	int _path_origin_x_ = 1250; // λ����ʾ��
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

	int _ADC_input_x_ = 1405; // ADC���������λ��
	int _ADC_input_y_ = 155;
	int _ADC_input_w_ = 75;
	int _ADC_input_h_ = 55;

	int _CHIRP_input_x_ = 1588; // Chirps�����λ��
	int _CHIRP_input_y_ = 155;
	int _CHIRP_input_w_ = 75;
	int _CHIRP_input_h_ = 55;

	int _FRAME_input_x_ = 1782; // Frame�����λ��
	int _FRAME_input_y_ = 155;
	int _FRAME_input_w_ = 75;
	int _FRAME_input_h_ = 55;

	int _AR_CURV_x_ = 35;  // ����
	int _AR_CURV_y_ = 90;
	int _AR_CURV_w_ = 1160;
	int _AR_CURV_h_ = 620;

	int _AR_CURV_offsetX_ = 10; // ��ťƫ��
	int _AR_CURV_offsetY_ = 10;
	int _AR_CURV_offsetW_ = -20;
	int _AR_CURV_offsetH_ = -20;

	int _AR_ENABLE_TELE_x_ = 1635; // �؂����ö��x
	int _AR_ENABLE_TELE_y_ = 290;
	int _AR_ENABLE_TELE_w_ = 240;
	int _AR_ENABLE_TELE_h_ = 110;

private:
    Ui::_AR_UI_Class ui;
	void _AR_UI_init(); // ��ʼ��
	void _Connect_COM_(); // ���Ӵ���
	void _Connect_UDP_(); // ����UDP�˿�
	void _DisConnect_UDP_(); // �Ͽ�UDP�˿�
	unsigned int _AR_UI_CNT_ = 0;
	bool disable_tx1_ = false;
	bool disable_tx2_ = false;
	bool disable_tx3_ = false; // ��Ҫʹ��TX1��TX2��TX3 true��ʱ��Ϊ��ʹ��

	bool _com_connected_ = false;
	bool _udp_connected_ = false;
	bool _tcp_connected_ = false; // �����Ƿ񱻽���

	QString save_dir = "";
	QString file_dir = "0.bin";
	unsigned int record_num = 0;
	QString file_addr = ".bin";

	bool com_write_initialized = false;
	void _write_cfg_();
	bool _wait_done_();
	
signals:
	void _info_is_coming_(); // ׼����ʾ����

public slots:
	void _AR_UI_exit(); // �˳�����
	void _AR_UI_::resizeEvent(QResizeEvent * event); // ���ش��ڴ�С���Ӻ���

	// �л�ģʽ
	void _AR_UI_CHANGE_MODE_(); // �����0��0��DISABLED �����ɼ� �����1������ LISTEN 1 ����ֹͣ���� 0 �����2�����ӻ��߶Ͽ�
	// �л�������λ
	void _AR_UI_CHANGE_ACTOR_(); // 0: �����ɼ� 1�������� 2���ͻ���
	void _Set_Save_Dir_(); // ���ñ����ļ���·��
	void _AR_UI_START_RECORD_(); // ��ʼ¼���״�����
	void _AR_UI_CHANGE_TX1_ENABLE_(); // �ı�TX1״̬
	void _AR_UI_CHANGE_TX2_ENABLE_(); // �ı�TX1״̬
	void _AR_UI_CHANGE_TX3_ENABLE_(); // �ı�TX1״̬
	void _AR_UI_Get_data_and_Draw(); // ��������
	void _AR_UI_Show_IP_Config(); // �ı�IP��ַ
	void _AR_UI_Change_IP_Button(); // �ı�IP��ַ
	void _AR_UI_Show_Info(); // ׼����ʾ����
	void _AR_UI_Change_TELE_MODE(); // �{���Ƿ���ܻ؂�
	void _AR_UI_UPDATE_TELE_MODE(); // �ς��؂���B
};
