// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "AsyncLogging.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include "LogFile.h"

/**
 * 构造函数，接受两个参数
 * @param logFileName_ 日志文件名字
 * @param flushInterval 刷新的时间间隔
 */
AsyncLogging::AsyncLogging(std::string logFileName_, int flushInterval)
    : flushInterval_(flushInterval),	//将flushInterval_赋值为flushInterval
      running_(false),					//running_赋值为false
      basename_(logFileName_),			//basename_赋值为logFileName_
	  //创建一个线程，其执行函数为threadFunc
      thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
	  //初始化mutex_和cond_,currentBuffer_和nextBuffer_,
	  // 并且将currentBuffer_和nextBuffer_清零
      mutex_(),
      cond_(mutex_),
      currentBuffer_(new Buffer),
      nextBuffer_(new Buffer),
      buffers_(),
	  //初始化latch_为1
      latch_(1) {
  assert(logFileName_.size() > 1);
  currentBuffer_->bzero();	//初始化
  nextBuffer_->bzero();		//初始化
  buffers_.reserve(16);	//设置buffers_容量为16
}

/**
 * 将指定长度的日志添加到当前缓冲区currentBuffer_中,
 * 如果缓冲区不够用，就用新的缓冲区替换当前缓冲区,
 * 并将日志添加到新缓冲区中
 * @param logline
 * @param len
 */
void AsyncLogging::append(const char* logline, int len) {
  MutexLockGuard lock(mutex_);				//使用互斥量mutex_锁定资源
  if (currentBuffer_->avail() > len)			//判断当前缓冲区的可用空间是否足够添加日志
    currentBuffer_->append(logline, len);	//够则将日志添加到当前缓冲区
  else {	//不够
    buffers_.push_back(currentBuffer_);			//将当前缓冲区放入缓冲区队列buffers_中
    currentBuffer_.reset();
    if (nextBuffer_)
      currentBuffer_ = std::move(nextBuffer_);	//将nextBuffer_赋值给当前缓冲区
    else
      currentBuffer_.reset(new Buffer);
    currentBuffer_->append(logline, len);	//将日志添加到当前缓冲区
    cond_.notify();								//通知条件变量cond_
  }
}

/**
 * 开启线程的时候会执行这部分代码，该函数的作用是将缓存池中的日志写入
 * 文件中，实现日志的异步记录的功能
 * 函数步骤穿插在下列实现代码中
 */
void AsyncLogging::threadFunc() {
  assert(running_ == true);
  latch_.countDown();					//计数器减一
  LogFile output(basename_);	//创建LogFile
  BufferPtr newBuffer1(new Buffer);	//初始化Buffer1
  BufferPtr newBuffer2(new Buffer);	//初始化Buffer2
  newBuffer1->bzero();					//将其中的数据清零
  newBuffer2->bzero();					//将其中的数据清零
  BufferVector buffersToWrite;
  buffersToWrite.reserve(16);		//预留16个空间
  while (running_) {
    assert(newBuffer1 && newBuffer1->length() == 0);	//将其中的数据清零
    assert(newBuffer2 && newBuffer2->length() == 0);	//将其中的数据清零
    assert(buffersToWrite.empty());						//将其中的数据清零

	//将缓存池中的缓存放入bufferToWrite容器中，并设置新的当前缓存；
    {
      MutexLockGuard lock(mutex_);
      if (buffers_.empty())  // unusual usage!
      {
        cond_.waitForSeconds(flushInterval_);
      }
      buffers_.push_back(currentBuffer_);		//将当前缓存放入缓存池
      currentBuffer_.reset();					//将当前缓存清空

      currentBuffer_ = std::move(newBuffer1);	//将Buffer1设置为新的当前缓存
      buffersToWrite.swap(buffers_);			//将缓存池中的缓存放入bufferToWrite容器中
      if (!nextBuffer_) {
        nextBuffer_ = std::move(newBuffer2);	//将Buffer2设置为新的下一个缓存
      }
    }

    assert(!buffersToWrite.empty());

    if (buffersToWrite.size() > 25) {	//如果buffersToWrite容器中缓存数量大于25，则将多余的缓存抛弃
      // char buf[256];
      // snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger
      // buffers\n",
      //          Timestamp::now().toFormattedString().c_str(),
      //          buffersToWrite.size()-2);
      // fputs(buf, stderr);
      // output.append(buf, static_cast<int>(strlen(buf)));
      buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
    }

    for (size_t i = 0; i < buffersToWrite.size(); ++i) {	//遍历buffersToWrite容器，将缓存中的数据写入文件中
      // FIXME: use unbuffered stdio FILE ? or use ::writev ?
      output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
    }

    if (buffersToWrite.size() > 2) {	//如果buffersToWrite容器中缓存数量大于2，则将多余的缓存抛弃
      // drop non-bzero-ed buffers, avoid trashing
      buffersToWrite.resize(2);
    }

	//将buffersToWrite容器中的缓存赋值给新的Buffer 1和Buffer 2
    if (!newBuffer1) {
      assert(!buffersToWrite.empty());
      newBuffer1 = buffersToWrite.back();
      buffersToWrite.pop_back();
      newBuffer1->reset();
    }

    if (!newBuffer2) {
      assert(!buffersToWrite.empty());
      newBuffer2 = buffersToWrite.back();
      buffersToWrite.pop_back();
      newBuffer2->reset();
    }

	//将缓存的数据写入到文件中，将文件的数据刷新到磁盘中
    buffersToWrite.clear();
    output.flush();
  }
  output.flush();
}
