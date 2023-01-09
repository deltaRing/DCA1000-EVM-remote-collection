#include "IPSetWidget.h"

/* ===========================消息的定义============================= */
extern std::vector <QByteArray> info; // 全局定义
extern std::vector <LEVEL> warn; // 警告程度
extern std::mutex info_mutex; // 

IPSetWidget::IPSetWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui._IP_BackGround_->setPixmap(QPixmap(":/IP/_AR_FIGURES_/_IPC_UI.png"));
	ui._IP_BackGround_->setScaledContents(true);

	connect(ui._IP_CONFIRM_, &QPushButton::clicked, 
		this, &IPSetWidget::click_confirm);
	connect(ui._IP_ABORT_, &QPushButton::clicked,
		this, &IPSetWidget::click_abort);
}

IPSetWidget::~IPSetWidget()
{

}

void IPSetWidget::click_abort() {
	this->hide();
	ui._IP_SET_IP_->clear();
	ui._IP_SET_PORT_->clear();
}

void IPSetWidget::click_confirm() {
	extern QString IP;
	extern QString Port;
	QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
	bool _isAddr = RegExp.exactMatch(ui._IP_SET_IP_->text().trimmed());
	bool _isNum = false;
	int port = ui._IP_SET_PORT_->text().toInt(&_isNum);
	if (_isAddr && _isNum && Port >= 0) {
		IP = ui._IP_SET_IP_->text().trimmed();
		Port = QString::number(port);
		emit ip_changed();
	}
	else {
		info_mutex.lock();
		info.push_back("IP Set: IP format is invalid! \n");
		warn.push_back(_UI_WARN_);
		info_mutex.unlock();
	}
	this->hide();
	ui._IP_SET_IP_->clear();
	ui._IP_SET_PORT_->clear();
}