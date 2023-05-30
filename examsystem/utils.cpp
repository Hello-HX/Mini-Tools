/*
* 此文件主要包含一些通用的算法
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

//字符串定位（KMP算法）
int indexOf(const char* text, const char* pattern) {
	int i = 0, j = -1;
	int tlen = strlen(text), plen = strlen(pattern);
	int* next = (int*)malloc(sizeof(int) * plen);

	// 初始化next数组
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

	// KMP匹配
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

//快速排序算法
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

//大写转小写
void toLowerCase(char* denstination, char* source) {
	while (*source != '\0') {
		*denstination = tolower(*source);
		denstination++;
		source++;
	}
	*denstination = '\0';
}

//跳过空格
char* skipSpace(char* str) {
	while (*str == ' ') {
		str++;
	}
	return str;
}

//字符串截取
void substring(char* denstination, const char* source, int start, int end) {
	int i = 0;
	for (; start < end; i++, start++) {
		denstination[i] = source[start];
	}
	denstination[i] = '\0';
}

//字符串前插
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
