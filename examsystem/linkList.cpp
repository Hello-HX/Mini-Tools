/*
* ���ļ�Ϊ�����һЩ��������ʵ��
* ������Ϊ˫��ѭ����������ͷ�ڵ����⣬�����ڵ��Ԫ�ز�����Ϊ�գ������жϸýڵ��Ƿ�Ϊͷ�ڵ�
*/

#include <stdlib.h>

#include "linkList.h"

//��������
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

//��������
void destory(linkNode* list, bool isFreeElem) {
	linkNode* start = list;
	linkNode* temp;
	list = list->next;
	while (list != start) {
		temp = list->next;
		if (isFreeElem)
			free(list->elem); //�ͷŴ��Ԫ�صĿռ�
		free(list);
		list = temp;
	}
	free(start);
}

//�ƶ�����β
linkNode* seekToEnd(linkNode* list) {
	while (list->next->elem != NULL) {
		list = list->next;
	}
	return list;
}

//׷�ӣ����ڱ�ͷ��
linkNode* appendNode(linkNode* list, void* elem) {
	if (list->elem != NULL || elem == NULL) {
		return NULL;
	}

	//�����½ڵ�
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

//�Ƴ�
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
