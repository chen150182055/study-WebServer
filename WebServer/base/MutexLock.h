// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <pthread.h>
#include <cstdio>
#include "noncopyable.h"

//MutexLock 类继承自 noncopyable 类，主要实现了锁的初始化、锁定和解锁
class MutexLock : noncopyable {
 public:
  MutexLock() {
	pthread_mutex_init(&mutex, NULL);
  }

  ~MutexLock() {
    pthread_mutex_lock(&mutex);
    pthread_mutex_destroy(&mutex);
  }

  /**
   *
   */
  void lock() {
	pthread_mutex_lock(&mutex);
  }

  /**
   *
   */
  void unlock() {
	pthread_mutex_unlock(&mutex);
  }

  /**
   *
   * @return
   */
  pthread_mutex_t *get() {
	return &mutex;
  }

 private:
  pthread_mutex_t mutex;

  // 友元类不受访问权限影响
 private:
  friend class Condition;	//声明了友元类 Condition，使 Condition 类可以访问 MutexLock 类的私有成员
};

//MutexLockGuard 类实现了 RAII 技术，在它的构造函数中锁定，析构函数中解锁
class MutexLockGuard : noncopyable {
 public:
  /**
   *
   * @param _mutex
   */
  explicit MutexLockGuard(MutexLock &_mutex) : mutex(_mutex) {
	mutex.lock();
  }

  ~MutexLockGuard() {
	mutex.unlock();
  }

 private:
  MutexLock &mutex;
};