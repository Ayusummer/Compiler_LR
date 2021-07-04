﻿/* 导入配置文件 */
#include"config.h"
#include"lexical_analysis.h"

/* 导入头文件 */
#include "stdio.h"
#include "string.h"
#include<Windows.h>
#include<iostream>
using namespace std;


ntab ntab2[200];
int label = 0;		// 指向ntab2的指针?

two_exp	// 词法分析结果缓冲区
n,				// 当前字符
n1,				// 当前表达式中的字符
E,				// 非终结符
sstack[100],	// 符号栈
ibuf[100],		// 缓冲区
stack[1000];	// 语法分析加工处理使用的符号栈

two_exp oth;//四元式中的空白位置


fourexp fexp[200];

int ssp = 0;//指向sstack[100]

int newt = 0;//临时变量
int nxq = 100;//nxq指向下一个形成的四元式的地址
int lr;//扫描LR分析表1过程中保存的当前状态值
int lr1;//扫描LR分析表2或3过程中保存的当前状态值
int sp = 0;//查找LR分析表时状态栈的栈顶指针
int stack1[100];//定义状态栈
int sp1 = 0;//定义状态栈1的栈顶指针
int num = 0;//缓冲区指针


ll labelmark[10];

int labeltemp[10];
int pointmark = -1, pointtemp = -1;
int sign = 0;//sign=1，表达式为赋值语句  sign=2，表达式为布尔表达式


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
	if (pbuf->typeCode_word >= 0)
	{
		n.typeCode_word = pbuf->typeCode_word;
		n.value_word = pbuf->value_word;
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
int gen(const char op1[], struct two_exp arg11, struct two_exp arg22, int result1)
{
	strcpy_s(fexp[nxq].op, op1);
	fexp[nxq].arg1.typeCode_word = arg11.typeCode_word;
	fexp[nxq].arg1.value_word = arg11.value_word;
	fexp[nxq].arg2.typeCode_word = arg22.typeCode_word;
	fexp[nxq].arg2.value_word = arg22.value_word;
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
	lr1 = action1[stack1[sp1]][change1(n1.typeCode_word)];
	if (lr1 == -1) {
		printf("\n算术表达式或赋值语句出错!\n");
		// getchar();
		cout << endl;
		exit(0);
	}
	/* 移进状态 */
	if ((lr1 < 10) && (lr1 >= 0)) {
		sp1++;
		stack1[sp1] = lr1;
		if (n1.typeCode_word != tempsy) {
			ssp++;
			num++;
			sstack[ssp].typeCode_word = n1.typeCode_word;//将变量名压栈
			sstack[ssp].value_word = n1.value_word;//将变量名地址压栈
		}
		n1.typeCode_word = ibuf[num].typeCode_word;
		n1.value_word = ibuf[num].value_word;
		lrparse1(num);
	}
	/* 归约状态 */
	if ((lr1 >= 100) && (lr1 < 105)) {
		switch (lr1) {
		case 100:break;// S'->E
		case 101:
			E.value_word = newtemp();// E->E+E
			gen("+", sstack[ssp - 2], sstack[ssp], E.value_word + 100);
			ssp = ssp - 2;
			sstack[ssp].typeCode_word = tempsy;
			sstack[ssp].value_word = E.value_word;
			sp1 = sp1 - 3;//产生式右部长度为3，故归约后栈指针减3
			break;
		case 102:
			E.value_word = newtemp();// E->E*E
			gen("*", sstack[ssp - 2], sstack[ssp], E.value_word + 100);
			ssp = ssp - 2;
			sstack[ssp].typeCode_word = tempsy;
			sstack[ssp].value_word = E.value_word;
			sp1 = sp1 - 3;
			break;
		case 103:
			E.value_word = sstack[ssp - 1].value_word; //E->(E)
			ssp = ssp - 2;
			sstack[ssp].typeCode_word = tempsy;
			sstack[ssp].value_word = E.value_word;
			sp1 = sp1 - 3;
			break;
		case 104:
			E.value_word = sstack[ssp].value_word;//E->i
			sp1--;
			break;
		}
		n1.typeCode_word = tempsy;//归约后为非终结符
		n1.value_word = E.value_word;
		lrparse1(num);
	}
	if ((lr1 == ACC) && (stack1[sp1] == 1))//归约A->i:=E
	{
		gen(":=", sstack[ssp], oth, ibuf[0].value_word);
		ssp = ssp - 3;
		sp1 = sp1 - 3;
	}
}
/***********************布尔表达式的分析**********************/
int lrparse2(int num)
{
	int templabel;
	lr1 = action2[stack1[sp1]][change2(n1.typeCode_word)];
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
		sstack[sp1].typeCode_word = n1.typeCode_word;
		sstack[ssp].value_word = n1.value_word;
		if ((n1.typeCode_word != tempsy) && (n1.typeCode_word != EA) && (n1.typeCode_word != E0))
			num++;
		n1.typeCode_word = ibuf[num].typeCode_word;
		n1.value_word = ibuf[num].value_word;
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
			n1.typeCode_word = tempsy;
			break;
		case 102://B->i rop i
			ntab2[label].tc = nxq;
			ntab2[label].fc = nxq + 1;
			switch (sstack[ssp - 1].value_word)
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
			n1.typeCode_word = tempsy;
			break;
		case 103://B->(B)
			label = label - 1;
			ssp = ssp - 3;
			sp1 = sp1 - 3;
			label++;
			n1.typeCode_word = tempsy;
			break;
		case 104://B->not B
			label = label - 1;
			templabel = ntab2[label].tc;
			ntab2[label].tc = ntab2[label].fc;
			ntab2[label].fc = templabel;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.typeCode_word = tempsy;
			break;
		case 105://A->B and
			backpatch(ntab2[label - 1].tc, nxq);
			label = label - 1;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.typeCode_word = EA;
			break;
		case 106://B->AB
			label = label - 2;
			ntab2[label].tc = ntab2[label + 1].tc;
			ntab2[label].fc = merg(ntab2[label].fc, ntab2[label + 1].fc);
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.typeCode_word = tempsy;
			break;
		case 107://O->B or
			backpatch(ntab2[label - 1].fc, nxq);
			label = label - 1;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.typeCode_word = E0;
			break;
		case 108://B->0B
			label = label - 2;
			ntab2[label].fc = ntab2[label + 1].fc;
			ntab2[label].tc = merg(ntab2[label].tc, ntab2[label + 1].tc);
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.typeCode_word = tempsy;
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
	if (test(n.typeCode_word))
	{
		if (stack[sp].typeCode_word == sy_while) sign = 2;
		else
		{
			if (stack[sp].typeCode_word == sy_if) sign = 3;
			else sign = 1;
		}
		do
		{
			ibuf[i1].typeCode_word = n.typeCode_word;
			ibuf[i1].value_word = n.value_word;
			readnu();
			i1++;
		} while (test(n.typeCode_word));//把算术或布尔表达式放入缓冲区
		ibuf[i1].typeCode_word = jinghao;
		pbuf--;//词法分析缓冲区指针减1
		sstack[0].typeCode_word = jinghao;
		ssp = 0;//符号栈初始化
		if (sign == 1)//赋值语句处理
		{
			sp1 = 0;
			stack1[sp1] = 0;//状态栈1初始化
			num = 2;
			n1.typeCode_word = ibuf[num].typeCode_word;
			n1.value_word = ibuf[num].value_word;
			lrparse1(num);//处理赋值语句
			n.typeCode_word = a;//当前文法符号置为a（赋值语句）
		}
		if ((sign == 2) || (sign == 3))//布尔表达式处理
		{
			pointmark++;
			labelmark[pointmark].nxq1 = nxq;
			sp1 = 0;
			stack1[sp1] = 0;
			num = 0;

			n1.typeCode_word = ibuf[num].typeCode_word;
			n1.value_word = ibuf[num].value_word;
			lrparse2(num);
			labelmark[pointmark].tc1 = ntab2[label - 1].tc;
			labelmark[pointmark].fc1 = ntab2[label - 1].fc;
			backpatch(labelmark[pointmark].tc1, nxq);
			n.typeCode_word = e;//当前万能法符号置e（布尔表达式）
		}
	}
	lr = action[stack[sp].value_word][n.typeCode_word];
	//输出状态栈信息
	printf("stack[%d]=%d\t\tn=%d\t\tlr=%d\n", sp, stack[sp].value_word, n.typeCode_word, lr);
	if ((lr < 19) && (lr >= 0))//移进状态
	{
		sp++;
		stack[sp].value_word = lr;
		stack[sp].typeCode_word = n.typeCode_word;
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
			n.typeCode_word = S;
			fexp[labeltemp[pointtemp]].result = nxq;
			pointtemp--;
			if (stack[sp].typeCode_word == sy_then)
			{
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			pointmark--;
			if (stack[sp].typeCode_word == sy_do)
			{
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 102:printf("\nS->while e do S 归约\n");
			sp = sp - 4;
			n.typeCode_word = S;
			pointmark--;
			if (stack[sp].typeCode_word == sy_do)
			{
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			if (stack[sp].typeCode_word == sy_then)
			{
				gen("j", oth, oth, 0);
				fexp[labelmark[pointmark].fc1].result = nxq;
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			break;
		case 103:printf("\nS->begin L end 归约\n");
			sp = sp - 3;
			n.typeCode_word = S;
			if (stack[sp].typeCode_word == sy_then)
			{
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			if (stack[sp].typeCode_word == sy_do)
			{
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 104:printf("\nS->a 归约\n");
			sp = sp - 1;
			n.typeCode_word = S;
			if (stack[sp].typeCode_word == sy_then)
			{
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			if (stack[sp].typeCode_word == sy_do)
			{
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 105:printf("\nL->S 归约\n");
			sp = sp - 1;
			n.typeCode_word = L;
			break;
		case 106:printf("\nL->S L 归约\n");
			sp = sp - 3;
			n.typeCode_word = L;
			break;
		}
		pbuf--;
		lrparse();
	}
	if (lr == ACC)  return ACC;
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
		if (fexp[temp1].arg1.typeCode_word == ident)//为变量
			printf("%s,\t", table_variable[fexp[temp1].arg1.value_word]);
		else
		{
			if (fexp[temp1].arg1.typeCode_word == tempsy)//为临时变量
				printf("T%d,\t", fexp[temp1].arg1.value_word);
			else
			{
				if (fexp[temp1].arg1.typeCode_word == intconst)//为整常量
					printf("%d,\t", fexp[temp1].arg1.value_word);
				else//否则
					printf(",\t");
			}
		}
		if (fexp[temp1].arg2.typeCode_word == ident)
			printf("%s,\t", table_variable[fexp[temp1].arg2.value_word]);
		else
		{
			if (fexp[temp1].arg2.typeCode_word == tempsy)
				printf("T%d,\t", fexp[temp1].arg2.value_word);
			else
			{
				if (fexp[temp1].arg2.typeCode_word == intconst)
					printf("%d,\t", fexp[temp1].arg2.value_word);
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
	printf("\n\n程序总共%d行，产生了%d个二元式!\n", lineNum_sourceCode, count_buf);
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
	lexical_analyse();	// 词法分析

	disp3();			// 显示变量名
	stack[sp].value_word = 0;
	stack[sp].typeCode_word = -1;	// 初始化状态栈
	stack1[sp1] = 0;	// 初始化状态栈1
	oth.typeCode_word = -1;
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
