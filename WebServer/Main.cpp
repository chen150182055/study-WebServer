// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include <getopt.h>
#include <string>
#include "EventLoop.h"
#include "Server.h"
#include "base/Logging.h"


int main(int argc, char *argv[]) {
  int threadNum = 4;    //定义初始线程数量
  int port = 80;        //定义默认端口
  std::string logPath = "./WebServer.log";   //默认日志文件

  // parse args
  int opt;        //获取命令行参数
  const char *str = "t:l:p:";  //短参数选项
  while ((opt = getopt(argc, argv, str)) != -1) {  //如果短参数不为空
    switch (opt) {    //不同的短参数选项,不同的意义
      case 't': {     //线程数量
        threadNum = atoi(optarg);  //将命令中的字符串形式的数字转换为整数,同时赋予线程数
        break;
      }
      case 'l': {     //日志路径
        logPath = optarg;  //将命令中的日志目录设置为程序的日志目录
        if (logPath.size() < 2 || optarg[0] != '/') {   //输入的路径长度不能太短且开头必须为"/"
          printf("logPath should start with \"/\"\n"); //输出错误
          abort();    //终止程序
        }
        break;
      }
      case 'p': {     //端口号
        port = atoi(optarg);  //将命令中的字符串形式的数字转换为整数并设置为端口号
        break;
      }
      default:
        break;
    }
  }
  Logger::setLogFileName(logPath);  //初始化日志的路径
// STL库在多线程上应用
#ifndef _PTHREADS
  LOG << "_PTHREADS is not defined !";
#endif
  EventLoop mainLoop;  //声明一个EventLoop对象
  Server myHTTPServer(&mainLoop, threadNum, port);  //声明一个Server对象,并使用构造函数初始化
  myHTTPServer.start();  //调用Server类中的start()函数
  mainLoop.loop();       //调用EventLoop类中的loop()函数
  return 0;
}
