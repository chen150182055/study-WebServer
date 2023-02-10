// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "LogStream.h"

class AsyncLogging;

//对外接口
class Logger {
 public:
  Logger(const char *fileName, int line);
  ~Logger();

  /**
   * 返回日志流
   * @return
   */
  LogStream &stream() {
	return impl_.stream_;
  }

  /**
   * 用于设置日志文件的名称
   * @param fileName
   */
  static void setLogFileName(std::string fileName) {
	logFileName_ = fileName;
  }

  /**
   * 获取日志文件的名称
   * @return
   */
  static std::string getLogFileName() {
	return logFileName_;
  }

 private:
  //该类实现日志记录的功能
  class Impl {
   public:
	Impl(const char *fileName, int line);
	void formatTime();

	LogStream stream_;
	int line_;
	std::string basename_;
  };
  Impl impl_;
  static std::string logFileName_;	//存储日志文件的名称
};

//宏定义LOG，可以方便地获取日志流
#define LOG Logger(__FILE__, __LINE__).stream()