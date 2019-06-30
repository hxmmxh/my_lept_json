#ifndef LEPT_JSON_H
#define LEPT_JSON_H

#include <cstdlib> // malloc(), realloc(), free(), size_t
#include <cstring> // memset(), memcpy(), memmove(), memcmp()

#ifndef LEPTJSON_NAMESPACE
#define LEPTJSON_NAMESPACE leptjson
#endif
#ifndef LEPTJSON_NAMESPACE_BEGIN
#define LEPTJSON_NAMESPACE_BEGIN \
    namespace LEPTJSON_NAMESPACE \
    {
#endif
#ifndef LEPTJSON_NAMESPACE_END
#define LEPTJSON_NAMESPACE_END }
#endif

//为了字节对齐把x调整成8的倍数
#ifndef LEPTJSON_ALIGN
#define LEPTJSON_ALIGN(x) (((x) + static_cast<size_t>(7u)) & ~static_cast<size_t>(7u))
#endif

//assert
#ifndef LEPTJSON_ASSERT
#include <cassert>
#define LEPTJSON_ASSERT(x) assert(x)
#endif

LEPTJSON_NAMESPACE_BEGIN

typedef unsigned SizeType;
using std::size_t;
enum Type
{
    NullType = 0,   // null
    FalseType = 1,  // false
    TrueType = 2,   // true
    ObjectType = 3, // object
    ArrayType = 4,  // array
    StringType = 5, // string
    NumberType = 6  // number
};
LEPTJSON_NAMESPACE_END

#endif