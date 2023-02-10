// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include "Condition.h"
#include "MutexLock.h"
#include "noncopyable.h"

//同步计数器,允许一个或多个线程等待,直到一个计数器为0时才继续执行
// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
class CountDownLatch : noncopyable {
 public:
  explicit CountDownLatch(int count);
  void wait();
  void countDown();

 private:
  mutable MutexLock mutex_;
  Condition condition_;
  int count_;
};