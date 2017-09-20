#include <stdio.h>
#define chartonumber(x) (x-'0')
int main(int argc, char * argv[]){
	while(1)
	{
	printf("              ***************************************************\n");
	printf("              *                     Game                        *\n");
	printf("              ***************************************************\n");
	printf("              *             Please choice the game              *\n");
	printf("              *                   1.Gobang                      *\n");
	printf("              *                   2.calculator                  *\n");
	printf("              *                   3.ic-tac-toe                  *\n");
	printf("              *                   4.Guess number                *\n");
	printf("              *                   5.N queen game                *\n");
	printf("              *                 Enter e to quit                 *\n");
	printf("              ***************************************************\n\n");
	
	char bur[128];
	read(0, bur, 128);
	if('1' == bur[0])
		Gobang();
	if('2' == bur[0])
		calculator();
	if('3' == bur[0])
		ic();
	if('4' == bur[0])
		guess();
	if('5' == bur[0])
		queen();
	if('e' == bur[0])
		break;
	printf("\n\n\n\n\n");
	}
	return 0;
}
/****************************N queen game*******************************/

void huisu(int l);

int jc(int l, int i);

int n, h[100];

int x;

void queen()

{

		printf("              ***************************************************\n");
		printf("              *               N Queen game                      *\n");
		printf("              ***************************************************\n");
		printf("              *                                                 *\n");
		printf("              *              Enter e to quit                    *\n");
		printf("              *                                                 *\n");
		printf("              ***************************************************\n\n");


		printf("N=");
		char bur[128];
		read(0, bur, 128);
		n = chartonumber(bur[0]);

		x = 0;

		huisu(1);

		printf("There are %d stacking methods\n", x);
	

}

void huisu(int l)

{

	int i, j;

	if (l == n + 1)

	{

		x = x + 1;

		printf("stacking methods are:\n", x);

		for (i = 1; i <= n; i++)

			printf("%d", h[i]);

		printf("\n");

	}

	for (i = 1; i <= n; i++)

	{

		h[l] = i;

		if (jc(l, i) != 1)

			huisu(l + 1);

	}

}

int jc(int l, int i)

