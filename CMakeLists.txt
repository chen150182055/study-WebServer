cmake_minimum_required(VERSION 3.22.1)

project(WebServer CXX)      #指定cmake工程名称为WebServer

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")       #指定构建类型为Debug
endif()

set(CXX_FLAGS
    -g
    -Wall
    -std=c++11
    -D_PTHREADS
    -Wno-unused-parameter
)               #设置变量CXX_FLAGS的值


set(CMAKE_CXX_COMPILER "g++")   #设置变量CMAKE_CXX_COMPILER的值
set(CMAKE_CXX_FLAGS_DEBUG "-O0")    #设置变量CMAKE_CXX_FLAGS_DEBUG的值
set(CMAKE_CXX_FLAGS_RELEASE "-O0")  #设置变量CMAKE_CXX_FLAGS_RELEASE的值

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}") #字符串替换操作


string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)  #字符串转换为大写操作
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})      #  打印日志

add_subdirectory(WebServer)     #将目录WebServer添加到构建