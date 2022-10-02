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

class TimerNode {		//定义TimerNode类
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

struct TimerCmp {			//定义TimerCmp类
  bool operator()(std::shared_ptr<TimerNode> &a,
                  std::shared_ptr<TimerNode> &b) const {
    return a->getExpTime() > b->getExpTime();
  }
};

class TimerManager {		//定义TimerManager类
 public:
  TimerManager();
  ~TimerManager();
  void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
  void handleExpiredEvent();

 private:
  typedef std::shared_ptr<TimerNode> SPTimerNode;
  std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp>
      timerNodeQueue;
  // MutexLock lock;
};