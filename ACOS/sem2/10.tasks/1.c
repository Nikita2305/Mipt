#include <errno.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

const int MAX_LENGTH = 1000000;

int decrypt(
    char* ciphertext,
    int ciphertext_len,
    char* salt,
    char* passw,
    char* plaintext)
{
    // printf("cipher_len: %d\n", ciphertext_len);
    int res;
    EVP_CIPHER_CTX* ctx;
    ctx = EVP_CIPHER_CTX_new();

    char key[MAX_LENGTH];
    char iv[MAX_LENGTH];

    EVP_BytesToKey(
        EVP_aes_256_cbc(),
        EVP_sha256(),
        salt,
        passw,
        strlen(passw),
        1,
        key,
        iv);

    // printf("%s %s", key, iv);

    EVP_DecryptInit(ctx, EVP_aes_256_cbc(), key, iv);

    int len;
    int plaintext_len;
    res = EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    // printf("upd: %d\nlen: %d\n", res, len);
    plaintext_len = len;

    res = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    // printf("final: %d\nlen: %d\n", res, len);
    plaintext_len += len;
    // EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

int main(int argc, char* argv[])
{
    /*ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_no_config();*/

    char salt[8];
    read(0, salt, 8);
    read(0, salt, 8);
    char* passw = argv[1];

    char text[MAX_LENGTH];
    int batch = 1024;
    char* ptr = text;
    int l = 0;
    while ((l = read(0, ptr, batch)) > 0) {
        ptr += l;
    }
    *ptr = '\0';
    // printf("%d\n", ptr-text);

    char output[MAX_LENGTH];
    int len = 0;
    if (-1 == (len = decrypt(text, ptr - text, salt, passw, output))) {
        return 1;
    }
    output[len] = '\0';
    printf("%s", output);

    /*
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();*/
}
