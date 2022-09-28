add_executable(LoggingTest LoggingTest.cpp)     #使用LoggingTest.cpp源文件向项目添加一个可执行文件。
target_link_libraries(LoggingTest libserver_base)   #为LoggingTest连接库的头文件路径