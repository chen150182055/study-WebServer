// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "EventLoopThreadPool.h"

/**
 * 用来创建EventLoopThreadPool对象
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
 * 开启一组线程,
 * 每个线程对应一个EventLoop对象，
 * 以实现多线程处理对象
 */
void EventLoopThreadPool::start() {   //成员函数start的定义
  //检查是否处于baseLoop_（主循环）线程中
  baseLoop_->assertInLoopThread();
  started_ = true;		//将started_设置为true,表示线程池已经启动
  //接着循环创建numThreads_个EventLoopThread对象
  for (int i = 0; i < numThreads_; ++i) {
    std::shared_ptr<EventLoopThread> t(new EventLoopThread());
    threads_.push_back(t);	//将每个EventLoopThread对象添加到threads_
    loops_.push_back(t->startLoop());	//将每个EventLoop对象添加到loops_中
  }
}

/**
 * 从EventLoopThreadPool中获取一个EventLoop
 * @return
 */
EventLoop *EventLoopThreadPool::getNextLoop() {   //成员函数getNextLoop的定义
  //检查当前线程是否为baseLoop_所在的线程
  baseLoop_->assertInLoopThread();
  assert(started_);				//检查started_是否为true
  EventLoop *loop = baseLoop_;
  if (!loops_.empty()) {
    loop = loops_[next_];		//从loops_中拿出下一个EventLoop
    next_ = (next_ + 1) % numThreads_;	//将next_加1，然后求模numThreads_
  }
  return loop;
}