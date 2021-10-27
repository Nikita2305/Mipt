#include <stdint.h>

extern void sum(ITYPE first, ITYPE second, ITYPE* res, int* CF)
{
    *res = 0;
    ITYPE shift = 1;
    ITYPE next = 0;
    for (int i = 0; i < 8 * sizeof(ITYPE); ++i) {
        (*res) |= (first & shift) ^ (second & shift) ^ next;
        next = ((first & shift) & (second & shift)) ^ ((first & shift) & next) ^
               (next & (second & shift));
        shift <<= 1;
        *CF = (next != 0);
        next <<= 1;
    }
}
