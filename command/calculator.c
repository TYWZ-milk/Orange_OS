/***************************calculator*********************************/
#include<stdio.h>
void main()

{
	printf("***************************************************\n");
	printf("*               Calculator                        *\n");
	printf("***************************************************\n");
	printf("*                                                 *\n");
	printf("*              Enter e to quit                    *\n");
	printf("*                                                 *\n");
	printf("***************************************************\n\n");
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
