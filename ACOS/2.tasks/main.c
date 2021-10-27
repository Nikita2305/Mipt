#include <stdio.h>
#include <math.h>

#include <stdint.h>

typedef enum {
    PlusZero = 0x00,
    MinusZero = 0x01,
    PlusInf = 0xF0,
    MinusInf = 0xF1,
    PlusRegular = 0x10,
    MinusRegular = 0x11,
    PlusDenormal = 0x20,
    MinusDenormal = 0x21,
    SignalingNaN = 0x30,
    QuietNaN = 0x31
} float_class_t;

extern float_class_t classify(double* value_ptr)
{
    uint64_t mantissa_mask = (1ull << 52) - 1;
    uint64_t exponent_mask = ((1ull << 63) - 1) ^ mantissa_mask;
    uint64_t sign_mask = (1ull << 63);
    uint64_t value = *((uint64_t*)value_ptr);
    int is_zero1 = (((~value) & mantissa_mask) == mantissa_mask);
    int is_one1 = ((value & mantissa_mask) == mantissa_mask);
    int is_zero2 = (((~value) & exponent_mask) == exponent_mask);
    int is_one2 = ((value & exponent_mask) == exponent_mask);
    int is_zero3 = (((~value) & sign_mask) == sign_mask);
    int is_one3 = ((value & sign_mask) == sign_mask);
    float_class_t res = PlusRegular;
    if (is_zero1) {
        if (is_zero2) {
            if (is_zero3) {
                res = PlusZero;
            } else if (is_one3) {
                res = MinusZero;
            }
        } else if (is_one2) {
            if (is_zero3) {
                res = PlusInf;
            } else if (is_one3) {
                res = MinusInf;
            }
        }
    } else {
        if (is_zero2) {
            if (is_zero3) {
                res = PlusDenormal;
            } else if (is_one3) {
                res = MinusDenormal;
            }
        } else if (is_one2) {
            if (value & (1ull << (52 - 1))) {
                res = SignalingNaN;
            } else {
                res = QuietNaN;
            }
        }
    }
    if (res == PlusRegular && is_one3) {
        res = MinusRegular;
    }
    return res;
}


void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

int main()
{
    double d = 0.0;
    d = 0.0;
    printf("Float is: %f. Int is: %d. Correct answer is: 0. Binary: ", d, classify(&d));
    printBits(sizeof(d), (unsigned long long *)(&d));
	d = -0.0;
    printf("Float is: %f. Int is: %d. Correct answer is: 1. Binary: ", d, classify(&d));
	printBits(sizeof(d), (unsigned long long *)(&d));
    d = 1 / 0.0;
    printf("Float is: %f. Int is: %d. Correct answer is: 240. Binary: ", d, classify(&d));
	printBits(sizeof(d), (unsigned long long *)(&d));
    d = 1 / -0.0;
    printf("Float is: %f. Int is: %d. Correct answer is: 241. Binary: ", d, classify(&d));
	printBits(sizeof(d), (unsigned long long *)(&d));
    d = 1.0;
    printf("Float is: %f. Int is: %d. Correct answer is: 16. Binary: ", d, classify(&d));
	printBits(sizeof(d), (unsigned long long *)(&d));
    d = -1.0;
    printf("Float is: %f. Int is: %d. Correct answer is: 17. Binary: ", d, classify(&d));
    printBits(sizeof(d), (unsigned long long *)(&d));
    d = (1.0 / 0.0) / (1.0 / 0.0);
    printf("Float is: %f. Int is: %d. Correct answer is: 49. Binary: ", d, classify(&d));
	printBits(sizeof(d), (unsigned long long *)(&d));
    long long cheatnan = (*((unsigned long long *)(&d))) ^ (1ull << 63);
    printf("Float is: %s. Int is: %d. Correct answer is: 48. Binary: ", "snan", classify((double*)(&cheatnan)));
    printBits(sizeof(d), (unsigned long long *)(&cheatnan));
}
