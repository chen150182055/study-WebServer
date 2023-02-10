// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "Logging.h"
#include "CurrentThread.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include <assert.h>
#include <iostream>
#include <time.h>  
#include <sys/time.h> 


static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging *AsyncLogger_;

std::string Logger::logFileName_ = "./WebServer.log";

/**
 * 初始化一次
 */
void once_init()
{
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start(); 
}

/**
 * 将日志信息传入AsyncLogging中
 * @param msg
 * @param len
 */
void output(const char* msg, int len)
{
    pthread_once(&once_control_, once_init);
    AsyncLogger_->append(msg, len);
}

/**
 * 在输出时，格式化输出时间
 * @param fileName
 * @param line
 */
Logger::Impl::Impl(const char *fileName, int line)
  : stream_(),
    line_(line),
    basename_(fileName)
{
    formatTime();
}

/**
 * 格式化当前时间，并将格式化后的时间字符串输出到stream_中
 */
void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL); 				//获取当前时间
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time); //localtime函数将time_t类型的时间转换为struct tm类型的时间
	//strftime函数将struct tm类型的时间格式化为指定的格式，最后将格式化后的时间字符串输出到stream_中
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

/**
 * 将日志信息收集
 * @param fileName
 * @param line
 */
Logger::Logger(const char *fileName, int line)
  : impl_(fileName, line)
{ }

/**
 * 把日志信息传入output函数中进行输出
 */
Logger::~Logger()
{
    impl_.stream_ << " -- " << impl_.basename_ << ':' << impl_.line_ << '\n';
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}