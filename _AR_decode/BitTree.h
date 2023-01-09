#pragma once
#ifndef _BITTREE_H_
#define _BITTREE_H_

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "a_coder_define.h"

// 定义
struct _bit_tree_ {
	_bit_tree_ * left = NULL;
	_bit_tree_ * right = NULL;
	_bit_tree_ * parent = NULL;
	int number = 0;
	int weight = 0;
	bool NYT = false;
	bool not_contain_data = false;
	char symbol = 'X';    // 数据符号
	uint8_t code;      // 编码
	char fixcode;   // 固定编码
};

// 初始化头节点
_bit_tree_ * _init_header_() {
	_bit_tree_ * _header_ = new _bit_tree_;
	_header_->NYT = true;
	_header_->not_contain_data = true;
	return _header_;
}

// 遍历各个节点
void _iterate_bit_tree(_bit_tree_ * bit_tree, int * weights, int * numbers, bool * NYTs) {
	if (bit_tree->right == NULL && bit_tree->left == NULL) {
		weights[bit_tree->number] = bit_tree->weight;
		numbers[bit_tree->number] = bit_tree->number;
		NYTs[bit_tree->number] = bit_tree->NYT;
		return;
	}
	if (bit_tree->left != NULL) {
		_iterate_bit_tree(bit_tree->left, weights, numbers, NYTs);
	}
	if (bit_tree->right != NULL) {
		_iterate_bit_tree(bit_tree->right, weights, numbers, NYTs);
	}
	weights[bit_tree->number] = bit_tree->weight;
	numbers[bit_tree->number] = bit_tree->number;
	NYTs[bit_tree->number] = bit_tree->NYT;
}

// 找到对应的节点
_bit_tree_ * _locate_bit_tree_(_bit_tree_ * header, int number) {
	if (header->number == number) {
		return header;
	}
	else {
		if (header->left != NULL) {
			_bit_tree_ * left_node = _locate_bit_tree_(header->left, number);
			if (left_node != NULL)
				return left_node;
		}
		if (header->right != NULL) {
			_bit_tree_ * right_node = _locate_bit_tree_(header->right, number);
			if (right_node != NULL)
				return right_node;
		}
	}
	return NULL;
}

// 计算NYT节点数目
int _compute_NYT_by_data_(_bit_tree_ * header, char data, int cnt = 0) {
	extern uint8_t NYT_node_record[_MAXIMUM_BUFF_DEPTH_]; // NYT记录缓存 记录NYT节点
	extern uint8_t NYT_node_cnt; // NYT记录计数器

	if (header->symbol == data) {
		return cnt;
	}
	else {
		int _cnt_ = -1;
		if (header->left != NULL) {
			_cnt_ = _compute_NYT_by_data_(header->left, data, cnt + 1);
			NYT_node_record[cnt] = 0; // 0: Left
		}
		if (_cnt_ >= 0) return _cnt_;
		if (header->right != NULL) {
			_cnt_ = _compute_NYT_by_data_(header->right, data, cnt + 1);
			NYT_node_record[cnt] = 1; // 1: Right
		}
		if (_cnt_ >= 0) return _cnt_;
	}

	return -1;
}


// 计算NYT节点数目
int _compute_NYT_by_NYT_(_bit_tree_ * header, int cnt = 0) {
	extern uint8_t NYT_node_record[_MAXIMUM_BUFF_DEPTH_]; // NYT记录缓存 记录NYT节点
	extern uint8_t NYT_node_cnt; // NYT记录计数器

	if (header->NYT) {
		return cnt;
	}
	else {
		int _cnt_ = -1;
		if (header->left != NULL) {
			_cnt_ = _compute_NYT_by_NYT_(header->left, cnt + 1);
			NYT_node_record[cnt] = 0; // 0: Left
		}
		if (_cnt_ >= 0) return _cnt_;
		if (header->right != NULL) {
			_cnt_ = _compute_NYT_by_NYT_(header->right, cnt + 1);
			NYT_node_record[cnt] = 1; // 1: Right
		}
		if (_cnt_ >= 0) return _cnt_;
	}

	return -1;
}

// 获取深度
int _get_depth_tree_(_bit_tree_ * bit_tree) {
	if (bit_tree == NULL) {
		return 0;
	}
	if (bit_tree->left == NULL && bit_tree->right == NULL) {
		return 1;
	}
	int depth_l = 0, depth_r = 0;
	if (bit_tree->left != NULL) {
		depth_l = _get_depth_tree_(bit_tree->left);
	}
	if (bit_tree->right != NULL) {
		depth_r = _get_depth_tree_(bit_tree->right);
	}
	return (depth_l > depth_r ? depth_l : depth_r) + 1;
}

