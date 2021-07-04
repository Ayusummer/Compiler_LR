#pragma once
#include"lexical_analysis.h"
#include <string.h>

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

int tt1 = 0;				// 变量名表指针
two_exp* pbuf = buffer_lexical;				// 指向词法分析缓冲区
int nlength = 0;			// 词法分析中记录单词长度



two_exp buffer_lexical[1000] = {};


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

/* 从缓冲区读取一个字符
	从一行字符缓冲区中读取一个字符*/
void readCh_from_buffer() {
	/* 如果当前字符为字符串结束标记, 那么说明缓冲区中没有待读取字符了
		需要再从源程序中读一行字符到缓冲区 */
	if (current_ch == '\0')
		readLine_to_buffer();	// 从源文件中读取一行进入缓冲区
	current_ch = *pointer_buffer;
	pointer_buffer++;
}


/***********************标识符和关键字的识别********************/
// 与变量名表中的变量进行匹配，查找变量名表
int find(char spel[]) {
	int ss1 = 0;
	int ii = 0;
	while ((ss1 == 0) && (ii < nlength)) {
		if (!strcmp(spel, table_variable[ii]))//查找匹配
			ss1 = 1;
		ii++;
	}
	if (ss1 == 1)
		return ii - 1;//找到
	else return -1;//未找到
}

/* 识别保留字和标识符 */
int identifier() {
	int iii = 0, j, k;
	int ss = 0;
	k = 0;
	do {
		spelling[k] = current_ch;
		k++;
		readCh_from_buffer();
	} while (((current_ch >= 'a') && (current_ch <= 'z')) || ((current_ch >= '0') && (current_ch <= '9')));
	pointer_buffer--;
	spelling[k] = '\0';
	while ((ss == 0) && (iii < 10)) {
		if (!strcmp(spelling, reswords[iii].sp))//保留字匹配
			//strcmp(s1,s2) 比较两个字符串并根据比较结果返回整数
			//当s1<s2 返回负数
			//当s1=s2 返回0
			//当s1>s2 返回正数
			ss = 1;
		iii++;
	}
	/*关键字匹配*/
	if (ss == 1) {	// 为保留字
		buffer_lexical[count_buf].typeCode_word = reswords[iii - 1].sy;
	}
	else {
		buffer_lexical[count_buf].typeCode_word = ident;//是标识符，变量名
		j = find(spelling);
		if (j == -1) {	//没在变量名表中，则添加
			buffer_lexical[count_buf].value_word = tt1;
			strcpy_s(table_variable[tt1], spelling);
			tt1++;
			nlength++;
		}
		else buffer_lexical[count_buf].value_word = j;//获得变量名自身的值
	}
	count_buf++;
	for (k = 0; k < 10; k++) spelling[k] = ' ';//清空单词符号缓冲区
}

/**********************数字识别*************************/
void number() {
	int ivalue = 0;
	int digit;
	do {
		digit = current_ch - '0';
		ivalue = ivalue * 10 + digit;//数字字符转换为十进制整常数
		readCh_from_buffer();
	} while ((current_ch >= '0') && (current_ch <= '9'));
	buffer_lexical[count_buf].typeCode_word = intconst;
	buffer_lexical[count_buf].value_word = ivalue;
	count_buf++;
	pointer_buffer--;
}

/***********************扫描函数************************/
/* 滤除多余空格并对主要单词分析处理 */
void scan() {
	while (current_ch != '~') {
		switch (current_ch) {
		case ' ':break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			identifier(); break;//识别保留字和标识符
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			number(); break;//识别整常数
		case '<':readCh_from_buffer();
			if (current_ch == '=')
				buffer_lexical[count_buf].value_word = 0;// <=
			else
			{
				if (current_ch == '>') buffer_lexical[count_buf].value_word = 4;// <>
				else
				{
					buffer_lexical[count_buf].value_word = 1;//<
					pointer_buffer--;
				}
			}
			buffer_lexical[count_buf].typeCode_word = rop;
			count_buf++;
			break;
		case '>':
			readCh_from_buffer();
			if (current_ch == '=')
				buffer_lexical[count_buf].value_word = 2;// >=
			else
			{
				buffer_lexical[count_buf].value_word = 3;// >
				pointer_buffer--;
			}
			buffer_lexical[count_buf].typeCode_word = rop;
			count_buf++;
			break;
		case '(':
			buffer_lexical[count_buf].typeCode_word = lparent;
			count_buf++;
			break;
		case ')':
			buffer_lexical[count_buf].typeCode_word = rparent;
			count_buf++;
			break;
		case '#':
			buffer_lexical[count_buf].typeCode_word = jinghao;
			count_buf++;
			break;
		case '+':
			buffer_lexical[count_buf].typeCode_word = op_plus;
			count_buf++;
			break;
			/*
			case '-':
				buffer_lexical[count_buf].typeCode_word = op_sub;
				count_buf++;
				break;
			*/
		case '*':
			buffer_lexical[count_buf].typeCode_word = op_times;
			count_buf++;
			break;
			/*
		case '/':
			buffer_lexical[count_buf].typeCode_word = op_div;
			count_buf++;
			break;
			*/
		case ':':
			readCh_from_buffer();
			if (current_ch == '=')
				buffer_lexical[count_buf].typeCode_word = becomes;// :=
			count_buf++;
			break;
		case '=':
			buffer_lexical[count_buf].typeCode_word = rop;
			buffer_lexical[count_buf].value_word = 5;
			count_buf++;
			break;
		case ';':
			buffer_lexical[count_buf].typeCode_word = semicolon;
			count_buf++;
			break;
		}
		readCh_from_buffer();
	}
	buffer_lexical[count_buf].typeCode_word = -1;
}

/*************************显示词法分析结果*******************************/
void disp1() {
	int temp1 = 0;
	printf("\n*****************词法分析结果********************\n");
	for (temp1 = 0; temp1 < count_buf; temp1++) {
		printf("\t%d\t\t%d\n", buffer_lexical[temp1].typeCode_word, buffer_lexical[temp1].value_word);
	}
	//getchar();
}

/* 词法分析及结果显示 */
void lexical_analyse() {
	init_source_file();		// 初始化输入文件
	readCh_from_buffer();	// 从一行字符缓冲区中读取一个字符
	scan();					// 词法分析
	disp1();				// 显示词法分析结果
}
