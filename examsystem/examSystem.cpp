/*
* 此文件主要是程序的界面
* 部分函数只能在Windows下使用
* 如果需要在其他系统运行，需要适当修改
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#include "libManager.h"
#include "linkList.h"
#include "utils.h"

#define LIB_FILE_NAME "libs\\"
#define REPORT_FILE_NAME "report\\"

#define MAX_ITEMS 10
#define MAX_CHARS 35

void init(); //基础初始化操作

//通用函数操作
void clear(); //清空屏幕
void printSplitLine(); //分割线
void printTitle(const char* title); //标题
void printText(const char* text); //正文
void printQuestion(question* ques); //题目详情
void printMenu(const char** items, int length, int* result); //展示选择菜单
void limitedPrintf(const char* text, int max); //限制字数的printf函数（针对ANSI编码）

void showTips(const char* text); //展示提醒
bool showInputBox(const char* title, const char* hint, int* content); //请求输入数字
bool showInputBox(const char* title, const char* hint, short int* content); //请求输入数字（短）
bool showTextInputBox(const char* title, const char* hint, char* content, int length); //请求输入文本
bool showInquiryBox(const char* title, const char* hint); //询问对话框

//相关程序界面

//题目选取界面
question* showQuesPicker(const char* title, linkNode* questions, int* index); //函数原型
question* showQuesPicker(const char* title, linkNode* questions); //忽略index参数的重载

void showMainMenu(); //主菜单
void showLibCreator(); //题库创建界面
void showLibLoader(); //题库载入界面
void showLibManager(); //题库管理界面

void showExamPreparation(); //考试准备界面
void showExam(int count, int time); //模拟考试界面
DWORD WINAPI examTimerTask(LPVOID lpParam); //考试计时线程的函数
void showExamResult(linkNode* questions, int* answers, int count); //显示考试结果
void showReportCreator(linkNode* questions, int* answers, int count); //导出错题报告

void showLibEditor(); //编辑题库
void showQuesDetail(question* ques); //显示题目细节
void showQuesCreator(); //新增题目
void showTitleSetter(); //更改题库名称

//全局变量
static DWORD mTimerThreadID; //考试计时线程ID
static int mTimerStatus = -1; //考试计时器状态

int main() {
	init();
	showMainMenu();
	return 0;
}

void init() {
	system("title 选择题考试系统");
	system("color f0");
	system("mode con:cols=100 lines=30");
	
	//检查题库存储目录
	if (_access(LIB_FILE_NAME, 0)) {
		_mkdir(LIB_FILE_NAME);
	}
	//检查报告存储目录
	if (_access(REPORT_FILE_NAME, 0)) {
		_mkdir(REPORT_FILE_NAME);
	}
}

//通用函数操作
void clear() {
	system("cls");
}

void pause() {
	system("pause");
}

void printSplitLine() {
	printf("  ================================================================================================  \n\n");
}

void printTitle(const char* title) {
	printf("                              %s\n\n", title);
}

void printText(const char* text) {
	printf("      %s\n\n", text);
}

void printQuestion(question* ques) {
	printText(ques->stem);
	for (int i = 0; i < CHOICES_NUM; i++) {
		printf("         【%d】 %s\n\n", i, ques->choices[i]);
	}
}

void printMenu(const char** items, int length, int* result) {
	for (int i = 0; i < length; i++) {
		printf("                         【%d】 %s\n\n", i, items[i]);
	}
	printSplitLine();
	printf("  请选择操作：");
	scanf("%d", result);
}

void limitedPrintf(const char* text, int max) {
	char ch;
	for (int i = 0, j = 0; j < max; i++, j++) {
		ch = text[i];
		if (ch == '\0') {
			break;
		}
		printf("%c", ch);
		//如果不是英文字符
		if (!(ch >= 0 && ch <= 127)) {
			i++;
			printf("%c", text[i]);
		}
	}
}

void showTips(const char* text) {
	clear();
	printSplitLine();
	printTitle("提示");
	printText(text);
	printSplitLine();
	pause();
}

bool showInputBox(const char* title, const char* hint, int* content) {
	clear();
	printSplitLine();
	printTitle(title);
	printSplitLine();
	printf("  %s（输入 -1 以取消）：", hint);
	scanf("%d", content);

	return *content != -1;
}

bool showInputBox(const char* title, const char* hint, short int* content) {
	clear();
	printSplitLine();
	printTitle(title);
	printSplitLine();
	printf("  %s（输入 -1 以取消）：", hint);
	scanf("%hd", content);

	return *content != -1;
}

bool showTextInputBox(const char* title, const char* hint, char* content, int length) {
	clear();
	printSplitLine();
	printTitle(title);
	printSplitLine();
	printf("  %s（输入 cancel 以取消）：\n  ", hint);
	gets_s(content, length);

	return strcmp(content, "cancel") != 0;
}

bool showInquiryBox(const char* title, const char* hint) {
	int operation;

	clear();
	printSplitLine();
	printTitle(title);
	printText(hint);
	printText("操作提示：确认：0 | 取消：1");
	printSplitLine();
	printf("  请输入操作：");
	scanf("%d", &operation);

	return operation == 0;
}

//题目选取界面（不带index参数的重载）
question* showQuesPicker(const char* title, linkNode* questions) {
	return showQuesPicker(title, questions, NULL);
}

//题目选取界面（带有index参数，返回结构体在链表中的下标，非必须）
question* showQuesPicker(const char* title, linkNode* questions, int* index) {
	int operation = -1, page = 0, items = 0, seekOperation = 0, seekBackItems;
	linkNode* pos = questions->next;

	while (operation != 13) {

		clear();
		printSplitLine();
		printTitle(title);

		//移动指针
		if (seekOperation != 1) {
			seekBackItems = items;
			if (seekOperation == -1) {
				seekBackItems += 10;
			}
			for (int i = 0; i < seekBackItems; i++) {
				pos = pos->prior;
			}
		}
		seekOperation = 0;
		items = 0;

		//打印题目
		for (int i = 0; i < MAX_ITEMS; i++) {
			if (pos->elem == NULL) {
				break;
			}

			printf("      【%d】 ", i);
			limitedPrintf(((question*)pos->elem)->stem, MAX_CHARS);
			printf("...\n\n");

			pos = pos->next;
			items++;
		}

		printText("操作提示： 选择：0 ~ 9 | 在以上选项中搜索：10 | 上一页：11 | 下一页：12 | 返回：13");
		printSplitLine();
		printf("  请选择操作：");
		scanf("%d", &operation);

		//处理操作
		switch (operation) {
			//搜索功能 递归调用实现代码复用
		case 10:
			char keyword[SHORT_STR_LENGTH];
			linkNode* relevant;
			question* result;

			getchar();
			if (showTextInputBox("在以上选项中搜索", "输入搜索关键词", keyword, SHORT_STR_LENGTH)) {
				relevant = searchLib(questions, keyword);
				result = showQuesPicker("搜索结果", relevant);
				destory(relevant, false);
				if (result != NULL) {
					return result;
				}
			}
			break;
		case 11:
			if (page == 0) {
				showTips("已经是第一页！");
			}
			else {
				page--;
				seekOperation = -1;
			}
			break;
		case 12:
			if (pos->elem == NULL) {
				showTips("已经是最后一页！");
			}
			else {
				page++;
				seekOperation = 1;
			}
			break;
			//退出选择界面
		case 13:
			break;
		default:
			if (operation >= 0 && operation < items) {
				for (int i = 0; i < items - operation; i++) {
					pos = pos->prior;
				}
				if (index != NULL) {
					*index = operation + page * 10;
				}
				return (question*)pos->elem;
			}
			else {
				showTips("无效操作！");
			}
			break;
		}
	}

	return NULL;
}

//主菜单
void showMainMenu() {
	int operation;
	clear();
	printSplitLine();
	printTitle("主菜单");
	const char* items[] = {"新建题库", "载入已有题库", "退出程序"};
	printMenu(items, 3, &operation);
	//跳转到相应界面
	switch (operation) {
	case 0:
		showLibCreator();
		break;
	case 1:
		showLibLoader();
		break;
	}
}

//题库创建界面
void showLibCreator() {
	char title[SHORT_STR_LENGTH], fileName[SHORT_STR_LENGTH];

	getchar();
	if (!showTextInputBox("新建题库", "输入题库标题", title, SHORT_STR_LENGTH)) {
		showMainMenu();
		return;
	}

	strcpy(fileName, title);
	insertAhead(fileName, LIB_FILE_NAME);
	strcat(fileName, QLIB_EXPAND);

	if (createQuestionsLib(fileName, title) && loadQuestionsLib(fileName)) {
		showTips("创建成功！即将跳转管理界面。");
		showLibManager();
		return;
	}
	showTips("创建或载入失败！");
	showMainMenu();
}

//题库载入界面
void showLibLoader() {
	char buffer[SHORT_STR_LENGTH];
	char* fileName;

	struct _finddata_t fileInfo;
	intptr_t hFile;

	linkNode* files = createLinkList();
	int index = 0;

	int exitCode;

	//生成文件夹路径
	sprintf(buffer, "%s*%s", LIB_FILE_NAME, QLIB_EXPAND);

	//载入界面
	clear();
	printSplitLine();
	printTitle("载入题库");

	if ((hFile = _findfirst(buffer, &fileInfo)) != -1)
	{
		do {
			printf("                         【%d】 %s\n\n", index, fileInfo.name);
			
			fileName = (char*)malloc(sizeof(fileInfo.name));
			strcpy(fileName, fileInfo.name);
			appendNode(files, fileName);
			index++;
		} while (_findnext(hFile, &fileInfo) == 0);

		// 关闭查找句柄
		_findclose(hFile);
	}

	printSplitLine();
	printf("  输入题库对应的序号（输入 %d 返回上一级）：", (exitCode = index));
	scanf("%d", &index);
	
	//排除掉无效输入
	if (index < 0 || index >= exitCode) {
		destory(files, true);
		if (index != exitCode) {
			showTips("无效操作！");
		}
		showMainMenu();
		return;
	}

	//载入相应题库
	files = seekToEnd(files);
	while (index > 0 && files->elem != NULL) {
		files = files->prior;
		index--;
	}

	strcpy(buffer, LIB_FILE_NAME);
	strcat(buffer, (char*)files->elem);
	destory(files, true);

	if (loadQuestionsLib(buffer)) {
		showLibManager();
	}
	else {
		showTips("打开题库失败！请重新检查。");
		showMainMenu();
	}
}

//题库管理界面
void showLibManager() {
	int operation;
	clear();
	printSplitLine();
	printTitle(getLibTitle());
	const char* items[] = { "模拟考试", "管理题目", "更改题库名称", "返回主菜单" };
	printMenu(items, 4, &operation);
	//跳转到相应界面
	switch (operation) {
	case 0:
		showExamPreparation();
		break;
	case 1:
		showLibEditor();
		break;
	case 2:
		showTitleSetter();
		break;
	default:
		applyChanges();
		releaseQuestionsLib();
		showMainMenu();
		break;
	}
}

//考试准备界面
void showExamPreparation() {
	int count, time;
	
	if (!showInputBox("考试参数设置", "请输入需要抽取的题目数量", &count)) {
		showLibManager();
		return;
	}

	if (count <= getQuestionsLength()) {
		showInputBox("考试参数设置", "请输入考试限时 | 单位：分钟", &time);
		showExam(count, time);
	}
	else {
		showTips("抽取题目数量超过题库总题目数！");
		showLibManager();
	}
}

//模拟考试界面
void showExam(int count, int examTime) {
	int operation = -1, position = 0, cursor;
	question* ques;
	char title[SHORT_STR_LENGTH];
	char tempChar;

	//抽取试题
	linkNode* questions = randomPickQuestions(count)->next;

	//存储答案的数组
	int* answers = (int*)malloc(sizeof(int) * count);
	for (int i = 0; i < count; i++) {
		answers[i] = -1;
	}

	//如果需要计时 启动子线程
	if (examTime > -1) {
		HANDLE hThread;
		OVERLAPPED overlapped = {};

		mTimerStatus = 0;
		hThread = CreateThread(NULL, 0, examTimerTask, &examTime, 0, &mTimerThreadID);
		if (hThread == NULL) {
			showTips("子线程启动失败！无法进行计时操作！");
		}
		else {
			CloseHandle(hThread);
		}
	}

 	//显示考试界面
	while (operation != 6 && operation != 7) {
		ques = (question*)questions->elem;

		clear();
		printSplitLine();
		sprintf(title, "题目 %d / %d", position + 1, count);
		printTitle(title);
		printQuestion(ques);
		if (answers[position] == -1) {
			printf("      已选择的答案：未选\n\n");
		}
		else {
			printf("      已选择的答案：%d\n\n", answers[position]);
		}
		printText("操作提示： 回答问题：0 ~ 3 | 上一题：4 | 下一题：5 | 交卷：6 | 放弃考试：7");
		printSplitLine();
		printf("  请选择操作：");
		
		cursor = 0;
		while (1) {
			//检查计时器状态
			if (mTimerStatus == 1) {
				showTips("考试时间结束！已经自动交卷！");
				operation = 6;
				break;
			}

			//非阻塞式输入（scanf阻塞线程会导致一些问题，故这里不能用）
			if (_kbhit()) {
				if ((tempChar = _getch()) != '\r') {
					operation = tempChar - 48;
					//回格键特殊处理
					if (tempChar == '\b') {
						if (cursor > 0) {
							printf("\b \b");
							cursor--;
						}
					}
					else {
						printf("%c", tempChar);
						cursor++;
					}
				}
				else {
					break;
				}
			}
		}

		//处理操作
		switch (operation) {
		case 4:
			if (position - 1 < 0) {
				showTips("已经是第一题！");
			}
			else {
				position--;
				questions = questions->prior;
			}
			break;
		case 5:
			if (position + 1 >= count) {
				showTips("已经是最后一题！");
			}
			else {
				position++;
				questions = questions->next;
			}
			break;
		case 6:
			break;
		case 7:
			break;
		default:
			if (operation >= 0 && operation <= 3) {
				answers[position] = operation;
				if (position + 1 < count) {
					position++;
					questions = questions->next;
				}
			}
			else {
				showTips("无效操作！");
			}
			break;
		}
	}

	//结束考试
	mTimerStatus = -1;
	if (operation == 6) {
		showExamResult(seekToEnd(questions)->next, answers, count);
	}
	destory(questions, false);
	free(answers);
	showLibManager();
}

//子线程 用于考试计时
DWORD WINAPI examTimerTask(LPVOID lpParam) {
	long startTime = time(NULL), lastTime;
	int max = *(int*)lpParam * 60;
	char title[SHORT_STR_LENGTH];

	while (1) {
		if (mTimerStatus != 0) {
			break;
		}

		lastTime = time(NULL) - startTime;
		if (lastTime <= max) {
			sprintf(title, "title 选择题考试系统（考试已进行：%lds）", lastTime);
			system(title);
		}
		else {
			mTimerStatus = 1;
			break;
		}

		Sleep(500);
	}

	system("title 选择题考试系统");

	return 0;
}

//展示考试结果
void showExamResult(linkNode* questions, int* answers, int count) {
	float weight = 100 / count, score = 0;
	int right = 0, lost = 0, index;
	question* temp;
	questions = questions->next;

	//核对答案
	for (int i = 0; i < count; i++) {
		if (((question*)questions->elem)->answer == answers[i]) {
			score += weight;
			right++;
		}
		else if (answers[i] == -1) {
			lost++;
		}
		questions = questions->next;
	}

	//展示考试结果
	char result[SHORT_STR_LENGTH];
	sprintf(result, "总共 %d 道题，答对 %d 道，未答 %d 道，得分：%.1f", count, right, lost, score);
	showTips(result);

	//显示题目
	questions = seekToEnd(questions)->next;
	while (1) {
		temp = showQuesPicker("考试详情", questions, &index);
		if (temp != NULL) {
			//显示做题详情
			clear();
			printSplitLine();
			printTitle("题目详情");
			printQuestion(temp);
			if (answers[index] == -1) {
				printf("      我的答案：未选\n\n      标准答案：%hd\n\n", temp->answer);
			}
			else {
				printf("      我的答案：%d\n\n      标准答案：%hd\n\n", answers[index], temp->answer);
			}
			printSplitLine();
			pause();
		}
		else {
			break;
		}
	}

	//询问错题报告
	if (right + lost < count) {
		showReportCreator(seekToEnd(questions)->next, answers, count);
	}
}

//输出错题报告
void showReportCreator(linkNode* questions, int* answers, int count) {
	if (showInquiryBox("错题报告", "是否需要导出错题报告？")) {
		//确定需要的文件名
		char fileName[SHORT_STR_LENGTH];
		time_t now = time(NULL);
		struct tm* tm_now = localtime(&now);
		strftime(fileName, sizeof(fileName), "%Y%m%d%H%M%S.txt", tm_now); // 格式化输出日期
		insertAhead(fileName, REPORT_FILE_NAME);

		//写入文件
		FILE* filep = fopen(fileName, "w");
		if (filep == NULL) {
			showTips("打开文件输出流失败！");
			return;
		}

		int order = 1;
		questions = questions->next;
		question* temp;

		fprintf(filep, "错题报告\n\n");
		for (int i = 0; i < count; i++) {
			temp = (question*)questions->elem;
			if (answers[i] != -1 && temp->answer != answers[i]) {
				fprintf(filep, "%d.%s\n", order, temp->stem);
				fprintf(filep, " 【0】 %s\n", temp->choices[0]);
				fprintf(filep, " 【1】 %s\n", temp->choices[1]);
				fprintf(filep, " 【2】 %s\n", temp->choices[2]);
				fprintf(filep, " 【3】 %s\n", temp->choices[3]);
				fprintf(filep, "我的答案：%d  标准答案：%d\n\n", answers[i], temp->answer);
				order++;
			}
			questions = questions->next;
		}
		fclose(filep);

		insertAhead(fileName, "已保存到：");
		showTips(fileName);
	}
}

//题库管理界面
void showLibEditor() {
	int operation;

	clear();
	printSplitLine();
	printTitle("题库管理");
	const char* items[] = { "查看题目", "新增题目", "返回上一级" };
	printMenu(items, 3, &operation);
	//跳转到相应界面
	switch (operation) {
	case 0:
		while (1) {
			question* target = showQuesPicker("查看题目", getQuestions());
			if (target != NULL) {
				showQuesDetail(target);
			}
			else {
				showLibEditor();
				break;
			}
		}
		break;
	case 1:
		showQuesCreator();
		break;
	case 2:
		showLibManager();
		break;
	}
}

//展示题目细节
void showQuesDetail(question* ques) {
	int operation = -1;

	while (operation != 6 && operation != 7) {
		clear();
		printSplitLine();
		printTitle("题目详情");
		printQuestion(ques);
		printf("      标准答案：%hd\n\n", ques->answer);
		printText("操作提示： 编辑题干：0 | 编辑选项：1 ~ 4 | 编辑答案：5 | 删除：6 | 返回上一级：7");
		printSplitLine();
		printf("  请选择操作：");
		scanf("%d", &operation);

		switch (operation) {
		case 0:
			char stem[LONG_STR_LENGTH];
			getchar();
			if (showTextInputBox("编辑题干", "请输入新题干", stem, LONG_STR_LENGTH)) {
				setQuesStem(ques, stem);
			}
			break;
		case 5:
			short int answer;

			if (showInputBox("编辑答案", "请输入答案 | 范围：0 ~ 3", &answer)) {
				if (answer >= 0 && answer <= 3) {
					setQuesAnswer(ques, answer);
				}
				else {
					showTips("参数无效！");
				}
			}
			break;
		case 6:
			if (showInquiryBox("删除", "确认删除吗？此操作不可逆转！")) {
				if (removeQuestion(ques)) {
					showTips("删除成功！");
				}
				else {
					showTips("删除失败！");
				}
			}
			break;
		case 7:
			break;
		default:
			if (operation >= 1 && operation <= 4) {
				char choice[SHORT_STR_LENGTH];
				getchar();
				if (showTextInputBox("编辑选项", "请输入新选项内容", choice, SHORT_STR_LENGTH)) {
					setQuesChoice(ques, operation - 1, choice);
				}
			}
			else {
				showTips("无效操作！");
			}
			break;
		}
	}
}

//新增题目界面
void showQuesCreator() {
	//需要录入的参数
	char buffer[SHORT_STR_LENGTH];
	char stem[LONG_STR_LENGTH];
	char choices[CHOICES_NUM][SHORT_STR_LENGTH];
	short int answer;

	getchar();
	if (!showTextInputBox("新增题目", "请输入题干", stem, LONG_STR_LENGTH)) {
		showLibEditor();
		return;
	}

	for (int i = 0; i < CHOICES_NUM; i++) {
		sprintf(buffer, "请输入选项 %d 的内容", i);
		if (!showTextInputBox("新增题目", buffer, choices[i], SHORT_STR_LENGTH)) {
			showLibEditor();
			return;
		}
	}

	if (!showInputBox("新增题目", "请输入答案 | 范围：0 ~ 3", &answer)) {
		showLibEditor();
		return;
	}
	if (answer >= 0 && answer <= 3) {
		appendQuestion(stem, choices, answer);
		showTips("新增成功！");
	}
	else {
		showTips("参数无效！");
	}
	showLibEditor();
}

//更改题库名称
void showTitleSetter() {
	char newTitle[SHORT_STR_LENGTH];

	getchar();
	if (showTextInputBox("更改题库名称", "请输入新名称", newTitle, SHORT_STR_LENGTH)) {
		setLibTitle(newTitle);
	}

	showLibManager();
}
