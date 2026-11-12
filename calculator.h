#ifndef INFINITE_DECIMAL_CALCULATOR_H
#define INFINITE_DECIMAL_CALCULATOR_H

#include <stddef.h>
#include <stdio.h>

#define DEFAULT_DIVISION_PRECISION 100U

typedef struct {
    int sign;
    char *digits;
    size_t len;
    size_t scale;
    int inexact;
} BigDecimal;

BigDecimal bd_zero(void);
BigDecimal bd_copy(const BigDecimal *value);
BigDecimal bd_from_literal(const char *text, size_t length, int *ok);
void bd_normalize(BigDecimal *value);
void bd_free(BigDecimal *value);
int bd_is_zero(const BigDecimal *value);

BigDecimal bd_add(const BigDecimal *a, const BigDecimal *b);
BigDecimal bd_sub(const BigDecimal *a, const BigDecimal *b);
BigDecimal bd_mul(const BigDecimal *a, const BigDecimal *b);
BigDecimal bd_div(const BigDecimal *a, const BigDecimal *b,
                  size_t precision, int *ok);

char *bd_to_string(const BigDecimal *value);
int calculator_eval(const char *expression, size_t division_precision,
                    BigDecimal *result, char *error, size_t error_size);
char *read_expression_file(const char *path, char *error, size_t error_size);

#endif
