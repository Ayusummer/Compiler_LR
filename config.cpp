#include"config.h"

const int ACC = -2,
sy_if = 0, sy_then = 1, sy_else = 2, sy_while = 3,
sy_begin = 4, sy_do = 5, sy_end = 6,
a = 7,
semicolon = 8,
e = 9;

char current_ch = '\0';		// 当前字符为空字符
int  count_buf = 0;			// 词法分析结果缓冲区计数器

// 保留字表初始化
rwords reswords[10] = {
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