// 层序标记树节点
void _label_nodes_(_bit_tree_ * bit_tree, int current_depth, int ex_depth, int & label) {
	// 本节点是叶子节点
	if (ex_depth == current_depth) {
		bit_tree->number = label;
		label++;
		return;
	}
	else {
		if (bit_tree->left != NULL) {
			_label_nodes_(bit_tree->left, current_depth + 1, ex_depth, label);
		}
		if (bit_tree->right != NULL) {
			_label_nodes_(bit_tree->right, current_depth + 1, ex_depth, label);
		}
		return;
	}
}

// 交换二叉树节点
void _swap_bit_tree_(_bit_tree_ * temp1, _bit_tree_ * temp2) {
	if (temp1 == NULL || temp2 == NULL)
		return;
	_bit_tree_ * temp1_parent = temp1->parent;
	_bit_tree_ * temp2_parent = temp2->parent; // 找到父节点

	_bit_tree_ * temp1_left = temp1->left;
	_bit_tree_ * temp2_left = temp2->left;
	_bit_tree_ * temp1_right = temp1->right;
	_bit_tree_ * temp2_right = temp2->right; // 找到子节点

	bool temp1_isleft = temp1_parent->left == temp1;
	bool temp2_isleft = temp2_parent->left == temp2; // 找到节点的左右顺序

	// 父节点交换
	if (temp1_isleft) {
		temp1_parent->left = temp2;
	}
	else {
		temp1_parent->right = temp2;
	}

	if (temp2_isleft) {
		temp2_parent->left = temp1;
	}
	else {
		temp2_parent->right = temp1;
	}
	/*
	// 子节点交换
	if (temp1_left != NULL)
		temp1_left->parent = temp2;
	if (temp1_right != NULL)
		temp1_right->parent = temp2;
	if (temp2_left != NULL)
		temp2_left->parent = temp1;
	if (temp2_right != NULL)
		temp2_right->parent = temp1;
	*/

	// 本节点父节点交换
	temp1->parent = temp2_parent;
	temp2->parent = temp1_parent;
	//temp1->left = temp2_left;
	//temp1->right = temp2_right;
	//temp2->left = temp1_left;
	//temp2->right = temp1_right;
}

// 更新二叉树的权重 应该向下搜索
void _update_bit_tree_weight_(_bit_tree_ * bit_tree) {
	if (bit_tree->left == NULL && bit_tree->right == NULL) {
		if (bit_tree->parent != NULL) {
			_update_bit_tree_weight_(bit_tree->parent);
		}
		return; // 叶子节点 不需要更新
	}
	else {
		bit_tree->weight = bit_tree->left->weight +
			bit_tree->right->weight;
		if (bit_tree->parent != NULL) {
			_update_bit_tree_weight_(bit_tree->parent);
		}
	}
}

// 更新所有的叶子节点的权重
void _update_all_weights_(_bit_tree_ * bit_tree, int & node_value) {
	// 先找叶子节点
	if (bit_tree == NULL)
		return;
	if (bit_tree->left != NULL && bit_tree->right != NULL) {
		int _left_node_value_ = 0, _right_node_value_ = 0;
		if (bit_tree->right != NULL)
			_update_all_weights_(bit_tree->right, _right_node_value_);
		if (bit_tree->left != NULL)
			_update_all_weights_(bit_tree->left, _left_node_value_);
		bit_tree->weight = _left_node_value_ + _right_node_value_;
		node_value = bit_tree->weight;
		return;
	}
	else {
		node_value = bit_tree->weight;
		return;
	}
	return;
}

