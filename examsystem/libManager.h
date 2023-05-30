#pragma once
#include "linkList.h"
#include "utils.h"

//基础参数
#define CHOICES_NUM 4
#define QLIB_EXPAND ".qlib"

//定义结构体
typedef struct question {
	char stem[LONG_STR_LENGTH];
	char choices[CHOICES_NUM][SHORT_STR_LENGTH];
	short int answer;
} question;

//函数
bool createQuestionsLib(char* fileName, char* title);

bool loadQuestionsLib(char* fileName);

linkNode* searchLib(linkNode* list, char* key);

void appendQuestion(const char* stem, const char choices[CHOICES_NUM][SHORT_STR_LENGTH], const int answer);

bool removeQuestion(question* ques);

linkNode* randomPickQuestions(int count);

void applyChanges();

void releaseQuestionsLib();

char* getLibTitle();

linkNode* getQuestions();

int getQuestionsLength();

void setLibTitle(const char* newTitle);

void setQuesStem(question* ques, char* stem);

void setQuesChoice(question* ques, int index, char* choice);

void setQuesAnswer(question* ques, short int answer);
