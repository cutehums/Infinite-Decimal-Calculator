#include "calculator.h"

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *text;
    size_t pos;
    size_t precision;
    char *error;
    size_t error_size;
} Parser;

static char *duplicate_text(const char *text, size_t length) {
    char *copy = (char *)malloc(length + 1U);
    if (copy == NULL) return NULL;
    if (length > 0U) memcpy(copy, text, length);
    copy[length] = '\0';
    return copy;
}

static void set_error(Parser *parser, const char *message) {
    if (parser->error != NULL && parser->error_size > 0U && parser->error[0] == '\0')
        snprintf(parser->error, parser->error_size, "%s at position %zu",
                 message, parser->pos + 1U);
}

BigDecimal bd_zero(void) {
    BigDecimal value;
    value.sign = 0;
    value.digits = duplicate_text("0", 1U);
    value.len = value.digits == NULL ? 0U : 1U;
    value.scale = 0U;
    value.inexact = 0;
    return value;
}

void bd_free(BigDecimal *value) {
    if (value == NULL) return;
    free(value->digits);
    value->digits = NULL;
    value->len = value->scale = 0U;
    value->sign = value->inexact = 0;
}

int bd_is_zero(const BigDecimal *value) {
    return value == NULL || value->sign == 0 || value->digits == NULL ||
           (value->len == 1U && value->digits[0] == '0');
}

void bd_normalize(BigDecimal *value) {
    size_t leading = 0U;
    if (value == NULL || value->digits == NULL || value->len == 0U) return;
    while (leading + 1U < value->len && value->digits[leading] == '0') ++leading;
    if (leading > 0U) {
        memmove(value->digits, value->digits + leading, value->len - leading);
        value->len -= leading;
        value->digits[value->len] = '\0';
    }
    while (value->scale > 0U && value->len > 1U &&
           value->digits[value->len - 1U] == '0') {
        --value->len;
        --value->scale;
        value->digits[value->len] = '\0';
    }
    if (value->len == 1U && value->digits[0] == '0') {
        value->sign = 0;
        value->scale = 0U;
        value->inexact = 0;
    }
}

BigDecimal bd_copy(const BigDecimal *value) {
    BigDecimal copy = bd_zero();
    char *digits;
    if (value == NULL || value->digits == NULL) return copy;
    digits = duplicate_text(value->digits, value->len);
    if (digits == NULL) return copy;
    free(copy.digits);
    copy = *value;
    copy.digits = digits;
    return copy;
}

BigDecimal bd_from_literal(const char *text, size_t length, int *ok) {
    BigDecimal value = bd_zero();
    char *digits;
    size_t i = 0U, out = 0U, dot_at = SIZE_MAX;
    int sign = 1;
    if (ok != NULL) *ok = 0;
    if (text == NULL || length == 0U) return value;
    if (text[i] == '+' || text[i] == '-') {
        sign = text[i] == '-' ? -1 : 1;
        ++i;
    }
    digits = (char *)malloc(length + 1U);
    if (digits == NULL) return value;
    for (; i < length; ++i) {
        if (text[i] == '.') {
            if (dot_at != SIZE_MAX) { free(digits); return value; }
            dot_at = out;
        } else if (isdigit((unsigned char)text[i])) {
            digits[out++] = text[i];
        } else {
            free(digits); return value;
        }
    }
    if (out == 0U) { free(digits); return value; }
    digits[out] = '\0';
    bd_free(&value);
    value.sign = sign;
    value.digits = digits;
    value.len = out;
    value.scale = dot_at == SIZE_MAX ? 0U : out - dot_at;
    value.inexact = 0;
    bd_normalize(&value);
    if (ok != NULL) *ok = 1;
    return value;
}

