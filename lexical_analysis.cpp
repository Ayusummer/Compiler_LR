#pragma once
#include"lexical_analysis.h"
#include <string.h>
#include <iostream>
#include<iomanip>
using namespace std;

/* 源程序文件
	因为需要调用函数进行初始化, 因此这里先定义为空 */
FILE* source_file = NULL;		// 源程序文件
/* 初始化为空, 理由同上, 后面类似情况不再赘述 */
errno_t error_source_file = 0;	// 读文件需要的错误返回码


char buffer_line[81] = { "" };	// 一行字符缓冲区(最多80个, 末尾是\0结束标记)
/* 字符缓冲区指针
	用于从源程序文件读取一行到缓冲区
	根据指针的后移来将源程序一行字符一个个移入缓冲区
	迁移完成后指针指向字符串直接标记' 然后重置为缓冲区首地址
	此外还用于缓冲区字符的逐个读取, 依然是根据指针后移挨个读字符*/
char* pointer_buffer = NULL;
char current_ch = '\0';			// 当前字符初始化为空字符

/* 源程序长度
	在从源程序读完一行字符后自动 +1*/
int lineNum_sourceCode = 0;


char word_recognized[10] = "";				// 识别字数组,存放识别的字
two_exp buffer_lexical[1000] = {};			// 词法分析结果缓冲区
two_exp* pointer_bufLex = buffer_lexical;	// 词法分析缓冲区指针指向词法分析缓冲区
int  count_lexResult = 0;					// 词法分析结果缓冲区计数器

// 保留字表初始化
keywords reserved_words[10] = {
	{"if",sy_if},
	{"do",sy_do},
	{"else",sy_else},
	{"while",sy_while},
	{"then",sy_then},
	{"begin",sy_begin},
	{"end",sy_end},
	{"and",op_and},
	{"or",op_or},
	{"not",op_not}
};
char table_variable[100][10] = {};		// 变量名表, 初始化为空
int num_variable = 0;					// 词法分析中识别放到变量名表中的变量名数目
int num_lexVariable = 0;				// 词法分析缓冲区中含有的变量的数目



// 源程序文件初始化
void init_source_file() {
	// 以 read_only 模式打开源文件并返回错误码, 这里偷懒没做错误处理
	error_source_file = fopen_s(&source_file, "pas.dat", "r");
}

/* 从源程序文件读一行到缓冲区 */
void readLine_to_buffer() {
	char nxt_char_source_file = getc(source_file);	// 读取来自 source_file 的下一个字符
	pointer_buffer = buffer_line;	// 字符缓冲区指针指向一行字符缓冲区的首地址
	/* 读取一行源文件字符 */
	while ((nxt_char_source_file != '\n') && (nxt_char_source_file != EOF)) {
		*pointer_buffer = nxt_char_source_file;
		pointer_buffer++;
		nxt_char_source_file = getc(source_file);
	}
	*pointer_buffer = '\0';		// 一行字符串尾为字符串结束标记 '\0'
	// 重置字符缓冲区指针为一行字符缓冲区的首地址以便下次读取一行程序
	pointer_buffer = buffer_line;
	lineNum_sourceCode++;	// 每读取一行，源程序长度(行数)+1
}

/* 从一行字符缓冲区中读取一个字符
	若当前是字符结束标记则从源程序先读一行字符串
	再从缓冲区中读一个字符 */
void readCh_from_buffer() {
	/* 如果当前字符为字符串结束标记, 那么说明缓冲区中没有待读取字符了
		需要再从源程序中读一行字符到缓冲区 */
	if (current_ch == '\0')
		readLine_to_buffer();	// 从源文件中读取一行进入缓冲区
	current_ch = *pointer_buffer;
	pointer_buffer++;
}

/* 展示缓冲区下一个字符
	用于 #~ 程序结束标记的判断
	若当前字符为字符串结束标记的话返回 NULL, 表示没有下一个字符 */
char show_nxtCh_buffer() {
	if (current_ch == '\0')
		return NULL;
	return *pointer_buffer;
}


/***********************标识符和关键字的识别********************/
/* 与变量名表中的变量进行匹配，查找变量名表
	若找到则返回其在变量名表中的索引
	若未找到则返回 -1 */
