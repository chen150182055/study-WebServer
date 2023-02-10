// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <errno.h>
#include <pthread.h>
#include <pthread.h>
#include <time.h>
#include <cstdint>
#include "MutexLock.h"
#include "noncopyable.h"

//该类用于支持线程同步
class Condition : noncopyable {
 public:
  /**
   * 接受一个MutexLock类型的引用，并使用它来初始化条件变量
   * @param _mutex MutexLock类型的引用
   */
  explicit Condition(MutexLock &_mutex) : mutex(_mutex) {
	pthread_cond_init(&cond, NULL);
  }

  ~Condition() {
	pthread_cond_destroy(&cond);
  }

  /**
   * 使线程进入等待状态，直到被通知或者超时
   */
  void wait() {
	pthread_cond_wait(&cond, mutex.get());
  }

  /**
   * 唤醒一个等待的线程
   */
  void notify() {
	pthread_cond_signal(&cond);
  }

  /**
   * 唤醒所有等待的线程
   */
  void notifyAll() {
	pthread_cond_broadcast(&cond);
  }

  /**
   * 对wait()函数的一个封装，使线程等待指定的秒数
   * @param seconds
   * @return
   */
  bool waitForSeconds(int seconds) {
	struct timespec abstime;
	clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_sec += static_cast<time_t>(seconds);
	return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
  }

 private:
  MutexLock &mutex;		//
  pthread_cond_t cond;	//
};