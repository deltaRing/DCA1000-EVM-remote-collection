#pragma once

#ifndef _WAVE_AR_DECODE_H_
#define _WAVE_AR_DECODE_H_

/* ===========================算法部分============================== */
#include "BitTree.h" // 動態哈夫曼編碼樹的定義

#include "../_AR_include_/_AR_UI_INFO_SUBSYSTEM_.h"
#include "../_AR_utility_/linked_list.h"
#include "../_AR_define_/TCP_packet_define.h"
#include <Windows.h>
#include <vector>
#include <mutex>

/* ===========================消息的定义============================= */
extern std::vector <QByteArray> info; // 全局定义
extern std::vector <LEVEL> warn; // 警告程度
extern std::mutex info_mutex; // 

/* =========================外部的停止开关============================ */
extern BOOL chan_data_stop;
extern BOOL new_wave_data;

/* =============================初始化樹=============================== */
_bit_tree_ * _header_ = _init_header_(); // 初始化編碼 HEADER
_bit_tree_ * de_header_ = _init_header_(); // 初始化解碼 HEADER

// 预分配单帧的内存
// ChirpNum x AntennaNum x ADCSampleNum
#define _ADC_SAMPLE_NUM_ 256 // ADC 采样数目
#define _ANTENNA_NUM_ 12 // 天线数目
#define _CHIRP_NUM_ 8 // 脉冲数目
#define IQ 2 // IQ 通道占用两个字节
#define SHORT_TYPE 2 // short 类型占用两个字节
#define _TX_NUM_ 3 // 发射天线的数目
#define _RX_NUM_ 4 // 接受天线的数目

unsigned int _CHIRP_NUMBER_ = 64;
unsigned int _ADC_NUMBER_ = 256;
unsigned int _antenna_num_ = 12;
unsigned int _buff_chirp_number = 64;
unsigned int _buff_adc_number = 256;
unsigned int _buff_antenna_number = 12;
unsigned int _change_chirp_number = 64;
unsigned int _change_adc_number = 256;
unsigned int _change_antenna_number = 12;
UINT32 DATA_COUNTER = 0;
UINT32 FINAL_COUNTER = 0;
UINT32 NEXT_PENDING = 0;

typedef struct _AR_radar_buff_ {
	INT16 ** buff_real = NULL; // [_CHIRP_NUM_][_ADC_SAMPLE_NUM_]
	INT16 ** buff_imag = NULL; // [_CHIRP_NUM_][_ADC_SAMPLE_NUM_]
}_AR_radar_buff_;

LIST_ * _AR_header_ = NULL; // 定义雷达数据链表初始化
LIST_ * _temp_header_ = NULL; // 临时记录的头
BOOL data_is_stop_sampling = TRUE; // 数据停止采集了
BOOL wave_data_is_ready = FALSE; // 波形数据准备完毕
BOOL chan_data_is_ready = FALSE; // 通道数据准备完毕
INT16 * lvds_data = NULL; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_ * IQ];
INT16 * lvds_data_real = NULL; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_];
INT16 * lvds_data_imag = NULL; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_];
INT16 * _TEMP_LVDS_DATA_BUFF_REAL_ = NULL; // [_CHIRP_NUM_ * _ADC_SAMPLE_NUM_ * _RX_NUM_];
INT16 * _TEMP_LVDS_DATA_BUFF_IMAG_ = NULL; // [_CHIRP_NUM_ * _ADC_SAMPLE_NUM_ * _RX_NUM_];
UINT8 * _BUFF_ = NULL; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_ * IQ * SHORT_TYPE]; // 当前使用到的buff
UINT8 * _TEMP_BUFF_ = NULL;   // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_ * IQ * SHORT_TYPE];
UINT32 _BUFF_COUNTER_ = 0; // 当前buffer计数器
_AR_radar_buff_ * _AR_RADAR_WAVE_ = NULL; // [_ANTENNA_NUM_]; // 定义雷达数据结构


