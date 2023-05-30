/*
* 此文件为链表的一些基础功能实现
* 该链表为双向循环链表，除了头节点以外，其他节点的元素不允许为空，用于判断该节点是否为头节点
*/

#include <stdlib.h>

#include "linkList.h"

//创建链表
linkNode* createLinkList() {
	linkNode* headNode = (linkNode*)malloc(sizeof(linkNode));
	if (headNode == NULL) {
		return NULL;
	}

	headNode->elem = NULL;
	headNode->prior = headNode;
	headNode->next = headNode;
	return headNode;
}

//销毁链表
void destory(linkNode* list, bool isFreeElem) {
	linkNode* start = list;
	linkNode* temp;
	list = list->next;
	while (list != start) {
		temp = list->next;
		if (isFreeElem)
			free(list->elem); //释放存放元素的空间
		free(list);
		list = temp;
	}
	free(start);
}

//移动到表尾
linkNode* seekToEnd(linkNode* list) {
	while (list->next->elem != NULL) {
		list = list->next;
	}
	return list;
}

//追加（插在表头）
linkNode* appendNode(linkNode* list, void* elem) {
	if (list->elem != NULL || elem == NULL) {
		return NULL;
	}

	//插入新节点
	linkNode* newNode = (linkNode*)malloc(sizeof(linkNode));
	if (newNode == NULL)
		return NULL;

	newNode->elem = elem;
	newNode->prior = list;
	newNode->next = list->next;
	list->next->prior = newNode;
	list->next = newNode;
	return newNode;
}

//移除
bool removeNode(linkNode* target, bool isFreeElem) {
	if (target->elem == NULL) {
		return false;
	}

	target->prior->next = target->next;
	target->next->prior = target->prior;

	if (isFreeElem) {
		free(target->elem);
	}
	free(target);

	return true;
}
