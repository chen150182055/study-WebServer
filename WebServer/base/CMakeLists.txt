set(LIB_SRC
    AsyncLogging.cpp
    CountDownLatch.cpp
    FileUtil.cpp
    LogFile.cpp
    Logging.cpp
    LogStream.cpp
    Thread.cpp
)   #设置LIB_SRC变量的值

add_library(libserver_base ${LIB_SRC})  #生成一个名为libserver_base的库
target_link_libraries(libserver_base pthread rt)    #指定链接给定目标和/或其依赖项时要使用的库

set_target_properties(libserver_base PROPERTIES OUTPUT_NAME "server_base")  #设置libserver_base的属性

add_subdirectory(tests) #在构建中添加一个子目录