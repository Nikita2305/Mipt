#include <stdio.h>
#include <stdlib.h>
void makeTL() {
int a;
while (1==1) a++; 


}
int main(int argc, char* argv[])
{
    int N = 15;
    for (int i=0;i<2000;i++) {
        if (N==0 || N>100) {return 0; }
        if (i%2==0) {
            N-=3; printf("%d\n", N); 
        } else {
            N+=5; printf("%d\n", N); 
        }
    }
    return 0;
}