int find_in_variableTable(char spel[]) {
	int is_variable = 0;	// 是否为变量名的标记, 是 -> 1, 否 -> 0
	int index_tableVariable = 0;	// 变量名表下标初始化为 0
	while ((is_variable == 0) && (index_tableVariable < num_variable)) {
		if (!strcmp(spel, table_variable[index_tableVariable]))//查找匹配
			is_variable = 1;
		index_tableVariable++;
	}
	if (is_variable == 1)
		return index_tableVariable - 1;	// 找到输入字符串所在变量名表中的索引
	else
		return -1;		// 未找到, 即输入字符串未录入变量名表
}

/* 识别保留字和标识符
	其输入应当是个字符串
	识别出关键词或者变量名
	识别中会操作变量名表和词法分析缓冲区 */
void stringRecognize() {
	int index_wordRec = 0;			// 识别字数组下标; 初始化为 0
	int index_resWords = 0;			// 保留字数组索引
	int is_keyword = 0;				// 是否为保留字标记, 是 -> 1, 否 -> 0
	int wordIndex_variableTable;	// 字符在变量名表中的索引, 若字符不在变量名表中则 -> -1


	// 将字母与数字读取到识别字数组中
	do {
		word_recognized[index_wordRec] = current_ch;
		index_wordRec++;
		readCh_from_buffer();
	} while (((current_ch >= 'a') && (current_ch <= 'z')) || ((current_ch >= '0') && (current_ch <= '9')));
	word_recognized[index_wordRec] = '\0';	// 读完字母与数字将字符串结束标记附在数组尾
	/* 由于每次最后一次识别都会多使缓冲区指针前进一次, 所以要相应后退一次
		毕竟每次 readCh_from_buffer 都会动一次指针 */
	pointer_buffer--;

	while ((is_keyword == 0) && (index_resWords < 10)) {
		if (!strcmp(word_recognized, reserved_words[index_resWords].word))	// 保留字匹配
			is_keyword = 1;
		index_resWords++;
	}
	/* 关键字匹配 */
	if (is_keyword == 1) {
		// 将已识别的保留字添加到词法分析缓冲区
		buffer_lexical[count_lexResult].wordCode = reserved_words[index_resWords - 1].wordCode;
	}
	else {
		// 一个字符串不是关键字那么就只能是是标识符，变量名喽
		buffer_lexical[count_lexResult].wordCode = word_variable;
		wordIndex_variableTable = find_in_variableTable(word_recognized);
		// 若当前字符串没在变量名表中
		if (wordIndex_variableTable == -1) {
			// 词法分析缓冲区当前字符子类码 = 当前缓冲区内的变量数目
			buffer_lexical[count_lexResult].wordSubCode = num_lexVariable;
			// 被识别变量加入到变量名表中
			strcpy_s(table_variable[num_variable], word_recognized);
			/* 变量名表和词法分析缓冲区域中的变量个数 +1
				实际上二者根本就是完全一致 */
			num_lexVariable++;
			num_variable++;
		}
		// 若当前字符串在变量名表中
		else
			/* 字符子类码则为变量在变量名表中的索引
				毕竟变量名表中变量的顺序和词法分析缓冲区域中变量的顺序完全一致
				因此在变量名表中的索引也即在词法分析缓冲区域中的索引 */
			buffer_lexical[count_lexResult].wordSubCode = wordIndex_variableTable;
	}
	count_lexResult++;	// 每识别完一个变量代表词法分析结果多了一个
	// 清空识别字数组
	for (index_wordRec = 0; index_wordRec < 10; index_wordRec++)
		word_recognized[index_wordRec] = ' ';
}

/**********************数字识别*************************/
void numberRecognize() {
	/* 当前已识别出来的数字
		由于不知道下面一个字符是不是还是数字因此只能算是 tmp(临时的)
		当下一个字符不再是数字是才结束迭代识别出完整的一个整数 */
	int tmp_valueRecognized = 0;
	int digit;
	do {
		digit = current_ch - '0';
		tmp_valueRecognized = tmp_valueRecognized * 10 + digit;	// 数字字符转换为十进制整常数
		readCh_from_buffer();
	} while ((current_ch >= '0') && (current_ch <= '9'));
	buffer_lexical[count_lexResult].wordCode = intconst;
	buffer_lexical[count_lexResult].wordSubCode = tmp_valueRecognized;
	count_lexResult++;
	pointer_buffer--;	// 由于每次最后一次识别都会多使缓冲区指针前进一次,所以要相应后退一次
}

