#pragma once
#include"lexical_analysis.h"
#include <string.h>

char current_ch = '\0';		// 当前字符为空字符
int lnum = 0;				// 源程序长度
int tt1 = 0;				// 变量名表指针
aa* pbuf = buffer_lexical;				// 指向词法分析缓冲区
int nlength = 0;			// 词法分析中记录单词长度
FILE* source_file = NULL;
char* pline = NULL;
errno_t error_source_file = 0;
aa buffer_lexical[1000] = {};

void init_source_file() {
	error_source_file = fopen_s(&source_file, "pas.dat", "r");//打开C源文件
}

/********************从文件读一行到缓冲区**********************/
void readline() {
	char nxt_char_source_file = getc(source_file);	// 读取来自 source_file 的下一个字符
	pline = buffer_line;
	/* 读取一行源文件字符 */
	while ((nxt_char_source_file != '\n') && (nxt_char_source_file != EOF)) {
		*pline = nxt_char_source_file;
		pline++;
		nxt_char_source_file = getc(source_file);
	}
	*pline = '\0';
	pline = buffer_line;
}

/**********************从缓冲区读取一个字符*********************/
void readch() {
	if (current_ch == '\0') {
		readline();
		lnum++;		// 读取一行，源程序长度+1
	}
	current_ch = *pline;
	pline++;
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
		readch();
	} while (((current_ch >= 'a') && (current_ch <= 'z')) || ((current_ch >= '0') && (current_ch <= '9')));
	pline--;
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
		buffer_lexical[count_buf].sy1 = reswords[iii - 1].sy;
	}
	else {
		buffer_lexical[count_buf].sy1 = ident;//是标识符，变量名
		j = find(spelling);
		if (j == -1) {	//没在变量名表中，则添加
			buffer_lexical[count_buf].pos = tt1;
			strcpy_s(table_variable[tt1], spelling);
			tt1++;
			nlength++;
		}
		else buffer_lexical[count_buf].pos = j;//获得变量名自身的值
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
		readch();
	} while ((current_ch >= '0') && (current_ch <= '9'));
	buffer_lexical[count_buf].sy1 = intconst;
	buffer_lexical[count_buf].pos = ivalue;
	count_buf++;
	pline--;
}

/***********************扫描函数************************/
/* 滤除多余空格并对主要单词分析处理 */
void scan() {
	while (current_ch != '~') {
		switch (current_ch)
		{
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
		case '<':readch();
			if (current_ch == '=')
				buffer_lexical[count_buf].pos = 0;// <=
			else
			{
				if (current_ch == '>') buffer_lexical[count_buf].pos = 4;// <>
				else
				{
					buffer_lexical[count_buf].pos = 1;//<
					pline--;
				}
			}
			buffer_lexical[count_buf].sy1 = rop;
			count_buf++;
			break;
		case '>':
			readch();
			if (current_ch == '=')
				buffer_lexical[count_buf].pos = 2;// >=
			else
			{
				buffer_lexical[count_buf].pos = 3;// >
				pline--;
			}
			buffer_lexical[count_buf].sy1 = rop;
			count_buf++;
			break;
		case '(':
			buffer_lexical[count_buf].sy1 = lparent;
			count_buf++;
			break;
		case ')':
			buffer_lexical[count_buf].sy1 = rparent;
			count_buf++;
			break;
		case '#':
			buffer_lexical[count_buf].sy1 = jinghao;
			count_buf++;
			break;
		case '+':
			buffer_lexical[count_buf].sy1 = op_plus;
			count_buf++;
			break;
			/*
			case '-':
				buffer_lexical[count_buf].sy1 = op_sub;
				count_buf++;
				break;
			*/
		case '*':
			buffer_lexical[count_buf].sy1 = op_times;
			count_buf++;
			break;
			/*
		case '/':
			buffer_lexical[count_buf].sy1 = op_div;
			count_buf++;
			break;
			*/
		case ':':
			readch();
			if (current_ch == '=')
				buffer_lexical[count_buf].sy1 = becomes;// :=
			count_buf++;
			break;
		case '=':
			buffer_lexical[count_buf].sy1 = rop;
			buffer_lexical[count_buf].pos = 5;
			count_buf++;
			break;
		case ';':
			buffer_lexical[count_buf].sy1 = semicolon;
			count_buf++;
			break;
		}
		readch();
	}
	buffer_lexical[count_buf].sy1 = -1;
}

/*************************显示词法分析结果*******************************/
void disp1() {
	int temp1 = 0;
	printf("\n*****************词法分析结果********************\n");
	for (temp1 = 0; temp1 < count_buf; temp1++) {
		printf("\t%d\t\t%d\n", buffer_lexical[temp1].sy1, buffer_lexical[temp1].pos);
	}
	//getchar();
}

void lexical_analyse() {
	init_source_file();	// 初始化输入文件
	readch();			// 从源文件读字符
	scan();				// 词法分析
	disp1();			// 显示词法分析结果
}
