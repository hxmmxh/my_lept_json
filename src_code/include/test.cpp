#include "leptjson.h"
#include <iostream>
#include <string>

using namespace feiyan;

static int test_count = 0;
static int test_pass = 0;

#define CHECKEQUAL(expect, actual)                                                            \
    do                                                                                        \
    {                                                                                         \
        test_count++;                                                                         \
        if ((expect) == (actual))                                                             \
            test_pass++;                                                                      \
        else                                                                                  \
        {                                                                                     \
            std::cout << "line: " << __LINE__ << "error:   " << #expect << " != " << #actual; \
            std::cout << "    " #expect << " = " << expect;                                   \
            std::cout << "    " #actual << " = " << actual << std::endl;                      \
        }                                                                                     \
    } while (0)

#define TEST_NUMBER(expect, json)            \
    do                                       \
    {                                        \
        Json v;                              \
        CHECKEQUAL(OK, v.parse(json));       \
        CHECKEQUAL(NumberType, v.getType()); \
        CHECKEQUAL(expect, v.getNumber());   \
    } while (0)

#define TEST_STRING(expect, json)                                     \
    do                                                                \
    {                                                                 \
        Json v;                                                       \
        CHECKEQUAL(OK, v.parse(json));                                \
        CHECKEQUAL(StringType, v.getType());                          \
        CHECKEQUAL(expect, std::string(v.getString(), v.getStringLength())); \
    } while (0)

#define TEST_ERROR(error, json)            \
    do                                     \
    {                                      \
        Json v;                            \
        CHECKEQUAL(error, v.parse(json));  \
        CHECKEQUAL(NullType, v.getType()); \
    } while (0)

//不要在头文件中声明static的全局函数，不要在cpp内声明非static的全局函数，
//如果你要在多个cpp中复用该函数，就把它的声明提到头文件里去，否则cpp内部声明需加上static修饰
//因为 static 函数的意思是指，该函数只作用于编译单元中，那么没有被调用时，编译器是能发现的。

static void test_parse_null()
{

    Json v;
    v.setBool(false);
    CHECKEQUAL(OK, v.parse("null"));
    CHECKEQUAL(NullType, v.getType());
}

static void test_parse_true()
{

    Json v;
    v.setBool(false);
    CHECKEQUAL(OK, v.parse("true"));
    CHECKEQUAL(TrueType, v.getType());
    CHECKEQUAL(true, v.getBool());
}

static void test_parse_false()
{

    Json v;
    v.setBool(true);
    CHECKEQUAL(OK, v.parse("false"));
    CHECKEQUAL(FalseType, v.getType());
    CHECKEQUAL(false, v.getBool());
}

static void test_parse_number()
{
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002");           /* the smallest number > 1 */
    TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308"); /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308"); /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308"); /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

static void test_parse_expect_value()
{
    TEST_ERROR(ExpectValue, "");
    TEST_ERROR(ExpectValue, " ");
}

static void test_parse_invalid_value()
{
    TEST_ERROR(InvalidValue, "nul");
    TEST_ERROR(InvalidValue, "? ");
    TEST_ERROR(InvalidValue, " sb");

    TEST_ERROR(InvalidValue, "+0");
    TEST_ERROR(InvalidValue, "+1");
    TEST_ERROR(InvalidValue, ".12345");
    TEST_ERROR(InvalidValue, "1.");
    TEST_ERROR(InvalidValue, "INF");
    TEST_ERROR(InvalidValue, "inf");
    TEST_ERROR(InvalidValue, "inf");
    TEST_ERROR(InvalidValue, "NAN");
    TEST_ERROR(InvalidValue, "nan");
}

static void test_parse_root_not_singular()
{
    TEST_ERROR(RootNotSingular, "null xxx");

    TEST_ERROR(RootNotSingular, "0000");
    TEST_ERROR(RootNotSingular, "0123");
    TEST_ERROR(RootNotSingular, "0sb");
}

static void test_parse_number_too_big()
{
    TEST_ERROR(NumberTooBig, "1e333");
    TEST_ERROR(NumberTooBig, "1123123e308");
    TEST_ERROR(NumberTooBig, "-1123123e308");
    TEST_ERROR(NumberTooBig, "-1e309");
}

static void test_parse_string()
{
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
}

static void test_parse_missing_quotation_mark()
{
    TEST_ERROR(MissQuotationMark, "\"");
    TEST_ERROR(MissQuotationMark, "\"abc");
}

static void test_parse_invalid_string_escape()
{
    TEST_ERROR(InvalidStringEscape, "\"\\v\"");
    TEST_ERROR(InvalidStringEscape, "\"\\'\"");
    TEST_ERROR(InvalidStringEscape, "\"\\0\"");
    TEST_ERROR(InvalidStringEscape, "\"\\x12\"");
}

static void test_parse_invalid_string_char()
{
    TEST_ERROR(InvalidStringChar, "\"\x01\"");
    TEST_ERROR(InvalidStringChar, "\"\x1F\"");
}


static void test_parse()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_number_too_big();
    test_parse_string();
    test_parse_missing_quotation_mark();
    test_parse_invalid_string_escape();
    test_parse_invalid_string_char();
}

int main()
{
    test_parse();
    std::cout << test_pass << "/" << test_count << " passed\n";
}