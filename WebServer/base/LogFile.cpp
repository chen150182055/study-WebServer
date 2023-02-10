// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "LogFile.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "FileUtil.h"

using namespace std;

/**
 * 构造函数实例化了一个文件追加类AppendFile，
 * 将写入的日志保存到文件中
 * @param basename 文件路径
 * @param flushEveryN 每写入N行日志就清空缓存
 */
LogFile::LogFile(const string &basename, int flushEveryN)
	: basename_(basename),
	  flushEveryN_(flushEveryN),
	  count_(0),
	  mutex_(new MutexLock) {
  // assert(basename.find('/') >= 0);
  file_.reset(new AppendFile(basename));
}

/**
 * 析构函数
 */
LogFile::~LogFile() {}

/**
 * 将日志写入文件中
 * @param logline
 * @param len
 */
void LogFile::append(const char *logline, int len) {
  MutexLockGuard lock(*mutex_);	//使用互斥锁保护
  append_unlocked(logline, len);
}

/**
 * 清空文件缓存
 */
void LogFile::flush() {
  MutexLockGuard lock(*mutex_);
  file_->flush();
}

/**
 * 更新文件缓存并写入到文件中
 * @param logline
 * @param len
 */
void LogFile::append_unlocked(const char *logline, int len) {
  file_->append(logline, len);
  ++count_;
  if (count_ >= flushEveryN_) {
	count_ = 0;
	file_->flush();
  }
}