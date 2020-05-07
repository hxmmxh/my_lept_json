#include "leptjson.h"
#include <string.h>

namespace feiyan
{

    void Json::parseWs()
    {
        const char *p = context_.data_;
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
            p++;
        context_.data_ = p;
    }

    JsonStat Json::expect(const char *string)
    {
        const char *p = context_.data_;
        if (memcmp(p, string, strlen(string)))
            return InvalidValue;
        context_.data_ += strlen(string);
        return OK;
    }

    JsonStat Json::parseNull()
    {
        if (OK == expect("null"))
            return OK;
        else
        {
            return InvalidValue;
        }
    }

    JsonStat Json::parseTrue()
    {
        if (OK == expect("true"))
        {
            type_ = TrueType;
            return OK;
        }
        else
        {
            return InvalidValue;
        }
    }

    JsonStat Json::parseFalse()
    {
        if (OK == expect("false"))
        {
            type_ = FalseType;
            return OK;
        }
        else
        {
            return InvalidValue;
        }
    }

    JsonStat Json::parseValue()
    {
        switch (*context_.data_)
        {
        case 'n':
            return parseNull();
        case 't':
            return parseTrue();
        case 'f':
            return parseFalse();
        case '\0':
            return ExpectValue;
        default:
            return InvalidValue;
        }
    }

    JsonStat Json::parse(const char *json)
    {
        context_.data_ = json;
        type_ = NullType;
        parseWs();
        JsonStat res = parseValue();
        if (res == OK)
        {
            parseWs();
            if (*context_.data_ != '\0')
                return RootNotSingular;
        }
        return res;
    }

} // namespace feiyan
