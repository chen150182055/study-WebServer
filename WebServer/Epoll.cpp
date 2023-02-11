// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "Epoll.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <deque>
#include <queue>
#include "Util.h"
#include "base/Logging.h"


#include <arpa/inet.h>
#include <iostream>
using namespace std;

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

typedef shared_ptr<Channel> SP_Channel;

/**
 * 构造函数
 */
Epoll::Epoll() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {	//
  assert(epollFd_ > 0);
}

/**
 * 析构函数
 */
Epoll::~Epoll() {}

/**
 * 向Epoll中添加新的文件描述符
 * @param request
 * @param timeout
 */
// 注册新描述符
void Epoll::epoll_add(SP_Channel request, int timeout) {	//
  int fd = request->getFd();				//从Channel中获取文件描述符
  if (timeout > 0) {						//如果timeout大于0
    add_timer(request, timeout);	//则为该文件描述符添加超时定时器
    fd2http_[fd] = request->getHolder();	//将文件描述符与其相应的HttpData对象存储到映射容器fd2http_中
  }
  //创建一个epoll_event结构体并将文件描述符赋值给其data.fd
  struct epoll_event event;
  event.data.fd = fd;
  //将Channel中获取的事件类型赋值给其events
  event.events = request->getEvents();

  //更新Channel中的lastEvents_
  request->EqualAndUpdateLastEvents();

  //将文件描述符与对应的Channel对象存储到映射容器fd2chan_中
  fd2chan_[fd] = request;
  //并使用epoll_ctl函数将文件描述符添加到Epoll中
  if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
	//如果失败则报错并将映射容器中的Channel对象释放
    perror("epoll_add error");
    fd2chan_[fd].reset();
  }
}

/**
 * 修改某个文件描述符在epoll中的事件触发方式
 * @param request
 * @param timeout
 */
// 修改描述符状态
void Epoll::epoll_mod(SP_Channel request, int timeout) {	//
  if (timeout > 0) add_timer(request, timeout);
  int fd = request->getFd();
  if (!request->EqualAndUpdateLastEvents()) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0) {
      perror("epoll_mod error");
      fd2chan_[fd].reset();
    }
  }
}

/**
 *
 * @param request
 */
// 从epoll中删除描述符
void Epoll::epoll_del(SP_Channel request) {		//
  int fd = request->getFd();
  struct epoll_event event;
  event.data.fd = fd;
  event.events = request->getLastEvents();
  // event.events = 0;
  // request->EqualAndUpdateLastEvents()
  if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) {
    perror("epoll_del error");
  }
  fd2chan_[fd].reset();
  fd2http_[fd].reset();
}

/**
 *
 * @return
 */
// 返回活跃事件数
std::vector<SP_Channel> Epoll::poll() {		//
  while (true) {
    int event_count =
        epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
    if (event_count < 0) perror("epoll wait error");
    std::vector<SP_Channel> req_data = getEventsRequest(event_count);
    if (req_data.size() > 0) return req_data;
  }
}

/**
 *
 */
void Epoll::handleExpired() { timerManager_.handleExpiredEvent(); }		//

/**
 *
 * @param events_num
 * @return
 */
// 分发处理函数
std::vector<SP_Channel> Epoll::getEventsRequest(int events_num) {		//
  std::vector<SP_Channel> req_data;
  for (int i = 0; i < events_num; ++i) {
    // 获取有事件产生的描述符
    int fd = events_[i].data.fd;

    SP_Channel cur_req = fd2chan_[fd];

    if (cur_req) {
      cur_req->setRevents(events_[i].events);
      cur_req->setEvents(0);
      // 加入线程池之前将Timer和request分离
      // cur_req->seperateTimer();
      req_data.push_back(cur_req);
    } else {
      LOG << "SP cur_req is invalid";
    }
  }
  return req_data;
}

/**
 *
 * @param request_data
 * @param timeout
 */
void Epoll::add_timer(SP_Channel request_data, int timeout) {		//
  shared_ptr<HttpData> t = request_data->getHolder();
  if (t)
    timerManager_.addTimer(t, timeout);
  else
    LOG << "timer add fail";
}