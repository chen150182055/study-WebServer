// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "EventLoopThread.h"
#include <functional>

/**
 * 构造函数
 */
EventLoopThread::EventLoopThread()
    : loop_(NULL),
      exiting_(false),
      thread_(bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
      mutex_(),
      cond_(mutex_) {}	//构造函数的定义,初始化成员

/**
 * 析构函数
 */
EventLoopThread::~EventLoopThread() {   //构造函数的定义,成员初始化
  exiting_ = true;
  if (loop_ != NULL) {	// 如果loop_不为空
	//退出循环
    loop_->quit();
	//调用thread_.join()函数等待线程结束
    thread_.join();
  }
}

/**
 * 启动一个新的工作线程
 * @return
 */
EventLoop* EventLoopThread::startLoop() {   //
  assert(!thread_.started());	//该函数所属的线程对象thread_是否有开始运行
  thread_.start();				//如果没有,则调用thread_的start函数启动一个新的线程
  {
    MutexLockGuard lock(mutex_);
    // 一直等到threadFun在Thread里真正跑起来
    while (loop_ == NULL) cond_.wait();
  }
  return loop_;
}

/**
 * 创建一个线程,
 * 并在该线程中创建一个EventLoop对象,
 * 并在循环中调用
 */
void EventLoopThread::threadFunc() {    //
  EventLoop loop;

  {
    MutexLockGuard lock(mutex_);		//通过互斥锁来保护loop_指针
    loop_ = &loop;						//将loop_指针指向新创建的EventLoop对象
    cond_.notify();						//最后由条件变量通知等待线程
  }

  loop.loop();
  // assert(exiting_);
  loop_ = NULL;
}