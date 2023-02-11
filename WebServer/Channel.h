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

//用于处理文件描述符上的事件
class Channel {
 private:
  typedef std::function<void()> CallBack;
  EventLoop *loop_;
  int fd_;
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

  /**
   * 用来设置holder_
   * @param holder
   */
  void setHolder(std::shared_ptr<HttpData> holder) {
	holder_ = holder;	//holder_是一个指向HttpData的shared_ptr智能指针
  }

  /**
   * 从holder_中获取一个std::shared_ptr对象
   * @return
   */
  std::shared_ptr<HttpData> getHolder() {
	std::shared_ptr<HttpData> ret(holder_.lock());
	return ret;
  }

  /**
   * 接受一个右值引用类型的参数readHandler，
   * 并将它赋值给readHandler_变量
   * @param readHandler
   */
  void setReadHandler(CallBack &&readHandler) {
	readHandler_ = readHandler;
  }

  /**
   * 接受一个CallBack类型的右值引用参数writeHandler，
   * 并将其赋值给writeHandler_成员变量
   * @param writeHandler
   */
  void setWriteHandler(CallBack &&writeHandler) {
	writeHandler_ = writeHandler;
  }

  /**
   *
   * @param errorHandler
   */
  void setErrorHandler(CallBack &&errorHandler) {
	errorHandler_ = errorHandler;
  }

  /**
   *
   * @param connHandler
   */
  void setConnHandler(CallBack &&connHandler) {
	connHandler_ = connHandler;
  }

  /**
   * 处理事件
   */
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

  /**
   *
   * @param ev
   */
  void setRevents(__uint32_t ev) {
	revents_ = ev;
  }

  /**
   *
   * @param ev
   */
  void setEvents(__uint32_t ev) {
	events_ = ev;
  }

  /**
   *
   * @return
   */
  __uint32_t &getEvents() { return events_; }

  /**
   *
   * @return
   */
  bool EqualAndUpdateLastEvents() {
	bool ret = (lastEvents_ == events_);
	lastEvents_ = events_;
	return ret;
  }

  /**
   *
   * @return
   */
  __uint32_t getLastEvents() {
	return lastEvents_;
  }
};

typedef std::shared_ptr<Channel> SP_Channel;