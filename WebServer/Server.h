// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server {		//定义Server类
 public:
  Server(EventLoop *loop, int threadNum, int port);	//构造函数
  ~Server() {}		//析构函数
  EventLoop *getLoop() const { return loop_; }
  void start();
  void handNewConn();

  /**
   *
   */
  void handThisConn() {
	loop_->updatePoller(acceptChannel_);
  }

 private:
  EventLoop *loop_;	//EventLoop类型的指针,用于指向
  int threadNum_;	//线程数
  std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
  bool started_;	//线程状态
  std::shared_ptr<Channel> acceptChannel_;
  int port_;		//端口号
  int listenFd_;	//监听的描述符
  static const int MAXFDS = 100000;		//静态成员,使用常量表达式进行初始化
};