#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    const int bsize = 4096 + 1;
    char input[bsize];
    char* ptr = input;
    const int bucket = 1024;
    int len = 0;
    while ((len = read(0, ptr, bucket)) > 0) {
        ptr += len;
    }
    *ptr = '\0';
    char hash[SHA512_DIGEST_LENGTH];
    SHA512(input, ptr - input, hash);
    
    printf("0x");
    char buffer[8];
    for(int i = 0; i < 64; i++) {
        int res = sprintf(buffer, "%02x", hash[i]);
        printf("%c%c", buffer[res - 2], buffer[res - 1]); 
    }
    printf("\n");    
    return 0;
}