BigDecimal bd_mul(const BigDecimal *a, const BigDecimal *b) {
    BigDecimal result = bd_zero();
    size_t total, i, first;
    int *work;
    char *digits;
    if (bd_is_zero(a) || bd_is_zero(b)) return result;
    if (a->len > SIZE_MAX - b->len) return result;
    total = a->len + b->len;
    work = (int *)calloc(total, sizeof(int));
    if (work == NULL) return result;
    for (i = a->len; i-- > 0U;) {
        size_t j;
        for (j = b->len; j-- > 0U;) {
            size_t index = i + j + 1U;
            int product = (a->digits[i] - '0') * (b->digits[j] - '0') + work[index];
            work[index] = product % 10;
            work[index - 1U] += product / 10;
        }
    }
    first = 0U;
    while (first + 1U < total && work[first] == 0) ++first;
    digits = (char *)malloc(total - first + 1U);
    if (digits == NULL) { free(work); return result; }
    for (i = first; i < total; ++i) digits[i - first] = (char)('0' + work[i]);
    digits[total - first] = '\0';
    free(work);
    bd_free(&result);
    result.sign = a->sign * b->sign;
    result.digits = digits;
    result.len = total - first;
    result.scale = a->scale + b->scale;
    result.inexact = a->inexact || b->inexact;
    bd_normalize(&result);
    return result;
}

static void strip_leading_zeros(char *digits, size_t *length) {
    size_t leading = 0U;
    while (leading + 1U < *length && digits[leading] == '0') ++leading;
    if (leading > 0U) {
        memmove(digits, digits + leading, *length - leading);
        *length -= leading;
        digits[*length] = '\0';
    }
}

static int integer_compare(const char *a, size_t a_len,
                           const char *b, size_t b_len) {
    int cmp;
    if (a_len != b_len) return a_len > b_len ? 1 : -1;
    cmp = memcmp(a, b, a_len);
    return cmp > 0 ? 1 : (cmp < 0 ? -1 : 0);
}

static void integer_subtract_in_place(char *a, size_t *a_len,
                                      const char *b, size_t b_len) {
    size_t ai = *a_len, bi = b_len;
    int borrow = 0;
    while (ai > 0U) {
        int av, bv = 0;
        --ai;
        if (bi > 0U) bv = b[--bi] - '0';
        av = (a[ai] - '0') - borrow - bv;
        if (av < 0) { av += 10; borrow = 1; } else borrow = 0;
        a[ai] = (char)('0' + av);
    }
    strip_leading_zeros(a, a_len);
}

static char *append_zeros(const char *digits, size_t len, size_t zeros,
                          size_t *out_len) {
    char *result;
    if (len > SIZE_MAX - zeros) return NULL;
    result = (char *)malloc(len + zeros + 1U);
    if (result == NULL) return NULL;
    memcpy(result, digits, len);
    memset(result + len, '0', zeros);
    result[len + zeros] = '\0';
    *out_len = len + zeros;
    return result;
}

BigDecimal bd_div(const BigDecimal *a, const BigDecimal *b,
                  size_t precision, int *ok) {
    BigDecimal result = bd_zero();
    long long exponent;
    char *numerator = NULL, *denominator = NULL, *quotient = NULL, *remainder = NULL;
    size_t numerator_len = 0U, denominator_len = 0U;
    size_t quotient_len = 0U, remainder_len = 1U, i;
    if (ok != NULL) *ok = 0;
    if (bd_is_zero(b)) return result;
    if (bd_is_zero(a)) { if (ok != NULL) *ok = 1; return result; }
    if (precision > 100000U || a->scale > (size_t)LLONG_MAX ||
        b->scale > (size_t)LLONG_MAX) return result;
    exponent = (long long)b->scale - (long long)a->scale + (long long)precision;
    if (exponent >= 0) {
        numerator = append_zeros(a->digits, a->len, (size_t)exponent, &numerator_len);
        denominator = duplicate_text(b->digits, b->len);
        denominator_len = b->len;
    } else {
        numerator = duplicate_text(a->digits, a->len);
        numerator_len = a->len;
        denominator = append_zeros(b->digits, b->len, (size_t)(-exponent), &denominator_len);
    }
    if (numerator == NULL || denominator == NULL) goto cleanup;
    quotient = (char *)malloc(numerator_len + 1U);
    remainder = (char *)malloc(denominator_len + 2U);
    if (quotient == NULL || remainder == NULL) goto cleanup;
    remainder[0] = '0'; remainder[1] = '\0';
    for (i = 0U; i < numerator_len; ++i) {
        int digit = 0;
        if (remainder_len == 1U && remainder[0] == '0') remainder[0] = numerator[i];
        else { remainder[remainder_len++] = numerator[i]; remainder[remainder_len] = '\0'; }
        strip_leading_zeros(remainder, &remainder_len);
        while (integer_compare(remainder, remainder_len, denominator, denominator_len) >= 0) {
            integer_subtract_in_place(remainder, &remainder_len, denominator, denominator_len);
            ++digit;
        }
        quotient[quotient_len++] = (char)('0' + digit);
    }
    quotient[quotient_len] = '\0';
    strip_leading_zeros(quotient, &quotient_len);
    bd_free(&result);
    result.sign = a->sign * b->sign;
    result.digits = quotient;
    quotient = NULL;
    result.len = quotient_len;
    result.scale = precision;
    result.inexact = a->inexact || b->inexact ||
                     !(remainder_len == 1U && remainder[0] == '0');
    bd_normalize(&result);
    if (ok != NULL) *ok = 1;
cleanup:
    free(numerator); free(denominator); free(quotient); free(remainder);
    return result;
}

