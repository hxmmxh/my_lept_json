#ifndef LEPT_JSON_H
#define LEPT_JSON_H

#include <cstdlib> // malloc(), realloc(), free(), size_t
#include <cstring> // memset(), memcpy(), memmove(), memcmp()
#include <assert.h>

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
        ExpectValue = 1,         //只含有空白
        InvalidValue = 2,        //不符合规范的值
        RootNotSingular = 3,     //空白之后还含有其它字符
        NumberTooBig = 4,        //数值过大
        MissQuotationMark = 5,   //丢失引号
        InvalidStringEscape = 6, //转义字符错误
        InvalidStringChar = 7    //普通字符错误

    };

    class JsonContext
    {
    public:
        JsonContext(size_t length = defaultInitSize);
        ~JsonContext()
        {
            assert(top_ == 0);
            free(stack_);
        }
        void reset();
        void push(const char *, size_t);
        void pushChar(char);
        void pop(size_t);

    public:
        static const size_t defaultInitSize = 256;

        char *stack_;
        size_t size_; //栈的容量
        size_t top_;  //栈顶位置
    };

    class Json
    {
    public:
        Json() : type_(NullType) {}
        ~Json() { LeptFree(); }

        JsonStat parse(const char *);
        JsonType getType() { return type_; }

        void setNull() { LeptFree(); }

        void setBool(bool);
        bool getBool();

        void setNumber(double);
        double getNumber();

        void setString(const char *s, size_t len);
        const char *getString();
        size_t getStringLength();

    private:
        void parseWs(); //空白可以有零个或多个，肯定成功，不需要返回状态
        JsonStat expect(const char *);
        JsonStat parseLiteral(const char *, JsonType);
        JsonStat parseNumber();
        JsonStat parseValue(JsonContext &);
        JsonStat parseString(JsonContext &);
        void LeptFree();

    private:
        JsonType type_;
        const char *stream_;
        //匿名union，在右花括号和分号之间没有任何声明
        //在匿名union定义所在的作用域内，该union的成员可以直接访问
        union {
            double n_;
            struct
            {
                char *s_;
                size_t len_;
            } string_;
        };
    };
} // namespace feiyan

#endif