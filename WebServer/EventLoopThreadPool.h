// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once	//避免同一个头文件被包含（include）多次
#include <memory>
#include <vector>
#include "EventLoopThread.h"
#include "base/Logging.h"
#include "base/noncopyable.h"


class EventLoopThreadPool : noncopyable {
 public:
  EventLoopThreadPool(EventLoop* baseLoop, int numThreads);	//构造函数声明

  ~EventLoopThreadPool() { LOG << "~EventLoopThreadPool()"; }	//析构函数声明

  void start();	//成员函数start

  EventLoop* getNextLoop();	//成员函数getNextLoop

 private:
  EventLoop* baseLoop_;
  bool started_;
  int numThreads_;
  int next_;
  std::vector<std::shared_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop*> loops_;
};