#pragma once

#ifndef _AR_UI_INFO_SUBSYSTEM_H_
#define _AR_UI_INFO_SUBSYSTEM_H_

#include <QByteArray>
#include <iostream>
#include <QColor>
#include <vector>

typedef unsigned int LEVEL; // 警告级别

std::vector <QByteArray> info; // 全局定义
std::vector <LEVEL> warn; // 警告程度

#define _UI_INFO_ 0 // UI显示级别
#define _UI_WARN_ 1 // UI警告级别
#define _UI_ERR_ 2 // UI 错误级别

#define _MAX_CONTAIN_ 5000 // 最多显示5000条数据

QColor _f_info("white"); // 信息的颜色
QColor _f_warn("yellow"); // 警告的颜色
QColor _f_err("red"); // 错误的颜色
QColor _f_background("black"); //  背景的颜色

#endif
