#pragma once
#include <cstdio>
#include"config.h"

extern FILE* source_file;				// 源程序文件
extern errno_t error_source_file;		// 读取源文件需要的异常返回变量

extern char  buffer_line[81];			// 一行字符缓冲区(最多80个)
extern char* pointer_buffer;			// 字符缓冲区指针
extern char  current_ch;				// 当前字符



extern int lineNum_sourceCode;			// 源程序长度(行数)
extern two_exp buffer_lexical[1000];	// 词法分析结果缓冲区

void init_source_file();				// 初始化源文件
void readLine_to_buffer();				// 从文件读一行到缓冲区
void readCh_from_buffer();				// 从缓冲区读取一个字符


int find(char spel[]);	// 标识符和关键字的识别
int identifier();		// 识别保留字和标识符
extern int tt1;			// 变量名表指针
extern two_exp* pbuf;	// 指向词法分析缓冲区
extern int nlength;		// 词法分析中记录单词长度

void number();			// 数字识别

/***********************扫描函数************************/
void scan();			// 滤除多余空格并对主要单词分析处理

void disp1();			// 显示词法分析结果

/* 词法分析及结果显示 */
void lexical_analyse();
