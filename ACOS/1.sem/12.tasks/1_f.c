#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int maxLen = 1e4;

int main()
{
    char* another = "print(";
    int start_size = strlen(another);
    int care_end = 1;
    char value[maxLen + start_size + care_end];
    for (int i = 0; i < start_size; i++) {
        value[i] = another[i];
    }
    fgets(value + start_size, maxLen, stdin);
    int i = strlen(value) - 1;
    while (i >= 0 && value[i] == '\n') {
        value[i] = '\0';
        i--;
    }
    value[i + 2] = '\0';
    value[i + 1] = ')';
    execlp("python3", "python3", "-c", value, NULL);
    perror("exec");
    exit(1);
}
