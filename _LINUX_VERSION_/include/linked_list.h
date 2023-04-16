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

LIST_ * list_init();
LIST_ * list_append(LIST_ * header, LIST_ * node);
void list_delete(LIST_ * header);

#endif
