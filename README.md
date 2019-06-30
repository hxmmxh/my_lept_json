# my_lept_-json
轻量级的json解释程序  
参照[从零开始的 JSON 库教程](https://github.com/hxmmxh/json-tutorial)以及[RapidJSON](http://miloyip.github.io/rapidjson/zh-cn/md_doc_tutorial_8zh-cn.html)

# 目录
1. json介绍



## JSON介绍


 JSON 包含 6 种数据类型：
* null: 表示为 null
* boolean: 表示为 true 或 false
* number: 一般的浮点数表示方式，在下一单元详细说明
* string: 表示为 "..."
* array: 表示为 [ ... ]
* object: 表示为 { ... }

## 代码结构

- leptjson.h 定义了一些常用宏
- allocator.h 空间配置器相关函数