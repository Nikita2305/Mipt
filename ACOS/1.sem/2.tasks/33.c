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
