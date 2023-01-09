#pragma once
#ifndef _TCP_DEFINE_H_
#define _TCP_DEFINE_H_

#define _DATA_TYPE_COMMAND_ 0x01 // ��������
#define _DATA_TYPE_REPLY_ 0x02 // �����؏�
#define _DATA_TYPE_DATA_ 0x03 // ������ʽ
#define _DATA_TYPE_DATA_LENGTH_ 0x04 // �����L��У�
#define _DATA_TYPE_CODING_ 0x05 // �ҕ���ʲ�ᾎ�a��ʽ
#define _DATA_MAX_LENGTH_ 1466 // ��������L��

// ʹ�Úv�� 
// ���磺һ�ֹ���������͡�һ�ֹ����a
// data coding 1 byte CMD 1 byte CODING CMD 
// data type 1 byte CMD/REPLY 1 byte CMD/REPLY CONTENT
// data length 1 CMD 4 BYTE DATA_LENGTH 
// data content 1 CMD 2 LENGTH the rest are BYTE DATA======================= MAX 1469 BYTE
//                              0x03 0xFF(�Ͱ�λ) 0xFF(�߰�λ)
//                              0x04 0xFF 0xFF 0xFF 0xFF

/* ======================����======================= */
#define _CMD_CLOSE_CLIENT_ 0x01
#define _CMD_RECORD_ 0x02 // ��ʼ�ɼ�����
#define _CMD_RECORD_DONOT_TRANSFER_ 0x03 // �_ʼ�ɼ����� ����Ҫ�؂�
#define _CMD_ARE_U_ALIVE_ 0xFF // �������z�y �����]��ʲ����̎��

/* ======================�؏�======================= */
#define _REPLY_SUCCESS_ 0x00 // �؏ͣ��ɹ�
#define _REPLY_FAILED_ 0x01 // �؏ͣ�ʧ��
#define _REPLY_DATA_IS_TRANSFERRED_ 0x02 // �؏� �����ѽ���ݔ
#define _REPLY_NOT_INIT_ 0x03 // �؏ͣ�δ��ʼ��
#define _REPLY_RECORD_OVER_ 0x04 // ���ݲɼ����
#define _REPLY_YEAP_RA9_ 0xFF // �؏ͣ��ǵ���߀������߀朽��ϵģ�

/* ============================���a���x============================ */
#define _CODE_NO_ENCODE_ 0x00 // �o���a
#define _CODE_DHUFF_ 0x01 // �ӑB������


#endif // !_TCP_DEFINE_H_
