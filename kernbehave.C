#include <stdio.h>
#define MAX 100

int main(int argc, char** argv)
{
    char str[MAX];
    FILE *fp;
    fp = fopen("/proc/cpuinfo","r");
    while(fgets(str,MAX,fp)!=NULL) {
    	printf(str);
    }
    return 0;
}
