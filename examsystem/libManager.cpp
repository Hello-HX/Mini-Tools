/*
* ���ļ���Ҫ�Ƕ�����һЩ���� / ������صĺ���
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "libManager.h"
#include "linkList.h"
#include "utils.h"

//�ڲ�����
static linkNode* indexOfLib(question* ques); //��λquestion�������е�λ��
static bool contains(question* ques, char* keyword); //�Ƿ�����ؼ���

static char libFileName[SHORT_STR_LENGTH]; //����ļ�·��
static char libTitle[SHORT_STR_LENGTH]; //������
static linkNode* libQuestions; //�洢��Ŀ������
static int libQuesLength = 0; //����е���Ŀ����

static bool isEdited = false; //����Ƿ񱻱༭��

//�������
bool createQuestionsLib(char* fileName, char* title) {
	FILE* filep = fopen(fileName, "w");
	if (filep != NULL) {
		fwrite(title, sizeof(char), sizeof(title), filep);
		fclose(filep);
		return true;
	}
	return false;
}

//�������
bool loadQuestionsLib(char* fileName) {
	if (libQuestions != NULL) {
		return false;
	}

	strcpy(libFileName, fileName);
	FILE* filep = fopen(libFileName, "r");
	if (filep == NULL) {
		return NULL;
	}
	//��ȡ����
	fgets(libTitle, SHORT_STR_LENGTH, filep);
	
	//��ȡ��Ŀ����
	libQuestions = createLinkList();
	linkNode* pointer = libQuestions;
	question* temp = (question*)malloc(sizeof(question));
	int program = 0;
	short int answer;
	char buffer[LONG_STR_LENGTH];
	while (true) {
		//¼�����
		switch (program) {
		case 0:
			fgets(buffer, LONG_STR_LENGTH, filep);
			//����ȡ�����Ƿ�Ϊ�գ���Ҫ���ǻ��з���
			if (buffer != NULL) {
				strcpy(buffer, skipSpace(buffer));
				if (*buffer != '\n' && *buffer != '\0') {
					substring(temp->stem, buffer, 0, strlen(buffer) - 1);
					program++;
				}
			}
			break;
		//¼���
		case 5:
			fgets(buffer, SHORT_STR_LENGTH, filep);
			if (buffer != NULL) {
				strcpy(buffer, skipSpace(buffer));
				if (*buffer != '\n' && *buffer != '\0') {
					answer = *buffer - 48;
					//��ֻ���ĸ�ѡ��
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
		//¼��ѡ��
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
		//�ļ�����ĩβ
		if (feof(filep)) {
			fclose(filep);
			free(temp);
			break;
		}
	}

	isEdited = false;
	return true;
}

//�������
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

//������Ŀ
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

//ɾ����Ŀ
bool removeQuestion(question* ques) {
	linkNode* target = indexOfLib(ques);
	if (target != NULL && removeNode(target, true)) {
		isEdited = true;
		libQuesLength--;
		return true;
	}
	return false;
}

//�����ȡ����
linkNode* randomPickQuestions(int count) {
	linkNode* result = createLinkList();
	int* random = (int*)malloc(sizeof(int) * libQuesLength);
	int* nums = (int*)malloc(sizeof(int) * count);
	int index;
	srand(time(NULL));

	//�������������Ҫȷ�����ظ��������㷨��ʱ�临�Ӷ������Ե�
	for (int i = 0; i < libQuesLength; i++) {
		random[i] = i;
	}
	for (int i = 0; i < count; i++) {
		index = rand() % (libQuesLength - i);
		nums[i] = random[index];
		random[index] = random[libQuesLength - 1 - i];
	}
	free(random);

	//����
	quickSort(nums, 0, count - 1);

	//��ȡ����
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

//���޸�д�����
void applyChanges() {
	//��������Ƿ��ѱ༭
	if (!isEdited) {
		return;
	}

	FILE* filep = fopen(libFileName, "w");

	//д�����
	fprintf(filep, "%s\n", libTitle);

	//д����Ŀ
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

//ж�����
void releaseQuestionsLib() {
	libQuesLength = 0;
	if (libQuestions != NULL) {
		destory(libQuestions, true);
		libQuestions = NULL;
	}
}

//�Ƿ����������
bool isLoadLib() {
	return libQuestions != NULL;
}

//��ȡ����
char* getLibTitle() {
	return libTitle;
}

//��ȡ���
linkNode* getQuestions() {
	return libQuestions;
}

//��ȡ����
int getQuestionsLength() {
	return libQuesLength;
}

//����������
void setLibTitle(const char* newTitle) {
	strcpy(libTitle, newTitle);
	isEdited = true;
}

//�������
void setQuesStem(question* ques, char* stem) {
	strcpy(ques->stem, stem);
	isEdited = true;
}

//����ѡ��
void setQuesChoice(question* ques, int index, char* choice) {
	strcpy(ques->choices[index], choice);
	isEdited = true;
}

//���ô�
void setQuesAnswer(question* ques, short int answer) {
	ques->answer = answer;
	isEdited = true;
}

//��λquestion�������е�λ��
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
	//ʹ��KMP�㷨����һƥ���ַ���
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
