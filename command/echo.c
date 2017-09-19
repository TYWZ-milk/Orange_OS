#include "stdio.h"

unsigned int randSeed = 0xEFABDCFA;

unsigned int curl_rand() {
	unsigned int r;
	r = randSeed = randSeed * 1103515245 + 12345;
	return (r << 16) | (r >> 16) & 0xFFFF;
}

int initMatrix(int mat[][4]);

int reachSuccess(int mat[][4]);
int insertNew(int mat[][4]);
void printMatrix(int mat[][4]);

void ClearArr(char *arr, int length);

int goLeft(int mat[][4]);
int goRight(int mat[][4]);
int goUp(int mat[][4]);
int goDown(int mat[][4]);

int main(){
    // int mat[16] = {0};
    int mat[4][4];
    for(int i = 0; i < 4; i++) {
    	for(int j = 0; j < 4; j++) {
    		mat[i][j] = 0;
    	}
    }
    int state = 0;
    char keys[128];
    printf("This is 2048 game\n");
    printf("HOW TO PLAY\n");
    printf("Use your arrow keys to move the tiles. When two tiles with the same number touch, they merge into one!\n");
    printf("You can start now!\n");
    while(1){
        printf("Initialize matrix:\n");
        initMatrix(mat);

        while(1){
            printf("direction:(w a s d)\n");
            ClearArr(keys, 128);
            int r = read(0, keys, 128);

            if (strcmp(keys, "a") == 0)
            {
                state = goLeft(mat);
            }
            else if (strcmp(keys, "s") == 0)
            {
                state = goDown(mat);
            }
            else if (strcmp(keys, "w") == 0)
            {
                state = goUp(mat);
            }
            else if (strcmp(keys, "d") == 0)
            {
                state = goRight(mat);
            }
            else if (strcmp(keys, "q") == 0)
            {
                return 0;
            }
            else
            {
                printf("Input Invalid, Please retry\n");
                continue;
            }

            if(state==0){
                printf("Problem occured, please retry\n");
                continue;
            }

            // reach 2048
            if(reachSuccess(mat)){
                printf("You win\n");
                break;
            }

            // insert a new num and check user failed or not
            if(!insertNew(mat)){
                printf("You lose\n");
                break;
            }

            // show result
            printMatrix(mat);
        }

        printf("Another one?(y or n):");

        ClearArr(keys, 128);
        int r = read(0, keys, 128);
        if (strcmp(keys, "n"))
        {
            break;
        }
    }
    return 0;
}

int initMatrix(int mat[][4])
{
    int i, j;
    //给一个随机数
    while(insertNew(mat) == 0) {
    }
    while(insertNew(mat) == 0) {
    }
    printMatrix(mat);
    return 0;
}

int reachSuccess(int mat[][4]){
    int i, j;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            if(mat[i][j] == 2048)
                return 1;
        }
    }
    return 0;
}

int insertNew(int mat[][4]){
    int r = curl_rand()%4;
    int c = curl_rand()%4;
    if(mat[r][c] == 0) {
    	return 0;
    }
    else {
    	mat[r][c] = 2<<(curl_rand()%2);
    	return 1;
    }
}

void printMatrix(int mat[][4]){
    int i, j;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            //这里需要规格化
            printf("%4d", mat[i][j]);
        }
        printf("\n");
    }
}

void ClearArr(char *arr, int length)
{
    int i;
    for (i = 0; i < length; i++)
        arr[i] = 0;
}

int goLeft(int mat[][4]){
    printf("Left\n");

    int i,j;
    int flag=0;
    int k=0,temp[4]={0},last=0;
    for(i=0;i<4;i++){
        memset(temp,0,sizeof(int)*4);
        for(j=0,k=0,last=0;j<4;j++){
            if(mat[i][j]!=0){
                temp[k]=mat[i][j];
                mat[i][j]=0;
                last=j+1;
                k++;
            }
        }
        if(k<last) flag=1;
        for(j=0;j<3;j++){
            if(temp[j]>0&&temp[j]==temp[j+1]){
                temp[j]+=temp[j];
                temp[j+1]=0;
                flag=1;
            }
        }
        for(j=0,k=0;k<4;k++){
            if(temp[k]!=0){
                mat[i][j]=temp[k];
                j++;
            }
        }
    }
    return flag;
}

int goRight(int mat[][4]){
    printf("Right\n");

    int i,j;
    int flag=0;
    int k=0,temp[4]={0},last=0;
    for(i=0;i<4;i++){
        memset(temp,0,sizeof(int)*4);
        for(j=3,k=3,last=3;j>=0;j--){
            if(mat[i][j]!=0){
                temp[k]=mat[i][j];
                mat[i][j]=0;
                last=j-1;
                k--;
            }
        }
        if(k>last) flag=1;
        for(j=3;j>=0;j--){
            if(temp[j]>0&&temp[j]==temp[j+1]){
                temp[j]+=temp[j];
                temp[j+1]=0;
                flag=1;
            }
        }
        for(j=3,k=3;k>=0;k--){
            if(temp[k]!=0){
                mat[i][j]=temp[k];
                j--;
            }
        }
    }
    return flag;
}

int goUp(int mat[][4]){
    printf("Up\n");

    int i,j;
    int flag=0;

    int k=0,temp[4]={0},last=0;
    for(i=0;i<4;i++){
        memset(temp,0,sizeof(int)*4);
        for(j=0,k=0,last=0;j<4;j++){
            if(mat[j][i]!=0){
                temp[k]=mat[j][i];
                mat[j][i]=0;
                last=j+1;
                k++;
            }
        }
        if(k<last) flag=1;
        for(j=0;j<3;j++){
            if(temp[j]>0&&temp[j]==temp[j+1]){
                temp[j]+=temp[j];
                temp[j+1]=0;
                flag=1;
            }
        }
        for(j=0,k=0;k<4;k++){
            if(temp[k]!=0){
                mat[j][i]=temp[k];
                j++;
            }
        }
    }
    return flag;
}

int goDown(int mat[][4]){
    printf("Down\n");

    int i,j;
    int flag=0;
    int k=0,temp[4]={0},last=0;
    for(j=0;j<4;j++){
        memset(temp,0,sizeof(int)*4);
        for(i=3,k=3,last=3;i>=0;i--){
            if(mat[i][j]!=0){
                temp[k]=mat[i][j];
                mat[i][j]=0;
                last=i-1;
                k--;
            }
        }
        if(k>last) flag=1;
        for(i=3;i>0;i--){
            if(temp[i]>0&&temp[i]==temp[i-1]){
                temp[i]+=temp[i];
                temp[i-1]=0;
                flag=1;
            }
        }
        for(i=3,k=3;k>=0;k--){
            if(temp[k]!=0){
                mat[i][j]=temp[k];
                i--;
            }
        }
    }
    return flag;
}

