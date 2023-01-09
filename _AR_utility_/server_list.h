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

// Client ����
typedef struct server_connnected_list {
	struct server_connnected_list * next;
	struct server_connnected_list * forward;
	UINT32 DISCON_TIME; // �Ͽ�����ʱ��
	SOCKET client;
	SOCKADDR_IN ip_addr;
	BOOL ALIVE = 1; // �Ƿ������˵�
	char describe[_DESCRIBE_MAX_LEN_]; // �����ڵ�
	unsigned int index;
} SERV_CON_LIST;

// �ýڵ��Ƿ������ϵĽṹ�嶨��
typedef struct server_client_info {
	BOOL exist;
	unsigned int index;
	SERV_CON_LIST * node_addr;
} SERV_CON_INFO;

SERV_CON_LIST * header = NULL; // ͷ�ڵ�
SERV_CON_LIST * footer = NULL; // β�ڵ�
UINT32 CONNECT_NUM = 0;

// node ��ʼ��
SERV_CON_LIST * server_client_node_init() {
	SERV_CON_LIST * node = (SERV_CON_LIST *)malloc(sizeof(SERV_CON_LIST));
	node->next = NULL;
	node->forward = NULL;
	node->ALIVE = 1;
	node->DISCON_TIME = 0;
	return node;
}

// ��� node
void server_client_append_node(SERV_CON_LIST * header, SERV_CON_LIST * node) {
	SERV_CON_LIST * h = header;
	while (h->next != NULL) h = h->next;
	h->next = node;
	node->forward = h;
	node->index = h->index + 1;
	footer = node; // β�ڵ����
}

// ���� index
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
	footer = h; // ����β�ڵ�
	printf("TCP SERVER: �Ѹ��½ڵ㣬��ǰ�ڵ�������ĿΪ��%d",
		CONNECT_NUM);
}

// ͨ�� index ɾ�� node
SERV_CON_LIST * server_client_delete_node_by_index(SERV_CON_LIST * header, int index) {
	extern SERV_CON_LIST * footer; // β�ڵ�
	extern UINT32 CONNECT_NUM;

	SERV_CON_LIST * h = header;

	if (index >= CONNECT_NUM || index < 0) {
		printf("TCP SERVER: Ҫɾ���Ľڵ�Ϊ: %d, ���ýڵ㲻���ڡ���Ŵ��󡢻��ߴ��ڵ�ǰ���ӵ���Ŀ %d\n",
			index,
			CONNECT_NUM
		);
		return h;
	}

	// �ȿ����������
	if (index == 0) { // �׽ڵ�
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
		h = footer->forward; // β�ڵ�
		closesocket(footer->client);
		free(footer);
		footer = h;
		footer->next = NULL;
		h = header;
		server_client_update(h);
		return h;
	}

	if (index < CONNECT_NUM / 2) { // �ȶ�λ
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
	// �Ѷ�λ
	SERV_CON_LIST * n = h;
	n->forward->next = n->next;
	n->next->forward = n->forward;
	closesocket(h->client);
	free(h);
	server_client_update(header);

	printf("TCP SERVER: ��ɾ��ָ���ڵ�");

	return header;
}

// ͨ�� IP ɾ�� node
SERV_CON_LIST * server_client_detele_node_by_ip(SERV_CON_LIST * header, SOCKADDR_IN ip) {
	extern SERV_CON_LIST * footer; // β�ڵ�
	extern UINT32 CONNECT_NUM;

	SERV_CON_LIST * h = header;

	while (h->ip_addr.sin_addr.S_un.S_addr != ip.sin_addr.S_un.S_addr) {
		if (h->next == NULL) {
			printf("TCP SERVER WARNING�� û�и�IP�Ľڵ�");
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

	printf("TCP SERVER: ��ɾ��ָ���ڵ�");

	return header;
}

// ���Ҹ�index�Ľڵ�
SERV_CON_INFO * server_client_check_exist_by_index(SERV_CON_LIST * header, int index) {
	extern SERV_CON_LIST * footer; // β�ڵ�
	extern UINT32 CONNECT_NUM;

	SERV_CON_INFO * info = (SERV_CON_INFO *)malloc(sizeof(SERV_CON_INFO));
	SERV_CON_LIST * h = header;
	info->exist = 0;
	info->index = -1;
	info->node_addr = NULL;
	if (index < 0 || index >= CONNECT_NUM) {
		printf("TCP SERVER: Ҫ���ҵĽڵ�Ϊ: %d, ���ýڵ㲻���ڡ���Ŵ��󡢻��ߴ��ڵ�ǰ���ӵ���Ŀ %d\n",
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
// ͨ��IP���ҽڵ�
SERV_CON_INFO * server_client_check_exist_by_ip(SERV_CON_LIST * header, SOCKADDR_IN ip) {


}
*/

// չʾ�����˵Ŀͻ���
void server_show_connected_client(SERV_CON_LIST * header) {
	SERV_CON_LIST * n = header;
	while (n != NULL) {
		printf("\n");
		n = n->next;
	}
}

// ɾ�����пͻ���
void server_delete_all_client(SERV_CON_LIST * header) {
	SERV_CON_LIST * n = header;
	while (n != NULL) {
		SERV_CON_LIST * node = n->next;
		if (n->ALIVE)
			closesocket(n->client);
		free(n);
		n = node;
	}
	printf("��ɾ�����нڵ�\n");
}


#endif // 

#endif // !_SERVER_LIST_H_
