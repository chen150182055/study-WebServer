// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"

class EventLoop;    //声明EventLoop
class HttpData;        //声明HttpData

class Channel {        //定义Channel类
 private:            //私有成员
  typedef std::function<void()> CallBack;    //
  EventLoop *loop_;        //一个指向EventLoop对象的指针
  int fd_;                //描述符
  __uint32_t events_;
  __uint32_t revents_;
  __uint32_t lastEvents_;

  // 方便找到上层持有该Channel的对象
  std::weak_ptr<HttpData> holder_;

 private:
  int parse_URI();
  int parse_Headers();
  int analysisRequest();

  CallBack readHandler_;
  CallBack writeHandler_;
  CallBack errorHandler_;
  CallBack connHandler_;

 public:            //共有成员
  Channel(EventLoop *loop);        //构造函数01
  Channel(EventLoop *loop, int fd);    //构造函数02
  ~Channel();                    //析构函数
  int getFd();                    //获取描述符函数
  void setFd(int fd);            //设置描述符函数

  void setHolder(std::shared_ptr<HttpData> holder) { holder_ = holder; }	//在类内部定义,inline

  std::shared_ptr<HttpData> getHolder() {
	std::shared_ptr<HttpData> ret(holder_.lock());
	return ret;
  }

  void setReadHandler(CallBack &&readHandler) { readHandler_ = readHandler; }
  void setWriteHandler(CallBack &&writeHandler) {
	writeHandler_ = writeHandler;
  }
  void setErrorHandler(CallBack &&errorHandler) {
	errorHandler_ = errorHandler;
  }
  void setConnHandler(CallBack &&connHandler) { connHandler_ = connHandler; }

  void handleEvents() {
	events_ = 0;
	if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
	  events_ = 0;
	  return;
	}
	if (revents_ & EPOLLERR) {
	  if (errorHandler_) errorHandler_();
	  events_ = 0;
	  return;
	}
	if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
	  handleRead();
	}
	if (revents_ & EPOLLOUT) {
	  handleWrite();
	}
	handleConn();
  }
  void handleRead();
  void handleWrite();
  void handleError(int fd, int err_num, std::string short_msg);
  void handleConn();

  void setRevents(__uint32_t ev) { revents_ = ev; }

  void setEvents(__uint32_t ev) { events_ = ev; }
  __uint32_t &getEvents() { return events_; }

  bool EqualAndUpdateLastEvents() {
	bool ret = (lastEvents_ == events_);
	lastEvents_ = events_;
	return ret;
  }

  __uint32_t getLastEvents() { return lastEvents_; }
};

typedef std::shared_ptr<Channel> SP_Channel;