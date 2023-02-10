// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include <string>
#include "CountDownLatch.h"
#include "noncopyable.h"

class Thread : noncopyable {    //定义Thread类
 public:
  typedef std::function<void()> ThreadFunc;
  explicit Thread(const ThreadFunc &, const std::string &name = std::string());        //构造函数
  ~Thread();
  void start();
  int join();

  bool started() const {
	return started_;
  }

  /**
   * 于获取线程的线程ID
   * @return
   */
  pid_t tid() const {
	return tid_;
  }

  /**
   * 获取线程的名字
   * @return
   */
  const std::string &name() const {
	return name_;
  }

 private:
  void setDefaultName();
  bool started_;
  bool joined_;
  pthread_t pthreadId_;
  pid_t tid_;
  ThreadFunc func_;
  std::string name_;
  CountDownLatch latch_;
};