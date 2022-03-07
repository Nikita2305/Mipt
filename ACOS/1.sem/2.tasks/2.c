#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern void mul(ITYPE first, ITYPE second, ITYPE* res, int* CF)
{
    *res = 0;
    *CF = 0;
    ITYPE one = 1;
    for (int i = 0; i < 8 * sizeof(ITYPE); ++i) {
        ITYPE shift = (one << i);
        ITYPE next = 0;
        ITYPE mul = ((first & (one << i)) != 0);
        int CF_temp = 0;
        for (ITYPE t = 0; i + t < 8 * sizeof(ITYPE); ++t) {
            ITYPE b1 = (*res) & shift, b2 = (second & mul) << i;
            (*res) = ((*res) ^ b1) | (b1 ^ b2 ^ next);
            next = (b1 & b2) ^ (b1 & next) ^ (next & b2);
            CF_temp = (next != 0);
            shift <<= 1;
            next <<= 1;
            mul <<= 1;
        }
        (*CF) |= CF_temp;
        for (int t = 8 * sizeof(ITYPE) - i; t < 8 * sizeof(ITYPE); ++t) {
            (*CF) |= ((second & mul) != 0);
            mul <<= 1;
        }
    }
}
