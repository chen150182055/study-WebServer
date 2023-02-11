// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <unistd.h>
#include <deque>
#include <memory>
#include <queue>
#include "HttpData.h"
#include "base/MutexLock.h"
#include "base/noncopyable.h"


class HttpData;		//声明HttpData类使其可见

//定时器节点类，用于记录超时时间
class TimerNode {
 public:
  TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
  ~TimerNode();
  TimerNode(TimerNode &tn);
  void update(int timeout);
  bool isValid();
  void clearReq();
  void setDeleted() { deleted_ = true; }
  bool isDeleted() const { return deleted_; }
  size_t getExpTime() const { return expiredTime_; }

 private:
  bool deleted_;
  size_t expiredTime_;
  std::shared_ptr<HttpData> SPHttpData;
};

//imerCmp类是一个比较器类，重载了>操作符，用于比较两个节点的超时时间
struct TimerCmp {
  bool operator()(std::shared_ptr<TimerNode> &a,
                  std::shared_ptr<TimerNode> &b) const {
    return a->getExpTime() > b->getExpTime();
  }
};

//定时器管理器类
class TimerManager {
 public:
  TimerManager();
  ~TimerManager();
  void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
  void handleExpiredEvent();	//处理超时事件

 private:
  typedef std::shared_ptr<TimerNode> SPTimerNode;
  std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp>
      timerNodeQueue;
  // MutexLock lock;
};