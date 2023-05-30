#pragma once

//定义链表结构体
typedef struct linkNode {
	void* elem;
	struct linkNode* prior;
	struct linkNode* next;
} linkNode;

//函数
linkNode* createLinkList();

void destory(linkNode* list, bool isFreeElem);

linkNode* seekToEnd(linkNode* list);

linkNode* appendNode(linkNode* list, void* elem);

bool removeNode(linkNode* target, bool isFreeElem);
