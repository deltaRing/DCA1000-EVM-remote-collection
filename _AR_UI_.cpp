#include "_AR_UI_.h"

extern unsigned int _OPEN_;
extern unsigned int PLAYER;
extern unsigned int _CLIENT_AVAILABLE_;
extern unsigned int PLAYER;

#if defined _WIN32
extern BOOL server_is_closed;
extern SOCKET server;
extern SOCKET clientSocket;
#endif

_AR_UI_::_AR_UI_(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	_AR_UI_init(); // 初始化界面

	_radar_com_ = new radar_com();
	_wave_chart_ = new QChart();
	_wave_chart_->setTheme(QChart::ChartThemeDark);
	_wave_chart_->setDropShadowEnabled(true);//背景阴影 
	_chart_view_ = new QChartView(_wave_chart_);
	_chart_view_->setRenderHint(QPainter::Antialiasing);
	axisX = new QValueAxis();
	axisY = new QValueAxis();
	ui._AR_UI_CURV_VIEW_->addWidget(_chart_view_);
	ui._AR_UI_CURV_VIEW_->setCurrentIndex(0); // 表格页面
	_timer_wave_update_ = new QTimer();
	_timer_wave_update_->start(1000); // 1s 更新一次数据
	_wave_form_ = new QSplineSeries();
	_wave_form_->setColor(QColor(0, 255, 0));
	_wave_chart_->addSeries(_wave_form_);
	
	_ip_set_ = new IPSetWidget(NULL);
	_ip_set_->hide();

	/*_radar_com_->connect_com_port();
	_radar_com_->OpenCfgFile();
	_radar_com_->WriteCfgFile();*/

#if defined _WIN32

	/*status = _radar_com_->connect_record_port();
	if (status) {
		printf("Error at DCA1000 init connect record port");
	}
	status = _radar_com_->addr_init();
	if (status) {
		printf("Error at DCA1000 init connect system configure port");
	}
	status = _radar_com_->reset_dca1000();
	if (status) {
		printf("Error at DCA1000 init reset capture board");
	}
	status = _radar_com_->configure_dca1000(config);
	if (status) {
		printf("Error at DCA1000 init configuration");
	}
	status = _radar_com_->set_packet_delay_dca1000(40);
	if (status) {
		printf("Error at DCA1000 set packet delay");
	}

	LIST_ * header = NULL;
	UINT32 data_length = 0;
	header = list_init();

	status = _radar_com_->data_record_dca1000(header, &data_length);
	if (!status) {
		printf("自定义代码 at DCA_CONTROL_C： DCA1000EVM 采集完毕！\n");
	}

	// 默认删除数据
	save_data(header, "ADC_data3.bin", 1, data_length);
	
	_radar_com_->close_dca1000();*/

#else
	status = _radar_com_->reset_dca1000();
	if (status) {
		printf("Error at DCA1000 init reset capture board");
	}
	//  配置DCA1000
	status = _radar_com_->configure_dca1000(config);
	if (status) {
		printf("Error at DCA1000 init configuration");
	}
	status = _radar_com_->set_packet_delay_dca1000(75);
	if (status) {
		printf("Error at DCA1000 set packet delay");
	}
	status = _radar_com_->handshake_dca1000();
	if (status) {
		printf("Error at Handshake");
	}	
	LIST_ * header = NULL;
	unsigned int data_length = 0;
	header = list_init();

	status = _radar_com_->data_record_dca1000(header, &data_length);
	if (status) {
		printf("自定义代码 at DCA_CONTROL_C： DCA1000EVM 采集完毕！\n");
	}

	// 默认删除数据
	save_data(header, "ADC_data3.bin", data_length, true);
	_radar_com_->close_dca1000();
#endif

	std::thread _update_info_box_thread_ = std::thread(&_AR_UI_::update_info_box, 
														this,
														_f_background);
	_update_info_box_thread_.detach();

	std::thread _update_client_command_ = std::thread(&_AR_UI_::listen_extern_command, this);
	_update_client_command_.detach();

	connect(ui._AR_UI_EXIT, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_exit);
	connect(ui._AR_UI_CON, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_CHANGE_MODE_);
	connect(ui._AR_UI_SERC, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_CHANGE_ACTOR_);
	connect(ui._AR_UI_SAMPLE_CHANGEPATH_, &QPushButton::clicked,
		this, &_AR_UI_::_Set_Save_Dir_);
	connect(ui._AR_TX1_ENABLE_, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_CHANGE_TX1_ENABLE_);
	connect(ui._AR_TX2_ENABLE, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_CHANGE_TX2_ENABLE_);
	connect(ui._AR_TX3_ENABLE, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_CHANGE_TX3_ENABLE_);
	connect(ui._AR_UI_SAMPLE_RECORD_, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_START_RECORD_);
	connect(this->_timer_wave_update_, &QTimer::timeout,
		this, &_AR_UI_::_AR_UI_Get_data_and_Draw);
	connect(ui._AR_UI_IP_SELECT_, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_Show_IP_Config);
	connect(ui._AR_UI_PORT_SELECT_, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_Show_IP_Config);
	connect(this->_ip_set_, &IPSetWidget::ip_changed,
		this, &_AR_UI_::_AR_UI_Change_IP_Button);
	connect(this, &_AR_UI_::start_record,
		this, &_AR_UI_::_AR_UI_START_RECORD_);
	connect(this, &_AR_UI_::_info_is_coming_,
		this, &_AR_UI_::_AR_UI_Show_Info);
	connect(ui._AR_TELE_ENABLE_, &QPushButton::clicked,
		this, &_AR_UI_::_AR_UI_Change_TELE_MODE); 
	connect(ui._AR_UI_SERC, &QPushButton::clicked,
			this, &_AR_UI_::_AR_UI_UPDATE_TELE_MODE); // 切換功能的時候 檢測一下Tele狀態
}