// 更新二叉树 并 交换二叉树的权重
void _update_bit_tree_(_bit_tree_ * bit_tree) {
	int label = 0, depth = 0;
	depth = _get_depth_tree_(bit_tree);
	while (depth > 0) {
		_label_nodes_(bit_tree, 1, depth, label);
		depth -= 1;
	}
	// _label_nodes_(bit_tree, label);
	// 先标号，再检查各自的权重
	int * weights = new int[label];
	int * numbers = new int[label];
	bool * NYTs = new bool[label];
	_iterate_bit_tree(bit_tree, weights, numbers, NYTs);

	//for (int ii = 0; ii < label; ii++) {
	//	cout << weights[ii] << " ";
	//}
	//cout << endl;

	for (int ii = 0; ii < label; ii++) {
		for (int jj = ii; jj < label; jj++) {
			if (weights[ii] > weights[jj]) {
				// 交换节点
				//std::cout << weights[ii] << " " << weights[jj] << endl;
				_bit_tree_ * temp1 = _locate_bit_tree_(bit_tree, ii);
				_bit_tree_ * temp2 = _locate_bit_tree_(bit_tree, jj);
				_swap_bit_tree_(temp1, temp2);
				int _temp_ = 0;
				_update_all_weights_(bit_tree, _temp_); // 更新权重
				_update_bit_tree_(bit_tree);
				delete[] weights;
				delete[] numbers;
				delete[] NYTs;
				return;
			}
		}
	}

	delete[] weights;
	delete[] numbers;
	delete[] NYTs;
}

// 创建二叉树
_bit_tree_ * _create_bit_tree_() {
	return new _bit_tree_;
}

// 默认是在右边插入节点 左边插入NYT节点
bool _insert_bit_tree_(_bit_tree_ * bit_tree, char data) {
	if (bit_tree == NULL) return false;

	_bit_tree_ * new_node = _create_bit_tree_();
	_bit_tree_ * NYT_node = _create_bit_tree_();

	new_node->parent = bit_tree;
	new_node->NYT = false;
	new_node->symbol = data;
	new_node->not_contain_data = false;
	new_node->weight = 1;
	NYT_node->parent = bit_tree;
	NYT_node->NYT = true;
	NYT_node->weight = 0;
	NYT_node->not_contain_data = true;

	if (bit_tree->NYT) {
		bit_tree->NYT = false;
	}
	bit_tree->left = NYT_node;
	bit_tree->right = new_node;

	_update_bit_tree_weight_(bit_tree);

	return true;
}

// 删除二叉树
void _remove_bit_tree_(_bit_tree_ * bit_tree) {
	// TODO
	if (bit_tree->left == NULL && bit_tree->right == NULL) {
		delete bit_tree;
		return;
		// 如果是叶子节点 返回
	}

	if (bit_tree->left != NULL) {
		_remove_bit_tree_(bit_tree->left);
	}
	if (bit_tree->right != NULL) {
		_remove_bit_tree_(bit_tree->right);
	}
	// 最后删除自己
	delete bit_tree;
}

// 找到节点并增加权重、如果没有找到权重，那就往后面增加节点
bool _find_nodes_(_bit_tree_ * bit_tree, char data, bool NYTnode = false, uint32_t cnt = 0) {
	extern uint8_t transmit_byte_counter;
	extern uint8_t transmit_byte[_MAXIMUM_BUFF_LENGTH_];

	if (!NYTnode) {
		// 先找节点，没有找到就找NYTnode
		if (bit_tree->symbol == data) {
			bit_tree->weight++;
			transmit_byte_counter = cnt;
			_update_bit_tree_weight_(bit_tree);
			return true;
		}
		else {
			if (bit_tree->left == NULL && bit_tree->right == NULL) {
				// 是否应该插入子节点？
				if (bit_tree->parent == NULL && bit_tree->weight == 0) {
					//_find_nodes_(bit_tree, data, true);
					//return false;
				} // 如果检查到该节点是初始节点，那么直接创建新节点
				return false; // 到叶子节点了 
			}
			bool left_node_found = false;
			if (bit_tree->left != NULL) {
				transmit_byte[cnt] = 0;
				left_node_found = _find_nodes_(bit_tree->left, data, false, cnt + 1);
				if (left_node_found)
					return true;
			}
			bool right_node_found = false;
			if (bit_tree->right != NULL) {
				transmit_byte[cnt] = 1;
				right_node_found = _find_nodes_(bit_tree->right, data, false, cnt + 1);
				if (right_node_found)
					return true;
			}
			// 遍历两边的
			return false;
		}
	}

	// 以上操作均没有找到节点，找到NYTnode
	if (!bit_tree->NYT) {
		if (bit_tree->left != NULL)
			_find_nodes_(bit_tree->left, data, true);
		if (bit_tree->right != NULL)
			_find_nodes_(bit_tree->right, data, true);
	}
	else {
		_insert_bit_tree_(bit_tree, data);
	}
	return false;
}

// 获取位
int _get_bit_(char data, int num) {
	if (num < 0 || num >= 8) {
		printf("DHuffman: 八位数据不可能大于8或者小于0\n");
		return -1;
	}
	else {
		return _rotr(data, 1) & 0x01;
	}
}

