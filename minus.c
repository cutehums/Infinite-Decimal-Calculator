#include "calculator.h"

BigDecimal bd_sub(const BigDecimal *a, const BigDecimal *b) {
    BigDecimal negated = bd_copy(b);
    BigDecimal result;
    negated.sign = -negated.sign;
    result = bd_add(a, &negated);
    bd_free(&negated);
    return result;
}
