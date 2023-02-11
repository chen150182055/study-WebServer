// @Author Lin Ya
// @Email xxbbb@vip.qq.com


// This file has not been used
#include "ThreadPool.h"

pthread_mutex_t ThreadPool::lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::notify = PTHREAD_COND_INITIALIZER;
std::vector<pthread_t> ThreadPool::threads;
std::vector<ThreadPoolTask> ThreadPool::queue;
int ThreadPool::thread_count = 0;
int ThreadPool::queue_size = 0;
int ThreadPool::head = 0;
int ThreadPool::tail = 0;
int ThreadPool::count = 0;
int ThreadPool::shutdown = 0;
int ThreadPool::started = 0;

/**
 * 创建一个线程池
 * @param _thread_count 线程的数量
 * @param _queue_size 任务队列的大小
 * @return
 */
int ThreadPool::threadpool_create(int _thread_count, int _queue_size) {   //
  bool err = false;
  do {
	//如果_thread_count和_queue_size小于等于0或大于最大值
	if (_thread_count <= 0 || _thread_count > MAX_THREADS || _queue_size <= 0 || _queue_size > MAX_QUEUE) {
	  //那么线程数量和任务队列大小分别将被设置为默认值4和1024
	  _thread_count = 4;
	  _queue_size = 1024;
	}

	thread_count = 0;
	queue_size = _queue_size;
	head = tail = count = 0;
	shutdown = started = 0;

	//根据_thread_count的值为threads和queue分配内存空间
	threads.resize(_thread_count);
	queue.resize(_queue_size);

	/* Start worker threads */
	for (int i = 0; i < _thread_count; ++i)    //创建_thread_count数量的线程
	{
	  if (pthread_create(&threads[i], NULL, threadpool_thread, (void *) (0)) != 0) {
		//threadpool_destroy(pool, 0);
		return -1;
	  }
	  ++thread_count;
	  ++started;
	}
  } while (false);

  if (err) {
	//threadpool_free(pool);
	return -1;
  }
  return 0;
}

/**
 * 向线程池中添加任务
 * @param args
 * @param fun
 * @return
 */
int ThreadPool::threadpool_add(std::shared_ptr<void> args, std::function<void(std::shared_ptr<void>)> fun) {   //
  int next, err = 0;
  //通过pthread_mutex_lock函数获取互斥锁
  if (pthread_mutex_lock(&lock) != 0)
	return THREADPOOL_LOCK_FAILURE;
  do {    //判断队列是否已满，以及线程池是否已经关闭
	next = (tail + 1) % queue_size;
	// 队列满
	if (count == queue_size) {
	  err = THREADPOOL_QUEUE_FULL;
	  break;
	}
	// 已关闭
	if (shutdown) {
	  err = THREADPOOL_SHUTDOWN;
	  break;
	}
	//将新任务添加到线程池队列中
	queue[tail].fun = fun;
	queue[tail].args = args;
	tail = next;
	++count;

	//通过pthread_cond_signal函数通知等待该任务的线程
	/* pthread_cond_broadcast */
	if (pthread_cond_signal(&notify) != 0) {
	  err = THREADPOOL_LOCK_FAILURE;
	  break;
	}
  } while (false);

  //通过pthread_mutex_unlock释放互斥锁
  if (pthread_mutex_unlock(&lock) != 0)
	err = THREADPOOL_LOCK_FAILURE;
  return err;
}

/**
 * 销毁线程池
 * @param shutdown_option
 * @return
 */
int ThreadPool::threadpool_destroy(ShutDownOption shutdown_option) {    //
  printf("Thread pool destroy !\n");
  int i, err = 0;

  //使用pthread_mutex_lock函数锁定线程池
  if (pthread_mutex_lock(&lock) != 0) {
	return THREADPOOL_LOCK_FAILURE;
  }
  do {
	//检查shutdown标志位
	if (shutdown) {
	  err = THREADPOOL_SHUTDOWN;    //表示线程池已经被销毁
	  break;                        //返回THREADPOOL_SHUTDOWN错误码
	}
	shutdown = shutdown_option;        //设置shutdown标志位

	//使用pthread_cond_broadcast函数唤醒所有等待线程
	if ((pthread_cond_broadcast(&notify) != 0) ||
		(pthread_mutex_unlock(&lock) != 0)) {
	  err = THREADPOOL_LOCK_FAILURE;
	  break;
	}

	for (i = 0; i < thread_count; ++i) {
	  //使用pthread_join函数等待线程退出
	  if (pthread_join(threads[i], NULL) != 0) {
		err = THREADPOOL_THREAD_FAILURE;
	  }
	}
  } while (false);

  //释放线程池中所有资源
  if (!err) {
	threadpool_free();
  }
  return err;
}

/**
 * 释放线程池
 * @return
 */
int ThreadPool::threadpool_free() {        //
  //检查当前线程池是否开始运行
  if (started > 0)
	return -1;
  //锁定线程池，销毁锁和条件变量，最后释放线程池并返回0
  pthread_mutex_lock(&lock);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&notify);
  return 0;
}

/**
 * 从线程池队列中取出任务，
 * 然后调用任务函数来执行任务，
 * 并将任务函数的返回值返回给调用者
 * @param args 线程池结构体指针
 * @return
 */
void *ThreadPool::threadpool_thread(void *args) {        //
  while (true) {
	ThreadPoolTask task;
	pthread_mutex_lock(&lock);		//上锁
	while ((count == 0) && (!shutdown)) {	// 当任务队列为空且没有收到关闭信号时，等待
	  pthread_cond_wait(&notify, &lock);
	}
	// 收到立即关闭或任务队列为空的优雅关闭，退出线程
	if ((shutdown == immediate_shutdown) || ((shutdown == graceful_shutdown) && (count == 0))) {
	  break;
	}
	task.fun = queue[head].fun;		//取出任务
	task.args = queue[head].args;	//
	queue[head].fun = NULL;
	queue[head].args.reset();
	head = (head + 1) % queue_size;	// 更新头指针位置
	--count;						//任务数量减一
	pthread_mutex_unlock(&lock);	//解锁
	(task.fun)(task.args);			//执行任务
  }
  --started;						//线程数量减一
  pthread_mutex_unlock(&lock);	//解锁
  printf("This threadpool thread finishs!\n");
  pthread_exit(NULL);			//退出线程
  return (NULL);
}