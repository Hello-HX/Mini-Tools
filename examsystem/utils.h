#pragma once

//公共参数定义
#define LONG_STR_LENGTH 1024
#define SHORT_STR_LENGTH 128

//通用基础算法
int indexOf(const char* text, const char* pattern);

void quickSort(int list[], int low, int high);

void toLowerCase(char* denstination, char* source);

char* skipSpace(char* str);

void substring(char* denstination, const char* source, int start, int end);

void insertAhead(char* denstination, const char* source);
