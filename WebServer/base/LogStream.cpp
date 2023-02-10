// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "LogStream.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <limits>

const char digits[] = "9876543210123456789";
const char *zero = digits + 9;

// From muduo
/**
 * 定义模板函数convert，将任意整数类型的数据转换成字符串
 * @tparam T
 * @param buf
 * @param value
 * @return
 */
template<typename T>
size_t convert(char buf[], T value) {
  T i = value;
  char *p = buf;

  do {
	int lsd = static_cast<int>(i % 10);
	i /= 10;
	*p++ = zero[lsd];
  } while (i != 0);

  if (value < 0) {
	*p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

template
class FixedBuffer<kSmallBuffer>;
template
class FixedBuffer<kLargeBuffer>;

/**
 * 将参数v（可以是任意的整型数）转换成字符串，
 * 并将它放到LogStream类的buffer_（一个缓冲区）中
 * @tparam T
 * @param v
 */
template<typename T>
void LogStream::formatInteger(T v) {
  // buffer容不下kMaxNumericSize个字符的话会被直接丢弃
  if (buffer_.avail() >= kMaxNumericSize) {					//检查缓冲区buffer_是否有足够的空间
	size_t len = convert(buffer_.current(), v);	//将v转换成字符串，并计算这段字符串的长度len
	buffer_.add(len);										//将字符串添加到缓冲区中
  }
}

/**
 * 将short类型转换为int类型输入LogStream类中
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(short v) {
  *this << static_cast<int>(v);
  return *this;
}

/**
 * 用来将unsigned short类型的变量v转换为unsigned int类型，
 * 并将结果添加到当前LogStream对象中，
 * 最后返回当前LogStream对象的引用
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}

/**
 * 实现 LogStream 类的 "<<" 重载运算符的函数，
 * 用于将 int 类型的数据输入到 LogStream 类中
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(int v) {
  formatInteger(v);	//用于将 int 类型的数据转换成字符串的形式
  return *this;		//返回 LogStream 类的实例指针 *this
}

/**
 *
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(unsigned int v) {
  formatInteger(v);
  return *this;
}

/**
 *
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(long v) {
  formatInteger(v);
  return *this;
}

/**
 *
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(unsigned long v) {
  formatInteger(v);
  return *this;
}

/**
 *
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(long long v) {
  formatInteger(v);
  return *this;
}

/**
 *
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(unsigned long long v) {
  formatInteger(v);
  return *this;
}

/**
 *
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(double v) {
  if (buffer_.avail() >= kMaxNumericSize) {
	int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
	buffer_.add(len);
  }
  return *this;
}

/**
 *
 * @param v
 * @return
 */
LogStream &LogStream::operator<<(long double v) {
  if (buffer_.avail() >= kMaxNumericSize) {
	int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
	buffer_.add(len);
  }
  return *this;
}