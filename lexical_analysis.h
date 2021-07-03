#pragma once
#include <cstdio>
#include"config.h"


extern FILE* source_file;	// 源程序文件
extern errno_t error_source_file;
extern char* pline;			// 字符缓冲区指针
extern char current_ch;		// 当前字符
static char line[81] = { "" };		// 一行字符缓冲区  最多80个
extern int lnum;	// 源程序长度
extern aa buf[1000];			// 词法分析结果缓冲区

void init_source_file();	// 初始化源文件
void readline();	// 从文件读一行到缓冲区
void readch();		// 从缓冲区读取一个字符


int find(char spel[]);	// 标识符和关键字的识别
int identifier();		// 识别保留字和标识符
extern int tt1;			// 变量名表指针
extern aa* pbuf;		// 指向词法分析缓冲区
extern int nlength;		// 词法分析中记录单词长度

void number();			// 数字识别

/***********************扫描函数************************/
void scan();			// 滤除多余空格并对主要单词分析处理

void disp1();			// 显示词法分析结果
