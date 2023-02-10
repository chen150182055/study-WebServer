// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <stdint.h>

namespace CurrentThread {
// internal
extern __thread int t_cachedTid;         //t_cachedTid用于缓存线程ID，
extern __thread char t_tidString[32];    //用于存储线程ID的字符串
extern __thread int t_tidStringLength;   //字符串的长度
extern __thread const char *t_threadName;//线程名字
void cacheTid();					     //缓存线程ID

/**
 * 获取线程ID
 * @return
 */
inline int tid() {
  if (__builtin_expect(t_cachedTid == 0, 0)) {
	cacheTid();
  }
  return t_cachedTid;
}

/**
 * 获取线程ID的字符串
 * @return
 */
inline const char *tidString()  // for logging
{
  return t_tidString;
}

/**
 * 获取线程ID的字符串的长度
 * @return
 */
inline int tidStringLength()  // for logging
{
  return t_tidStringLength;
}

/**
 * 获取线程名字
 * @return
 */
inline const char *name() {
  return t_threadName;
}
}
