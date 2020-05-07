#include "leptjson.h"
#include <iostream>

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

//不要在头文件中声明static的全局函数，不要在cpp内声明非static的全局函数，
//如果你要在多个cpp中复用该函数，就把它的声明提到头文件里去，否则cpp内部声明需加上static修饰
//因为 static 函数的意思是指，该函数只作用于编译单元中，那么没有被调用时，编译器是能发现的。

static void test_parse_null()
{

    Json v;
    CHECKEQUAL(OK, v.parse("null"));
    CHECKEQUAL(NullType, v.getType());
}

static void test_parse_true()
{

    Json v;
    CHECKEQUAL(OK, v.parse("true"));
    CHECKEQUAL(TrueType, v.getType());
}

static void test_parse_false()
{

    Json v;
    CHECKEQUAL(OK, v.parse("false"));
    CHECKEQUAL(FalseType, v.getType());
}

static void test_parse_expect_value()
{
    Json v;
    CHECKEQUAL(ExpectValue, v.parse(""));
    CHECKEQUAL(NullType, v.getType());
    CHECKEQUAL(ExpectValue, v.parse(" "));
    CHECKEQUAL(NullType, v.getType());
}

static void test_parse_invalid_value()
{
    Json v;
    CHECKEQUAL(InvalidValue, v.parse("nul"));
    CHECKEQUAL(NullType, v.getType());
    CHECKEQUAL(InvalidValue, v.parse("asdfas"));
    CHECKEQUAL(NullType, v.getType());
}

static void test_parse_root_not_singular()
{
    Json v;
    CHECKEQUAL(RootNotSingular, v.parse("null xa"));
    CHECKEQUAL(NullType, v.getType());
}

static void test_parse()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main()
{
    test_parse();
    std::cout << test_pass << "/" << test_count << " passed\n";
}