// 准备显示消息
void _AR_UI_::_AR_UI_Show_Info() {
	info_mutex.lock();
	if (info.size() > 0 && warn.size() > 0) {
		_AR_UI_CNT_++;
		QByteArray data;
		std::string data_temp(info[0]);
		data = data_temp.c_str();
		LEVEL l_data = warn[0];
		QTextCharFormat fmt;
		QColor f_color;
		if (l_data == _UI_INFO_) {
			f_color = _f_info;
		}
		else if (l_data == _UI_WARN_) {
			f_color = _f_warn;
		}
		else if (l_data == _UI_ERR_) {
			f_color = _f_err;
		}
		else {
			f_color = _f_info;
		}
		fmt.setForeground(f_color);
		ui._AR_UI_INFOVIEW->mergeCurrentCharFormat(fmt);
		ui._AR_UI_INFOVIEW->appendPlainText(data);
		info.erase(info.begin());
		warn.erase(warn.begin());
	}
	info_mutex.unlock();
}

// 監聽外部的消息
void _AR_UI_::listen_extern_command() { // 监听外部命令
	while (true) { // 重复该循环
		if (Command == _NON_COMMAND_) {

		}
		else if (Command == _RECORD_COMMAND_) {
			Command = _NON_COMMAND_;
			emit start_record();
		}
		else if (Command == _EXIT_COMMAND_) {
			exit(0);
		}
		else {
			// Unknown Command
			Command = _NON_COMMAND_;;
		}
	}
}

// 按钮改变IP地址
void _AR_UI_::_AR_UI_Change_IP_Button() {
	ui._AR_UI_IP_SELECT_->setText(IP);
	ui._AR_UI_PORT_SELECT_->setText(Port);
}

// 改变IP地址
void _AR_UI_::_AR_UI_Show_IP_Config(){
	_ip_set_->show();
}

