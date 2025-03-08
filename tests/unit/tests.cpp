#define GTEST
#include <gtest/gtest.h>

extern "C" {
#include "../../src/calculator.c"
}

TEST(ParseNumberTest, ValidNumber)
{
    const char* input = "12345";
    p = input;
    double result = parse_number();
    EXPECT_EQ(result, 12345);
    EXPECT_EQ(*p, '\0');
}

TEST(ParseNumberTest, ExceedsLimit)
{
    const char* input = "2000000001";
    p = input;
    EXPECT_EXIT(parse_number(), ::testing::ExitedWithCode(1), "Error: Number out of allowed");
}

TEST(IsAllowedCharTest, ValidChars)
{
    EXPECT_TRUE(is_allowed_char(' '));
    EXPECT_TRUE(is_allowed_char('5'));
    EXPECT_TRUE(is_allowed_char('+'));
}

TEST(IsAllowedCharTest, InvalidChars)
{
    EXPECT_FALSE(is_allowed_char('a'));
    EXPECT_FALSE(is_allowed_char('$'));
}

TEST(SkipSpacesTest, SkipsSpaces)
{
    const char* input = "   123";
    p = input;
    skip_spaces();
    EXPECT_EQ(*p, '1');
}

TEST(CheckRangeTest, WithinRange)
{
    check_range(1000);
}

TEST(CheckRangeTest, OutsideRange)
{
    EXPECT_EXIT(check_range(LIMIT + 1), ::testing::ExitedWithCode(1), "Error: Intermediate");
}

TEST(IntDivTest, PositiveResult)
{
    double result = int_div(10, 3);
    EXPECT_EQ(result, 3);
}

TEST(IntDivTest, NegativeResult)
{
    double result = int_div(-10, 3);
    EXPECT_EQ(result, -4);
}

TEST(FloatDivTest, NormalDivision)
{
    float_mode = true;
    double result = float_div(10, 4);
    EXPECT_DOUBLE_EQ(result, 2.5);
}

TEST(ParseFactorTest, UnaryMinus)
{
    const char* input = "-123";
    p = input;
    double result = parse_factor();
    EXPECT_EQ(result, -123);
}

TEST(ParseFactorTest, Parentheses)
{
    const char* input = "(456)";
    p = input;
    double result = parse_factor();
    EXPECT_EQ(result, 456);
}

TEST(ParseTermTest, Multiplication)
{
    const char* input = "2*3*4";
    p = input;
    double result = parse_term();
    EXPECT_EQ(result, 24);
}

TEST(ParseTermTest, Division)
{
    const char* input = "20/3";
    p = input;
    float_mode = false;
    double result = parse_term();
    EXPECT_EQ(result, 6);
}

TEST(ParseExprTest, AdditionSubtraction)
{
    const char* input = "5+3-2";
    p = input;
    double result = parse_expr();
    EXPECT_EQ(result, 6);
}

TEST(ParseExprTest, MixedOperations)
{
    const char* input = "2+3*4";
    p = input;
    double result = parse_expr();
    EXPECT_EQ(result, 14);
}

TEST(RemoveSpacesTest, RemovesAllSpaces)
{
    char input[] = " 12 + 34 ";
    remove_spaces(input);
    EXPECT_STREQ(input, "12+34");
}

TEST(ValidateCharsetTest, InvalidChar)
{
    const char* input = "5 + a";
    EXPECT_EXIT(validate_charset(input), ::testing::ExitedWithCode(1), "Error: Invalid character");
}
