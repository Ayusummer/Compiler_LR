/* config.h
	包含常量定义以及数据结构 */
#pragma once
extern const int ACC;
extern const int sy_if, sy_then, sy_else, sy_while;
extern const int sy_begin, sy_do, sy_end;
extern const int a, semicolon, e;

constexpr auto jinghao = 10;	// # 本来是想先声明后定义的, 但是不知为何那样做的话就无法被识别为常量
constexpr auto S = 11, L = 12,
tempsy = 15,
EA = 18,
E0 = 19,
op_plus = 34, op_sub = 35,
op_times = 36, op_div = 37,
becomes = 38,
op_and = 39, op_or = 40, op_not = 41,
rop = 42,
lparent = 48, rparent = 49,
ident = 56,
intconst = 57;


extern int  count_buf;		// 词法分析结果缓冲区计数器

static char spelling[10] = { "" };	// 存放识别的字

static char table_variable[100][10];		// 变量名表

struct ntab {
	int tc;		// 真
	int fc;		// 假
};

/* 保留字表的结构 */
struct rwords {
	char sp[10];
	int  sy;
};
extern rwords reswords[10];

// 二元式结构
struct aa {
	int sy1;	// 单词种别编码
	int pos;	// 单词自身的值
};

/* 四元式的结构 */
struct fourexp {
	char op[10];
	struct aa arg1;
	struct aa arg2;
	int result;
};

struct ll {
	int nxq1;
	int tc1;
	int fc1;
};
