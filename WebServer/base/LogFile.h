// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <memory>
#include <string>
#include "FileUtil.h"
#include "MutexLock.h"
#include "noncopyable.h"

// TODO 提供自动归档功能
//LogFile进一步封装FileUtil.h
//于将日志记录到文件中，它继承了noncopyable类,
// noncopyable类用于禁止拷贝构造函数和赋值操作符
class LogFile : noncopyable {
 public:
  // 每被append flushEveryN次，flush一下，会往文件写，只不过，文件也是带缓冲区的
  LogFile(const std::string &basename, int flushEveryN = 1024);
  ~LogFile();

  void append(const char *logline, int len);
  void flush();
  bool rollFile();

 private:
  void append_unlocked(const char *logline, int len);

  const std::string basename_;
  const int flushEveryN_;

  int count_;
  std::unique_ptr<MutexLock> mutex_;
  std::unique_ptr<AppendFile> file_;
};