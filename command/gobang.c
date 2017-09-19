/*******************GObang Game********************/

#define N	15
#include<stdio.h>
int chessboard[N + 1][N + 1] = { 0 };

//用来记录轮到玩家1还是玩家2
int whoseTurn = 0;

void initGame(void);
void printChessboard(void);
void playChess(void);
int judge(int, int);
int stop=0;
int main()
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

	
	printf("***************************************************\n");
	printf("*               Gobang Game                       *\n");
	printf("***************************************************\n");
	printf("*                                                 *\n");
	printf("*              Enter e to quit                    *\n");
	printf("*                                                 *\n");
	printf("***************************************************\n\n");
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