{

	int k;

	for (k = 1; k<l; k++)

		if ((l - k == h[k] - i) || i == h[k])

			return 1;

	return 0;

}
/***************************guess number********************************/
int my_atoi(const char *s)
{
	int num, i;
	char ch;
	num = 0;
	for (i = 0; i < 3; i++)
	{
		ch = s[i];
		if (ch < '0' || ch > '9')
			break;
		num = num*10 + (ch - '0');
	}
	return num;
}
void guess()
{
	printf("              ***************************************************\n");
	printf("              *               Guess number                      *\n");
	printf("              ***************************************************\n");
	printf("              *                                                 *\n");
	printf("              *              Enter e to quit                    *\n");
	printf("              *                                                 *\n");
	printf("              ***************************************************\n\n");
	int stop = 0;
	int a,b;
	char c;
	a = 543;
	printf("I have a number between 1 and 999.\nCan you guess my number?\nPlease type your first guess.\n");
	char bufr[128];
	read(0, bufr, 128);
	b=my_atoi(bufr);
	while (b!= -1)
	{
		if (b == a)
		{
			printf("Excellent! You guessed the number!\nWould you like to play again(y or n)?");
			char bur[128];
			read(0, bur, 128);
			switch (bur[0]) {
			case 'y':
				printf("I have a number between 1 and 1000.\nCan you guess my number?\nPlease type your first guess.\n");
				read(0, bufr, 128);
				break;
			case 'n':
				stop = 1;
				break;
			}
			if (stop == 1)
				break;
		}
		while (b<a)
		{
			printf("Too low.Try again.\n");
			read(0, bufr, 128);
			b=my_atoi(bufr);
		}
		while (b>a)
		{
			printf("Too high.Try again.\n");
			read(0, bufr, 128);
			b=my_atoi(bufr);
		}
		if(bufr[0]=='e')
			break;
	}
}
/***************************tic-tac-toe*********************************/


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
	printf("              \n"); printf("\n");
	printf("               %c | %c | %c\n", a[1], a[2], a[3]);
	printf("               --------------\n");
	printf("               %c | %c | %c\n", a[4], a[5], a[6]);
	printf("               --------------\n");
	printf("               %c | %c | %c\n", a[7], a[8], a[9]);
	printf("              \n"); printf("\n");
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
void ic()
{

	printf("              ***************************************************\n");
	printf("              *               ic-tac-toe                        *\n");
	printf("              ***************************************************\n");
	printf("              *                                                 *\n");
	printf("              *              Enter e to quit                    *\n");
	printf("              *                                                 *\n");
	printf("              ***************************************************\n\n");
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
/***************************calculator*********************************/
void calculator()

{
	printf("              ***************************************************\n");
	printf("              *               Calculator                        *\n");
	printf("              ***************************************************\n");
	printf("              *                                                 *\n");
	printf("              *              Enter e to quit                    *\n");
	printf("              *                                                 *\n");
	printf("              ***************************************************\n\n");
	while(1){	
	char result;

	char bufr[128];
	read(0, bufr, 128);


	switch(bufr[1])

	{

    	case '+':result=chartonumber(bufr[0])+chartonumber(bufr[2]);break;

	case '-':result=chartonumber(bufr[0])-chartonumber(bufr[2]);break;

	case '*':result=chartonumber(bufr[0])*chartonumber(bufr[2]);break;

	case '/':result=chartonumber(bufr[0])/chartonumber(bufr[2]);break;
	

	}
	if(bufr[0]=='e')
		break;
	else
		printf("%d %c %d = %d\n",chartonumber(bufr[0]),bufr[1],chartonumber(bufr[2]),result);
	}

}
/*******************GObang Game********************/

#define N	15

int chessboard[N + 1][N + 1] = { 0 };

//用来记录轮到玩家1还是玩家2
int whoseTurn = 0;

void initGame(void);
void printChessboard(void);
void playChess(void);
int judge(int, int);
int stop=0;
int Gobang()
{
	//初始化游戏
	initGame();
	whoseTurn = 0;
	stop=0;
	chessboard[N + 1][N + 1] =0;
	printf("Player1:");
	while (1)
	{
			//每次循环自增1，实现玩家轮流下子
		whoseTurn++;

		playChess();
		if(stop==1)
			break;

	}
	stop=0;

	return 0;
}

void initGame(void)
{

	
	printf("              ***************************************************\n");
	printf("              *               Gobang Game                       *\n");
	printf("              ***************************************************\n");
	printf("              *                                                 *\n");
	printf("              *              Enter e to quit                    *\n");
	printf("              *                                                 *\n");
	printf("              ***************************************************\n\n");
	printChessboard();
	
}

void printChessboard(void)
{
	int i, j;

	for (i = 0; i <= N; i++)
	{
		for (j = 0; j <= N; j++)
		{
			if (0 == i)
				printf("%3d", j);
			else if (j == 0)
				printf("%3d", i);
			else if (1 == chessboard[i][j])
				printf("  !");
			else if (2 == chessboard[i][j])
				printf("  &");
			else
				printf("  *");
		}
		printf("\n");
	}
}

void playChess(void)
{
	int i, j;
	char bufr[128];
	read(0, bufr, 128);

	if (1 == whoseTurn % 2)
	{
		
		i = chartonumber(bufr[0]);
		j = chartonumber(bufr[2]);
		chessboard[i][j] = 1;
	}
	if (0 == whoseTurn % 2)
	{
		
		i = chartonumber(bufr[0]);
		j = chartonumber(bufr[2]);
		chessboard[i][j] = 2;
	}

	if(bufr[0]=='e')
		stop=1;
	else{
		printChessboard();
		if (1 == whoseTurn % 2)
			printf("Player2:");
		else
			printf("Player1:");
	}
	if (judge(i, j))
	{
		if (1 == whoseTurn % 2)
			printf("Player1 win!\n");
		else{
			printf("Player2 win!\n");
		}
		stop=1;
	}
    
}

int judge(int x, int y)
{
	int i, j;
	int t = 2 - whoseTurn % 2;

	for (i = x - 4, j = y; i <= x; i++)
	{
		if (i >= 1 && i <= N - 4 && t == chessboard[i][j] && t == chessboard[i + 1][j] && t == chessboard[i + 2][j] && t == chessboard[i + 3][j] && t == chessboard[i + 4][j])
			return 1;
	}
	for (i = x, j = y - 4; j <= y; j++)
	{
		if (j >= 1 && j <= N - 4 && t == chessboard[i][j] && t == chessboard[i][j + 1] && t == chessboard[i][j + 1] && t == chessboard[i][j + 3] && t == chessboard[i][j + 4])
			return 1;
	}
	for (i = x - 4, j = y - 4; i <= x, j <= y; i++, j++)
	{
		if (i >= 1 && i <= N - 4 && j >= 1 && j <= N - 4 && t == chessboard[i][j] && t == chessboard[i + 1][j + 1] && t == chessboard[i + 2][j + 2] && t == chessboard[i + 3][j + 3] && t == chessboard[i + 4][j + 4])
			return 1;
	}
	for (i = x + 4, j = y - 4; i >= 1, j <= y; i--, j++)
	{
		if (i >= 1 && i <= N - 4 && j >= 1 && j <= N - 4 && t == chessboard[i][j] && t == chessboard[i - 1][j + 1] && t == chessboard[i - 2][j + 2] && t == chessboard[i - 3][j + 3] && t == chessboard[i - 4][j + 4])
			return 1;
	}

	return 0;
}
