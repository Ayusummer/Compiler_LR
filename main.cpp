/* 导入配置文件 */
#include"config.h"
#include"lexical_analysis.h"
#include"parsing.h"

/* 导入头文件 */
#include "stdio.h"
#include "string.h"
#include<Windows.h>
#include<iostream>
using namespace std;


/***********************主函数***************************/
int main() {
	lexical_analyse_global();	// 词法分析
	parsing_global();			// 语法分析
	printf("**************************************************************");
	printf("\n程序运行结束!\n");
	return 0;
	system("pause");
}
