#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define BUF_SIZE 1024

static const char *p;

void skip_spaces(void) {
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
}

int parse_number(void) {
    int result = 0;
    while (*p && isdigit((unsigned char)*p)) {
        result = result * 10 + (*p - '0');
        p++;
    }
    return result;
}

int parse_factor(void);
int parse_term(void);
int parse_expr(void);

int parse_factor(void) {
    skip_spaces();
    int value = 0;

    if (*p == '(') {
        p++;
        value = parse_expr();
        skip_spaces();
        if (*p == ')') {
            p++;
       } else {

        }
    } else {
        value = parse_number();
    }
    return value;
}

int parse_term(void) {
    int value = parse_factor();
    skip_spaces();
    while (*p == '*' || *p == '/') {
        char op = *p;
        p++;
        skip_spaces();
        int rhs = parse_factor();
        skip_spaces();
        if (op == '*') {
            value *= rhs;
        } else {
            value /= rhs;
        }
    }
    return value;
}

int parse_expr(void) {
    int value = parse_term();
    skip_spaces();
    while (*p == '+' || *p == '-') {
        char op = *p;
        p++;
        skip_spaces();
        int rhs = parse_term();
        skip_spaces();
        if (op == '+') {
            value += rhs;
        } else {
            value -= rhs;
        }
    }
    return value;
}

void remove_spaces(char *str) {
    char *dst = str;
    for (char *src = str; *src; src++) {
        if (!isspace((unsigned char)*src)) {
            *dst++ = *src;
        }
    }
    *dst = '\0';
}

int main(void) {
    char buffer[BUF_SIZE];
    size_t len = 0;
    int c;

    while ((c = getchar()) != EOF && len < BUF_SIZE - 1) {
        buffer[len++] = (char)c;
    }
    buffer[len] = '\0';

    remove_spaces(buffer);

    p = buffer;

    int result = parse_expr();

    printf("%d\n", result);

    return 0;
}