char *bd_to_string(const BigDecimal *value) {
    size_t integer_digits, leading_zeros = 0U, sign_chars, dot_chars, suffix_chars;
    size_t total, out = 0U;
    char *text;
    if (value == NULL || value->digits == NULL) return duplicate_text("0", 1U);
    sign_chars = value->sign < 0 ? 1U : 0U;
    dot_chars = value->scale > 0U ? 1U : 0U;
    suffix_chars = value->inexact ? 3U : 0U;
    if (value->scale >= value->len) {
        integer_digits = 1U;
        leading_zeros = value->scale - value->len;
    } else integer_digits = value->len - value->scale;
    total = sign_chars + integer_digits + dot_chars + leading_zeros +
            (value->scale >= value->len ? value->len : value->scale) + suffix_chars;
    text = (char *)malloc(total + 1U);
    if (text == NULL) return NULL;
    if (value->sign < 0) text[out++] = '-';
    if (value->scale == 0U) {
        memcpy(text + out, value->digits, value->len); out += value->len;
    } else if (value->scale >= value->len) {
        text[out++] = '0'; text[out++] = '.';
        memset(text + out, '0', leading_zeros); out += leading_zeros;
        memcpy(text + out, value->digits, value->len); out += value->len;
    } else {
        memcpy(text + out, value->digits, integer_digits); out += integer_digits;
        text[out++] = '.';
        memcpy(text + out, value->digits + integer_digits, value->scale); out += value->scale;
    }
    if (value->inexact) { memcpy(text + out, "...", 3U); out += 3U; }
    text[out] = '\0';
    return text;
}

static void skip_spaces(Parser *parser) {
    while (isspace((unsigned char)parser->text[parser->pos])) ++parser->pos;
}

static int parse_expression(Parser *parser, BigDecimal *result);

static int parse_number(Parser *parser, BigDecimal *result) {
    size_t start, digits = 0U;
    int saw_dot = 0, ok = 0;
    skip_spaces(parser);
    start = parser->pos;
    while (1) {
        char ch = parser->text[parser->pos];
        if (isdigit((unsigned char)ch)) { ++digits; ++parser->pos; }
        else if (ch == '.' && !saw_dot) { saw_dot = 1; ++parser->pos; }
        else break;
    }
    if (digits == 0U) { set_error(parser, "number expected"); return 0; }
    *result = bd_from_literal(parser->text + start, parser->pos - start, &ok);
    if (!ok || result->digits == NULL) {
        set_error(parser, "invalid number or out of memory"); bd_free(result); return 0;
    }
    return 1;
}

