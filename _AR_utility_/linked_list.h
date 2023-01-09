#pragma once

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <cstdlib>
#include <cstdio>
#define MAX_PACKAGE 2048

typedef struct _linked_list_ {
	unsigned char data[MAX_PACKAGE];
	unsigned int data_size;
	struct _linked_list_ * next;
	struct _linked_list_ * forward;
} LIST_;

LIST_ * list_init() {
	LIST_ * header = (LIST_ *)malloc(sizeof(LIST_));
	header->data_size = 0;
	header->forward = NULL;
	header->next = NULL;
	return header;
}

LIST_ * list_append(LIST_ * header, LIST_ * node) {
	LIST_ * h = header;
	while (h->next != NULL) {
		h = h->next;
	}
	h->next = node;
	node->forward = h;
	return node;
}

void list_delete(LIST_ * header) {
	LIST_ * h = header;
	while (h != NULL) {
		LIST_ * n = h->next;
		free(h);
		h = n;
	}
}

#endif
