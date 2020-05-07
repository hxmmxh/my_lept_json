#ifndef LEPT_JSON_H
#define LEPT_JSON_H

#include <cstdlib> // malloc(), realloc(), free(), size_t
#include <cstring> // memset(), memcpy(), memmove(), memcmp()

//为了字节对齐把x调整成8的倍数
#ifndef LEPTJSON_ALIGN
#define LEPTJSON_ALIGN(x) (((x) + static_cast<size_t>(7u)) & ~static_cast<size_t>(7u))
#endif

namespace feiyan
{
    enum JsonType
    {
        NullType = 0,   // null
        FalseType = 1,  // false
        TrueType = 2,   // true
        NumberType = 3, // number
        StringType = 4, // string
        ArrayType = 5,  // array
        ObjectType = 6  // object
    };
    enum JsonStat
    {
        OK = 0,
        ExpectValue = 1,//只含有空白
        InvalidValue = 2,//不符合规范的值
        RootNotSingular = 3//空白之后还含有其它字符
    };

    class JsonContext
    {
    public:
        const char *data_;
    };

    class Json
    {
    public:
        JsonStat parse(const char *);
        JsonType getType() { return type_; }

    private:
        void parseWs();//空白可以有零个或多个，肯定成功，不需要返回状态
        JsonStat expect(const char *);
        JsonStat parseNull();
        JsonStat parseValue();
        JsonStat parseTrue();
        JsonStat parseFalse();

    private:
        JsonType type_;
        JsonContext context_;
    };
} // namespace feiyan

#endif