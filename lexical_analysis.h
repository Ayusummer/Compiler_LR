#pragma once
#include <cstdio>
#include"config.h"

extern FILE* source_file;				// 源程序文件
extern errno_t error_source_file;		// 读取源文件需要的异常返回变量
void init_source_file();				// 初始化源文件

extern char  buffer_line[81];			// 一行字符缓冲区(最多80个)
extern char* pointer_buffer;			// 字符缓冲区指针
extern char  current_ch;				// 当前字符
extern int lineNum_sourceCode;			// 源程序长度(行数)
void readLine_to_buffer();				// 从文件读一行到缓冲区
void readCh_from_buffer();				// 从缓冲区读取一个字符 
char show_nxtCh_buffer();				// 展示缓冲区下一个字符


extern char word_recognized[10];		// 识别字数组,存放识别的字
/* 保留字表的结构 */
struct keywords {
	char word[10];	// 保留字
	int  wordCode;	// 保留字编码
};
extern keywords reserved_words[10];		// 保留字表

extern char table_variable[100][10];	// 变量名表
extern int num_variable;				// 词法分析中识别放到变量名表中的变量名数目
int find_in_variableTable(char spel[]);	// 标识符和关键字的识别
void stringRecognize();					// 识别保留字和标识符,变量名

extern two_exp buffer_lexical[1000];	// 词法分析结果缓冲区
extern int  count_lexResult;			// 词法分析结果缓冲区计数器
extern two_exp* pointer_bufLex;			// 词法分析缓冲区指针
extern int num_lexVariable;				// 词法分析缓冲区中含有的变量的数目

void numberRecognize();		// 数字识别

void lexical_analyse();		// 扫描源程序->缓冲区->滤除多余空格并对主要单词分析处理
void lexical_analyse_show();				// 显示词法分析结果

/* 打印变量名表 */
void show_variableTable();
/* 词法分析及结果显示 */
void lexical_analyse_global();
