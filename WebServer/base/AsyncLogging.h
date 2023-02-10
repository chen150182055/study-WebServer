// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once			//表示这个文件只能被包含一次,防止重复定义
#include <functional>
#include <string>
#include <vector>
#include "CountDownLatch.h"
#include "LogStream.h"
#include "MutexLock.h"
#include "Thread.h"
#include "noncopyable.h"

//核心类,启动一个log线程，专门用来将log写入LogFile
//该类有一个构造函数，两个辅助函数(append()和threadFunc())以及start()和stop()函数
class AsyncLogging : noncopyable {
 public:
  AsyncLogging(const std::string basename, int flushInterval = 2);	//
  ~AsyncLogging() {
    if (running_) stop();
  }
  void append(const char* logline, int len);

  void start() {		//启动线程，并等待latch_计数为0
    running_ = true;
    thread_.start();
    latch_.wait();
  }

  void stop() {			//将running_置为false
    running_ = false;
    cond_.notify();
    thread_.join();
  }

 private:
  void threadFunc();		//私有函数，用于线程处理，每隔flushInterval_毫秒，便将缓存中的日志写入文件
  typedef FixedBuffer<kLargeBuffer> Buffer;						//固定大小的缓存区，用于存放日志信息
  typedef std::vector<std::shared_ptr<Buffer>> BufferVector;	//Buffer的智能指针数组，用于存放缓存区
  typedef std::shared_ptr<Buffer> BufferPtr;					//Buffer的智能指针
  const int flushInterval_;	//刷新频率，默认值为2
  bool running_;			//运行状态
  std::string basename_;	//文件名
  Thread thread_;			//线程
  MutexLock mutex_;			//互斥锁
  Condition cond_;			//条件变量
  BufferPtr currentBuffer_;	//指向当前缓冲区
  BufferPtr nextBuffer_;	//指向下一个缓冲区
  BufferVector buffers_;	//缓冲区向量
  CountDownLatch latch_;	//倒计时锁，等待线程运行结束
};