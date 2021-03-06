#include"config.h"
#include"lexical_analysis.h"
#include"parsing.h"

#include<iostream>
using namespace std;

two_exp n = {};				// 当前字符
two_exp stack[1000] = {};	// 语法分析加工处理使用的符号栈
int word = 0;				// 查找LR分析表时状态栈的栈顶指针
int stack1[100] = {};		// 定义状态栈
int sp1 = 0;				// 定义状态栈1的栈顶指针
two_exp oth = {};			// 四元式中的空白位置
int sign = 0;				// sign=1，表达式为赋值语句  sign=2，表达式为布尔表达式
two_exp ibuf[100] = {};		// 缓冲区
two_exp sstack[100] = {};	// 符号栈
int ssp = 0;				// 指向sstack[100]
two_exp n1 = {};			// 当前表达式中的字符

int lr1 = 0;				// 扫描LR分析表2或3过程中保存的当前状态值
two_exp E = {};				// 非终结符

int newt = 0;				// 临时变量

fourexp fexp[200] = {};
int nxq = 100;				// nxq指向下一个形成的四元式的地址

int pointmark = -1;

ll labelmark[10] = {};

ntab ntab2[200] = {};
int label = 0;				// 指向ntab2的指针

int lr = 0;					// 扫描LR分析表1过程中保存的当前状态值

int labeltemp[10] = {};
int pointtemp = -1;

int num = 0;				// 缓冲区指针

/********************从二元式读入一个符号*********************/
void readnu() {
	if (pointer_bufLex->wordCode >= 0) {
		n.wordCode = pointer_bufLex->wordCode;
		n.wordSubCode = pointer_bufLex->wordSubCode;
		pointer_bufLex++;
	}
}

