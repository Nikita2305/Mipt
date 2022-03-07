#include <stdio.h>

int main()
{
    int x = 5;
    printf("%d", '\n');
    return 0;
    

    int ans_good = 0, ans_bad = 0;
    const int char_size = 8;
    while (1) {
        int value = getchar();
        if (value == EOF) {
            break;
        }
        int prefix = 0;
        for (int i = char_size - 1; i >= 0; i--) {
            if ((value & (1 << i)) == 0)
                break;
            prefix++;
        }
        if (prefix == 0) {
            ans_good++;
            continue;
        }
        if (prefix == 1 || prefix == char_size - 1 || prefix == char_size) {
            printf("%d %d", ans_good, ans_bad);
            return 1;
        }
        for (int i = 0; i < prefix - 1; i++) {
            int temp_value = getchar();
            int temp_prefix = 0;
            for (int i = char_size - 1; i >= 0; i--) {
                if ((temp_value & (1 << i)) == 0)
                    break;
                temp_prefix++;
            }
            if (temp_value == EOF || temp_prefix != 1) {
                printf("%d %d", ans_good, ans_bad);
                return 1;
            }
        }
        ans_bad++;
    }
    printf("%d %d", ans_good, ans_bad);
    return 0;
}
