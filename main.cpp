/* 导入配置文件 */
#include"config.h"
/* 导入头文件 */
#include "stdio.h"
#include "string.h"
#include<Windows.h>
#include<iostream>
using namespace std;


FILE* cfile;//源程序文件



/********************从文件读一行到缓冲区**********************/
void readline() {
	char ch1;
	pline = line;
	ch1 = getc(cfile);
	while ((ch1 != '\n') && (ch1 != EOF))
	{
		*pline = ch1;
		pline++;
		ch1 = getc(cfile);
	}
	*pline = '\0';
	pline = line;
}

/**********************从缓冲区读取一个字符*********************/
void readch()
{
	if (current_ch == '\0')
	{
		readline();
		lnum++;//读取一行，源程序长度+1
	}
	current_ch = *pline;
	pline++;
}

/***********************标识符和关键字的识别********************/
int find(char spel[])//与变量名表中的变量进行匹配，查找变量名表
{
	int ss1 = 0;
	int ii = 0;
	while ((ss1 == 0) && (ii < nlength))
	{
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
		buf[count_buf].sy1 = reswords[iii - 1].sy;
	}
	else {
		buf[count_buf].sy1 = ident;//是标识符，变量名
		j = find(spelling);
		if (j == -1) {	//没在变量名表中，则添加
			buf[count_buf].pos = tt1;
			strcpy_s(table_variable[tt1], spelling);
			tt1++;
			nlength++;
		}
		else buf[count_buf].pos = j;//获得变量名自身的值
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
	buf[count_buf].sy1 = intconst;
	buf[count_buf].pos = ivalue;
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
				buf[count_buf].pos = 0;// <=
			else
			{
				if (current_ch == '>') buf[count_buf].pos = 4;// <>
				else
				{
					buf[count_buf].pos = 1;//<
					pline--;
				}
			}
			buf[count_buf].sy1 = rop;
			count_buf++;
			break;
		case '>':
			readch();
			if (current_ch == '=')
				buf[count_buf].pos = 2;// >=
			else
			{
				buf[count_buf].pos = 3;// >
				pline--;
			}
			buf[count_buf].sy1 = rop;
			count_buf++;
			break;
		case '(':
			buf[count_buf].sy1 = lparent;
			count_buf++;
			break;
		case ')':
			buf[count_buf].sy1 = rparent;
			count_buf++;
			break;
		case '#':
			buf[count_buf].sy1 = jinghao;
			count_buf++;
			break;
		case '+':
			buf[count_buf].sy1 = op_plus;
			count_buf++;
			break;
			/*
			case '-':
				buf[count_buf].sy1 = op_sub;
				count_buf++;
				break;
			*/
		case '*':
			buf[count_buf].sy1 = op_times;
			count_buf++;
			break;
			/*
		case '/':
			buf[count_buf].sy1 = op_div;
			count_buf++;
			break;
			*/
		case ':':
			readch();
			if (current_ch == '=')
				buf[count_buf].sy1 = becomes;// :=
			count_buf++;
			break;
		case '=':
			buf[count_buf].sy1 = rop;
			buf[count_buf].pos = 5;
			count_buf++;
			break;
		case ';':
			buf[count_buf].sy1 = semicolon;
			count_buf++;
			break;
		}
		readch();
	}
	buf[count_buf].sy1 = -1;
}