void WINAPI _putting_in_AR_radar_buff_() {
	if (!wave_data_is_ready) {
		info_mutex.lock();
		info.push_back("Radar Architecture: Waveform decoder failure! \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}
	extern INT16 * lvds_data; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_ * IQ];
	extern INT16 * lvds_data_real; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_];
	extern INT16 * lvds_data_imag; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_];
	extern INT16 *_TEMP_LVDS_DATA_BUFF_REAL_; // [_CHIRP_NUM_ * _ADC_SAMPLE_NUM_ * _RX_NUM_];
	extern INT16 * _TEMP_LVDS_DATA_BUFF_IMAG_; // [_CHIRP_NUM_ * _ADC_SAMPLE_NUM_ * _RX_NUM_];
	extern _AR_radar_buff_ * _AR_RADAR_WAVE_;

	if (chan_data_stop) return;

	if (lvds_data != NULL) {
		delete[] lvds_data;
		delete[] lvds_data_real;
		delete[] lvds_data_imag;
		delete[] _TEMP_LVDS_DATA_BUFF_REAL_;
		delete[] _TEMP_LVDS_DATA_BUFF_IMAG_;
		for (int ii = 0; ii < _buff_antenna_number; ii++) {
			for (int jj = 0; jj < _buff_chirp_number; jj++) {
				delete [] _AR_RADAR_WAVE_[ii].buff_real[jj];
				delete [] _AR_RADAR_WAVE_[ii].buff_imag[jj];
			}
			delete[] _AR_RADAR_WAVE_[ii].buff_real;
			delete[] _AR_RADAR_WAVE_[ii].buff_imag;
		}
		delete[] _AR_RADAR_WAVE_;
	}
	_buff_antenna_number = _change_antenna_number;
	_buff_adc_number = _change_adc_number;
	_buff_chirp_number = _change_chirp_number;

	lvds_data = new INT16[_buff_antenna_number * IQ * _buff_adc_number * _buff_chirp_number];
	lvds_data_real = new INT16[_buff_antenna_number * IQ * _buff_adc_number * _buff_chirp_number];
	lvds_data_imag = new INT16[_buff_antenna_number * IQ * _buff_adc_number * _buff_chirp_number];
	_TEMP_LVDS_DATA_BUFF_REAL_ = new INT16[_buff_chirp_number * _buff_adc_number * _RX_NUM_];
	_TEMP_LVDS_DATA_BUFF_IMAG_ = new INT16[_buff_chirp_number * _buff_adc_number * _RX_NUM_];
	_AR_RADAR_WAVE_ = new _AR_radar_buff_[_buff_antenna_number];

	memcpy(lvds_data, _TEMP_BUFF_, _buff_adc_number * _buff_antenna_number *
		_buff_chirp_number * IQ * SHORT_TYPE); // 复制内存


	for (int ii = 0; ii < _buff_adc_number * _buff_antenna_number * _buff_chirp_number * IQ; ii += 4) {
		lvds_data_real[ii / 2] = lvds_data[ii];
		lvds_data_real[ii / 2 + 1] = lvds_data[ii + 1];
		lvds_data_imag[ii / 2] = lvds_data[ii + 2];
		lvds_data_imag[ii / 2 + 1] = lvds_data[ii + 3];
	}

	for (int ii = 0; ii < _buff_antenna_number; ii++) {
		_AR_RADAR_WAVE_[ii].buff_real = new INT16 *[_buff_chirp_number];
		_AR_RADAR_WAVE_[ii].buff_imag = new INT16 *[_buff_chirp_number];
		for (int jj = 0; jj < _buff_chirp_number; jj++) {
			_AR_RADAR_WAVE_[ii].buff_real[jj] = new INT16[_buff_adc_number];
			_AR_RADAR_WAVE_[ii].buff_imag[jj] = new INT16[_buff_adc_number];
		}
	}

	for (int ii = 0; ii < _TX_NUM_; ii++) {
		memcpy(_TEMP_LVDS_DATA_BUFF_REAL_,
			lvds_data_real
			+ ii * _buff_adc_number * _RX_NUM_ * _buff_chirp_number * SHORT_TYPE,
			_buff_chirp_number * _buff_adc_number * _RX_NUM_ * SHORT_TYPE);
		memcpy(_TEMP_LVDS_DATA_BUFF_IMAG_,
			lvds_data_imag
			+ ii * _buff_adc_number * _RX_NUM_ * _buff_chirp_number * SHORT_TYPE,
			_buff_chirp_number * _buff_adc_number * _RX_NUM_ * SHORT_TYPE);

		for (int rr = 0; rr < _RX_NUM_; rr++) {
			for (int jj = 0; jj < _buff_chirp_number; jj++) {
				for (int kk = 0; kk < _buff_adc_number; kk++) {
					_AR_RADAR_WAVE_[ii * _RX_NUM_ + rr].buff_real[jj][kk] =
						_TEMP_LVDS_DATA_BUFF_REAL_[rr * _buff_chirp_number * _buff_adc_number +
						jj * _buff_adc_number + kk];
					_AR_RADAR_WAVE_[ii * _RX_NUM_ + rr].buff_imag[jj][kk] =
						_TEMP_LVDS_DATA_BUFF_IMAG_[rr * _buff_chirp_number * _buff_adc_number +
						jj * _buff_adc_number + kk];
				}
			}
		}
	}
	chan_data_is_ready = TRUE;
}

