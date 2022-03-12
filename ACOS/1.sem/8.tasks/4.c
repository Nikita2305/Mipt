#include <windows.h>
#include <inttypes.h>

void swap(char* a, char* b) {
    char x = *a;
    *a = *b;
    *b = x;
}

int big_endian(int little_endian) {
    char* ptr = (char*)(&little_endian);
    swap(ptr, ptr + 3);
    swap(ptr + 1, ptr + 2);
    return *(int*)(ptr);
}

int convert_to_chars(int value, char* arr, int arr_size) {
    int copy = (value < 0 ? -value : value);
    int index = arr_size - 1;
    arr[index] = ' ';
    index--; 
    while (copy > 0) {
        arr[index] = '0' + (copy % 10);
        copy /= 10;
        index--;
    }
    if (value < 0) {
        arr[index] = '-';
        index--;
    }
    if (index == arr_size - 1) {
        arr[index] = '0';
        index--;
    }
    return index + 1;
}

struct Item {
    int value;
    uint32_t next_pointer;
};

int main(int argc, char* argv[])
{
    HANDLE fd1 = CreateFileA(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    struct Item just_read;
    const int temp_size = 100;
    char temp[temp_size];
    while (1) {
        DWORD read_bytes = 0;
        ReadFile(fd1, &just_read, sizeof(struct Item), &read_bytes, NULL);
        if (read_bytes != sizeof(struct Item)) {
            break;
        }
        int index = convert_to_chars(just_read.value, temp, temp_size);  
        HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        WriteFile(ConsoleOutput, temp + index, temp_size - index, NULL, NULL);
        if (just_read.next_pointer == 0) {
            break;
        }
        LARGE_INTEGER li;
        li.u.LowPart = just_read.next_pointer;
        li.u.HighPart = 0;
        SetFilePointerEx(fd1, li, NULL, FILE_BEGIN);
    }
    CloseHandle(fd1);
    return 0;
}
