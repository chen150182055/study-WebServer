// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "Timer.h"


class EventLoop;
class TimerNode;
class Channel;

//表示HTTP请求的处理过程
enum ProcessState {
  STATE_PARSE_URI = 1,
  STATE_PARSE_HEADERS,
  STATE_RECV_BODY,
  STATE_ANALYSIS,
  STATE_FINISH
};

//解析URL的状态
enum URIState {
  PARSE_URI_AGAIN = 1,
  PARSE_URI_ERROR,
  PARSE_URI_SUCCESS,
};

//头部的状态
enum HeaderState {
  PARSE_HEADER_SUCCESS = 1,
  PARSE_HEADER_AGAIN,
  PARSE_HEADER_ERROR
};

//分析请求的状态
enum AnalysisState { ANALYSIS_SUCCESS = 1, ANALYSIS_ERROR };

//解析头部的具体过程
enum ParseState {
  H_START = 0,
  H_KEY,
  H_COLON,
  H_SPACES_AFTER_COLON,
  H_VALUE,
  H_CR,
  H_LF,
  H_END_CR,
  H_END_LF
};

//连接的状态
enum ConnectionState { H_CONNECTED = 0, H_DISCONNECTING, H_DISCONNECTED };

//HTTP请求的方法
enum HttpMethod { METHOD_POST = 1, METHOD_GET, METHOD_HEAD };

//HTTP请求的版本
enum HttpVersion { HTTP_10 = 1, HTTP_11 };

class MimeType {	//定义MimeType类
 private:
  static void init();	//静态成员
  static std::unordered_map<std::string, std::string> mime;	//静态成员
  MimeType();	//构造函数
  MimeType(const MimeType &m);	//构造函数

 public:
  static std::string getMime(const std::string &suffix);

 private:
  static pthread_once_t once_control;	//静态成员
};

//用于处理HTTP请求的类,类，继承自enable_shared_from_this模板
//它的主要作用是允许一个对象（HttpData）被shared_ptr管理,而且可以获得一个shared_ptr指向它本身
class HttpData : public std::enable_shared_from_this<HttpData> {	//定义HttpData类
 public:
  HttpData(EventLoop *loop, int connfd);
  ~HttpData() { close(fd_); }
  void reset();
  void seperateTimer();
  void linkTimer(std::shared_ptr<TimerNode> mtimer) {
    // shared_ptr重载了bool, 但weak_ptr没有
    timer_ = mtimer;
  }
  std::shared_ptr<Channel> getChannel() { return channel_; }
  EventLoop *getLoop() { return loop_; }
  void handleClose();
  void newEvent();

 private:
  EventLoop *loop_;
  std::shared_ptr<Channel> channel_;
  int fd_;
  std::string inBuffer_;
  std::string outBuffer_;
  bool error_;
  ConnectionState connectionState_;

  HttpMethod method_;
  HttpVersion HTTPVersion_;
  std::string fileName_;
  std::string path_;
  int nowReadPos_;
  ProcessState state_;
  ParseState hState_;
  bool keepAlive_;
  std::map<std::string, std::string> headers_;
  std::weak_ptr<TimerNode> timer_;

  void handleRead();
  void handleWrite();
  void handleConn();
  void handleError(int fd, int err_num, std::string short_msg);
  URIState parseURI();
  HeaderState parseHeaders();
  AnalysisState analysisRequest();
};