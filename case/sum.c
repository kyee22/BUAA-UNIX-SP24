#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<string.h>
#define max(a,b) (((a)<(b))?(b):(a))
#define min(a,b) (((a)<(b))?(a):(b))
#define LL long long

int main(int argc, char **argv){
    if (argc <= 1) {
        printf("error! usage: ./sum <arg1> <arg2> <arg3> ...'\n");
        return -1;
    }
    int sum = 0;
    int tmp;
    for (int i = 1; i < argc; ++i) {
        tmp = atoi(argv[i]);
        printf(i == 1 ? "%d" : " + %d");
        sum += tmp;
    }
    printf(" = %d\n",sum);
    return 0;
}