/* config.h
	包含常量定义以及数据结构*/
#pragma once
constexpr auto ACC = -2;
constexpr auto sy_if = 0, sy_then = 1, sy_else = 2, sy_while = 3;
constexpr auto sy_begin = 4, sy_do = 5, sy_end = 6;
constexpr auto a = 7;		// 转化的时候注意到与库文件有类似的地方
constexpr auto semicolon = 8;
constexpr auto e = 9;
constexpr auto jinghao = 10;
constexpr auto S = 11;
constexpr auto L = 12;

constexpr auto tempsy = 15;
constexpr auto EA = 18;
constexpr auto E0 = 19;

constexpr auto op_plus = 34, op_sub = 35;
constexpr auto op_times = 36, op_div = 37;
constexpr auto becomes = 38;
constexpr auto op_and = 39, op_or = 40, op_not = 41;
constexpr auto rop = 42;

constexpr auto lparent = 48, rparent = 49;
constexpr auto ident = 56;
constexpr auto intconst = 57;


char current_ch = '\0';		// 当前字符
int  count_buf = 0;			// 词法分析结果缓冲区计数器
static char spelling[10] = { "" };	// 存放识别的字
static char line[81] = { "" };		// 一行字符缓冲区  最多80个
char* pline;	// 字符缓冲区指针


static char table_variable[100][10];		// 变量名表

struct ntab {
	int tc;//真
	int fc;//假
}ntab2[200];
int label = 0;		// 指向ntab2的指针?

struct rwords {//保留字表的结构
	char sp[10];
	int  sy;
};
rwords reswords[10] = {//保留字表初始化
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

struct aa {
	int sy1;	// 单词种别编码
	int pos;	// 单词自身的值
};
aa	buf[1000],	// 词法分析结果缓冲区
n,				// 当前字符
n1,				// 当前表达式中的字符
E,				// 非终结符
sstack[100],	// 符号栈
ibuf[100],		// 缓冲区
stack[1000];	// 语法分析加工处理使用的符号栈

aa oth;//四元式中的空白位置

struct fourexp//四元式的结构
{
	char op[10];
	struct aa arg1;
	struct aa arg2;
	int result;
};
fourexp fexp[200];


int ssp = 0;//指向sstack[100]

struct aa* pbuf = buf;//指向词法分析缓冲区
int nlength = 0;//词法分析中记录单词长度
int lnum = 0;//源程序长度
int tt1 = 0;//变量名表指针

int newt = 0;//临时变量
int nxq = 100;//nxq指向下一个形成的四元式的地址
int lr;//扫描LR分析表1过程中保存的当前状态值
int lr1;//扫描LR分析表2或3过程中保存的当前状态值
int sp = 0;//查找LR分析表时状态栈的栈顶指针
int stack1[100];//定义状态栈
int sp1 = 0;//定义状态栈1的栈顶指针
int num = 0;//缓冲区指针

struct ll
{
	int nxq1;
	int tc1;
	int fc1;
};
ll labelmark[10];


int labeltemp[10];
int pointmark = -1, pointtemp = -1;
int sign = 0;//sign=1，表达式为赋值语句  sign=2，表达式为布尔表达式
