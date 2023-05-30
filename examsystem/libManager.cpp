/*
* 此文件主要是对题库的一些管理 / 操作相关的函数
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "libManager.h"
#include "linkList.h"
#include "utils.h"

//内部参数
static linkNode* indexOfLib(question* ques); //定位question在链表中的位置
static bool contains(question* ques, char* keyword); //是否包含关键词

static char libFileName[SHORT_STR_LENGTH]; //题库文件路径
static char libTitle[SHORT_STR_LENGTH]; //题库标题
static linkNode* libQuestions; //存储题目的链表
static int libQuesLength = 0; //题库中的题目数量

static bool isEdited = false; //题库是否被编辑过

//创建题库
bool createQuestionsLib(char* fileName, char* title) {
	FILE* filep = fopen(fileName, "w");
	if (filep != NULL) {
		fwrite(title, sizeof(char), sizeof(title), filep);
		fclose(filep);
		return true;
	}
	return false;
}

//载入题库
bool loadQuestionsLib(char* fileName) {
	if (libQuestions != NULL) {
		return false;
	}

	strcpy(libFileName, fileName);
	FILE* filep = fopen(libFileName, "r");
	if (filep == NULL) {
		return NULL;
	}
	//读取标题
	fgets(libTitle, SHORT_STR_LENGTH, filep);
	
	//读取题目数据
	libQuestions = createLinkList();
	linkNode* pointer = libQuestions;
	question* temp = (question*)malloc(sizeof(question));
	int program = 0;
	short int answer;
	char buffer[LONG_STR_LENGTH];
	while (true) {
		//录入题干
		switch (program) {
		case 0:
			fgets(buffer, LONG_STR_LENGTH, filep);
			//检查读取内容是否为空（需要考虑换行符）
			if (buffer != NULL) {
				strcpy(buffer, skipSpace(buffer));
				if (*buffer != '\n' && *buffer != '\0') {
					substring(temp->stem, buffer, 0, strlen(buffer) - 1);
					program++;
				}
			}
			break;
		//录入答案
		case 5:
			fgets(buffer, SHORT_STR_LENGTH, filep);
			if (buffer != NULL) {
				strcpy(buffer, skipSpace(buffer));
				if (*buffer != '\n' && *buffer != '\0') {
					answer = *buffer - 48;
					//答案只有四个选项
					if (answer >= 0 && answer <= 3) {
						temp->answer = answer;
						appendNode(pointer, temp);
						libQuesLength++;
						temp = (question*)malloc(sizeof(question));
					}
					program = 0;
				}
			}
			break;
		//录入选项
		default:
			fgets(buffer, SHORT_STR_LENGTH, filep);
			if (buffer != NULL) {
				strcpy(buffer, skipSpace(buffer));
				if (*buffer != '\n' && *buffer != '\0') {
					substring(temp->choices[program - 1], buffer, 0, strlen(buffer) - 1);
					program++;
				}
			}
			break;
		}
		//文件读到末尾
		if (feof(filep)) {
			fclose(filep);
			free(temp);
			break;
		}
	}

	isEdited = false;
	return true;
}

//搜索题库
linkNode* searchLib(linkNode* list, char* key) {
	char keyword[SHORT_STR_LENGTH];
	toLowerCase(keyword, key);
	linkNode* result = createLinkList();
	question* ques;

	list = list->next;
	while (list->elem != NULL) {
		if (contains((ques = (question*)list->elem), keyword)) {
			appendNode(result, ques);
		}
		list = list->next;
	}
	return result;
}

//新增题目
void appendQuestion(const char* stem, const char choices[CHOICES_NUM][SHORT_STR_LENGTH], const int answer) {
	question* ques = (question*)malloc(sizeof(question));
	strcpy(ques->stem, stem);
	strcpy(ques->choices[0], choices[0]);
	strcpy(ques->choices[1], choices[1]);
	strcpy(ques->choices[2], choices[2]);
	strcpy(ques->choices[3], choices[3]);
	ques->answer = answer;
	appendNode(libQuestions, ques);
	libQuesLength++;
	isEdited = true;
}

//删除题目
bool removeQuestion(question* ques) {
	linkNode* target = indexOfLib(ques);
	if (target != NULL && removeNode(target, true)) {
		isEdited = true;
		libQuesLength--;
		return true;
	}
	return false;
}

//随机抽取试题
linkNode* randomPickQuestions(int count) {
	linkNode* result = createLinkList();
	int* random = (int*)malloc(sizeof(int) * libQuesLength);
	int* nums = (int*)malloc(sizeof(int) * count);
	int index;
	srand(time(NULL));

	//生成随机数（需要确保不重复），该算法的时间复杂度是线性的
	for (int i = 0; i < libQuesLength; i++) {
		random[i] = i;
	}
	for (int i = 0; i < count; i++) {
		index = rand() % (libQuesLength - i);
		nums[i] = random[index];
		random[index] = random[libQuesLength - 1 - i];
	}
	free(random);

	//排序
	quickSort(nums, 0, count - 1);

	//抽取试题
	linkNode* qlib = libQuestions->next;
	for (int i = 0, index = 0; i < libQuesLength && index < count; i++) {
		if (i == nums[index]) {
			appendNode(result, qlib->elem);
			index++;
		}
		qlib = qlib->next;
	}

	free(nums);
	return result;
}

//将修改写入题库
void applyChanges() {
	//检查内容是否已编辑
	if (!isEdited) {
		return;
	}

	FILE* filep = fopen(libFileName, "w");

	//写入标题
	fprintf(filep, "%s\n", libTitle);

	//写入题目
	linkNode* questions = seekToEnd(libQuestions);
	question* temp;
	for (int i = 0; i < libQuesLength; i++) {
		temp = (question*)questions->elem;
		fprintf(filep, "%s\n", temp->stem);
		fprintf(filep, "%s\n", temp->choices[0]);
		fprintf(filep, "%s\n", temp->choices[1]);
		fprintf(filep, "%s\n", temp->choices[2]);
		fprintf(filep, "%s\n", temp->choices[3]);
		fprintf(filep, "%d\n", temp->answer);
		questions = questions->prior;
	}

	fclose(filep);
	isEdited = false;
}

//卸载题库
void releaseQuestionsLib() {
	libQuesLength = 0;
	if (libQuestions != NULL) {
		destory(libQuestions, true);
		libQuestions = NULL;
	}
}

//是否已载入题库
bool isLoadLib() {
	return libQuestions != NULL;
}

//获取标题
char* getLibTitle() {
	return libTitle;
}

//获取题库
linkNode* getQuestions() {
	return libQuestions;
}

//获取长度
int getQuestionsLength() {
	return libQuesLength;
}

//设置题库标题
void setLibTitle(const char* newTitle) {
	strcpy(libTitle, newTitle);
	isEdited = true;
}

//设置题干
void setQuesStem(question* ques, char* stem) {
	strcpy(ques->stem, stem);
	isEdited = true;
}

//设置选项
void setQuesChoice(question* ques, int index, char* choice) {
	strcpy(ques->choices[index], choice);
	isEdited = true;
}

//设置答案
void setQuesAnswer(question* ques, short int answer) {
	ques->answer = answer;
	isEdited = true;
}

//定位question在链表中的位置
static linkNode* indexOfLib(question* ques) {
	linkNode* pos = libQuestions->next;
	while (pos->elem != NULL) {
		if ((question*)pos->elem == ques) {
			return pos;
		}
		pos = pos->next;
	}
	return NULL;
}

static bool contains(question* ques, char* keyword) {
	char buffer[LONG_STR_LENGTH];
	//使用KMP算法，逐一匹配字符串
	toLowerCase(buffer, ques->stem);
	if (indexOf(buffer, keyword) > -1) {
		return true;
	}
	toLowerCase(buffer, ques->choices[0]);
	if (indexOf(buffer, keyword) > -1) {
		return true;
	}
	toLowerCase(buffer, ques->choices[1]);
	if (indexOf(buffer, keyword) > -1) {
		return true;
	}
	toLowerCase(buffer, ques->choices[2]);
	if (indexOf(buffer, keyword) > -1) {
		return true;
	}
	toLowerCase(buffer, ques->choices[3]);
	if (indexOf(buffer, keyword) > -1) {
		return true;
	}
	return false;
}
