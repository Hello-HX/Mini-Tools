/*
* ���ļ���Ҫ����һЩͨ�õ��㷨
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

//�ַ�����λ��KMP�㷨��
int indexOf(const char* text, const char* pattern) {
	int i = 0, j = -1;
	int tlen = strlen(text), plen = strlen(pattern);
	int* next = (int*)malloc(sizeof(int) * plen);

	// ��ʼ��next����
	next[0] = -1;
	while (i < plen - 1) {
		if (j == -1 || pattern[i] == pattern[j]) {
			++i;
			++j;
			next[i] = j;
		}
		else {
			j = next[j];
		}
	}

	// KMPƥ��
	i = 0; j = 0;
	while (i < tlen && j < plen) {
		if (j == -1 || text[i] == pattern[j]) {
			++i;
			++j;
		}
		else {
			j = next[j];
		}
	}

	return j == plen ? i - j : -1;
}

//���������㷨
static int partition(int list[], int low, int high) {
	int pivot = list[low];
	while (low < high) {
		while (low < high && list[high] >= pivot)
			high--;
		list[low] = list[high];
		while (low < high && list[low] <= pivot)
			low++;
		list[high] = list[low];
	}
	list[low] = pivot;
	return low;
}

void quickSort(int list[], int low, int high) {
	if (low < high) {
		int pivot = partition(list, low, high);
		quickSort(list, low, pivot - 1);
		quickSort(list, pivot + 1, high);
	}
}

//��дתСд
void toLowerCase(char* denstination, char* source) {
	while (*source != '\0') {
		*denstination = tolower(*source);
		denstination++;
		source++;
	}
	*denstination = '\0';
}

//�����ո�
char* skipSpace(char* str) {
	while (*str == ' ') {
		str++;
	}
	return str;
}

//�ַ�����ȡ
void substring(char* denstination, const char* source, int start, int end) {
	int i = 0;
	for (; start < end; i++, start++) {
		denstination[i] = source[start];
	}
	denstination[i] = '\0';
}

//�ַ���ǰ��
void insertAhead(char* denstination, const char* source) {
	int dlen, slen;
	dlen = strlen(denstination);
	slen = strlen(source);
	for (int i = dlen - 1; i >= 0; i--) {
		denstination[i + slen] = denstination[i];
	}
	for (int i = 0; i < slen; i++) {
		denstination[i] = source[i];
	}
	denstination[dlen + slen] = '\0';
}
