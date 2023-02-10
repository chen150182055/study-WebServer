// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "CountDownLatch.h"

/**
 * 设置计数器的初始值
 * @param count
 */
CountDownLatch::CountDownLatch(int count)
    : mutex_(), condition_(mutex_), count_(count) {}

/**
 * 当计数器大于0时，线程将被阻塞，
 * 直到计数器为0时，才会被唤醒
 */
void CountDownLatch::wait() {
  MutexLockGuard lock(mutex_);
  while (count_ > 0) condition_.wait();
}

/**
 * 每次调用此函数，计数器的值将减少1，
 * 当计数器为0时，所有被阻塞的线程将被唤醒
 */
void CountDownLatch::countDown() {
  MutexLockGuard lock(mutex_);
  --count_;
  if (count_ == 0)
	condition_.notifyAll();
}