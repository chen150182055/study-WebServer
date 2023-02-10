set(LIB_SRC
    AsyncLogging.cpp
    CountDownLatch.cpp
    FileUtil.cpp
    LogFile.cpp
    Logging.cpp
    LogStream.cpp
    Thread.cpp
)   #设置LIB_SRC变量的值

add_library(libserver_base ${LIB_SRC})              #构建一个名为libserver_base的库,包含一系列源文件
target_link_libraries(libserver_base pthread rt)    #将pthread和rt库链接到该库上

set_target_properties(libserver_base PROPERTIES OUTPUT_NAME "server_base")  #将该库的输出名称设置为server_base

add_subdirectory(tests)                             #添加一个名为tests的子目录到构建中