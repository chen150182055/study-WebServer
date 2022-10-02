// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <stdint.h>

namespace CurrentThread {
// internal
extern __thread int t_cachedTid;
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;
extern __thread const char *t_threadName;
void cacheTid();

/**
 *
 * @return
 */
inline int tid() {
  if (__builtin_expect(t_cachedTid == 0, 0)) {
	cacheTid();
  }
  return t_cachedTid;
}

/**
 *
 * @return
 */
inline const char *tidString()  // for logging
{
  return t_tidString;
}

/**
 *
 * @return
 */
inline int tidStringLength()  // for logging
{
  return t_tidStringLength;
}

/**
 *
 * @return
 */
inline const char *name() {
  return t_threadName;
}
}
