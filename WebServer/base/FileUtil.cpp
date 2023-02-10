// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "FileUtil.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

/**
 * 用于新建一个新的文件,并对其进行追加操作
 * @param filename
 */
AppendFile::AppendFile(string filename) : fp_(fopen(filename.c_str(), "ae")) {
  // 用户提供缓冲区
  setbuffer(fp_, buffer_, sizeof buffer_);
}

/**
 *
 */
AppendFile::~AppendFile() {
  fclose(fp_);
}

/**
 * 用于把logline写入到fp_文件中
 * @param logline
 * @param len
 */
void AppendFile::append(const char *logline, const size_t len) {
  size_t n = this->write(logline, len);
  size_t remain = len - n;
  while (remain > 0) {
	size_t x = this->write(logline + n, remain);
	if (x == 0) {
	  int err = ferror(fp_);
	  if (err) fprintf(stderr, "AppendFile::append() failed !\n");
	  break;
	}
	n += x;
	remain = len - n;
  }
}

/**
 * 把fp_文件的缓冲数据刷新到文件中
 */
void AppendFile::flush() {
  fflush(fp_);
}

/**
 * 把logline写入到fp_文件中，并返回写入的字节数
 * @param logline
 * @param len
 * @return 写入的字节数
 */
size_t AppendFile::write(const char *logline, size_t len) {
  return fwrite_unlocked(logline, 1, len, fp_);
}