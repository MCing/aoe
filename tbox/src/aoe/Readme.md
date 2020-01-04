# 初衷：用C语言构建模块（主要是AT业务）自动化测试工具；

## 目前环境：

开源库：tbox/libUART

开发环境：windows；mingw 编译工具链；xmake构建工程；source insight 



## 前言：

为什么是windows?

--主要测试环境是在windows下；

为什么是xmake？

--支持跨平台构建，但其实目前只是想能在windows下不依赖IDE（VC++/VS)构建项目；

为什么是C语言？

--最近java/python 遇到性能瓶颈，有必要出一个软件工程师用的自动化测试工具；

为什么基于tbox?

--windows 下编程不熟，API资料不好找，而tbox是跨平台的，封装了很多平台相关的接口（线程/socket/定时器等），不需要关注细节，以后想了解平台的实现方式可以研究其代码；后续还会根据需求加入其他开源库。





正则表达式



## 需求：

1.测试指令格式

<AT CMD> <result> <timeout> <run time>

正则表达式



2.处理主动上报

<start str> <end str> <handler>



3.获取特定回码

OK/ERROR/>/CONNECT











