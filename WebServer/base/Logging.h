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
  LogStream &stream() { return impl_.stream_; }

  /**
   * 将成员logFileName_的值设置为fileName
   * @param fileName
   */
  static void setLogFileName(std::string fileName) {
	logFileName_ = fileName;
  }

  /**
   * 返回成员logFileName_的值
   * @return
   */
  static std::string getLogFileName() {
	return logFileName_;
  }

 private:
  class Impl {
   public:
	Impl(const char *fileName, int line);
	void formatTime();

	LogStream stream_;
	int line_;
	std::string basename_;
  };
  Impl impl_;
  static std::string logFileName_;
};

#define LOG Logger(__FILE__, __LINE__).stream()