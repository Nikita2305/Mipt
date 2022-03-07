#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

struct Customers {
	int value = 0;
	unsigned int x = 0; 
};

int main()
{	
    FILE *fp_output;

	fp_output = fopen("input.txt", "wb");

	int arr[100];
    for (int i = 0; i < 100; i++) {
        arr[i] = 100 - i;
    }
	fwrite(arr + 2, sizeof(int), 8, fp_output);

	fclose(fp_output);

	return 0;
}

