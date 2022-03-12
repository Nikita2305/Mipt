#include <stdint.h>
#include <stdio.h>

uint64_t convert_char(char c)
{
    if (c >= '0' && c <= '9') {
        return 1ull << (c - '0');
    }
    if (c >= 'A' && c <= 'Z') {
        return 1ull << (10 + (c - 'A'));
    }
    return 1ull << (10 + 26 + (c - 'a'));
}

char convert_value(uint64_t value)
{
    for (int i = 0; i < 26; i++) {
        if (value == convert_char('a' + i)) {
            return ('a' + i);
        }
        if (value == convert_char('A' + i)) {
            return ('A' + i);
        }
    }
    for (int i = 0; i < 10; i++) {
        if (value == convert_char('0' + i)) {
            return ('0' + i);
        }
    }
    return '!';
}

int main()
{
    uint64_t res = 0, buf = 0;
    while (1) {
        char c = getchar();
        if (c == '|') {
            res |= buf;
            buf = 0;
        } else if (c == '&') {
            res &= buf;
            buf = 0;
        } else if (c == '^') {
            res ^= buf;
            buf = 0;
        } else if (c == '~') {
            res = ~res;
        } else if (
            (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z')) {
            buf |= convert_char(c);
        } else {
            break;
        }
    }
    for (uint64_t i = 0; i < 62; i++) {
        if (res & (1ull << i)) {
            printf("%c", convert_value(1ull << i));
        }
    }
}
