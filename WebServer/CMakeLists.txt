set(SRCS
    Channel.cpp
    Epoll.cpp
    EventLoop.cpp
    EventLoopThread.cpp
    EventLoopThreadPool.cpp
    HttpData.cpp
    Main.cpp
    Server.cpp
    ThreadPool.cpp
    Timer.cpp
    Util.cpp
)  #设置SRCS的值
include_directories(${PROJECT_SOURCE_DIR}/base) #将包含的目录添加到构建


add_executable(WebServer ${SRCS})   #使用指定的源文件将可执行文件添加到项目中,这里是上面变量的文件名
target_link_libraries(WebServer libserver_base) #为WebServer连接库的头文件路径和对应定义的函数库路径


add_subdirectory(base) #添加子目录base并构建该子目录base
add_subdirectory(tests) #添加子目录tests并构建该子目录tests