// 指定 0---255 是各类数据的编码
void _dhuff_encode_(_bit_tree_ * header, unsigned char data, bool _in_tree_) {
	extern uint8_t transmit_byte_counter;
	extern uint8_t transmit_buff_byte_cnt;
	extern uint16_t transmit_buff_counter;
	extern uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_];
	extern uint8_t transmit_byte[_MAXIMUM_BUFF_LENGTH_];

	uint8_t byte_in_buff = transmit_byte_counter;
	uint8_t internal_cnt = transmit_buff_byte_cnt;
	uint16_t external_cnt = transmit_buff_counter;
	unsigned char data_buff = transmit_buff[external_cnt]; // 取出数据
	uint32_t NYTs = 0;
	if (_in_tree_) {

	}
	else {
		init_NYT_buff();
		// 计算出多少个NYTs
		// NYTs = _compute_NYT_by_data_(header, data, 0);
		NYTs = _compute_NYT_by_NYT_(header, 0);
		if (NYTs < 0) {
			// printf("DHuffman: NYTs 不可能为负数！\n");
			return;
		}
		for (int ii = 0; ii < NYTs; ii++) {
			data_buff = _rotl(data_buff, 1); // 0
			data_buff += NYT_node_record[ii]; // 第一层 
			internal_cnt += 1;
			if (internal_cnt % 8 == 0) {
				internal_cnt = 0;
				transmit_buff[external_cnt] = data_buff;
				external_cnt += 1;
				data_buff = transmit_buff[external_cnt];
			}
		}

		for (int ii = 0; ii < 8; ii++) {
			data_buff = _rotl(data_buff, 1); // 0
			data_buff += _rotr((_rotl(data, ii)) & 0x80, 7);
			internal_cnt += 1;
			if (internal_cnt % 8 == 0) {
				internal_cnt = 0;
				transmit_buff[external_cnt] = data_buff;
				external_cnt += 1;
				data_buff = transmit_buff[external_cnt];
			}
		}
		transmit_buff_byte_cnt = internal_cnt;
		transmit_buff_counter = external_cnt;
		transmit_buff[external_cnt] = data_buff;
		return;
	}

	// 取出编码数据
	for (int ii = 0; ii < byte_in_buff; ii++) {
		if (transmit_byte[ii]) {
			// 1
			data_buff = _rotl(data_buff, 1); data_buff += 1;
		}
		else {
			// 0
			data_buff = _rotl(data_buff, 1); // 0
		}
		internal_cnt += 1;
		if (internal_cnt % 8 == 0) {
			internal_cnt = 0;
			transmit_buff[external_cnt] = data_buff;
			external_cnt += 1;
			data_buff = transmit_buff[external_cnt];
		}
	}

	transmit_buff_byte_cnt = internal_cnt;
	transmit_buff_counter = external_cnt;
	transmit_buff[external_cnt] = data_buff;
}

// 解码子程序3 根据新节点自增数据 并同时更新权重
void _dhuff_sub_proc_update_tree(_bit_tree_ * _header_, uint8_t data) {
	_find_nodes_(_header_, data);
	_update_bit_tree_(_header_);
}

// 解码子程序2 新建立数据
void _dhuff_build_new_node_sub_proc(_bit_tree_ * header, uint8_t data) {
	_find_nodes_(header, data, true);
	_update_bit_tree_(header);
}

// 解码子程序1 返回 _header_
// 正常返回：非NULL节点
// 异常返回：NULL节点需要新建立子节点
_bit_tree_ * _dhuff_decode_sub_proc(_bit_tree_ * _header_, uint8_t bit) {
	extern uint8_t temp_read_buff[_MAXIMUM_BUFF_READ_];
	extern uint8_t used_bit; // 用到的位 计数
	extern uint8_t used_byte; // 用到的字节 计数

	if (bit == 1) {
		if (_header_->right == NULL) {
			return NULL;
		}
		else {
			_bit_tree_ * _right_ = _header_->right;
			return _right_;
		}
	}
	else {
		if (_header_->left == NULL) {
			return NULL;
		}
		else {
			_bit_tree_ * _left_ = _header_->left;
			return _left_;
		}
	}
	return NULL;
}

