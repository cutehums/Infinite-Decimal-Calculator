#include "calculator.h"

#include <errno.h>
#include <stdlib.h>

static size_t read_precision(const char *text) {
    char *end = NULL;
    unsigned long value;
    if (text == NULL) return DEFAULT_DIVISION_PRECISION;
    errno = 0;
    value = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || value > 100000UL)
        return DEFAULT_DIVISION_PRECISION;
    return (size_t)value;
}

int main(int argc, char **argv) {
    char error[256];
    char response[32];
    char *owned_expression = NULL;
    const char *expression;
    size_t precision = argc >= 3 ? read_precision(argv[2]) : DEFAULT_DIVISION_PRECISION;
    BigDecimal result;
    char *formatted;

    if (argc >= 2) {
        expression = argv[1];
    } else {
        owned_expression = read_expression_file("input.txt", error, sizeof(error));
        if (owned_expression == NULL) {
            fprintf(stderr, "Input error: %s\n", error);
            return 1;
        }
        expression = owned_expression;
        printf("Welcome to the Infinite Decimal Calculator!\n");
        printf("Expression in input.txt: %s\n", expression);
        printf("Calculate it? [Y/n]: ");
        if (fgets(response, sizeof(response), stdin) != NULL &&
            response[0] != '\n' && response[0] != 'Y' && response[0] != 'y') {
            free(owned_expression);
            puts("Calculation cancelled.");
            return 0;
        }
    }

    if (!calculator_eval(expression, precision, &result, error, sizeof(error))) {
        fprintf(stderr, "Calculation error: %s\n", error);
        free(owned_expression);
        return 1;
    }
    formatted = bd_to_string(&result);
    if (formatted == NULL) {
        fprintf(stderr, "Calculation error: out of memory\n");
        bd_free(&result);
        free(owned_expression);
        return 1;
    }
    printf("Result: %s\n", formatted);
    if (result.inexact)
        printf("(truncated to %zu decimal places)\n", precision);

    free(formatted);
    bd_free(&result);
    free(owned_expression);
    return 0;
}
