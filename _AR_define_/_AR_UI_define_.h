#pragma once

#ifndef _AR_UI_DEFINE_H_
#define _AR_UI_DEFINE_H_

// ���ⲿ�����ṩ����
unsigned char Command = 0x00; // ��ָ��

#define _NON_COMMAND_ 0x00
#define _RECORD_COMMAND_ 0x01
#define _EXIT_COMMAND_ 0xFF 

// ���ⲿ�����ṩ�ظ�
unsigned char Reply = 0x00; // ���ݴ����Զ����Ļظ�

#define _NON_REPLY_ 0x00; // �޻ظ�

#endif 