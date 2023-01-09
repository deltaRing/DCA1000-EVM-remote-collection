#pragma once

#ifndef _SERVER_LIST_H_
#define _SERVER_LIST_H_

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#define _DESCRIBE_MAX_LEN_ 1024

#if defined _WIN32
#include <Windows.h>
#include <winsock.h>
#pragma comment (lib,"ws2_32.lib")

// Client 定义
typedef struct server_connnected_list {
	struct server_connnected_list * next;
	struct server_connnected_list * forward;
	UINT32 DISCON_TIME; // 断开链接时长
	SOCKET client;
	SOCKADDR_IN ip_addr;
	BOOL ALIVE = 1; // 是否还链接了的
	char describe[_DESCRIBE_MAX_LEN_]; // 描述节点
	unsigned int index;
} SERV_CON_LIST;

// 该节点是否还连接上的结构体定义
typedef struct server_client_info {
	BOOL exist;
	unsigned int index;
	SERV_CON_LIST * node_addr;
} SERV_CON_INFO;

SERV_CON_LIST * header = NULL; // 头节点
SERV_CON_LIST * footer = NULL; // 尾节点
UINT32 CONNECT_NUM = 0;

// node 初始化
SERV_CON_LIST * server_client_node_init() {
	SERV_CON_LIST * node = (SERV_CON_LIST *)malloc(sizeof(SERV_CON_LIST));
	node->next = NULL;
	node->forward = NULL;
	node->ALIVE = 1;
	node->DISCON_TIME = 0;
	return node;
}

// 添加 node
void server_client_append_node(SERV_CON_LIST * header, SERV_CON_LIST * node) {
	SERV_CON_LIST * h = header;
	while (h->next != NULL) h = h->next;
	h->next = node;
	node->forward = h;
	node->index = h->index + 1;
	footer = node; // 尾节点更新
}

// 更新 index
void server_client_update(SERV_CON_LIST * header) {
	extern UINT32 CONNECT_NUM;
	if (header == NULL) return;
	int index = 0;
	CONNECT_NUM = 0;
	SERV_CON_LIST * h = header;
	h->index = index;
	while (h->next != NULL) {
		index++; CONNECT_NUM++;
		h = h->next;
		h->index = index;
	}
	footer = h; // 更新尾节点
	printf("TCP SERVER: 已更新节点，当前节点连接数目为：%d",
		CONNECT_NUM);
}

// 通过 index 删除 node
SERV_CON_LIST * server_client_delete_node_by_index(SERV_CON_LIST * header, int index) {
	extern SERV_CON_LIST * footer; // 尾节点
	extern UINT32 CONNECT_NUM;

	SERV_CON_LIST * h = header;

	if (index >= CONNECT_NUM || index < 0) {
		printf("TCP SERVER: 要删除的节点为: %d, 但该节点不存在、序号错误、或者大于当前连接的数目 %d\n",
			index,
			CONNECT_NUM
		);
		return h;
	}

	// 先考虑特殊情况
	if (index == 0) { // 首节点
		if (h->next != NULL) {
			h = h->next;
			h->forward = NULL;
			server_client_update(h);
		}
		else h = NULL;
		closesocket(header->client);
		free(header);
		return h;
	}
	else if (index == CONNECT_NUM - 1) {
		h = footer->forward; // 尾节点
		closesocket(footer->client);
		free(footer);
		footer = h;
		footer->next = NULL;
		h = header;
		server_client_update(h);
		return h;
	}

	if (index < CONNECT_NUM / 2) { // 先定位
		while (h->index != index) {
			h = h->next;
		}
	}
	else {
		h = footer;
		while (h->index != index) {
			h = h->forward;
		}
	}
	// 已定位
	SERV_CON_LIST * n = h;
	n->forward->next = n->next;
	n->next->forward = n->forward;
	closesocket(h->client);
	free(h);
	server_client_update(header);

	printf("TCP SERVER: 已删除指定节点");

	return header;
}

// 通过 IP 删除 node
SERV_CON_LIST * server_client_detele_node_by_ip(SERV_CON_LIST * header, SOCKADDR_IN ip) {
	extern SERV_CON_LIST * footer; // 尾节点
	extern UINT32 CONNECT_NUM;

	SERV_CON_LIST * h = header;

	while (h->ip_addr.sin_addr.S_un.S_addr != ip.sin_addr.S_un.S_addr) {
		if (h->next == NULL) {
			printf("TCP SERVER WARNING： 没有该IP的节点");
			return header;
		}
		h = h->next;
	}

	SERV_CON_LIST * n = h;
	if (n->forward == NULL) {
		header = n->next;
		closesocket(n->client);
		free(n);
	}
	else if (n->next == NULL && n->forward != NULL) {
		n->forward->next = NULL;
		closesocket(n->client);
		free(n);
	}
	else {
		n->forward->next = n->next;
		n->next->forward = n->forward;
		closesocket(h->client);
		free(h);
	}
	server_client_update(header);

	printf("TCP SERVER: 已删除指定节点");

	return header;
}

// 查找该index的节点
SERV_CON_INFO * server_client_check_exist_by_index(SERV_CON_LIST * header, int index) {
	extern SERV_CON_LIST * footer; // 尾节点
	extern UINT32 CONNECT_NUM;

	SERV_CON_INFO * info = (SERV_CON_INFO *)malloc(sizeof(SERV_CON_INFO));
	SERV_CON_LIST * h = header;
	info->exist = 0;
	info->index = -1;
	info->node_addr = NULL;
	if (index < 0 || index >= CONNECT_NUM) {
		printf("TCP SERVER: 要查找的节点为: %d, 但该节点不存在、序号错误、或者大于当前连接的数目 %d\n",
			index,
			CONNECT_NUM
		);
		return info;
	}

	if (index < CONNECT_NUM / 2) {
		while (h->index != index) h = h->next;
	}
	else {
		h = footer;
		while (h->index != index) h = h->forward;
	}
	info->exist = 1;
	info->index = index;
	info->node_addr = h;

	return info;
}

/*
// 通过IP查找节点
SERV_CON_INFO * server_client_check_exist_by_ip(SERV_CON_LIST * header, SOCKADDR_IN ip) {


}
*/

// 展示连接了的客户端
void server_show_connected_client(SERV_CON_LIST * header) {
	SERV_CON_LIST * n = header;
	while (n != NULL) {
		printf("\n");
		n = n->next;
	}
}

// 删除所有客户端
void server_delete_all_client(SERV_CON_LIST * header) {
	SERV_CON_LIST * n = header;
	while (n != NULL) {
		SERV_CON_LIST * node = n->next;
		if (n->ALIVE)
			closesocket(n->client);
		free(n);
		n = node;
	}
	printf("已删除所有节点\n");
}


#endif // 

#endif // !_SERVER_LIST_H_