/************测试字符是否为表达式中的值(不包括":")****************/
int test(int value) {
	switch (value) {
	case intconst:
	case word_variable:
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

/*********************************************************/
int change1(int chan) {
	switch (chan) {
	case word_variable:
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


/***********************中间变量的生成*********************/
int newtemp() {
	newt++;
	return newt;
}

/***********************生成四元式**************************/
int gen(const char op1[], struct two_exp arg11, struct two_exp arg22, int result1) {
	strcpy_s(fexp[nxq].op, op1);
	fexp[nxq].arg1.wordCode = arg11.wordCode;
	fexp[nxq].arg1.wordSubCode = arg11.wordSubCode;
	fexp[nxq].arg2.wordCode = arg22.wordCode;
	fexp[nxq].arg2.wordSubCode = arg22.wordSubCode;
	fexp[nxq].result = result1;
	nxq++;
	return nxq - 1;
}


/*********************赋值语句和算术表达式的分析**********************/
int lrparse1(int num) {
	lr1 = action1[stack1[sp1]][change1(n1.wordCode)];
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
		if (n1.wordCode != tempsy) {
			ssp++;
			num++;
			sstack[ssp].wordCode = n1.wordCode;//将变量名压栈
			sstack[ssp].wordSubCode = n1.wordSubCode;//将变量名地址压栈
		}
		n1.wordCode = ibuf[num].wordCode;
		n1.wordSubCode = ibuf[num].wordSubCode;
		lrparse1(num);
	}
	/* 归约状态 */
	if ((lr1 >= 100) && (lr1 < 105)) {
		switch (lr1) {
		case 100:break;// S'->E
		case 101:
			E.wordSubCode = newtemp();// E->E+E
			gen("+", sstack[ssp - 2], sstack[ssp], E.wordSubCode + 100);
			ssp = ssp - 2;
			sstack[ssp].wordCode = tempsy;
			sstack[ssp].wordSubCode = E.wordSubCode;
			sp1 = sp1 - 3;//产生式右部长度为3，故归约后栈指针减3
			break;
		case 102:
			E.wordSubCode = newtemp();// E->E*E
			gen("*", sstack[ssp - 2], sstack[ssp], E.wordSubCode + 100);
			ssp = ssp - 2;
			sstack[ssp].wordCode = tempsy;
			sstack[ssp].wordSubCode = E.wordSubCode;
			sp1 = sp1 - 3;
			break;
		case 103:
			E.wordSubCode = sstack[ssp - 1].wordSubCode; //E->(E)
			ssp = ssp - 2;
			sstack[ssp].wordCode = tempsy;
			sstack[ssp].wordSubCode = E.wordSubCode;
			sp1 = sp1 - 3;
			break;
		case 104:
			E.wordSubCode = sstack[ssp].wordSubCode;//E->i
			sp1--;
			break;
		}
		n1.wordCode = tempsy;//归约后为非终结符
		n1.wordSubCode = E.wordSubCode;
		lrparse1(num);
	}
	if ((lr1 == ACC) && (stack1[sp1] == 1))//归约A->i:=E
	{
		gen(":=", sstack[ssp], oth, ibuf[0].wordSubCode);
		ssp = ssp - 3;
		sp1 = sp1 - 3;
	}
}


int change2(int chan) {
	switch (chan) {
	case word_variable:
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


// 返填函数
void backpatch(int p, int t) {
	int tempq;
	int q;
	q = p;
	while (q != 0) {
		tempq = fexp[q].result;
		fexp[q].result = t;
		q = tempq;
	}
}


/***********************布尔表达式的匹配**********************/
int merg(int p1, int p2) {
	int p;
	if (p2 == 0) return p1;
	else {
		p = p2;
		while (fexp[p].result != 0)
			p = fexp[p].result;
		fexp[p].result = p1;
		return p2;
	}
}

/***********************布尔表达式的分析**********************/
int lrparse2(int num) {
	int templabel;
	lr1 = action2[stack1[sp1]][change2(n1.wordCode)];
	if (lr1 == -1) {
		if (sign == 2) printf("\nwhile语句出错\n");
		if (sign == 3) printf("\nif语句出错\n");
		// getchar();
		cout << endl;
		exit(0);
	}
	// 移进状态
	if ((lr1 < 16) && (lr1 >= 0)) {
		sp1++;
		stack1[sp1] = lr1;
		ssp++;
		sstack[sp1].wordCode = n1.wordCode;
		sstack[ssp].wordSubCode = n1.wordSubCode;
		if ((n1.wordCode != tempsy) && (n1.wordCode != EA) && (n1.wordCode != E0))
			num++;
		n1.wordCode = ibuf[num].wordCode;
		n1.wordSubCode = ibuf[num].wordSubCode;
		lrparse2(num);
	}
	// 归约状态
	if ((lr1 >= 100) && (lr1 < 109)) {
		switch (lr1) {
		case 100:break;//S'->B
		case 101://B->i
			ntab2[label].tc = nxq;
			ntab2[label].fc = nxq + 1;
			gen("jnz", sstack[ssp], oth, 0);
			gen("j", oth, oth, 0);
			sp1--;
			ssp--;
			label++;
			n1.wordCode = tempsy;
			break;
		case 102://B->i rop i
			ntab2[label].tc = nxq;
			ntab2[label].fc = nxq + 1;
			switch (sstack[ssp - 1].wordSubCode) {
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
			n1.wordCode = tempsy;
			break;
		case 103://B->(B)
			label = label - 1;
			ssp = ssp - 3;
			sp1 = sp1 - 3;
			label++;
			n1.wordCode = tempsy;
			break;
		case 104://B->not B
			label = label - 1;
			templabel = ntab2[label].tc;
			ntab2[label].tc = ntab2[label].fc;
			ntab2[label].fc = templabel;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.wordCode = tempsy;
			break;
		case 105://A->B and
			backpatch(ntab2[label - 1].tc, nxq);
			label = label - 1;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.wordCode = EA;
			break;
		case 106://B->AB
			label = label - 2;
			ntab2[label].tc = ntab2[label + 1].tc;
			ntab2[label].fc = merg(ntab2[label].fc, ntab2[label + 1].fc);
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.wordCode = tempsy;
			break;
		case 107://O->B or
			backpatch(ntab2[label - 1].fc, nxq);
			label = label - 1;
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.wordCode = E0;
			break;
		case 108://B->0B
			label = label - 2;
			ntab2[label].fc = ntab2[label + 1].fc;
			ntab2[label].tc = merg(ntab2[label].tc, ntab2[label + 1].tc);
			ssp = ssp - 2;
			sp1 = sp1 - 2;
			label++;
			n1.wordCode = tempsy;
			break;
		}
		lrparse2(num);
	}
	if (lr1 == ACC) return 1;
}



/*************程序语句处理****************************************/
int lrparse() {
	int i1 = 0;
	int num = 0;//指向表达式缓冲区
	if (test(n.wordCode)) {
		if (stack[word].wordCode == sy_while) sign = 2;
		else {
			if (stack[word].wordCode == sy_if) sign = 3;
			else sign = 1;
		}
		do {
			ibuf[i1].wordCode = n.wordCode;
			ibuf[i1].wordSubCode = n.wordSubCode;
			readnu();
			i1++;
		} while (test(n.wordCode));//把算术或布尔表达式放入缓冲区
		ibuf[i1].wordCode = jinghao;
		pointer_bufLex--;//词法分析缓冲区指针减1
		sstack[0].wordCode = jinghao;
		ssp = 0;//符号栈初始化
		//赋值语句处理
		if (sign == 1) {
			sp1 = 0;
			stack1[sp1] = 0;//状态栈1初始化
			num = 2;
			n1.wordCode = ibuf[num].wordCode;
			n1.wordSubCode = ibuf[num].wordSubCode;
			lrparse1(num);//处理赋值语句
			n.wordCode = a;//当前文法符号置为a（赋值语句）
		}
		//布尔表达式处理
		if ((sign == 2) || (sign == 3)) {
			pointmark++;
			labelmark[pointmark].nxq1 = nxq;
			sp1 = 0;
			stack1[sp1] = 0;
			num = 0;

			n1.wordCode = ibuf[num].wordCode;
			n1.wordSubCode = ibuf[num].wordSubCode;
			lrparse2(num);
			labelmark[pointmark].tc1 = ntab2[label - 1].tc;
			labelmark[pointmark].fc1 = ntab2[label - 1].fc;
			backpatch(labelmark[pointmark].tc1, nxq);
			n.wordCode = e;//当前万能法符号置e（布尔表达式）
		}
	}
	lr = action[stack[word].wordSubCode][n.wordCode];
	//输出状态栈信息
	printf("stack[%d]=%d\t\tn=%d\t\tlr=%d\n", word, stack[word].wordSubCode, n.wordCode, lr);
	//移进状态
	if ((lr < 19) && (lr >= 0)) {
		word++;
		stack[word].wordSubCode = lr;
		stack[word].wordCode = n.wordCode;
		readnu();
		lrparse();
	}
	//归约状态
	if ((lr <= 106) && (lr >= 100)) {
		switch (lr) {
		case 100:break;//S'->S
		case 101://S->if e then s else s
			printf("\nS->if e then S else S 归约\n");
			word = word - 6;
			n.wordCode = S;
			fexp[labeltemp[pointtemp]].result = nxq;
			pointtemp--;
			if (stack[word].wordCode == sy_then) {
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			pointmark--;
			if (stack[word].wordCode == sy_do) {
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 102:printf("\nS->while e do S 归约\n");
			word = word - 4;
			n.wordCode = S;
			pointmark--;
			if (stack[word].wordCode == sy_do) {
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			if (stack[word].wordCode == sy_then) {
				gen("j", oth, oth, 0);
				fexp[labelmark[pointmark].fc1].result = nxq;
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			break;
		case 103:printf("\nS->begin L end 归约\n");
			word = word - 3;
			n.wordCode = S;
			if (stack[word].wordCode == sy_then) {
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			if (stack[word].wordCode == sy_do) {
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 104:printf("\nS->a 归约\n");
			word = word - 1;
			n.wordCode = S;
			if (stack[word].wordCode == sy_then) {
				gen("j", oth, oth, 0);
				backpatch(labelmark[pointmark].fc1, nxq);
				pointtemp++;
				labeltemp[pointtemp] = nxq - 1;
			}
			if (stack[word].wordCode == sy_do) {
				gen("j", oth, oth, labelmark[pointmark].nxq1);
				backpatch(labelmark[pointmark].fc1, nxq);
			}
			break;
		case 105:printf("\nL->S 归约\n");
			word = word - 1;
			n.wordCode = L;
			break;
		case 106:printf("\nL->S L 归约\n");
			word = word - 3;
			n.wordCode = L;
			break;
		}
		pointer_bufLex--;
		lrparse();
	}
	if (lr == ACC)  return ACC;
}


/****************************四元式分析结果**********************************/
void disp2() {
	int temp1 = 100;
	printf("\n**************四元式分析结果********************\n");
	for (temp1 = 100; temp1 < nxq; temp1++) {
		if (temp1 > 100)
			printf("\n");
		printf("%d\t", temp1);
		printf("（%s,\t", fexp[temp1].op);
		if (fexp[temp1].arg1.wordCode == word_variable)//为变量
			printf("%s,\t", table_variable[fexp[temp1].arg1.wordSubCode]);
		else {
			if (fexp[temp1].arg1.wordCode == tempsy)//为临时变量
				printf("T%d,\t", fexp[temp1].arg1.wordSubCode);
			else {
				if (fexp[temp1].arg1.wordCode == intconst)//为整常量
					printf("%d,\t", fexp[temp1].arg1.wordSubCode);
				else//否则
					printf(",\t");
			}
		}
		if (fexp[temp1].arg2.wordCode == word_variable)
			printf("%s,\t", table_variable[fexp[temp1].arg2.wordSubCode]);
		else {
			if (fexp[temp1].arg2.wordCode == tempsy)
				printf("T%d,\t", fexp[temp1].arg2.wordSubCode);
			else {
				if (fexp[temp1].arg2.wordCode == intconst)
					printf("%d,\t", fexp[temp1].arg2.wordSubCode);
				else printf(",\t");
			}
		}
		if (fexp[temp1].op[0] != 'j') {
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



void parsing_global() {
	stack[word].wordSubCode = 0;
	stack[word].wordCode = -1;		// 初始化状态栈
	stack1[sp1] = 0;				// 初始化状态栈1
	oth.wordCode = -1;
	printf("\n*************状态栈加工过程及归约顺序*************\n");
	readnu();		// 从二元式读一个字符
	lrparse();
	cout << endl;
	//四元式分析
	disp2();
	cout << endl;
}
