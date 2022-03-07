#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* conf = CONFIG;

int is_number(const char* arr, int len)
{
    if (len == 0) {
        return 0;
    }
    int i = 0;
    if (arr[0] == '-' || arr[0] == '+') {
        i += 1;
    }
    for (; i < len; i++) {
        if (arr[i] < '0' || arr[i] > '9') {
            return 0;
        }
    }
    return 1;
}

long long get_number(const char* arr, int len)
{
    int sign = 1;
    int i = 0;
    if (arr[0] == '-') {
        sign = -1;
        i += 1;
    } else if (arr[0] == '+') {
        i += 1;
    }
    long long ans = 0;
    for (; i < len; i++) {
        ans = ans * 10 + (arr[i] - '0');
    }
    return ans * sign;
}

int main()
{
    const int len = strlen(conf);
    char* temp = (char*)malloc(len);
    int ptr = -1;
    long long sum = 0;

    for (int i = 0; i < len; i++) {
        if (conf[i] == '=') {
            ptr = 0;
        } else if (conf[i] == '$') {
            if (is_number(temp, ptr)) {
                sum += get_number(temp, ptr);
            } else {
                for (int j = 0; j < ptr; j++) {
                    printf("%c", temp[j]);
                }
                printf("\n");
            }
            ptr = -1;
        } else {
            if (ptr == -1) {
                continue;
            }
            temp[ptr] = conf[i];
            ptr += 1;
        }
    }

    printf("%lld\n", sum);
    free(temp);
}