// 断开UDP端口
void _AR_UI_::_DisConnect_UDP_() {
	if (!_udp_connected_) {
		info_mutex.lock();
		info.push_back("UI UDP disconnect: UDP is not connected \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
	}
	else {
		_radar_com_->close_dca1000();
		_udp_connected_ = false;
	}
}

// 连接串口
void _AR_UI_::_Connect_COM_() 
{
	_radar_com_->connect_com_port();
	if (!_radar_com_->ComIsOpen) {
		info_mutex.lock();
		info.push_back("UI Com Connect: Com open failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
	}
	else {
		_com_connected_ = true; // 别随便拔插USB！没做保护程序！
	}
}

// 连接UDP端口
void _AR_UI_::_Connect_UDP_()
{
#if defined _WIN32
	int status = 0;

	DCA_FPGA_CONFIG config;
	config.eLogMode = 1;
	config.eLvdsMode = 2;
	config.eDataXferMode = 1;
	config.eDataCaptureMode = 2;
	config.eDataFormatMode = 3;
	config.u8Timer = 50;

	status = _radar_com_->connect_record_port();
	if (status) {
		info_mutex.lock();
		info.push_back("UI UDP Connect: Error at DCA1000 init connect record port \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}
	status = _radar_com_->addr_init();
	if (status) {
		info_mutex.lock();
		info.push_back("UI UDP Connect: Error at DCA1000 init connect system configure port \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}
	status = _radar_com_->reset_dca1000();
	if (status) {
		info_mutex.lock();
		info.push_back("UI UDP Connect: Error at DCA1000 init reset capture board \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}
	status = _radar_com_->configure_dca1000(config);
	if (status) {
		info_mutex.lock();
		info.push_back("UI UDP Connect: Error at DCA1000 init configuration \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}
	status = _radar_com_->set_packet_delay_dca1000(50);
	if (status) {
		info_mutex.lock();
		info.push_back("UI UDP Connect: Error at DCA1000 set packet delay \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_udp_connected_ = true;
#else
	info.push_back("UI UDP Connect: Not Supported \n");
	warn.push_back(_UI_ERR_);
#endif
}

// 等待Done 信号
bool _AR_UI_::_wait_done_() {
	unsigned int found_result = 100;
	while (found_result) {
		Sleep(125);
		_radar_com_->ReadPortLD();
		if (!_radar_com_->com_port_data.contains("Done")) {
			// 没有接收到Done提示符
			found_result--;
			continue;
		}
		else {
			break;
		}
		if (!_radar_com_->com_port_data.contains("Error")) {
			// 接收到了错误
			return false;
		}
	}
	if (found_result == 0) {
		return false;
	}
	return true;
}

// 写入数据
void _AR_UI_::_write_cfg_() {
	this->com_write_initialized = true;
	_radar_com_->ReadPortLD(); // clear buffer 
	_radar_com_->WritePortLD("dfeDataOutputMode 1\n");
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write dfeDataOutputMode Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("channelCfg 15 7 0\n"); // 使用4个RX
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write channelCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("adcCfg 2 1\n"); // ADC 默认用16bit的 并且 complex 1x
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write adcCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("adcbufCfg -1 0 1 1 1\n"); // 使用复数模式
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write adcbufCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	QByteArray temp;
	// 如果用6843 请将77改为64
	temp = QByteArray("profileCfg 0 77 7 7 58 0 0 68 1 ");
	int ADCNum = 256; // default
	bool AdcData_isOK = false;
	ui._AR_UI_SAMPLE_ADCNUM_->text().toInt(&AdcData_isOK);
	if (!AdcData_isOK) {
		info_mutex.lock();
		info.push_back("UI Write CFG: ADC data number is not healthy \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
	}
	else {
		if (ui._AR_UI_SAMPLE_ADCNUM_->text().toInt() <= 0) {
			info_mutex.lock();
			info.push_back("UI Write CFG: ADC data number is not healthy \n");
			warn.push_back(_UI_WARN_);
			info_mutex.unlock();
		}
		else {
			ADCNum = ui._AR_UI_SAMPLE_ADCNUM_->text().toInt();
			info_mutex.lock();
			info.push_back("UI Write CFG: ADC data number is set \n");
			warn.push_back(_UI_INFO_);
			info_mutex.unlock();
		}
	}
	_change_adc_number = ADCNum;
	temp.append(QString::fromStdString(std::to_string(ADCNum)));
	temp.append(" 5500 0 0 30\n");
	_radar_com_->WritePortLD(temp); // 写入采集模式
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write profileCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	if (!disable_tx1_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("chirpCfg 0 0 0 0 0 0 0 1\n"); // 使用复数模式
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write TX1 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}

	if (!disable_tx2_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("chirpCfg 1 1 0 0 0 0 0 2\n"); // 使用复数模式
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write TX2 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}

	if (!disable_tx3_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("chirpCfg 2 2 0 0 0 0 0 4\n"); // 使用复数模式
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write TX3 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}

	temp = QByteArray("frameCfg 0 2 ");
	int chirp_num = 64;
	int frame_num = 256;
	bool ChirpNum_isOK = false, FrameNum_isOK = false;
	ui._AR_UI_SAMPLE_CHIRPS_->text().toInt(&ChirpNum_isOK);
	ui._AR_UI_SAMPLE_FRAMES_->text().toInt(&FrameNum_isOK); // 数据OK不
	if (!ChirpNum_isOK) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Chirp number is not healthy Using Default Number \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
	}
	else {
		if (ui._AR_UI_SAMPLE_CHIRPS_->text().toInt() <= 0) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Chirp number is not healthy Using Default Number \n");
			warn.push_back(_UI_WARN_);
			info_mutex.unlock();
		}
		else {
			info_mutex.lock();
			chirp_num = ui._AR_UI_SAMPLE_ADCNUM_->text().toInt();
			info.push_back("UI Write CFG: Chirp number is set \n");
			warn.push_back(_UI_INFO_);
			info_mutex.unlock();
		}
	}
	_change_chirp_number = chirp_num;
	if (!FrameNum_isOK) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Frame number is not healthy \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
	}
	else {
		if (ui._AR_UI_SAMPLE_FRAMES_->text().toInt() <= 0) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Frame number is not healthy Using Default Number \n");
			warn.push_back(_UI_WARN_);
			info_mutex.unlock();
		}
		else {
			frame_num = ui._AR_UI_SAMPLE_FRAMES_->text().toInt();
			info_mutex.lock();
			info.push_back("UI Write CFG: Frame number is set \n");
			warn.push_back(_UI_INFO_);
			info_mutex.unlock();
		}
	}
	temp.append(QString::fromStdString(std::to_string(chirp_num)));
	temp.append(" ");
	temp.append(QString::fromStdString(std::to_string(frame_num)));
	temp.append(" 40 1 0\n");
	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD(temp);
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write frameCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("lowPower 0 0\n"); // 使用常规ADC模式
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write lowPower Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("lvdsStreamCfg -1 0 1 0\n"); // 设置LVDS
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write lvdsStreamCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("calibMonCfg 1 1\n"); //
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write calibMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("monCalibReportCfg 1 1 0\n"); //
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write monCalibReportCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	if (!disable_tx1_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("txPowerMonCfg 1 0 0\n"); //
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write txPowerMonCfg 1 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}
	if (!disable_tx2_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("txPowerMonCfg 1 1 0\n"); // 使用复数模式
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write txPowerMonCfg 2 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}
	if (!disable_tx3_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("txPowerMonCfg 1 2 0\n"); // 使用复数模式
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write txPowerMonCfg 3 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}
	if (!disable_tx1_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("txBallbreakMonCfg 1 0\n"); //
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write txBallbreakMonCfg 1 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}
	if (!disable_tx2_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("txBallbreakMonCfg 1 1\n"); // 使用复数模式
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write txBallbreakMonCfg 2 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}
	if (!disable_tx3_) {
		_radar_com_->ReadPortLD();
		_radar_com_->WritePortLD("txBallbreakMonCfg 1 2\n"); // 使用复数模式
		if (!_wait_done_()) {
			info_mutex.lock();
			info.push_back("UI Write CFG: Com write txBallbreakMonCfg 3 Failed \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			return;
		}
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("rxGainPhaseMonCfg 1 0\n"); //
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write rxGainPhaseMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("tempMonCfg 1 20\n"); //
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write tempMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("synthFreqMonCfg 1 0\n"); //
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write synthFreqMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("pllConVoltMonCfg 1\n"); // 锁相环的设置？
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write pllConVoltMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("dualClkCompMonCfg 1\n"); //
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write dualClkCompMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("rxIfStageMonCfg 1 0\n"); //
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write rxIfStageMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("extAnaSigMonCfg 0\n"); // 
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write extAnaSigMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("pmClkSigMonCfg 1 0\n"); // 
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write pmClkSigMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("rxIntAnaSigMonCfg 1 0\n"); // 
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write rxIntAnaSigMonCfg  Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	_radar_com_->ReadPortLD();
	_radar_com_->WritePortLD("gpadcSigMonCfg 1\n"); // 
	if (!_wait_done_()) {
		info_mutex.lock();
		info.push_back("UI Write CFG: Com write gpadcSigMonCfg Failed \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}
}

// 开始采集
void _AR_UI_::_AR_UI_START_RECORD_() {
	if (PLAYER == _SERVER_PLAYER_) { // 檢測是不是服務器角色
		info_mutex.lock();
		info.push_back("UI Record: Recording... \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
		// 发送指令
		unsigned char command[_CLIENT_MAX_BUFFER_];
		if (_TELE_ENABLE_) {
			command[0] = _DATA_TYPE_COMMAND_;
			command[1] = _CMD_RECORD_;
		}
		else {
			command[0] = _DATA_TYPE_COMMAND_;
			command[1] = _CMD_RECORD_DONOT_TRANSFER_;
		}
		extern SERV_CON_LIST * header;
		SERV_CON_LIST * h = header;
		bool _found_client_ = false;
		// 找到雷达客户端
		while (h != NULL) {
			if (!h->ALIVE) {
				h = h->next;
				continue;
			}
			if (QString(h->describe).contains("Radar", Qt::CaseSensitive)) {
				tcp_server_send_data((char *)command, h);
				_found_client_ = true;
				break;
			}
		}
		if (!_found_client_) {
			info_mutex.lock();
			info.push_back("UI Record: Radar Client is not found!\n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
		}
		return;
	}
	if (!_com_connected_) {
		info_mutex.lock();
		info.push_back("UI Record: COM Not connected \n");
		warn.push_back(_UI_ERR_);
		info.push_back("UI Record: Try to Connect COM \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
		_Connect_COM_();
		if (!_com_connected_) {
			info_mutex.lock();
			info.push_back("UI Record: COM Cannot connected \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock();
			if (PLAYER == _CLIENT_PLAYER_) {
				reply_code(_REPLY_FAILED_);
			}
			return;
		}
		info_mutex.lock();
		info.push_back("UI Record: COM is connected \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
	}
	if (!_udp_connected_) {
		info_mutex.lock();
		info.push_back("UI Record: UDP Not connected \n");
		warn.push_back(_UI_ERR_);
		info.push_back("UI Record: Try to Connect UDP Server \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
		_Connect_UDP_();
		if (!_udp_connected_) {
			info_mutex.lock();
			info.push_back("UI Record: UDP server Cannot connected \n");
			warn.push_back(_UI_ERR_);
			info_mutex.unlock(); 
			if (PLAYER == _CLIENT_PLAYER_) {
				reply_code(_REPLY_FAILED_);
			}
			return;
		}
		info_mutex.lock();
		info.push_back("UI Record: UDP server is connected \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
	}
	
	chan_data_stop = 1; // 停止波形
	_write_cfg_();
	// 写入 cfg // 根据 设置 默认：ADC 256 Chirp 64 Frames 256
	if (header == NULL){
		header = list_init();
	}
	else {
		list_delete(header);
		header = list_init();
	}
	int status = _radar_com_->data_record_dca1000(header, &data_length);

	if (!status) {
		info_mutex.lock();
		info.push_back("UI Record: DCA1000EVM Sample Complete \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock(); 
		if (PLAYER == _CLIENT_PLAYER_) {
			reply_code(_REPLY_SUCCESS_);
		}
	}
	else {
		info_mutex.lock();
		info.push_back("UI Record: DCA1000EVM Sample Failure \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		// 回复给服务端
		if (PLAYER == _CLIENT_PLAYER_) {
			reply_code(_REPLY_FAILED_);
		}
		return;
	}
	chan_data_stop = 0;
	// 填入数据
	_AR_header_ = header;
	new_wave_data = 1;
	// 默认不删除 header 数据
	save_data(header, ui._AR_UI_SAMPLE_SAVEPATH_->text().
		toStdString().data(), 0, data_length);

	record_num++;
	file_dir = "";
	file_dir.append(QString::fromStdString(std::to_string(record_num)));
	file_dir.append(file_addr);
	ui._AR_UI_SAMPLE_SAVEPATH_->setText(save_dir + file_dir);
	QPalette palette;
	palette.setColor(QPalette::Text, QColor(255, 255, 255));
	ui._AR_UI_SAMPLE_SAVEPATH_->setPalette(palette);
	QPalette p = ui._AR_UI_SAMPLE_SAVEPATH_->palette();
	p.setColor(QPalette::Active, QPalette::Base, Qt::black);
	p.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
	ui._AR_UI_SAMPLE_SAVEPATH_->setPalette(p);
}

// 选择保存数据路径
void _AR_UI_::_Set_Save_Dir_() {
	save_dir = QFileDialog::getExistingDirectory(this, "Please Select Save Data Path", "./");
	if (save_dir.size() == 0) {
		save_dir.append(QString::fromStdString(std::to_string(record_num)));
		save_dir.append(file_addr);
		ui._AR_UI_SAMPLE_SAVEPATH_->setText(save_dir);
		QPalette palette;
		palette.setColor(QPalette::Text, QColor(255, 255, 255));
		ui._AR_UI_SAMPLE_SAVEPATH_->setPalette(palette);
		QPalette p = ui._AR_UI_SAMPLE_SAVEPATH_->palette();
		p.setColor(QPalette::Active, QPalette::Base, Qt::black);
		p.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
		ui._AR_UI_SAMPLE_SAVEPATH_->setPalette(p);
		return;
	}
	save_dir.append("/");
	file_dir = save_dir;
	file_dir.append(QString::fromStdString(std::to_string(record_num)));
	file_dir.append(file_addr);
	ui._AR_UI_SAMPLE_SAVEPATH_->setText(file_dir);
	QPalette palette;
	palette.setColor(QPalette::Text, QColor(255, 255, 255));
	ui._AR_UI_SAMPLE_SAVEPATH_->setPalette(palette);
	QPalette p = ui._AR_UI_SAMPLE_SAVEPATH_->palette();
	p.setColor(QPalette::Active, QPalette::Base, Qt::black);
	p.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
	ui._AR_UI_SAMPLE_SAVEPATH_->setPalette(p);
}

// 更新info
void _AR_UI_::update_info_box(QColor backgroundColor) {
	// welcome to _AR_Program_
	QByteArray _welcome_info_("  -------------------- Welcome to --------------------\n -------------------- _AR_Program_ -------------------- \n");
	QTextCharFormat fmt;
	fmt.setForeground(QColor("white"));
	ui._AR_UI_INFOVIEW->mergeCurrentCharFormat(fmt);
	ui._AR_UI_INFOVIEW->appendPlainText(_welcome_info_);

	while (true) {
		if (info.size() > 0 && warn.size() > 0) {
			emit _info_is_coming_(); // 发射信号 有数据到来
		}
	}
}

// 改变工作模式 ：
// 1. 正常采集模式：无  DISABLED
// 2. 服务器接受模式：监听、停止监听 LISTEN CLOSED
// 3. 客户端模式：连接、断开 CONNECT & DISCONNECT
void _AR_UI_::_AR_UI_CHANGE_MODE_() {
	_OPEN_ += 1;
	_OPEN_ %= _OPEN_MAX_STATUS_;

	if (PLAYER == _NORMAL_COLLECTOR_) {
		_OPEN_ = 0;
		ui._AR_UI_L_CON_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_DALB_.png"));

		// 保证服务器 客户端断开链接
		server_is_closed = 1;
		closesocket(server);
	}
	else if (PLAYER == _SERVER_RECEIVER_) {
		// 服务器端
		if (_OPEN_) {
			// 建立服务器
			int result = build_tcp_server();
			if (result) {
				info_mutex.lock();
				info.push_back("UI Build Server: Server Build Error! And Return ");
				warn.push_back(_UI_ERR_);
				QString temp;
				info.push_back((temp.number(result) + QString("\n")).toUtf8());
				warn.push_back(_UI_ERR_);
				info_mutex.unlock();
			}
			else {
				info_mutex.lock();
				info.push_back("UI Build Server: Server is Built! \n");
				warn.push_back(_UI_INFO_);
				info_mutex.unlock();
				server_is_closed = 0; // 开启服务器
				std::thread _thread_wait_connect_ = std::thread(tcp_server_waiting_connect);
				_thread_wait_connect_.detach();
			}
			// 打开
			ui._AR_UI_L_CON_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_CLOSED_.png"));
		}
		else {
			// 关闭
			ui._AR_UI_L_CON_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_LISTEN_.png"));
			server_is_closed = 1;
			closesocket(server);
		}
	}
	else {
		// 客户端
		if (_OPEN_) {
			// 建立客户端
			int try_num = 0;
			int result = build_radar_data_transfer_client();
			if (result) {
				info_mutex.lock();
				info.push_back("UI Build Client: Server Client Error! And Return ");
				warn.push_back(_UI_ERR_);
				QString temp;
				info.push_back((temp.number(result) + QString("\n")).toUtf8());
				warn.push_back(_UI_ERR_);
				info_mutex.unlock();
				return;
			}
			_CLIENT_AVAILABLE_ = _OFFLINE_MODE_; // 默认设置为离线采集模式
			std::thread _thread_recv_command_client_ = std::thread(recv_command_client);
			_thread_recv_command_client_.detach();
			// 打开
			ui._AR_UI_L_CON_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_DCON_.png"));
		}
		else {
			closesocket(clientSocket);
			// 关闭
			ui._AR_UI_L_CON_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_CON_.png"));
		}
	}
}

// 改变工作模式：
void _AR_UI_::_AR_UI_CHANGE_ACTOR_() {
	PLAYER += 1;
	PLAYER %= _PLAYER_MAX_STATUS_;

	if (PLAYER == _NORMAL_COLLECTOR_) {
		QImage image = QImage(":/_AR_UI_/_AR_FIGURES_/_AR_SAMPLE_.png");
		ui._AR_UI_L_FUNC_->setPixmap(QPixmap::fromImage(image));
	}
	else if (PLAYER == _SERVER_RECEIVER_) {
		QImage image = QImage(":/_AR_UI_/_AR_FIGURES_/_AR_SERVER_.png");
		ui._AR_UI_L_FUNC_->setPixmap(QPixmap::fromImage(image));
	}
	else {
		QImage image = QImage(":/_AR_UI_/_AR_FIGURES_/_AR_CLIENT_.png");
		ui._AR_UI_L_FUNC_->setPixmap(QPixmap::fromImage(image));
	}
	_OPEN_ = -1;
	_AR_UI_CHANGE_MODE_(); // 重置状态 // 链接或者...
}

// 改变TX1状态
void _AR_UI_::_AR_UI_CHANGE_TX1_ENABLE_() {
	if (disable_tx2_ && disable_tx3_) {
		info_mutex.lock();
		info.push_back("UI Warning: Cannot change TX1 \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
		return;
	}
	disable_tx1_ = !disable_tx1_;
	if (disable_tx1_) {
		ui._AR_TX1_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_D_EN_.png"));
		info_mutex.lock();
		info.push_back("UI: Disable TX1 \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
	}
	else {
		ui._AR_TX1_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_EN_.png"));
		info_mutex.lock();
		info.push_back("UI: Enable TX1 \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
	}
}

// 改变TX2状态
void _AR_UI_::_AR_UI_CHANGE_TX2_ENABLE_() {
	if (disable_tx1_ && disable_tx3_) {
		info_mutex.lock();
		info.push_back("UI Warning: Cannot change TX2 \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
		return;
	}
	disable_tx2_ = !disable_tx2_;
	if (disable_tx2_) {
		ui._AR_TX2_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_D_EN_.png"));
		info_mutex.lock();
		info.push_back("UI: Disable TX2 \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
	}
	else {
		ui._AR_TX2_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_EN_.png"));
		info_mutex.lock();
		info.push_back("UI: Enable TX2 \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
	}
}

// 改变TX3状态
void _AR_UI_::_AR_UI_CHANGE_TX3_ENABLE_() {
	if (disable_tx1_ && disable_tx2_) {
		info_mutex.lock();
		info.push_back("UI Warning: Cannot change TX3 \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
		return;
	}
	disable_tx3_ = !disable_tx3_;
	if (disable_tx3_) {
		ui._AR_TX3_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_D_EN_.png"));
		info_mutex.lock();
		info.push_back("UI: Disable TX3 \n");
		warn.push_back(_UI_INFO_);
		info_mutex.unlock();
	}
	else {
		ui._AR_TX3_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_EN_.png"));
		info_mutex.lock();
		info.push_back("UI: Enable TX3 \n");
		warn.push_back(_UI_INFO_); 
		info_mutex.unlock();
	}
}

// 窗口初始化
void _AR_UI_::_AR_UI_init() {
	ui._AR_UI_L_FUNC_->setScaledContents(true);
	ui._AR_UI_L_CON_->setScaledContents(true);
	ui._AR_UI_BG->setScaledContents(true);
	ui._UI_LABEL_LOC_OF_IP_->setVisible(false); // 隐藏标注模块
	ui._UI_LABEL_LOC_OF_INFO->setVisible(false); // 隐藏标注模块
	ui._UI_LABEL_CURVE_AREA_->setVisible(false); // 隐藏标注模块
	ui._UI_LABEL_CONFIG_AREA_->setVisible(false); // 隐藏标注模块
	ui._UI_LABEL_LOC_OF_PORT_->setVisible(false); // 隐藏标注模块
	QImage image = QImage(":/_AR_UI_/_AR_FIGURES_/_AR_SAMPLE_.png");
	ui._AR_UI_L_FUNC_->setPixmap(QPixmap::fromImage(image));
	ui._AR_UI_L_CON_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_DALB_.png")); // 初始化标记
	ui._AR_UI_SERC->setText(""); // 清空文字
	ui._AR_UI_CON->setText(""); // 清空文字
	ui._AR_UI_EXIT->setText(""); // 清空文字
	this->setWindowIcon(QIcon(":/_AR_UI_/_AR_FIGURES_/_AR_UI_ICON_.png"));

	ui._AR_TX1_ENABLE_LABEL_->setText("");
	ui._AR_TX2_ENABLE_LABEL_->setText("");
	ui._AR_TX3_ENABLE_LABEL_->setText("");
	ui._AR_TX1_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_EN_.png")); // 初始化标记
	ui._AR_TX2_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_EN_.png")); // 初始化标记
	ui._AR_TX3_ENABLE_LABEL_->setPixmap(QPixmap(":/EN_DEN/_AR_FIGURES_/_AR_INFO_EN_.png")); // 初始化标记
	ui._AR_TX1_ENABLE_LABEL_->setScaledContents(true);
	ui._AR_TX2_ENABLE_LABEL_->setScaledContents(true);
	ui._AR_TX3_ENABLE_LABEL_->setScaledContents(true);

	ui._AR_TELE_ENABLE_LABEL_->setText("");
	ui._AR_TELE_ENABLE_LABEL_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_TELE_CF_.png")); // 初始化按鈕
	ui._AR_TELE_ENABLE_LABEL_->setScaledContents(true);

	QPalette p = ui._AR_UI_INFOVIEW->palette();
	p.setColor(QPalette::Active, QPalette::Base, Qt::black);
	p.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
	ui._AR_UI_INFOVIEW->setPalette(p);

	ui._AR_UI_SAMPLE_SAVEPATH_->setText(file_dir);
	QPalette palette;
	palette.setColor(QPalette::Text, QColor(255, 255, 255));
	ui._AR_UI_SAMPLE_SAVEPATH_->setPalette(palette);
	p = ui._AR_UI_SAMPLE_SAVEPATH_->palette();
	p.setColor(QPalette::Active, QPalette::Base, Qt::black);
	p.setColor(QPalette::Inactive, QPalette::Base, Qt::black);
	ui._AR_UI_SAMPLE_SAVEPATH_->setPalette(p);

	setWindowState(Qt::WindowFullScreen); // 设置全屏
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint); // 设置全屏

	ui._AR_UI_INFOVIEW->setMaximumBlockCount(_MAX_CONTAIN_);
}

void _AR_UI_::_AR_UI_exit() {
	exit(0); // 正常退出
}

void _AR_UI_::resizeEvent(QResizeEvent * event) {
	// 背景图像是从这里加载的
	ui._AR_UI_BG->resize(this->size()); // 覆盖大小
	QPixmap scaledPixmap = QPixmap(":/_AR_UI_/_AR_FIGURES_/AR_UI.png").scaled(this->size());
	ui._AR_UI_BG->setPixmap(scaledPixmap);
	ui._AR_UI_BG->setScaledContents(true);

	int w = this->size().width();
	int h = this->size().height(); // 窗体大小

	double _ratio_w_ = (double)w / _original_window_x_;
	double _ratio_h_ = (double)h / _original_window_y_; // 比例
	{
		int new_exit_original_x_ = _exit_original_x_ * _ratio_w_;
		int new_exit_original_y_ = _exit_original_y_ * _ratio_h_;
		int new_exit_original_w_ = _exit_button_w_ * _ratio_w_;
		int new_exit_original_h_ = _exit_button_h_ * _ratio_h_;
		ui._AR_UI_EXIT->setGeometry(QRect(
			new_exit_original_x_,
			new_exit_original_y_,
			new_exit_original_w_,
			new_exit_original_h_)); // 更新按钮

		int new_con_original_x_ = _con_original_x_ * _ratio_w_;
		int new_con_original_y_ = _con_original_y_ * _ratio_h_;
		int new_con_button_w_ = _con_button_w_ * _ratio_w_;
		int new_con_button_h_ = _con_button_h_ * _ratio_h_;
		ui._AR_UI_CON->setGeometry(QRect(
			new_con_original_x_,
			new_con_original_y_,
			new_con_button_w_,
			new_con_button_h_)); // 更新按钮大小

		int new_ser_original_x_ = _ser_original_x_ * _ratio_w_;
		int new_ser_original_y_ = _ser_original_y_ * _ratio_h_;
		int new_ser_button_w_ = _ser_button_w_ * _ratio_w_;
		int new_ser_button_h_ = _ser_button_h_ * _ratio_h_;
		ui._AR_UI_SERC->setGeometry(QRect(
			new_ser_original_x_,
			new_ser_original_y_,
			new_ser_button_w_,
			new_ser_button_h_)); // 更新按钮


		int new_info_original_x_ = _info_original_x_ * _ratio_w_;
		int new_info_original_y_ = _info_original_y_ * _ratio_h_;
		int new_info_width_ = _info_width_ * _ratio_w_;
		int new_info_height_ = _info_height_ * _ratio_h_;
		ui._AR_UI_INFOVIEW->setGeometry(QRect(
			new_info_original_x_,
			new_info_original_y_,
			new_info_width_,
			new_info_height_)); // 更新提示框大小

		int new_ip_origin_x_ = _ip_origin_x_ * _ratio_w_; // IP提示框X坐标
		int new_ip_origin_y_ = _ip_origin_y_ * _ratio_h_; // IP提示框Y坐标
		int new_ip_width_ = _ip_width_ * _ratio_w_; // IP提示框宽度
		int new_ip_height_ = _ip_height_ * _ratio_h_; // IP提示框高度
		ui._AR_UI_IP_SELECT_->setGeometry(QRect(
			new_ip_origin_x_,
			new_ip_origin_y_,
			new_ip_width_,
			new_ip_height_)); // 更新IP提示框大小

		int new_port_origin_x_ = _port_origin_x_ * _ratio_w_; // 端口提示框X坐标
		int new_port_origin_y_ = _port_origin_y_ * _ratio_h_; // 端口提示框Y坐标
		int new_port_width_ = _port_width_ * _ratio_w_; // 端口提示框宽度
		int new_port_height_ = _port_height_ * _ratio_h_; // 端口提示框高度
		ui._AR_UI_PORT_SELECT_->setGeometry(QRect(
			new_port_origin_x_,
			new_port_origin_y_,
			new_port_width_,
			new_port_height_)); // 更新Port提示框大小

		int new_l_con_original_x_ = _l_con_original_x_ * _ratio_w_; // 连接按钮X坐标
		int new_l_con_original_y_ = _l_con_original_y_ * _ratio_h_; // 连接按钮Y坐标
		int new_l_con_button_w_ = _l_con_button_w_ * _ratio_w_; // 连接按钮宽度
		int new_l_con_button_h_ = _l_con_button_h_ * _ratio_h_; // 连接按钮高度
		ui._AR_UI_L_CON_->setGeometry(
			QRect(new_l_con_original_x_,
				new_l_con_original_y_,
				new_l_con_button_w_,
				new_l_con_button_h_)
		);

		int new_l_ser_original_x_ = _l_ser_original_x_ * _ratio_w_; // 工作模式按钮X坐标
		int new_l_ser_original_y_ = _l_ser_original_y_ * _ratio_h_; // 工作模式按钮Y坐标
		int new_l_ser_button_w_ = _l_ser_button_w_ * _ratio_w_; // 工作模式按钮宽度
		int new_l_ser_button_h_ = _l_ser_button_h_ * _ratio_h_; // 工作模式按钮高度
		ui._AR_UI_L_FUNC_->setGeometry(
			QRect(new_l_ser_original_x_,
				new_l_ser_original_y_,
				new_l_ser_button_w_,
				new_l_ser_button_h_));

		int new_record_origin_x_ = _record_origin_x_ * _ratio_w_; // 保存的按钮
		int new_record_origin_y_ = _record_origin_y_ * _ratio_h_;
		int new_record_width_ = _record_width_ * _ratio_w_;
		int new_record_height_ = _record_height_ * _ratio_h_;
		ui._AR_UI_SAMPLE_RECORD_->setGeometry(
			QRect(new_record_origin_x_,
				new_record_origin_y_,
				new_record_width_,
				new_record_height_));

		int new_save_path_origin_x_ = _save_path_origin_x_ * _ratio_w_; // 保存的路径
		int new_save_path_origin_y_ = _save_path_origin_y_ * _ratio_h_;
		int new_save_path_width_ = _save_path_width_ * _ratio_w_;
		int new_save_path_height_ = _save_path_height_ * _ratio_h_;
		ui._AR_UI_SAMPLE_CHANGEPATH_->setGeometry(
			QRect(new_save_path_origin_x_,
				new_save_path_origin_y_,
				new_save_path_width_,
				new_save_path_height_));

		int new_path_origin_x_ = _path_origin_x_ * _ratio_w_; // 位置显示区
		int new_path_origin_y_ = _path_origin_y_ * _ratio_h_;
		int new_path_width_ = _path_width_ * _ratio_w_;
		int new_path_height_ = _path_height_ * _ratio_h_;
		ui._AR_UI_SAMPLE_SAVEPATH_->setGeometry(
			QRect(new_path_origin_x_,
				new_path_origin_y_,
				new_path_width_,
				new_path_height_));

		int new_TX1_ENABLE_x_ = _TX1_ENABLE_x_ * _ratio_w_;
		int new_TX1_ENABLE_y_ = _TX1_ENABLE_y_ * _ratio_h_;
		int new_TX1_ENABLE_w_ = _TX1_ENABLE_w_ * _ratio_w_;
		int new_TX1_ENABLE_h_ = _TX1_ENABLE_h_ * _ratio_h_;
		ui._AR_TX1_ENABLE_->setGeometry(
			QRect(new_TX1_ENABLE_x_,
				new_TX1_ENABLE_y_,
				new_TX1_ENABLE_w_,
				new_TX1_ENABLE_h_));
		ui._AR_TX1_ENABLE_LABEL_->setGeometry(
			QRect(new_TX1_ENABLE_x_,
				new_TX1_ENABLE_y_,
				new_TX1_ENABLE_w_,
				new_TX1_ENABLE_h_));

		int new_TX2_ENABLE_x_ = _TX2_ENABLE_x_ * _ratio_w_;
		int new_TX2_ENABLE_y_ = _TX2_ENABLE_y_ * _ratio_h_;
		int new_TX2_ENABLE_w_ = _TX2_ENABLE_w_ * _ratio_w_;
		int new_TX2_ENABLE_h_ = _TX2_ENABLE_h_ * _ratio_h_;
		ui._AR_TX2_ENABLE->setGeometry(
			QRect(new_TX2_ENABLE_x_,
				new_TX2_ENABLE_y_,
				new_TX2_ENABLE_w_,
				new_TX2_ENABLE_h_));
		ui._AR_TX2_ENABLE_LABEL_->setGeometry(
			QRect(new_TX2_ENABLE_x_,
				new_TX2_ENABLE_y_,
				new_TX2_ENABLE_w_,
				new_TX2_ENABLE_h_));

		int new_TX3_ENABLE_x_ = _TX3_ENABLE_x_ * _ratio_w_;
		int new_TX3_ENABLE_y_ = _TX3_ENABLE_y_ * _ratio_h_;
		int new_TX3_ENABLE_w_ = _TX3_ENABLE_w_ * _ratio_w_;
		int new_TX3_ENABLE_h_ = _TX3_ENABLE_h_ * _ratio_h_;
		ui._AR_TX3_ENABLE->setGeometry(
			QRect(new_TX3_ENABLE_x_,
				new_TX3_ENABLE_y_,
				new_TX3_ENABLE_w_,
				new_TX3_ENABLE_h_));
		ui._AR_TX3_ENABLE_LABEL_->setGeometry(
			QRect(new_TX3_ENABLE_x_,
				new_TX3_ENABLE_y_,
				new_TX3_ENABLE_w_,
				new_TX3_ENABLE_h_));

		int new_ADC_input_x_ = _ADC_input_x_ * _ratio_w_; // ADC采样输入框位置
		int new_ADC_input_y_ = _ADC_input_y_ * _ratio_h_;
		int new_ADC_input_w_ = _ADC_input_w_ * _ratio_w_;
		int new_ADC_input_h_ = _ADC_input_h_ * _ratio_h_;
		ui._AR_UI_SAMPLE_ADCNUM_->setGeometry(
			QRect(new_ADC_input_x_,
				new_ADC_input_y_,
				new_ADC_input_w_,
				new_ADC_input_h_)
		);

		int new_CHIRP_input_x_ = _CHIRP_input_x_ * _ratio_w_; // Chirps输入框位置
		int new_CHIRP_input_y_ = _CHIRP_input_y_ * _ratio_h_;
		int new_CHIRP_input_w_ = _CHIRP_input_w_ * _ratio_w_;
		int new_CHIRP_input_h_ = _CHIRP_input_h_ * _ratio_h_;
		ui._AR_UI_SAMPLE_CHIRPS_->setGeometry(
			QRect(new_CHIRP_input_x_,
				new_CHIRP_input_y_,
				new_CHIRP_input_w_,
				new_CHIRP_input_h_)
		);

		int new_FRAME_input_x_ = _FRAME_input_x_ * _ratio_w_; // Frame输入框位置
		int new_FRAME_input_y_ = _FRAME_input_y_ * _ratio_h_;
		int new_FRAME_input_w_ = _FRAME_input_w_ * _ratio_w_;
		int new_FRAME_input_h_ = _FRAME_input_h_ * _ratio_h_;
		ui._AR_UI_SAMPLE_FRAMES_->setGeometry(
			QRect(new_FRAME_input_x_,
				new_FRAME_input_y_,
				new_FRAME_input_w_,
				new_FRAME_input_h_)
		);

		int new_AR_CURV_x_ = _AR_CURV_x_ * _ratio_w_;  // 曲线
		int new_AR_CURV_y_ = _AR_CURV_y_ * _ratio_h_;
		int new_AR_CURV_w_ = _AR_CURV_w_ * _ratio_w_;
		int new_AR_CURV_h_ = _AR_CURV_h_ * _ratio_h_;
		ui._AR_UI_CURV_VIEW_->setGeometry(
			QRect(new_AR_CURV_x_,
				new_AR_CURV_y_,
				new_AR_CURV_w_,
				new_AR_CURV_h_)
		);

		int _AR_B_CURV_x_ = new_AR_CURV_x_ + _AR_CURV_offsetX_ * _ratio_w_;  // 曲线
		int _AR_B_CURV_y_ = new_AR_CURV_y_ + _AR_CURV_offsetY_ * _ratio_h_;
		int _AR_B_CURV_w_ = new_AR_CURV_w_ + _AR_CURV_offsetW_ * _ratio_w_;
		int _AR_B_CURV_h_ = new_AR_CURV_h_ + _AR_CURV_offsetH_ * _ratio_h_;
		ui._AR_UI_CURV_BUTTON_->setGeometry(
			QRect(_AR_B_CURV_x_,
				_AR_B_CURV_y_,
				_AR_B_CURV_w_,
				_AR_B_CURV_h_)
		);

		int new_AR_ENABLE_TELE_x_ = _AR_ENABLE_TELE_x_ * _ratio_w_; // Chirps输入框位置
		int new_AR_ENABLE_TELE_y_ = _AR_ENABLE_TELE_y_ * _ratio_h_;
		int new_AR_ENABLE_TELE_w_ = _AR_ENABLE_TELE_w_ * _ratio_w_;
		int new_AR_ENABLE_TELE_h_ = _AR_ENABLE_TELE_h_ * _ratio_h_;
		ui._AR_TELE_ENABLE_->setGeometry(
			QRect(new_AR_ENABLE_TELE_x_,
				new_AR_ENABLE_TELE_y_,
				new_AR_ENABLE_TELE_w_,
				new_AR_ENABLE_TELE_h_)
		);
		ui._AR_TELE_ENABLE_LABEL_->setGeometry(
			QRect(new_AR_ENABLE_TELE_x_,
				new_AR_ENABLE_TELE_y_,
				new_AR_ENABLE_TELE_w_,
				new_AR_ENABLE_TELE_h_)
		);
	}
}

void _AR_UI_::_AR_UI_Get_data_and_Draw() {
	int Select_Chan_index = 0;
	int Select_Chrip_index = 0;
	_putting_in_single_frame_();
	if (chan_data_is_ready) {
		_wave_form_->clear();

		int _min_ = 65535;
		int _max_ = -65535;
		for (int ii = 0; ii < _ADC_NUMBER_; ii++) {
			if (_max_ < _AR_RADAR_WAVE_[Select_Chan_index].buff_real[Select_Chrip_index][ii]) {
				_max_ = _AR_RADAR_WAVE_[Select_Chan_index].buff_real[Select_Chrip_index][ii];
			}
			if (_min_ > _AR_RADAR_WAVE_[Select_Chan_index].buff_real[Select_Chrip_index][ii]) {
				_min_ = _AR_RADAR_WAVE_[Select_Chan_index].buff_real[Select_Chrip_index][ii];
			}
			// 默认显示实数部分波形
			int wave_real = _AR_RADAR_WAVE_[Select_Chan_index].buff_real[Select_Chrip_index][ii];
			_wave_form_->append(QPointF(ii, wave_real));
		}

		axisX->setRange(0, _ADC_NUMBER_);
		if (_min_ < 0) _min_ *= 1.1;
		else _min_ *= 0.9;
		if (_max_ < 0) _max_ *= 0.9;
		else _max_ *= 1.1; // 设置范围
		axisY->setRange(_min_, _max_);
		if (axis_is_initialized) {
			_wave_chart_->removeAxis(axisX);
			_wave_chart_->removeAxis(axisY);
		}
		_wave_chart_->setAxisX(axisX, _wave_form_);
		_wave_chart_->setAxisY(axisY, _wave_form_);

		axis_is_initialized = true;
		chan_data_is_ready = 0; // 设置为False 继续解码数据
	}
}

// 調整是否接受回傳
void _AR_UI_::_AR_UI_Change_TELE_MODE()
{
	if (PLAYER != _SERVER_RECEIVER_) {
		info_mutex.lock();
		info.push_back("UI Change Tele: Need Server Mode \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}
	_TELE_ENABLE_ = !_TELE_ENABLE_;
	if (_TELE_ENABLE_) {
		ui._AR_TELE_ENABLE_LABEL_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_TELE_CF_.png")); // 初始化按鈕
	}
	else {
		ui._AR_TELE_ENABLE_LABEL_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_TELE_DIS_.png")); // 初始化按鈕
	}
}

// 上傳回傳狀態
void _AR_UI_::_AR_UI_UPDATE_TELE_MODE()
{
	if (_TELE_ENABLE_) {
		ui._AR_TELE_ENABLE_LABEL_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_TELE_CF_.png")); // 初始化按鈕
	}
	else {
		ui._AR_TELE_ENABLE_LABEL_->setPixmap(QPixmap(":/_AR_UI_/_AR_FIGURES_/_AR_TELE_DIS_.png")); // 初始化按鈕
	}
}