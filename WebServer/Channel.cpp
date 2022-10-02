// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "Channel.h"

#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <queue>

#include "Epoll.h"
#include "EventLoop.h"
#include "Util.h"

using namespace std;

/**
 *
 * @param loop
 */
Channel::Channel(EventLoop *loop): loop_(loop), events_(0), lastEvents_(0), fd_(0) {}	//构造函数定义,初始化成员.传入一个指针变量类型的参数


/**
 *
 * @param loop
 * @param fd
 */
Channel::Channel(EventLoop *loop, int fd): loop_(loop), fd_(fd), events_(0), lastEvents_(0) {}	//构造函数定义

/**
 * 析构函数
 */
Channel::~Channel() {    //析构函数的定义
  // loop_->poller_->epoll_del(fd, events_);
  // close(fd_);
}

/**
 *
 * @return
 */
int Channel::getFd() { return fd_; }

/**
 *
 * @param fd
 */
void Channel::setFd(int fd) { fd_ = fd; }

/**
 *
 */
void Channel::handleRead() {    //
  if (readHandler_) {
	readHandler_();
  }
}

/**
 *
 */
void Channel::handleWrite() {    //
  if (writeHandler_) {
	writeHandler_();
  }
}

/**
 *
 */
void Channel::handleConn() {    //
  if (connHandler_) {
	connHandler_();
  }
}