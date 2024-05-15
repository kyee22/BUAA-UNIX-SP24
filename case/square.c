#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<string.h>
#define max(a,b) (((a)<(b))?(b):(a))
#define min(a,b) (((a)<(b))?(a):(b))
#define LL long long

int main(int argc, char **argv){
    if (argc != 2) {
        printf("error! usage: ./square <arg1>'\n");
        return -1;
    }
    int a = atoi(argv[1]);
    printf("%d\n",a * a);
    return 0;

    return 0;
}