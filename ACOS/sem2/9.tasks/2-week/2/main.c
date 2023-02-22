#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct String {
    char* value;
    size_t size;
};

size_t callback_function(void* ptr, size_t size, size_t nmemb, void* user_data) {
    struct String* string = (struct String*)user_data;
    string->value = realloc(string->value, string->size + size * nmemb + 1);
    memcpy(string->value + string->size, ptr, size * nmemb);
    string->size += size * nmemb;
    string->value[string->size] = '\0';
    return size * nmemb;
}

void send_request(CURL* handle, struct String* string, char* name) {
    handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, name);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, callback_function);     
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)string);
    CURLcode res = curl_easy_perform(handle);
}

void print_result(CURL* handle, struct String* string) {
    char* i1 = strstr(string->value, "<title>") + 7;
    char* i2 = strstr(string->value, "</title>");
    *i2 = '\0';
    printf("%s\n", i1);
    curl_easy_cleanup(handle);
    free(string->value);
    curl_global_cleanup();
}

int main(int argc, char* argv[])
{
    struct String string = {malloc(1), 0};
    CURL* handle;
    send_request(handle, &string, argv[1]);
    print_result(handle, &string); 
}
