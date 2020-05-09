#include "leptjson.h"
#include <string.h>
#include <cctype>
#include <errno.h>
#include <math.h>

namespace feiyan
{
    JsonContext::JsonContext(size_t length)
    {
        stack_ = static_cast<char *>(malloc(length));
        size_ = length;
        top_ = 0;
    }

    void JsonContext::push(const char *string, size_t length)
    {
        //容量不足，扩大1.5倍
        while (top_ + length >= size_)
            size_ += size_ >> 1;
        stack_ = static_cast<char *>(realloc(stack_, size_));
        memcpy(stack_ + top_, string, length);
        top_ += length;
    }

    void JsonContext::pushChar(const char ch)
    {
        char c = ch;
        push(&c, 1);
    }

    void JsonContext::pop(size_t length)
    {
        assert(top_ >= length);
        top_ -= length;
    }

    void Json::LeptFree()
    {
        if (type_ == StringType)
            free(string_.s_);
        type_ == NullType;
    }

    void Json::setBool(bool b)
    {
        LeptFree();
        if (b)
            type_ = TrueType;
        else
            type_ = FalseType;
    }

    bool Json::getBool()
    {
        assert(type_ == TrueType || type_ == FalseType);
        return type_ == TrueType;
    }

    void Json::setNumber(double n)
    {
        LeptFree();
        type_ = NumberType;
        n_ = n;
    }

    double Json::getNumber()
    {
        assert(type_ == NumberType);
        return n_;
    }

    void Json::setString(const char *string, size_t length)
    {
        assert(string != nullptr || length == 0);
        LeptFree();
        string_.s_ = static_cast<char *>(malloc(length + 1));
        memcpy(string_.s_, string, length);
        *(string_.s_ + length) = '\0';
        string_.len_ = length;
        type_ = StringType;
    }

    const char *Json::getString()
    {
        assert(type_ == StringType);
        return string_.s_;
    }

    size_t Json::getStringLength()
    {
        assert(type_ == StringType);
        return string_.len_;
    }

    void Json::parseWs()
    {
        const char *p = stream_;
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
            p++;
        stream_ = p;
    }

    JsonStat Json::expect(const char *string)
    {
        const char *p = stream_;
        if (memcmp(p, string, strlen(string)))
            return InvalidValue;
        stream_ += strlen(string);
        return OK;
    }

    JsonStat Json::parseLiteral(const char *string, JsonType type)
    {
        if (OK == expect(string))
        {
            type_ = type;
            return OK;
        }
        else
        {
            return InvalidValue;
        }
    }

    //少了必要的元素会返回InvalidValue
    //如果多了元素，会在parseValue时返回RootNotSingular，例如开头有多个0
    JsonStat Json::parseNumber()
    {
        const char *p = stream_;
        if (*p == '-')
            ++p;
        if (*p == '0')
            ++p;
        else
        {
            if (!isdigit(*p))
                return InvalidValue;
            while (isdigit(*p))
                ++p;
        }
        if (*p == '.')
        {
            ++p;
            if (!isdigit(*p))
                return InvalidValue;
            while (isdigit(*p))
                ++p;
        }
        if (*p == 'e' || *p == 'E')
        {
            ++p;
            if (*p == '+' || *p == '-')
                ++p;
            if (!isdigit(*p))
                return InvalidValue;
            while (isdigit(*p))
                ++p;
        }

        //一些标准库函数通过写入正整数到 errno 指定错误。
        //通常，会将 errno 的值设置为错误码之一。
        //错误码作为以字母 E 后随大写字母或数字开始的宏常量，列于 <errno.h> 。
        errno = 0;
        n_ = strtod(stream_, nullptr);
        if (errno == ERANGE && (n_ == HUGE_VAL || n_ == -HUGE_VAL))
            return NumberTooBig;
        type_ = NumberType;
        stream_ = p;
        return OK;
    }

    JsonStat Json::parseString(JsonContext &context_)
    {
        expect("\"");
        const char *p = stream_;
        size_t head = context_.top_; //记录一开始的栈顶位置
        size_t len;
        for (;; ++p)
        {
            switch (*p)
            {
            case '\"':                      //结束标志
                len = context_.top_ - head; //求出字符串长度
                setString(context_.stack_ + head, len);
                context_.pop(len);
                stream_ = ++p;
                return OK;
                break;
            case '\0': //提前结束，字符串不完整
                context_.top_ = head;
                return MissQuotationMark;
            case '\\': //处理转义字符
                ++p;
                switch (*p)
                {
                case '\"':
                    context_.pushChar('\"');
                    break;
                case '\\':
                    context_.pushChar('\\');
                    break;
                case '/':
                    context_.pushChar('/');
                    break;
                case 'b':
                    context_.pushChar('\b'); //退格
                    break;
                case 'f':
                    context_.pushChar('\f'); //进纸符，换页
                    break;
                case 'n':
                    context_.pushChar('\n'); //换行
                    break;
                case 'r':
                    context_.pushChar('\r'); //回车
                    break;
                case 't':
                    context_.pushChar('\t'); //tab
                    break;
                default:
                    context_.top_ = head;
                    return InvalidStringEscape;
                    break;
                };
                break;
            default:
                if (static_cast<unsigned char>(*p) < 0x20)
                {
                    context_.top_ = head;
                    return InvalidStringChar;
                }
                context_.pushChar(*p);
                break;
            }
        }
    }

    JsonStat Json::parseValue(JsonContext &context_)
    {
        switch (*stream_)
        {
        case 'n':
            return parseLiteral("null", NullType);
        case 't':
            return parseLiteral("true", TrueType);
        case 'f':
            return parseLiteral("false", FalseType);
        case '\"':
            return parseString(context_);
        case '\0':
            return ExpectValue;
        //无论default位置在前在后，都是先判断各个case, 最后进default。
        default:
            return parseNumber();
        }
    }

    JsonStat Json::parse(const char *json)
    {
        stream_ = json;
        JsonContext context_;
        parseWs();
        JsonStat res = parseValue(context_);
        if (res == OK)
        {
            parseWs();
            if (*stream_ != '\0')
            {
                type_ = NullType;
                return RootNotSingular;
            }
        }
        return res;
    }

} // namespace feiyan
