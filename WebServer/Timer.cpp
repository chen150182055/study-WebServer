// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "Timer.h"
#include <sys/time.h>
#include <unistd.h>
#include <queue>

/**
 * 构造函数
 * @param requestData
 * @param timeout
 */
TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout)
    : deleted_(false), SPHttpData(requestData) {    //初始化了类成员deleted_和SPHttpData
  struct timeval now;
  gettimeofday(&now, NULL);	//获取当前时区和时间并填入now
  // 以毫秒计
  expiredTime_ =(((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

/**
 * 调用SPHttpData对象的handleClose()函数来处理关闭操作
 */
TimerNode::~TimerNode() {    //析构函数
  if (SPHttpData)
	SPHttpData->handleClose();
}

/**
 * 构造函数
 * @param tn
 */
TimerNode::TimerNode(TimerNode &tn)
    : SPHttpData(tn.SPHttpData), expiredTime_(0) {}

/**
 * 更新计时器节点的过期时间 expiredTime_
 * @param timeout
 */
void TimerNode::update(int timeout) {    //
  struct timeval now;
  //使用gettimeofday函数获取当前的系统时间
  gettimeofday(&now, NULL);
  //把timeout与当前时间相加，最后将结果赋给expiredTime_
  expiredTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

/**
 * 判断定时器是否超时
 * @return
 */
bool TimerNode::isValid() {		//
  struct timeval now;
  gettimeofday(&now, NULL);
  size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
  //比较当前系统时间和定时器超时时间
  if (temp < expiredTime_)
    return true;		//定时器没有超时
  else {
    this->setDeleted();	//设置定时器为已删除状态
    return false;		//定时器已超时
  }
}

/**
 * 清除请求
 */
void TimerNode::clearReq() {	//
  SPHttpData.reset();	//将SPHttpData重置
  this->setDeleted();	//将TimerNode状态设置为已删除
}

/**
 * 构造函数
 */
TimerManager::TimerManager() {}

/**
 * 析构函数
 */
TimerManager::~TimerManager() {}

/**
 * 向TimerManager中添加定时器
 * @param SPHttpData 指向HttpData的智能指针
 * @param timeout 超时时间
 */
void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout) {	//
  SPTimerNode new_node(new TimerNode(SPHttpData, timeout));
  timerNodeQueue.push(new_node);
  SPHttpData->linkTimer(new_node);
}

/* 处理逻辑是这样的~
因为(1) 优先队列不支持随机访问
(2) 即使支持，随机删除某节点后破坏了堆的结构，需要重新更新堆结构。
所以对于被置为deleted的时间节点，会延迟到它(1)超时 或
(2)它前面的节点都被删除时，它才会被删除。
一个点被置为deleted,它最迟会在TIMER_TIME_OUT时间后被删除。
这样做有两个好处：
(1) 第一个好处是不需要遍历优先队列，省时。
(2)
第二个好处是给超时时间一个容忍的时间，就是设定的超时时间是删除的下限(并不是一到超时时间就立即删除)，如果监听的请求在超时后的下一次请求中又一次出现了，
就不用再重新申请RequestData节点了，这样可以继续重复利用前面的RequestData，减少了一次delete和一次new的时间。
*/

/**
 * 从定时器管理器（TimerManager）对象的定时器节点队列（timerNodeQueue）
 * 中清除已过期或已删除的定时器节点（SPTimerNode）
 */
void TimerManager::handleExpiredEvent() {	//
  // MutexLockGuard locker(lock);
  //使用while循环从定时器节点队列中取出定时器节点
  while (!timerNodeQueue.empty()) {
    SPTimerNode ptimer_now = timerNodeQueue.top();
	//如果该定时器节点已经过期或已被删除
    if (ptimer_now->isDeleted())
      timerNodeQueue.pop();	//从队列中清除该定时器节点
    else if (ptimer_now->isValid() == false)
      timerNodeQueue.pop();	//从队列中清除该定时器节点
    else
      break;
  }
}