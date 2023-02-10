// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "Thread.h"
#include <assert.h>
#include <errno.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include "CurrentThread.h"

#include <iostream>
using namespace std;

//定义一些全局变量，用于保存当前线程的信息
//__thread关键字表示这些变量只在当前线程中有效，其他线程无法访问
namespace CurrentThread {
__thread int t_cachedTid = 0;		//当前线程的线程ID
__thread char t_tidString[32];		//当前线程的线程ID的字符串表示
__thread int t_tidStringLength = 6;	//t_tidString的长度
__thread const char *t_threadName = "default";//表示当前线程的名字
}

/**
 * 获取当前进程的线程标识ID
 * 该函数使用系统调用SYS_gettid获取当前进程的TID，
 * 并将结果用pid_t类型强制转换后返回
 * @return
 */
pid_t gettid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));	//系统调用
}

/**
 * 将当前线程的ID缓存起来，用于后续使用
 */
void CurrentThread::cacheTid() {
  if (t_cachedTid == 0) {			//检查全局变量t_cachedTid是否为0
	t_cachedTid = gettid();			//如果为0，表示之前没有缓存线程ID，此时调用gettid()函数获取当前线程ID
	//将该ID赋值给t_cachedTid，然后利用snprintf函数将t_cachedTid转换成字符串形式存储在t_tidString中,
	// 最后将字符串长度存储在t_tidStringLength中
	t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
  }
}

// 为了在线程中保留name,tid这些数据
/**
 * 封装了线程函数,线程名.线程id指针,以及countdownlatch指针
 */
struct ThreadData {
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc func_;
  string name_;
  pid_t *tid_;
  CountDownLatch *latch_;

  ThreadData(const ThreadFunc &func, const string &name, pid_t *tid,
			 CountDownLatch *latch)
	  : func_(func), name_(name), tid_(tid), latch_(latch) {}

  void runInThread() {
	*tid_ = CurrentThread::tid();	//将tid_指向的线程id赋值为当前线程id
	tid_ = NULL;
	latch_->countDown();
	latch_ = NULL;

	CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
	prctl(PR_SET_NAME, CurrentThread::t_threadName);

	func_();
	CurrentThread::t_threadName = "finished";	//将当前线程名设置为finished
  }
};

/**
 *
 * @param obj
 * @return
 */
void *startThread(void *obj) {
  ThreadData *data = static_cast<ThreadData *>(obj);
  data->runInThread();
  delete data;
  return NULL;
}

/**
 * 初始化Thread类的各个成员变量
 * @param func
 * @param n
 */
Thread::Thread(const ThreadFunc &func, const string &n)
	: started_(false),		//设置为false表示线程未启动
	  joined_(false),		//设置为false表示线程尚未join
	  pthreadId_(0),		//设置为0
	  tid_(0),				//设置为0
	  func_(func),			//设置为传入的ThreadFunc类型参数func
	  name_(n),				//设置为传入的string类型参数n
	  latch_(1) {		//设置1
  setDefaultName();			//设置默认的线程名
}

/**
 * 如果Thread对象已经启动（started_为true），
 * 并且没有被join（joined_为false），
 * 则调用pthread_detach释放用pthreadId_标识的线程
 */
Thread::~Thread() {
  if (started_ && !joined_)
	pthread_detach(pthreadId_);
}

/**
 * 如果Thread类的name_成员变量为空，则将其设置为"Thread"
 */
void Thread::setDefaultName() {
  if (name_.empty()) {
	char buf[32];
	snprintf(buf, sizeof buf, "Thread");	//将"Thread"字符串写入buf数组中
	name_ = buf;		//将buf数组的内容赋值给name_成员变量
  }
}

/**
 *
 */
void Thread::start() {
  assert(!started_);	//断言已经启动的状态为false
  started_ = true;
  //创建一个ThreadData类的实例
  ThreadData *data = new ThreadData(func_, name_, &tid_, &latch_);
  //调用pthread_create函数创建新线程
  if (pthread_create(&pthreadId_, NULL, &startThread, data)) {
	started_ = false;	//如果创建失败,则设置为false
	delete data;		//释放实例
  } else {
	latch_.wait();		//如果成功调用wait等待线程完成初始化
	assert(tid_ > 0);	//断言tid>0
  }
}

/**
 * 将当前线程与指定的线程pthreadId_相关联
 * @return
 */
int Thread::join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  //使用pthread_join函数等待其结束执行，并将返回值赋值给joined_，表示线程已经被加入
  return pthread_join(pthreadId_, NULL);
}