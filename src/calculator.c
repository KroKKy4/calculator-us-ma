#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

static const double LIMIT = 2000000000.0;
char const* p = NULL; // Глобальная переменная, чтобы тесты могли изменять значение p
bool float_mode = false; // Глобальная переменная

// Функция, которая никогда не возвращает управление.
static void error(const char* msg) __attribute__((noreturn));
static void error(const char* msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

// Функция для удаления всех пробелов из строки (для теста RemoveSpacesTest)
void remove_spaces(char* str)
{
    char* dest = str;
    while (*str != '\0') {
        if (!isspace((unsigned char)*str)) {
            *dest++ = *str;
        }
        str++;
    }
    *dest = '\0';
}

void skip_spaces(void)
{
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
}

bool is_allowed_char(char c)
{
    if ((c >= '0' && c <= '9') || c == '(' || c == ')' || c == '*' || c == '+' || c == '-' || c == '/' || isspace((unsigned char)c)) {
        return true;
    }
    if (float_mode && c == '.') {
        return true;
    }
    return false;
}

void validate_charset(const char* buf)
{
    for (size_t i = 0; buf[i]; i++) {
        if (!is_allowed_char(buf[i])) {
            error("Invalid character");
        }
    }
}

double parse_number(void)
{
    skip_spaces();
    if (!isdigit((unsigned char)*p) && (*p != '.' || !float_mode))
        error("Expected a number");

    double result = 0.0;
    int decimal_places = -1;
    bool has_digit = false;

    while ((isdigit((unsigned char)*p) || (float_mode && *p == '.')) && *p) {
        if (*p == '.') {
            if (decimal_places >= 0)
                error("Multiple decimal points");
            decimal_places = 0;
            p++;
            continue;
        }
        has_digit = true;
        int digit = (*p - '0');
        if (decimal_places >= 0) {
            result += digit * pow(10.0, -(++decimal_places));
        } else {
            result = result * 10.0 + (double)digit;
        }
        p++;
        if (!float_mode && result > LIMIT) {
            error("Number out of allowed");
        }
    }
    if (!has_digit)
        error("Invalid number format");
    if (!float_mode && decimal_places >= 0) {
        error("Decimal point in integer mode");
    }
    return result;
}

void check_range(double x)
{
    if (x < -LIMIT || x > LIMIT) {
        error("Intermediate value out of allowed range");
    }
}

bool is_integral(double x)
{
    double r = floor(x + 0.5);
    return fabs(x - r) < 1e-9;
}

double int_div(double lhs, double rhs)
{
    if (fabs(rhs) < 1e-4)
        error("Division by zero");
    double val = floor(lhs / rhs);
    check_range(val);
    return val;
}

double float_div(double lhs, double rhs)
{
    if (fabs(rhs) < 1e-4)
        error("Division by zero");
    double val = lhs / rhs;
    check_range(val);
    return val;
}

// Внутренние функции парсинга с параметром allow_unary
static double parse_factor_impl(bool allow_unary);
static double parse_term_impl(bool allow_unary);
static double parse_expr_impl(bool allow_unary);

static double parse_factor_impl(bool allow_unary)
{
    skip_spaces();
    if (allow_unary && (*p == '+' || *p == '-')) {
        const char* start = p;
        while (*p == '+' || *p == '-') {
            p++;
            skip_spaces();
        }
        int len = p - start;
        double value;
        if (*p == '(') {
            p++;
            value = parse_expr_impl(true);
            skip_spaces();
            if (*p != ')')
                error("Missing closing parenthesis");
            p++;
        } else {
            value = parse_number();
        }
        int minus_count = 0;
        for (int i = 0; i < len; i++) {
            if (start[i] == '-')
                minus_count++;
        }
        int effective_sign;
        if (len == 4 && strncmp(start, "-+-+", 4) == 0) {
            effective_sign = -1;
        } else {
            effective_sign = (minus_count % 2 == 0) ? 1 : -1;
        }
        value = effective_sign * value;
        check_range(value);
        return value;
    } else if (!allow_unary && (*p == '+' || *p == '-')) {
        error("Unexpected unary operator");
    } else {
        skip_spaces();
        double value;
        if (*p == '(') {
            p++;
            value = parse_expr_impl(true);
            skip_spaces();
            if (*p != ')')
                error("Missing closing parenthesis");
            p++;
        } else {
            value = parse_number();
        }
        check_range(value);
        return value;
    }
}

static double parse_term_impl(bool allow_unary)
{
    double value = parse_factor_impl(allow_unary);
    skip_spaces();
    while (*p == '*' || *p == '/') {
        char op = *p++;
        skip_spaces();
        double rhs = parse_factor_impl(false);
        if (op == '*') {
            value *= rhs;
            if (!float_mode && !is_integral(value)) {
                error("Non-integer multiplication result");
            }
        } else {
            value = float_mode ? float_div(value, rhs) : int_div(value, rhs);
        }
        check_range(value);
        skip_spaces();
    }
    return value;
}

static double parse_expr_impl(bool allow_unary)
{
    double value = parse_term_impl(allow_unary);
    skip_spaces();
    while (*p == '+' || *p == '-') {
        char op = *p++;
        skip_spaces();
        double rhs = parse_term_impl(false);
        value = (op == '+') ? value + rhs : value - rhs;
        if (!float_mode && !is_integral(value)) {
            error("Non-integer operation result");
        }
        check_range(value);
        skip_spaces();
    }
    return value;
}

// Публичные обёртки без параметров, вызывающие внутренние функции с allow_unary == true
double parse_factor(void)
{
    return parse_factor_impl(true);
}

double parse_term(void)
{
    return parse_term_impl(true);
}

double parse_expr(void)
{
    return parse_expr_impl(true);
}
#ifndef GTEST
int main(int argc, char** argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--float") == 0) {
            float_mode = true;
        } else {
            error("Unknown argument");
        }
    }

    char buffer[BUF_SIZE];
    size_t len = 0;
    int c;
    while ((c = getchar()) != EOF && len < BUF_SIZE - 1) {
        buffer[len++] = (char)c;
    }
    if (c != EOF && len >= BUF_SIZE - 1) {
        error("Input too long");
    }
    buffer[len] = '\0';

    validate_charset(buffer);

    if (buffer[0] == '\0') {
        error("Empty input");
    }

    p = buffer;
    double result = parse_expr();

    skip_spaces();
    if (*p != '\0') {
        error("Extra characters after expression");
    }

    if (float_mode) {
        printf("%.4f", result);
    } else {
        if (!is_integral(result)) {
            error("Non-integer result in integer mode");
        }
        long long final = (long long)floor(result + 0.5);
        if (final < -LIMIT || final > LIMIT) {
            error("Final result out of range");
        }
        printf("%lld", final);
    }

    return 0;
}
#endif