/***********************扫描函数************************/
/* 滤除多余空格并对主要单词分析处理 */
void lexical_analyse() {
	while (!(current_ch == '#' && show_nxtCh_buffer() == '~')) {
		switch (current_ch) {
		case ' ':break;
		case 'a': /* continue case, case 语句不写 break 那么程序就会执行下一个case
					所以说下面所有的 case 空操作都会依次执行,
					也就是说只要识别到字母那么必然会一路执行到 case z 的 stringRecognize()来识别保留字和标识符*/
		case 'b': case 'c':	case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':	case 'j':
		case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's':
		case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
			stringRecognize();	// 识别保留字和标识符, 变量名
			break;
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
			numberRecognize();	// 识别整常数
			break;
		case '<':
			readCh_from_buffer();
			// <= 小于等于
			if (current_ch == '=')
				buffer_lexical[count_lexResult].wordSubCode = 0;
			else {
				// <> 不等于
				if (current_ch == '>')
					buffer_lexical[count_lexResult].wordSubCode = 4;
				// < 小于
				else {
					buffer_lexical[count_lexResult].wordSubCode = 1;	// <
					pointer_buffer--;
				}
			}
			// 识别到 < 那么主种别编码就是关系运算符 rop
			buffer_lexical[count_lexResult].wordCode = rop;
			count_lexResult++;
			break;
		case '>':
			readCh_from_buffer();
			// >= 大于等于
			if (current_ch == '=')
				buffer_lexical[count_lexResult].wordSubCode = 2;
			// > 大于
			else {
				buffer_lexical[count_lexResult].wordSubCode = 3;
				pointer_buffer--;
			}
			buffer_lexical[count_lexResult].wordCode = rop;
			count_lexResult++;
			break;
		case '=':
			buffer_lexical[count_lexResult].wordCode = rop;
			buffer_lexical[count_lexResult].wordSubCode = 5;
			count_lexResult++;
			break;
		case '(':
			buffer_lexical[count_lexResult].wordCode = lparent;
			count_lexResult++;
			break;
		case ')':
			buffer_lexical[count_lexResult].wordCode = rparent;
			count_lexResult++;
			break;
		case '#':
			buffer_lexical[count_lexResult].wordCode = jinghao;
			count_lexResult++;
			break;
		case '+':
			buffer_lexical[count_lexResult].wordCode = op_plus;
			count_lexResult++;
			break;
		case '-':
			buffer_lexical[count_lexResult].wordCode = op_sub;
			count_lexResult++;
			break;
		case '*':
			buffer_lexical[count_lexResult].wordCode = op_times;
			count_lexResult++;
			break;
		case '/':
			buffer_lexical[count_lexResult].wordCode = op_div;
			count_lexResult++;
			break;
		case ':':
			readCh_from_buffer();
			if (current_ch == '=')
				buffer_lexical[count_lexResult].wordCode = becomes;// :=
			count_lexResult++;
			break;
		case ';':
			buffer_lexical[count_lexResult].wordCode = semicolon;
			count_lexResult++;
			break;
		}
		readCh_from_buffer();
	}
	buffer_lexical[count_lexResult].wordCode = -1;
}

/*************************显示词法分析结果*******************************/
void lexical_analyse_show() {
	int temp1 = 0;
	printf("\n***************** 词法分析结果 ********************\n");
	cout << "    " << setw(8) << "种别编码"
		<< "                     " << setw(8) << "子类编码"
		<< endl;
	for (temp1 = 0; temp1 < count_lexResult; temp1++) {
		cout << "" << setw(8) << buffer_lexical[temp1].wordCode
			<< "                     " << setw(8) << buffer_lexical[temp1].wordSubCode
			<< endl;
	}
}

/*************************打印变量名表*******************************/
void show_variableTable() {
	int tttt;
	printf("\n\n程序总共%d行，产生了%d个二元式!\n", lineNum_sourceCode, count_lexResult);
	// getchar();
	cout << endl;
	printf("\n******************变量名表**********************\n");
	for (tttt = 0; tttt < num_lexVariable; tttt++)
		printf("%d\t%s\n", tttt, table_variable[tttt]);
	// getchar();
	cout << endl;
}

/* 词法分析及结果显示 */
void lexical_analyse_global() {
	init_source_file();		// 初始化输入文件
	/* 从一行字符缓冲区中读取一个字符
		若当前是字符结束标记则从源程序先读一行字符串
		再从缓冲区中读一个字符 */
	readCh_from_buffer();
	lexical_analyse();					// 词法分析
	lexical_analyse_show();				// 显示词法分析结果
	show_variableTable();			// 显示变量名
}
