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

* JSON解析需要完成的需求
  * 能把 JSON 文本解析为一个树状数据结构（parse）。
  * 能提供接口访问该数据结构（access）。
  * 能把数据结构转换成 JSON 文本（stringify）

## 代码结构

- leptjson.h 定义了一些常用宏
- allocator.h 空间配置器相关函数



轻量级的json解释程序  
参照[从零开始的 JSON 库教程](https://github.com/hxmmxh/json-tutorial)以及[RapidJSON](http://miloyip.github.io/rapidjson/zh-cn/md_doc_tutorial_8zh-cn.html)