// 填充单帧的数据
// 将其填充为一维列表数据
void WINAPI _putting_in_single_frame_() {
	extern UINT8 * _BUFF_; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_ * IQ * SHORT_TYPE];
	extern UINT8 * _TEMP_BUFF_; // [_ADC_SAMPLE_NUM_ * _ANTENNA_NUM_ * _CHIRP_NUM_ * IQ * SHORT_TYPE];
	extern LIST_ * _AR_header_;
	extern LIST_ * _temp_header_;
	extern UINT32 DATA_COUNTER;
	extern UINT32 FINAL_COUNTER;
	extern UINT32 NEXT_PENDING;

	if (chan_data_stop) { return; }

	if (_BUFF_ != NULL) {
		delete[] _BUFF_;
		delete[] _TEMP_BUFF_;
	}

	if (_temp_header_ == NULL || new_wave_data) {
		_temp_header_ = _AR_header_;
		new_wave_data = 0;
	}
	_antenna_num_ = _change_antenna_number;
	_ADC_NUMBER_ = _change_adc_number;
	_CHIRP_NUMBER_ = _change_chirp_number;
	_BUFF_ = new UINT8[_ADC_NUMBER_ * _antenna_num_ * 
		_CHIRP_NUMBER_ * IQ * SHORT_TYPE];
	_TEMP_BUFF_ = new UINT8[_ADC_NUMBER_ * _antenna_num_ * 
		_CHIRP_NUMBER_ * IQ * SHORT_TYPE];

	if (_AR_header_ == NULL) {
		info_mutex.lock();
		info.push_back("Radar Architecture: _AR_header_ is NULL! \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}
	if (_AR_header_->data_size == 0) {
		info_mutex.lock();
		info.push_back("Radar Architecture: Data is Empty! \n");
		warn.push_back(_UI_ERR_);
		info_mutex.unlock();
		return;
	}

	while (_temp_header_ != NULL) {
		if (chan_data_stop) {
			break;
		}
		if (FINAL_COUNTER == 0) { // 数据已经读取完毕的情况
			if (DATA_COUNTER + _temp_header_->data_size - _PACKAGE_NUMBER_SEQ_LEN_ > _ADC_NUMBER_ * _antenna_num_ * _CHIRP_NUMBER_ * IQ * SHORT_TYPE) {
				FINAL_COUNTER = DATA_COUNTER + _temp_header_->data_size - _ADC_NUMBER_ * _antenna_num_ * _CHIRP_NUMBER_ * IQ * SHORT_TYPE - _PACKAGE_NUMBER_SEQ_LEN_; // 大于了当前一帧的数据
				FINAL_COUNTER = _temp_header_->data_size - FINAL_COUNTER - _PACKAGE_NUMBER_SEQ_LEN_;
			}
			else {
				FINAL_COUNTER = _temp_header_->data_size - _PACKAGE_NUMBER_SEQ_LEN_; // 没有大于
			}
		}
		wave_data_is_ready = FALSE;
		memcpy(_BUFF_ + DATA_COUNTER, _temp_header_->data + _PACKAGE_NUMBER_SEQ_LEN_ + NEXT_PENDING, FINAL_COUNTER);
		if (_temp_header_->data_size - _PACKAGE_NUMBER_SEQ_LEN_ == FINAL_COUNTER) {
			DATA_COUNTER += FINAL_COUNTER;
			_temp_header_ = _temp_header_->next;
			if (_temp_header_ == NULL) {
				_temp_header_ = _AR_header_; // 返回到头节点
				wave_data_is_ready = TRUE; // 最后一个数据
			}
			FINAL_COUNTER = 0;
			NEXT_PENDING = 0;
		}
		else if (NEXT_PENDING != 0) {
			DATA_COUNTER += FINAL_COUNTER;
			_temp_header_ = _temp_header_->next;
			if (_temp_header_ == NULL) {
				_temp_header_ = _AR_header_; // 返回到头节点
				wave_data_is_ready = TRUE; // 最后一个数据
			}
			NEXT_PENDING = 0;
			FINAL_COUNTER = 0;
		}
		else {
			DATA_COUNTER = 0;
			NEXT_PENDING = FINAL_COUNTER;
			FINAL_COUNTER = _temp_header_->data_size - FINAL_COUNTER - _PACKAGE_NUMBER_SEQ_LEN_; // 没读取完毕 继续读取
			memcpy(_TEMP_BUFF_, _BUFF_, _ADC_NUMBER_ * _antenna_num_ * 
				_CHIRP_NUMBER_ * IQ * SHORT_TYPE); // 复制内存
			wave_data_is_ready = TRUE;
			_putting_in_AR_radar_buff_(); // 写入结构体中
			return;
		}
	}
}

void _get_DPCM_code_() {

}

// 不含解碼結果
// 返回：編碼結果 transmit_buff
// 需要輸入：原始數據、原始數據長度
void _get_DHuff_code_(unsigned char d_data[_MAXIMUM_BUFF_CONTAIN_], 
	unsigned int data_size) {
	extern uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_]; // byte数组
	memset(transmit_buff, 0, _MAXIMUM_BUFF_CONTAIN_);

	if (_header_ == NULL) {
		_header_ = _init_header_();
	}

	for (int ii = 0; ii < data_size; ii++) {
		init_transmit_byte();
		bool _res_ = _find_nodes_(_header_, d_data[ii]);
		if (!_res_) {
			_find_nodes_(_header_, d_data[ii], true);
		}
		_dhuff_encode_(_header_, d_data[ii], _res_);
		_update_bit_tree_(_header_);
	}

	_remove_bit_tree_(_header_);
	_header_ = NULL;
}

// 對自適應哈夫曼編碼進行解碼
// 返回：decode_result 解碼結果
// 需要得到：傳輸的數據 内部的bit計數 以及外部的BUFF計數
void _get_D_DHuff_code_(uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_],
	uint8_t transmit_buff_byte_cnt,
	uint16_t transmit_buff_counter) {
	extern uint8_t decode_result[_MAXIMUM_BUFF_CONTAIN_]; // 解碼結果
	memset(decode_result, 0, _MAXIMUM_BUFF_CONTAIN_);

	if (de_header_ == NULL) {
		de_header_ = _init_header_();
	}
	_dhuff_decode_(de_header_, transmit_buff, transmit_buff_byte_cnt, transmit_buff_counter);

	_remove_bit_tree_(de_header_);
	de_header_ = NULL;
}

#endif