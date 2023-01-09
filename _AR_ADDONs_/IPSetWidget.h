#pragma once

#include <mutex>
#include <QWidget>
#include <QHostAddress>
#include "ui_IPSetWidget.h"
#include "../_AR_define_/_net_work_define_.h"
#include "../_AR_include_/_AR_UI_INFO_SUBSYSTEM_.h"

extern QString IP;
extern QString Port;

class IPSetWidget : public QWidget
{
	Q_OBJECT

public:
	IPSetWidget(QWidget *parent = nullptr);
	~IPSetWidget();

private:
	Ui::IPSetWidgetClass ui;

signals:
	void ip_changed();

public slots:
	void click_abort(); // 取消IP设置
	void click_confirm(); // 确定IP设置
};
