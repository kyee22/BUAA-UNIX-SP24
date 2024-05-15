#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<string.h>
#define max(a,b) (((a)<(b))?(b):(a))
#define min(a,b) (((a)<(b))?(a):(b))
#define LL long long

int main(int argc, char **argv){
    if (argc != 3) {
        printf("error! usage: ./plus <arg1> <arg2>'\n");
        return -1;
    }
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    printf("%d + %d = %d\n",a,b,a+b);
    return 0;
}