/********************程序语句的LR分析表********************/
static int action[19][13] =
{
 {2,-1,-1,3,4,-1,-1,5,-1,-1,-1,1,-1},
 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,ACC,-1,-1},
 {-1,-1,-1,-1,-1,-1,-1,-1,-1,6,-1,-1,-1},
 {-1,-1,-1,-1,-1,-1,-1,-1,-1,7,-1,-1,-1},
 {2,-1,-1,3,4,-1,-1,5,-1,-1,-1,9,8},
 {-1,-1,104,-1,-1,-1,104,-1,104,-1,104,-1,-1},
 {-1,10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
 {-1,-1,-1,-1,-1,11,-1,-1,-1,-1,-1,-1,-1},
 {-1,-1,-1,-1,-1,-1,12,-1,-1,-1,-1,-1,-1},
 {-1,-1,-1,-1,-1,-1,105,-1,13,-1,-1,-1,-1},
 {2,-1,-1,3,4,-1,-1,5,-1,-1,-1,14,-1},
 {2,-1,-1,3,4,-1,-1,5,-1,-1,-1,15,-1},
 {-1,-1,103,-1,-1,-1,103,-1,103,-1,103,-1,-1},
 {2,-1,-1,3,4,-1,-1,5,-1,-1,-1,9,16},
 {-1,-1,17,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
 {-1,-1,102,-1,-1,-1,102,-1,102,-1,102,-1,-1},
 {-1,-1,-1,-1,-1,-1,106,-1,-1,-1,-1,-1,-1},
 {2,-1,-1,3,4,-1,-1,5,-1,-1,-1,18,-1},
 {-1,-1,101,-1,-1,-1,101,-1,101,-1,101,-1,-1}
};
/********************算术表达式的LR分析表********************/
static int action1[10][7] =
/*    i  +  *  (  )  #   E  */
{ {3,-1,-1,2,-1,-1,1},
 {-1,4,5,-1,-1,ACC,-1},
 {3,-1,-1,2,-1,-1,6},
 {-1,104,104,-1,104,104,-1},
 {3,-1,-1,2,-1,-1,7},
 {3,-1,-1,2,-1,-1,8},
 {-1,4,5,-1,9,-1,-1},
 {-1,101,5,-1,101,101,-1},
 {-1,102,102,-1,102,102,-1},
 {-1,103,103,-1,103,103,-1}
};

/********************布尔表达式的LR分析表*********************/
static int action2[16][11] =
{ {1,-1,4,-1,5,-1,-1,-1,13,7,8},
 {-1,2,-1,101,-1,101,101,101,-1,-1,-1},
 {3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
 {-1,-1,-1,102,-1,102,102,102,-1,-1,-1},
 {1,-1,4,-1,5,-1,-1,-1,11,7,8},
 {1,-1,4,-1,5,-1,-1,-1,6,7,8},
 {-1,-1,-1,104,-1,9,10,104,-1,-1,-1},
 {1,-1,4,-1,5,-1,-1,-1,14,7,8},
 {1,-1,4,-1,5,-1,-1,-1,15,7,8},
 {105,-1,105,-1,105,-1,-1,-1,-1,-1,-1},
 {107,-1,107,-1,107,-1,-1,-1,-1,-1,-1},
 {-1,-1,-1,12,-1,9,10,-1,-1,-1,-1},
 {-1,-1,-1,103,-1,103,103,103,-1,-1,-1},
 {-1,-1,-1,-1,-1,9,10,ACC,-1,-1,-1},
 {-1,-1,-1,106,-1,9,10,106,-1,-1,-1},
 {-1,-1,-1,108,-1,9,10,108,-1,-1,-1}
};


/********************从二元式读入一个符号*********************/
void readnu()
{
	if (pbuf->sy1 >= 0)
	{
		n.sy1 = pbuf->sy1;
		n.pos = pbuf->pos;
		pbuf++;
	}
}
/***********************中间变量的生成*********************/
int newtemp()
{
	newt++;
	return newt;
}
/***********************生成四元式**************************/
int gen(const char op1[], struct aa arg11, struct aa arg22, int result1)
{
	strcpy_s(fexp[nxq].op, op1);
	fexp[nxq].arg1.sy1 = arg11.sy1;
	fexp[nxq].arg1.pos = arg11.pos;
	fexp[nxq].arg2.sy1 = arg22.sy1;
	fexp[nxq].arg2.pos = arg22.pos;
	fexp[nxq].result = result1;
	nxq++;
	return nxq - 1;
}
/***********************布尔表达式的匹配**********************/
int merg(int p1, int p2)
{
	int p;
	if (p2 == 0) return p1;
	else
	{
		p = p2;
		while (fexp[p].result != 0)
			p = fexp[p].result;
		fexp[p].result = p1;
		return p2;
	}
}
void backpatch(int p, int t)//返填函数
{
	int tempq;
	int q;
	q = p;
	while (q != 0)
	{
		tempq = fexp[q].result;
		fexp[q].result = t;
		q = tempq;
	}
}

/*********************************************************/
int change1(int chan) {
	switch (chan) {
	case ident:
	case intconst:return 0;
	case op_plus:return 1;
		//case op_sub:return 2;
	case op_times:return 2;
		//case op_div:return 4;
	case lparent:return 3;
	case rparent:return 4;
	case jinghao:return 5;
	case tempsy:return 6;
	default:
		return -1;
	}
}

int change2(int chan) {
	switch (chan) {
	case ident:
	case intconst:return 0;
	case rop:return 1;
	case lparent:return 2;
	case rparent:return 3;
	case op_not:return 4;
	case op_and:return 5;
	case op_or:return 6;
	case jinghao:return 7;
	case tempsy:return 8;
	case EA:return 9;
	case E0:return 10;
	default:
		return -1;
		// do nothing
	}
}
/*********************赋值语句和算术表达式的分析**********************/
int lrparse1(int num)
{
	lr1 = action1[stack1[sp1]][change1(n1.sy1)];
	if (lr1 == -1) {
		printf("\n算术表达式或赋值语句出错!\n");
		// getchar();
		cout << endl;
		exit(0);
	}
	/* 移进状态 */
	if ((lr1 < 10) && (lr1 >= 0))
	{
		sp1++;
		stack1[sp1] = lr1;
		if (n1.sy1 != tempsy) {
			ssp++;
			num++;
			sstack[ssp].sy1 = n1.sy1;//将变量名压栈
			sstack[ssp].pos = n1.pos;//将变量名地址压栈
		}
		n1.sy1 = ibuf[num].sy1;
		n1.pos = ibuf[num].pos;
		lrparse1(num);
	}
	/* 归约状态 */
	if ((lr1 >= 100) && (lr1 < 105)) {
		switch (lr1) {
		case 100:break;// S'->E
		case 101:
			E.pos = newtemp();// E->E+E
			gen("+", sstack[ssp - 2], sstack[ssp], E.pos + 100);
			ssp = ssp - 2;
			sstack[ssp].sy1 = tempsy;
			sstack[ssp].pos = E.pos;
			sp1 = sp1 - 3;//产生式右部长度为3，故归约后栈指针减3
			break;
		case 102:
			E.pos = newtemp();// E->E*E
			gen("*", sstack[ssp - 2], sstack[ssp], E.pos + 100);
			ssp = ssp - 2;
			sstack[ssp].sy1 = tempsy;
			sstack[ssp].pos = E.pos;
			sp1 = sp1 - 3;
			break;
		case 103:
			E.pos = sstack[ssp - 1].pos; //E->(E)
			ssp = ssp - 2;
			sstack[ssp].sy1 = tempsy;
			sstack[ssp].pos = E.pos;
			sp1 = sp1 - 3;
			break;
		case 104:
			E.pos = sstack[ssp].pos;//E->i
			sp1--;
			break;
		}
		n1.sy1 = tempsy;//归约后为非终结符
		n1.pos = E.pos;
		lrparse1(num);
	}
	if ((lr1 == ACC) && (stack1[sp1] == 1))//归约A->i:=E   
	{
		gen(":=", sstack[ssp], oth, ibuf[0].pos);
		ssp = ssp - 3;
		sp1 = sp1 - 3;
	}
}
/***********************布尔表达式的分析**********************/
int lrparse2(int num)
{
	int templabel;
	lr1 = action2[stack1[sp1]][change2(n1.sy1)];
	if (lr1 == -1)
	{
		if (sign == 2) printf("\nwhile语句出错\n");
		if (sign == 3) printf("\nif语句出错\n");
		// getchar();
		cout << endl;
		exit(0);
	}
	if ((lr1 < 16) && (lr1 >= 0))//移进状态
	{
		sp1++;
		stack1[sp1] = lr1;
		ssp++;
		sstack[sp1].sy1 = n1.sy1;
		sstack[ssp].pos = n1.pos;
		if ((n1.sy1 != tempsy) && (n1.sy1 != EA) && (n1.sy1 != E0))
			num++;
		n1.sy1 = ibuf[num].sy1;
		n1.pos = ibuf[num].pos;
		lrparse2(num);
	}
	if ((lr1 >= 100) && (lr1 < 109))//归约状态
	{
		switch (lr1)
		{
		case 100:break;//S'->B
		case 101://B->i
			ntab2[label].tc = nxq;
			ntab2[label].fc = nxq + 1;
			gen("jnz", sstack[ssp], oth, 0);
			gen("j", oth, oth, 0);
			sp1--;
			ssp--;
			label++;
			n1.sy1 = tempsy;
			break;
		case 102://B->i rop i
			ntab2[label].tc = nxq;
			ntab2[label].fc = nxq + 1;
			switch (sstack[ssp - 1].pos)
			{
			case 0:
				gen("j<=", sstack[ssp - 2], sstack[ssp], 0);
				break;
			case 1:
				gen("j<", sstack[ssp - 2], sstack[ssp], 0);
				break;
			case 2:
				gen("j>=", sstack[ssp - 2], sstack[ssp], 0);
				break;
			case 3:
				gen("j>", sstack[ssp - 2], sstack[ssp], 0);
				break;
			case 4:
				gen("j<>", sstack[ssp - 2], sstack[ssp], 0);
				break;
			case 5:
				gen("j=", sstack[ssp - 2], sstack[ssp], 0);
				break;
			}
			gen("j", oth, oth, 0);
			sp1 = sp1 - 3;
			ssp = ssp - 3;
			label++;
			n1.sy1 = tempsy;
			break;
		case 103://B->(B)
			label = label - 1;
			ssp = ssp - 3;
			sp1 = sp1 - 3;
			label++;
			n1.sy1 = tempsy;
			break;
		case 104://B->not B
			label = label - 1;
			templabel = ntab2[label].tc;
			ntab2[label].tc = ntab2[label].fc;
			ntab2[label].fc = templabel;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.sy1 = tempsy;
			break;
		case 105://A->B and
			backpatch(ntab2[label - 1].tc, nxq);
			label = label - 1;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.sy1 = EA;
			break;
		case 106://B->AB
			label = label - 2;
			ntab2[label].tc = ntab2[label + 1].tc;
			ntab2[label].fc = merg(ntab2[label].fc, ntab2[label + 1].fc);
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.sy1 = tempsy;
			break;
		case 107://O->B or
			backpatch(ntab2[label - 1].fc, nxq);
			label = label - 1;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.sy1 = E0;
			break;
		case 108://B->0B
			label = label - 2;
			ntab2[label].fc = ntab2[label + 1].fc;
			ntab2[label].tc = merg(ntab2[label].tc, ntab2[label + 1].tc);
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.sy1 = tempsy;
			break;
		}
		lrparse2(num);
	}
	if (lr1 == ACC) return 1;
}
/************测试字符是否为表达式中的值(不包括":")****************/
int test(int value)
{
	switch (value)
	{
	case intconst:
	case ident:
	case op_plus:
	case op_sub:
	case op_times:
	case op_div:
	case becomes:
	case lparent:
	case rparent:
	case rop:
	case op_and:
	case op_or:
	case op_not:
		return 1;
	default:
		return 0;
	}
}
/*************程序语句处理****************************************/
int lrparse()
{
	int i1 = 0;
	int num = 0;//指向表达式缓冲区
	if (test(n.sy1))
	{
		if (stack[sp].sy1 == sy_while) sign = 2;
		else
		{
			if (stack[sp].sy1 == sy_if) sign = 3;
			else sign = 1;
		}
		do
		{
			ibuf[i1].sy1 = n.sy1;
			ibuf[i1].pos = n.pos;
			readnu();
			i1++;
		} while (test(n.sy1));//把算术或布尔表达式放入缓冲区
		ibuf[i1].sy1 = jinghao;
		pbuf--;//词法分析缓冲区指针减1
		sstack[0].sy1 = jinghao;
		ssp = 0;//符号栈初始化
		if (sign == 1)//赋值语句处理
		{
			sp1 = 0;
			stack1[sp1] = 0;//状态栈1初始化
			num = 2;
			n1.sy1 = ibuf[num].sy1;
			n1.pos = ibuf[num].pos;
			lrparse1(num);//处理赋值语句
			n.sy1 = a;//当前文法符号置为a（赋值语句）
		}
		if ((sign == 2) || (sign == 3))//布尔表达式处理
		{
			pointmark++;
			labelmark[pointmark].nxq1 = nxq;
			sp1 = 0;
			stack1[sp1] = 0;
			num = 0;

			n1.sy1 = ibuf[num].sy1;
			n1.pos = ibuf[num].pos;
			lrparse2(num);
			labelmark[pointmark].tc1 = ntab2[label - 1].tc;
			labelmark[pointmark].fc1 = ntab2[label - 1].fc;
			backpatch(labelmark[pointmark].tc1, nxq);
			n.sy1 = e;//当前万能法符号置e（布尔表达式）
		}
	}
	lr = action[stack[sp].pos][n.sy1];
	//输出状态栈信息
	printf("stack[%d]=%d\t\tn=%d\t\tlr=%d\n", sp, stack[sp].pos, n.sy1, lr);
	if ((lr < 19) && (lr >= 0))//移进状态
	{
		sp++;
		stack[sp].pos = lr;
		stack[sp].sy1 = n.sy1;
		readnu();
		lrparse();
	}
	if ((lr <= 106) && (lr >= 100))//归约状态
	{
		switch (lr)
		{
		case 100:break;//S'->S
		case 101://S->if e then s else s 
			printf("\nS->if e then S else S 归约\n");
			sp = sp - 6;
			n.sy1 = S;
			fexp[labeltemp[pointtemp]].result = nxq;
			pointtemp--;
			if (stack[sp].sy1 == sy_then)
			{
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			pointmark--;
			if (stack[sp].sy1 == sy_do)
			{
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 102:printf("\nS->while e do S 归约\n");
			sp = sp - 4;
			n.sy1 = S;
			pointmark--;
			if (stack[sp].sy1 == sy_do)
			{
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			if (stack[sp].sy1 == sy_then)
			{
				gen("j", oth, oth, 0);
				fexp[labelmark[pointmark].fc1].result = nxq;
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			break;
		case 103:printf("\nS->begin L end 归约\n");
			sp = sp - 3;
			n.sy1 = S;
			if (stack[sp].sy1 == sy_then)
			{
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			if (stack[sp].sy1 == sy_do)
			{
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 104:printf("\nS->a 归约\n");
			sp = sp - 1;
			n.sy1 = S;
			if (stack[sp].sy1 == sy_then)
			{
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			if (stack[sp].sy1 == sy_do)
			{
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 105:printf("\nL->S 归约\n");
			sp = sp - 1;
			n.sy1 = L;
			break;
		case 106:printf("\nL->S L 归约\n");
			sp = sp - 3;
			n.sy1 = L;
			break;
		}
		pbuf--;
		lrparse();
	}
	if (lr == ACC)  return ACC;
}

/*************************显示词法分析结果*******************************/
void disp1()
{
	int temp1 = 0;
	printf("\n*****************词法分析结果********************\n");
	for (temp1 = 0; temp1 < count_buf; temp1++)
	{
		printf("\t%d\t\t%d\n", buf[temp1].sy1, buf[temp1].pos);
	}
	//getchar(); 
}
/****************************四元式分析结果**********************************/
void disp2()
{
	int temp1 = 100;
	printf("\n**************四元式分析结果********************\n");
	for (temp1 = 100; temp1 < nxq; temp1++)
	{
		if (temp1 > 100)
			printf("\n");
		printf("%d\t", temp1);
		printf("（%s,\t", fexp[temp1].op);
		if (fexp[temp1].arg1.sy1 == ident)//为变量
			printf("%s,\t", table_variable[fexp[temp1].arg1.pos]);
		else
		{
			if (fexp[temp1].arg1.sy1 == tempsy)//为临时变量
				printf("T%d,\t", fexp[temp1].arg1.pos);
			else
			{
				if (fexp[temp1].arg1.sy1 == intconst)//为整常量
					printf("%d,\t", fexp[temp1].arg1.pos);
				else//否则
					printf(",\t");
			}
		}
		if (fexp[temp1].arg2.sy1 == ident)
			printf("%s,\t", table_variable[fexp[temp1].arg2.pos]);
		else
		{
			if (fexp[temp1].arg2.sy1 == tempsy)
				printf("T%d,\t", fexp[temp1].arg2.pos);
			else
			{
				if (fexp[temp1].arg2.sy1 == intconst)
					printf("%d,\t", fexp[temp1].arg2.pos);
				else printf(",\t");
			}
		}
		if (fexp[temp1].op[0] != 'j')
		{
			if (fexp[temp1].result >= 100)
				printf("T%d\t)", fexp[temp1].result - 100);
			else
				printf("%s\t)", table_variable[fexp[temp1].result]);
		}
		else
			printf("%d\t)", fexp[temp1].result);
		if (temp1 == 20) {
			printf("\npress any key to continue......\n");
			// getchar();
			cout << endl;
		}
		printf("\n");
	}
	//getchar(); 
}
/*************************打印变量名表*******************************/
void disp3()
{
	int tttt;
	printf("\n\n程序总共%d行，产生了%d个二元式!\n", lnum, count_buf);
	// getchar();
	cout << endl;
	printf("\n******************变量名表**********************\n");
	for (tttt = 0; tttt < tt1; tttt++)
		printf("%d\t%s\n", tttt, table_variable[tttt]);
	// getchar();
	cout << endl;
}

/***********************主函数***************************/
int main() {
	errno_t ccfile;
	ccfile = fopen_s(&cfile, "pas.dat", "r");//打开C源文件
	readch();//从源文件读字符
	scan();//词法分析
	disp1();//显示词法分析结果
	disp3();//显示变量名
	stack[sp].pos = 0;
	stack[sp].sy1 = -1;//初始化状态栈
	stack1[sp1] = 0;//初始化状态栈1
	oth.sy1 = -1;
	printf("\n*************状态栈加工过程及归约顺序*************\n");
	readnu();//从二元式读一个字符
	lrparse();
	// getchar();
	cout << endl;
	//四元式分析
	disp2();
	// getchar();
	cout << endl;

	printf("**************************************************************");
	printf("\n程序运行结束!\n");

	return 0;
	system("pause");
}
