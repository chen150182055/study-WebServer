// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "EventLoopThreadPool.h"

/**
 * 该构造函数通过传入的两个参数初始化成员变量baseLoop和numThread_
 * ,其他的成员变量使用默认的值初始化
 * @param baseLoop
 * @param numThreads
 */
EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, int numThreads)
    : baseLoop_(baseLoop), started_(false), numThreads_(numThreads), next_(0) {   //构造函数的定义
  if (numThreads_ <= 0) {
    LOG << "numThreads_ <= 0";
    abort();
  }
}

/**
 * start函数
 */
void EventLoopThreadPool::start() {   //成员函数start的定义
  baseLoop_->assertInLoopThread();
  started_ = true;
  for (int i = 0; i < numThreads_; ++i) {
    std::shared_ptr<EventLoopThread> t(new EventLoopThread());
    threads_.push_back(t);
    loops_.push_back(t->startLoop());
  }
}

/**
 * getNextLoop函数
 * @return
 */
EventLoop *EventLoopThreadPool::getNextLoop() {   //成员函数getNextLoop的定义
  baseLoop_->assertInLoopThread();
  assert(started_);
  EventLoop *loop = baseLoop_;
  if (!loops_.empty()) {
    loop = loops_[next_];
    next_ = (next_ + 1) % numThreads_;
  }
  return loop;
}