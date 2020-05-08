# 目录

1. [JSON介绍](#JSON介绍)

## JSON介绍

* JSON（JavaScript Object Notation）是一个用于数据交换的文本格式
* JSON是树状结构，包含 6 种数据类型：
  * null: 表示为 null
  * boolean: 表示为 true 或 false
  * number: 一般的浮点数表示方式，详细说明见[number](#number)
  * string: 表示为 "..."
  * array: 表示为 [ ... ]
  * object: 表示为 { ... }
* JSON语法
  * %xhh 表示以 16 进制表示的字符，/ 是多选一，* 是零或多个，( ) 用于分组
  * JSON 文本由 3 部分组成，首先是空白（whitespace），接着是一个值，最后是空白。
  * 空白，是由零或多个空格符（space U+0020）、制表符（tab U+0009）、换行符（LF U+000A）、回车符（CR U+000D）所组成。
  * 值可以是上述6种数据类型

```json
JSON-text = ws value ws
ws = *(%x20 / %x09 / %x0A / %x0D)
value = null / false / true
null  = "null"
false = "false"
true  = "true"
```

* 数字语法
  * JSON的number类型以十进制表示，它主要由 4 部分顺序组成：负号、整数、小数、指数。其中整数是必需部分。
  * 整数部分如果是 0 开始，只能是单个 0；而由 1-9 开始的话，可以加任意数量的数字（0-9）。也就是说，`0123` 不是一个合法的 JSON 数字。
  * 小数部分比较直观，就是小数点后是一或多个数字（0-9）。
  * 指数部分由大写 E 或小写 e 开始，然后可有正负号，之后是一或多个数字（0-9）。

```c
number = [ "-" ] int [ frac ] [ exp ]
int = "0" / digit1-9 *digit
frac = "." 1*digit
exp = ("e" / "E") ["-" / "+"] 1*digit

```

* 字符串语法
  * 由前后两个双引号夹着零至多个字符。字符分为无转义字符或转义序列。
  * 使用 \（反斜线）作为转义字符
  * \uXXXX，当中 XXXX 为 16 进位的 UTF-16 编码，

```c
string = quotation-mark *char quotation-mark
char = unescaped /
   escape (
       %x22 /          ; "    quotation mark  U+0022
       %x5C /          ; \    reverse solidus U+005C
       %x2F /          ; /    solidus         U+002F
       %x62 /          ; b    backspace       U+0008
       %x66 /          ; f    form feed       U+000C
       %x6E /          ; n    line feed       U+000A
       %x72 /          ; r    carriage return U+000D
       %x74 /          ; t    tab             U+0009
       %x75 4HEXDIG )  ; uXXXX                U+XXXX
escape = %x5C          ; \
quotation-mark = %x22  ; "
unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
```









* JSON解析需要完成的需求
  * 能把 JSON 文本解析为一个树状数据结构（parse）。
  * 能提供接口访问该数据结构（access）。
  * 能把数据结构转换成 JSON 文本（stringify）

## 代码结构

- leptjson.h 定义了一些常用宏
- allocator.h 空间配置器相关函数



轻量级的json解释程序  
参照[从零开始的 JSON 库教程](https://github.com/hxmmxh/json-tutorial)以及[RapidJSON](http://miloyip.github.io/rapidjson/zh-cn/md_doc_tutorial_8zh-cn.html)
