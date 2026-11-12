#include "calculator.h"

#include <stdlib.h>
#include <string.h>

static char *scaled_digits(const BigDecimal *value, size_t scale, size_t *length) {
    size_t zeros = scale - value->scale;
    char *digits = (char *)malloc(value->len + zeros + 1U);
    if (digits == NULL) return NULL;
    memcpy(digits, value->digits, value->len);
    memset(digits + value->len, '0', zeros);
    digits[value->len + zeros] = '\0';
    *length = value->len + zeros;
    return digits;
}

static int compare_magnitude(const char *a, size_t a_len,
                             const char *b, size_t b_len) {
    int cmp;
    if (a_len != b_len) return a_len > b_len ? 1 : -1;
    cmp = memcmp(a, b, a_len);
    return cmp > 0 ? 1 : (cmp < 0 ? -1 : 0);
}

static char *add_magnitude(const char *a, size_t a_len,
                           const char *b, size_t b_len, size_t *out_len) {
    size_t max_len = a_len > b_len ? a_len : b_len;
    char *result = (char *)malloc(max_len + 2U);
    size_t ai = a_len, bi = b_len, ri = max_len + 1U;
    int carry = 0;
    if (result == NULL) return NULL;
    result[ri] = '\0';
    while (ai > 0U || bi > 0U || carry != 0) {
        int av = ai > 0U ? a[--ai] - '0' : 0;
        int bv = bi > 0U ? b[--bi] - '0' : 0;
        int sum = av + bv + carry;
        result[--ri] = (char)('0' + sum % 10);
        carry = sum / 10;
    }
    memmove(result, result + ri, max_len + 2U - ri);
    *out_len = max_len + 1U - ri;
    return result;
}

static char *subtract_magnitude(const char *larger, size_t larger_len,
                                const char *smaller, size_t smaller_len,
                                size_t *out_len) {
    char *result = (char *)malloc(larger_len + 1U);
    size_t li = larger_len, si = smaller_len, leading = 0U;
    int borrow = 0;
    if (result == NULL) return NULL;
    result[larger_len] = '\0';
    while (li > 0U) {
        int lv, sv = 0;
        --li;
        if (si > 0U) sv = smaller[--si] - '0';
        lv = (larger[li] - '0') - borrow - sv;
        if (lv < 0) {
            lv += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result[li] = (char)('0' + lv);
    }
    while (leading + 1U < larger_len && result[leading] == '0') ++leading;
    if (leading > 0U)
        memmove(result, result + leading, larger_len - leading + 1U);
    *out_len = larger_len - leading;
    return result;
}

BigDecimal bd_add(const BigDecimal *a, const BigDecimal *b) {
    BigDecimal result = bd_zero();
    size_t scale, a_len, b_len, result_len = 0U;
    char *a_digits, *b_digits, *result_digits = NULL;
    int result_sign = 0;

    if (bd_is_zero(a)) return bd_copy(b);
    if (bd_is_zero(b)) return bd_copy(a);
    scale = a->scale > b->scale ? a->scale : b->scale;
    a_digits = scaled_digits(a, scale, &a_len);
    b_digits = scaled_digits(b, scale, &b_len);
    if (a_digits == NULL || b_digits == NULL) {
        free(a_digits); free(b_digits);
        return result;
    }
    if (a->sign == b->sign) {
        result_digits = add_magnitude(a_digits, a_len, b_digits, b_len, &result_len);
        result_sign = a->sign;
    } else {
        int compare = compare_magnitude(a_digits, a_len, b_digits, b_len);
        if (compare == 0) {
            free(a_digits); free(b_digits);
            return result;
        } else if (compare > 0) {
            result_digits = subtract_magnitude(a_digits, a_len, b_digits, b_len, &result_len);
            result_sign = a->sign;
        } else {
            result_digits = subtract_magnitude(b_digits, b_len, a_digits, a_len, &result_len);
            result_sign = b->sign;
        }
    }
    free(a_digits); free(b_digits);
    if (result_digits == NULL) return result;
    bd_free(&result);
    result.sign = result_sign;
    result.digits = result_digits;
    result.len = result_len;
    result.scale = scale;
    result.inexact = a->inexact || b->inexact;
    bd_normalize(&result);
    return result;
}