static int parse_factor(Parser *parser, BigDecimal *result) {
    int sign = 1;
    skip_spaces(parser);
    while (parser->text[parser->pos] == '+' || parser->text[parser->pos] == '-') {
        if (parser->text[parser->pos] == '-') sign = -sign;
        ++parser->pos; skip_spaces(parser);
    }
    if (parser->text[parser->pos] == '(') {
        ++parser->pos;
        if (!parse_expression(parser, result)) return 0;
        skip_spaces(parser);
        if (parser->text[parser->pos] != ')') {
            set_error(parser, "')' expected"); bd_free(result); return 0;
        }
        ++parser->pos;
    } else if (!parse_number(parser, result)) return 0;
    result->sign *= sign;
    return 1;
}

static int parse_term(Parser *parser, BigDecimal *result) {
    BigDecimal left;
    if (!parse_factor(parser, &left)) return 0;
    while (1) {
        char op;
        BigDecimal right, combined;
        int ok = 1;
        skip_spaces(parser); op = parser->text[parser->pos];
        if (op != '*' && op != '/') break;
        ++parser->pos;
        if (!parse_factor(parser, &right)) { bd_free(&left); return 0; }
        if (op == '*') combined = bd_mul(&left, &right);
        else {
            combined = bd_div(&left, &right, parser->precision, &ok);
            if (!ok) set_error(parser, bd_is_zero(&right) ? "division by zero" : "division failed");
        }
        bd_free(&left); bd_free(&right);
        if (!ok || combined.digits == NULL) { bd_free(&combined); return 0; }
        left = combined;
    }
    *result = left;
    return 1;
}

static int parse_expression(Parser *parser, BigDecimal *result) {
    BigDecimal left;
    if (!parse_term(parser, &left)) return 0;
    while (1) {
        char op;
        BigDecimal right, combined;
        skip_spaces(parser); op = parser->text[parser->pos];
        if (op != '+' && op != '-') break;
        ++parser->pos;
        if (!parse_term(parser, &right)) { bd_free(&left); return 0; }
        combined = op == '+' ? bd_add(&left, &right) : bd_sub(&left, &right);
        bd_free(&left); bd_free(&right);
        if (combined.digits == NULL) { set_error(parser, "out of memory"); return 0; }
        left = combined;
    }
    *result = left;
    return 1;
}

int calculator_eval(const char *expression, size_t division_precision,
                    BigDecimal *result, char *error, size_t error_size) {
    Parser parser;
    BigDecimal value;
    if (error != NULL && error_size > 0U) error[0] = '\0';
    if (expression == NULL || result == NULL) {
        if (error != NULL && error_size > 0U) snprintf(error, error_size, "expression is missing");
        return 0;
    }
    parser.text = expression; parser.pos = 0U; parser.precision = division_precision;
    parser.error = error; parser.error_size = error_size;
    if (!parse_expression(&parser, &value)) return 0;
    skip_spaces(&parser);
    if (parser.text[parser.pos] != '\0') {
        set_error(&parser, "unexpected character"); bd_free(&value); return 0;
    }
    *result = value;
    return 1;
}

char *read_expression_file(const char *path, char *error, size_t error_size) {
    FILE *file;
    long length;
    size_t read_count;
    char *text;
    if (error != NULL && error_size > 0U) error[0] = '\0';
    file = fopen(path, "rb");
    if (file == NULL) {
        if (error != NULL && error_size > 0U) snprintf(error, error_size, "cannot open %s", path);
        return NULL;
    }
    if (fseek(file, 0L, SEEK_END) != 0 || (length = ftell(file)) < 0L ||
        fseek(file, 0L, SEEK_SET) != 0) {
        fclose(file);
        if (error != NULL && error_size > 0U) snprintf(error, error_size, "cannot read %s", path);
        return NULL;
    }
    text = (char *)malloc((size_t)length + 1U);
    if (text == NULL) { fclose(file); return NULL; }
    read_count = fread(text, 1U, (size_t)length, file);
    fclose(file);
    text[read_count] = '\0';
    while (read_count > 0U && isspace((unsigned char)text[read_count - 1U]))
        text[--read_count] = '\0';
    if (read_count == 0U) {
        free(text);
        if (error != NULL && error_size > 0U) snprintf(error, error_size, "%s is empty", path);
        return NULL;
    }
    return text;
}
