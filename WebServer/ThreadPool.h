// @Author Lin Ya
// @Email xxbbb@vip.qq.com

// This file has not been used
#pragma once
#include "Channel.h"
#include <pthread.h>
#include <functional>
#include <memory>
#include <vector>

const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUEUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;

const int MAX_THREADS = 1024;
const int MAX_QUEUE = 65535;

typedef enum {
  immediate_shutdown = 1,
  graceful_shutdown = 2
} ShutDownOption;

struct ThreadPoolTask {
  std::function<void(std::shared_ptr<void>)> fun;
  std::shared_ptr<void> args;
};

class ThreadPool { 	//定义ThreadPool类
 private:
  static pthread_mutex_t lock;		//静态成员
  static pthread_cond_t notify;		//静态成员

  static std::vector<pthread_t> threads;	//静态成员
  static std::vector<ThreadPoolTask> queue;	//静态成员
  static int thread_count;	//静态成员
  static int queue_size;	//静态成员
  static int head;			//静态成员
  // tail 指向尾节点的下一节点
  static int tail;		//静态成员
  static int count;		//静态成员
  static int shutdown;	//静态成员
  static int started;	//静态成员
 public:
  static int threadpool_create(int _thread_count, int _queue_size);										//静态成员
  static int threadpool_add(std::shared_ptr<void> args, std::function<void(std::shared_ptr<void>)> fun);//静态成员
  static int threadpool_destroy(ShutDownOption shutdown_option = graceful_shutdown);					//静态成员
  static int threadpool_free();					//静态成员
  static void *threadpool_thread(void *args);	//静态成员
};
