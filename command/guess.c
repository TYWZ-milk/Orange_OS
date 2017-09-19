/***************************guess number********************************/
#include<stdio.h>
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
void main()
{
	printf("***************************************************\n");
	printf("*               Guess number                      *\n");
	printf("***************************************************\n");
	printf("*                                                 *\n");
	printf("*              Enter e to quit                    *\n");
	printf("*                                                 *\n");
	printf("***************************************************\n\n");
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
