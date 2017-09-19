/***************************tic-tac-toe*********************************/

#include<stdio.h>
typedef char chess[10];
typedef int temparr[10];
chess arr;
temparr brr;
int number, suc, n3, c3, n2, c2, n1, c1;
char ch;
void inarrdata(chess a)
{
	a[1] = '1'; a[2] = '2'; a[3] = '3';
	a[4] = '4'; a[5] = '5'; a[6] = '6';
	a[7] = '7'; a[8] = '8'; a[9] = '9';
}
void display(chess a)
{
	printf("\n"); printf("\n");
	printf(" %c | %c | %c\n", a[1], a[2], a[3]);
	printf(" --------------\n");
	printf(" %c | %c | %c\n", a[4], a[5], a[6]);
	printf(" --------------\n");
	printf(" %c | %c | %c\n", a[7], a[8], a[9]);
	printf("\n"); printf("\n");
}
int arrfull()
{
	int i;
	int arrf = 0;
	for (i = 1; i <= 9; i++)
		if (i == arr[i] - 48)
			arrf = 1;
	return arrf;
}
void cn(int line)
{
	switch (line)
	{
	case 0:c3 = c3 + 1; break;
	case 1:n2 = n2 + 1; break;
	case 2:c2 = c2 + 1; break;
	case 3:n1 = n1 + 1; break;
	case 4:c1 = c1 + 1; break;
	case 5:n3 = n3 + 1; break;
	}
}
int linenum(char a, char b, char c)
{
	int ln = 6;
	if ((a == 'X') && (b == 'X') && (c == 'X'))
		ln = 0;
	if (((a == 'O') && (b == 'O') && (c != 'O')) || ((a == 'O') && (b != 'O') && (c == 'O')) || ((a != 'O') && (b == 'O') && (c == 'O')))
		ln = 1;
	if (((a == 'X') && (b == 'X') && (c != 'X')) || ((a == 'X') && (b != 'X') && (c == 'X')) || ((a != 'X') && (b == 'X') && (c == 'X')))
		ln = 2;
	if (((a == 'O') && (b != 'O') && (c != 'O')) || ((a != 'O') && (b == 'O') && (c != 'O')) || ((a != 'O') && (b != 'O') && (c == 'O')))
		ln = 3;
	if (((a == 'X') && (b != 'X') && (c != 'x')) || ((a != 'X') && (b == 'X') && (c != 'X')) || ((a != 'X') && (b != 'X') && (c == 'X')))
		ln = 4;
	if ((a == 'O') && (b == 'O') && (c == 'O'))
		ln = 5;
	return ln;
}
int maxbrr(int *br)
{
	int temp, i, mb;
	temp = -888;
	for (i = 1; i <= 9; i++)
	{
		if (temp <= br[i])
		{
			temp = br[i];
			mb = i;
		}
	}
	return mb;
}
void manstep() //人走棋处理模块
{
	int j;
	display(arr);
	if (arrfull()) //如果棋盘上还有下棋的位置，给人走一步棋
	{
		printf("Which step are you going to take? please enter a number(1--9):");
		char bur[128];
		read(0, bur, 128);
		j = chartonumber(bur[0]);

		arr[j] = 'O';
		c3 = 0; n2 = 0; c2 = 0; n1 = 0; c1 = 0;
		number = linenum(arr[1], arr[2], arr[3]); cn(number);
		number = linenum(arr[4], arr[5], arr[6]); cn(number);
		number = linenum(arr[7], arr[8], arr[9]); cn(number);
		number = linenum(arr[1], arr[4], arr[7]); cn(number);
		number = linenum(arr[2], arr[5], arr[8]); cn(number);
		number = linenum(arr[3], arr[6], arr[9]); cn(number);
		number = linenum(arr[1], arr[5], arr[9]); cn(number);
		number = linenum(arr[3], arr[5], arr[7]); cn(number);
		if (n3 == 0) //你赢了
		{
			display(arr);
			printf("\n");
			printf("You win!!!\n");
		
			suc = 0;
		}
	}
}
void computerstep() //计算机走棋处理模块
{
	int i;
	if (arrfull()) //如果棋盘上还有可下棋的位置，则计算机走棋
	{
		for (i = 1; i <= 9; i++) //对每一步可走的棋进行计算
		{
			if (i == arr[i] - 48)
			{
				c3 = 0; n2 = 0; c2 = 0; n1 = 0; c1 = 0;
				arr[i] = 'X';
				number = linenum(arr[1], arr[2], arr[3]); cn(number);
				number = linenum(arr[4], arr[5], arr[6]); cn(number);
				number = linenum(arr[7], arr[8], arr[9]); cn(number);
				number = linenum(arr[1], arr[4], arr[7]); cn(number);
				number = linenum(arr[2], arr[5], arr[8]); cn(number);
				number = linenum(arr[3], arr[6], arr[9]); cn(number);
				number = linenum(arr[1], arr[5], arr[9]); cn(number);
				number = linenum(arr[3], arr[5], arr[7]); cn(number);
				brr[i] = (128 * c3 - 63 * n2 + 31 * c2 - 15 * n1 + 7 * c1); //计算此步权值
				arr[i] = i + 48;
			}
			else
				brr[i] = -999;
		}
		arr[maxbrr(brr)] = 'X'; //确定计算机走哪一步，权值最大的一步
		c3 = 0; n2 = 0; c2 = 0; n1 = 0; c1 = 0;
		number = linenum(arr[1], arr[2], arr[3]); cn(number);
		number = linenum(arr[4], arr[5], arr[6]); cn(number);
		number = linenum(arr[7], arr[8], arr[9]); cn(number);
		number = linenum(arr[1], arr[4], arr[7]); cn(number);
		number = linenum(arr[2], arr[5], arr[8]); cn(number);
		number = linenum(arr[3], arr[6], arr[9]); cn(number);
		number = linenum(arr[1], arr[5], arr[9]); cn(number);
		number = linenum(arr[3], arr[5], arr[7]); cn(number);
		if (c3 != 0) //计算机已赢
		{
			display(arr);
			printf("\n");
			printf("PC win!!!\n");

			suc = 0;
		}
	}
	else
		suc = 0;

}
void main()
{

	printf("***************************************************\n");
	printf("*               ic-tac-toe                        *\n");
	printf("***************************************************\n");
	printf("*                                                 *\n");
	printf("*              Enter e to quit                    *\n");
	printf("*                                                 *\n");
	printf("***************************************************\n\n");
	inarrdata(arr); //棋盘坐标编号
	display(arr); //显示初始棋盘
	suc = 1;
	printf("Do you want to go first?(y/n)");
	char bufr[128];
	read(0, bufr, 128);
	if ((bufr[0] == 'y') || (bufr[0] == 'Y')) //输入Y，表示人先走棋
	{
		while (suc)
		{
			manstep();
			computerstep();
		}
		display(arr);
	}
	else //计算机先走棋
	{
		while (suc)
		{
			computerstep();
			if (suc)
				manstep();
		}
	}
	if(!arrfull())
		printf("\nNo winer !\n");
}
