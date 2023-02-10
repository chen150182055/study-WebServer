// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <string>
#include "noncopyable.h"

/**
 * 最底层的文件类,封装log文件的打开、写入等
 * 用于最底层文件的封装，提供打开文件，写入，
 * 关闭文件等功能，变量有文件指针，缓冲区等
 */
class AppendFile : noncopyable {
 public:
  explicit AppendFile(std::string filename);
  ~AppendFile();
  // append 会向文件写
  void append(const char *logline, const size_t len);
  void flush();

 private:
  size_t write(const char *logline, size_t len);
  FILE *fp_;
  char buffer_[64 * 1024];
};