// 动态霍夫曼树 解码
void _dhuff_decode_(_bit_tree_ * _header_, uint8_t transmit_buff[_MAXIMUM_BUFF_CONTAIN_], 
	uint8_t transmit_buff_byte_cnt, uint16_t transmit_buff_counter) {
	extern uint8_t temp_read_buff[_MAXIMUM_BUFF_READ_];
	extern uint8_t decode_result[_MAXIMUM_BUFF_CONTAIN_];
	extern uint8_t used_bit; // 用到的位 计数
	extern uint8_t used_byte; // 用到的字节 计数
	extern uint16_t decode_cnt;

	decode_cnt = 0;
	init_write_buff();
	init_read_buff();
	// 初始化

	if (_header_ == NULL) _header_ = _init_header_(); // 初始化头表

	uint8_t internal_cnt = transmit_buff_byte_cnt;
	uint16_t external_cnt = transmit_buff_counter;
	uint16_t current_cnt = 0; // 当前循环使用到的位
	uint16_t previous_loop_used_bit = 0; // 上次循环使用到的位
	_bit_tree_ * temp_tree = _header_;

	if (internal_cnt != 0) {
		uint8_t _temp_buff_ = transmit_buff[external_cnt];
		_temp_buff_ = _rotl(_temp_buff_, 8 - internal_cnt);
		/*transmit_buff[external_cnt] = _rotr(_temp_buff_ & 0x80, 7) +
			_rotl(_rotr(_temp_buff_ & 0x40, 6), 1) +
			_rotl(_rotr(_temp_buff_ & 0x20, 5), 2) +
			_rotl(_rotr(_temp_buff_ & 0x10, 4), 3) +
			_rotl(_rotr(_temp_buff_ & 0x08, 3), 4) +
			_rotl(_rotr(_temp_buff_ & 0x04, 2), 5) +
			_rotl(_rotr(_temp_buff_ & 0x02, 1), 6) +
			_rotl(_rotr(_temp_buff_ & 0x01, 0), 7);*/
		transmit_buff[external_cnt] = _temp_buff_;
	}

	for (int ii = 0; ii <= external_cnt; ii++) {
		// 检测数据internal_cnt是不是刚好为0 如果是 不需要最后一层外部external_cnt遍历
		if (ii == external_cnt && internal_cnt == 0) {
			break;
		}
		uint8_t buff = transmit_buff[ii]; // 取出数据
		uint8_t internal_cnt_loop = (ii == external_cnt ? internal_cnt : 8); // 计算偏移数据
		// previous_loop_used_bit = (ii == external_cnt ? internal_cnt : previous_loop_used_bit); // 给最后一位数据做偏移
		bool receive_new_data = false;
		for (int jj = previous_loop_used_bit; jj < internal_cnt_loop; jj++) {
			uint8_t bit = _rotr((_rotl(buff, jj) & 0x80), 7);
			current_cnt = jj;
			temp_tree = _dhuff_decode_sub_proc(temp_tree, bit);
			if (temp_tree == NULL) {
				receive_new_data = true;
				temp_tree = _header_; // 重置节点
				current_cnt++;
				break; // 接受新的数据
			}
			else {
				if (temp_tree->not_contain_data) {
					continue;
				}
				else {
					uint8_t data = temp_tree->symbol;
					decode_result[decode_cnt] = data;
					decode_cnt += 1;
					_dhuff_sub_proc_update_tree(_header_, data); // 更新节点
					temp_tree = _header_; // 重置节点
				}
			}
		}
		previous_loop_used_bit = 0;
		if (receive_new_data) {
			uint8_t temp_new_data = 0;
			uint8_t temp_buff = transmit_buff[ii];
			for (int iii = current_cnt; iii < internal_cnt_loop; iii++) {
				// 定义数据都是8位的
				temp_new_data = _rotl(temp_new_data, 1);
				uint8_t bit = _rotr(_rotl(temp_buff, iii) & 0x80, 7);
				temp_new_data += bit;
			}
			uint8_t next_data_end = current_cnt;
			temp_buff = transmit_buff[ii + 1]; // 进入下一个字节
			for (int iii = 0; iii < next_data_end; iii++) {
				// 定义数据都是8位的
				temp_new_data = _rotl(temp_new_data, 1);
				uint8_t bit = _rotr(_rotl(temp_buff, iii) & 0x80, 7);
				temp_new_data += bit;
			}
			previous_loop_used_bit = next_data_end; // 更新
			_dhuff_build_new_node_sub_proc(_header_, temp_new_data);
			temp_tree = _header_;
			decode_result[decode_cnt] = temp_new_data;
			decode_cnt += 1;
		}
	}
}

// 动态霍夫曼树 预编码
void _dhuff_signal_pre_estimate() {

}

#endif
