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

    EVP_DecryptInit(ctx, EVP_aes_256_cbc(), key, iv);

    int len;
    int plaintext_len;
    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;

    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);

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
    text[read(0, text, sizeof(text))] = '\0';
    char output[MAX_LENGTH];
    int len = 0;
    if (-1 == (len = decrypt(text, strlen(text), salt, passw, output))) {
        return 1;
    }
    output[len] = '\0';
    printf("%s", output);

    /*
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();*/
}
