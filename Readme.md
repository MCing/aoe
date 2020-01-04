## 简介

AOE 是一个底层基于TBOX，上层集成rt-thread(AT CLIENT组件)/LWIP(TCPIP/PPP)的AT通信模块测试工具。

本工具基于C语言开发，可以实现：

1.AT指令测试

2.ppp拨号测试

3.基于ppp拨号的TCP/IP 应用层测试





## 编译环境

### 1.下载xmake(windows)

https://github.com/xmake-io/xmake/releases

### 2.安装 mingw (解压后即可使用)

http://pdidc.jb51.net:81/201910/tools/x86_64-8.1.0-release-win32-seh-rt_v6-rev0.7z

### 3.配置编译环境

进入根xmake.lua路径，即tbox目录

xmake f -p mingw --sdk="G:/mingw64"

其中 sdk 路径为 mingw64解压后的路径（bin的上一层路径）

### 4.编译

xmake

### 5.运行

xmake run







