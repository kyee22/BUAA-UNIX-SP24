#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<string.h>
#define max(a,b) (((a)<(b))?(b):(a))
#define min(a,b) (((a)<(b))?(a):(b))
#define LL long long

int main(){
    int n;
    scanf("%d",&n);
    printf((n&1) ? "%d is odd\n" : "%d is even!\n",n);
    return 0;
}