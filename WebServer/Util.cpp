// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "Util.h"

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


const int MAX_BUFF = 4096;
/**
 * 从描述符fd中读取n个字节到buff内存空间
 * @param fd 描述符
 * @param buff 内存空间
 * @param n 字节数
 * @return
 */
ssize_t readn(int fd, void *buff, size_t n) {		//
  size_t nleft = n;		//初始化nleft为要读取的字节数
  ssize_t nread = 0;	//读取的字节数
  ssize_t readSum = 0;	//总共读取的字节数
  char *ptr = (char *)buff;	//指向buff的首地址
  while (nleft > 0) {		//当nleft大于0时
	//调用read函数从描述符fd中读取nleft字节到buff中
    if ((nread = read(fd, ptr, nleft)) < 0) {
	  //如果读取失败,判断是否是EINTR错误
      if (errno == EINTR)
        nread = 0;	//如果是EINTR错误，将nread置为0，继续读取
      else if (errno == EAGAIN) {
        return readSum;	//如果是EAGAIN错误，返回当前总共读取的字节数
      } else {
        return -1;		//如果是其他错误，返回-1
      }
    } else if (nread == 0)
      break;
	//如果读取成功
    readSum += nread;	//更新readSum的值
    nleft -= nread;		//更新nleft的值
    ptr += nread;		//更新ptr的指针
	//当nleft的值为0或者描述符fd中没有可读取的字节时，结束循环
  }
  return readSum;		//返回总共读取的字节数
}

/**
 * 从文件描述符fd中读取数据，
 * 将读取到的内容追加到inBuffer中，
 * 并通过zero参数返回是否读取到文件末尾
 * @param fd
 * @param inBuffer
 * @param zero
 * @return
 */
ssize_t readn(int fd, std::string &inBuffer, bool &zero) {		//
  ssize_t nread = 0;
  ssize_t readSum = 0;
  while (true) {
    char buff[MAX_BUFF];	//缓冲数组buff，用于存放从文件描述符fd中读取的内容
	//使用read函数从fd中读取数据，读取的长度不能超过buff的最大长度，
	// 将读取到的内容保存到buff中，并将读取到的字符串长度赋值给nread
    if ((nread = read(fd, buff, MAX_BUFF)) < 0) {	//如果nread < 0，则根据errno的值分别做出不同的处理
      if (errno == EINTR)
        continue;		//如果errno为EINTR，则继续循环
      else if (errno == EAGAIN) {
        return readSum;	//如果errno为EAGAIN，则返回已经读取到的字符串长度readSum
      } else {
        perror("read error");
        return -1;		//如果errno为其他，则输出错误信息，返回-1
      }
    } else if (nread == 0) {
      // printf("redsum = %d\n", readSum);
      zero = true;		//若nread == 0，则将zero置为true,表示已经读取到文件末尾，并跳出循环
      break;
    }
    // printf("before inBuffer.size() = %d\n", inBuffer.size());
    // printf("nread = %d\n", nread);
    readSum += nread;	//将读取到的长度nread累加到readSum中
    // buff += nread;
    inBuffer += std::string(buff, buff + nread);	//将读取到的字符串buff追加到inBuffer中
    // printf("after inBuffer.size() = %d\n", inBuffer.size());
  }
  return readSum;	//返回读取到的字符串长度readSum
}

/**
 * 读取文件描述符fd中的数据，
 * 并把读取的数据放置在inBuffer字符串中
 * @param fd 文件描述符
 * @param inBuffer 读取的数据放置的位置
 * @return
 */
ssize_t readn(int fd, std::string &inBuffer) {		//
  ssize_t nread = 0;		//记录每次read操作读取字节数
  ssize_t readSum = 0;		//记录总的读取字节数
  while (true) {			//死循环读取数据
    char buff[MAX_BUFF];	//每次最多读取
    if ((nread = read(fd, buff, MAX_BUFF)) < 0) {
      if (errno == EINTR)
        continue;			//继续读取
      else if (errno == EAGAIN) {
        return readSum;		//返回已经读取的字节数
      } else {
        perror("read error");
        return -1;			//
      }
    } else if (nread == 0) {
      // printf("redsum = %d\n", readSum);
      break;
    }
    // printf("before inBuffer.size() = %d\n", inBuffer.size());
    // printf("nread = %d\n", nread);
    readSum += nread;		//累加已经读取的字节数
    // buff += nread;
    inBuffer += std::string(buff, buff + nread);	//把读取的数据放入inBuffer中
    // printf("after inBuffer.size() = %d\n", inBuffer.size());
  }
  return readSum;			//返回读取的总字节数
}

/**
 * 将缓冲区中的数据写入指定的文件描述符
 * @param fd 文件描述符
 * @param buff 缓冲区
 * @param n 希望写入的字节数
 * @return
 */
ssize_t writen(int fd, void *buff, size_t n) {		//
  size_t nleft = n;			//记录剩余未写入的字节数
  ssize_t nwritten = 0;		//记录每次写入的字节数
  ssize_t writeSum = 0;		//记录已经写入的字节数
  char *ptr = (char *)buff;	//指向缓冲区buff中待写入的字节
  while (nleft > 0) {		//循环写入文件，直到写入的字节数等于n
	//执行write函数，来写入nleft个字节，并将写入的字节数赋值给nwritten
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
	  //如果nwritten小于0，则表明写入文件出现了错误
      if (nwritten < 0) {
        if (errno == EINTR) {
          nwritten = 0;
          continue;			//继续写入
        } else if (errno == EAGAIN) {
          return writeSum;	//返回已写入的字节数writeSum
        } else
          return -1;
      }
    }
    writeSum += nwritten;	//将写入的字节数加到writeSum中
    nleft -= nwritten;		//将nleft的值减少相应的字节数,直到nleft=0,表示所有字节写入完毕
    ptr += nwritten;		//将ptr的值减少相应的字节数
  }
  return writeSum;			//返回写入的总字节数writeSum
}

/**
 * 将字符串sbuff中的内容通过文件描述符fd写到磁盘文件中
 * @param fd 文件描述符
 * @param sbuff 字符串
 * @return
 */
ssize_t writen(int fd, std::string &sbuff) {	//
  size_t nleft = sbuff.size();		//表示sbuff中还未写入的字节数,初始值为sbuff的长度
  ssize_t nwritten = 0;				//表示write函数每次写入的字节数
  ssize_t writeSum = 0;				//表示总共写入的字节数
  const char *ptr = sbuff.c_str();	//指向sbuff的首地址
  //判断nleft是否大于0，如果大于0，则调用write函数将sbuff中的内容写入磁盘文件，否则跳出循环
  while (nleft > 0) {
    if ((nwritten = write(fd, ptr, nleft)) <= 0) {
      if (nwritten < 0) {
        if (errno == EINTR) {
          nwritten = 0;
          continue;
        } else if (errno == EAGAIN)
          break;
        else
          return -1;
      }
    }
    writeSum += nwritten;			//更新writeSum的值
    nleft -= nwritten;				//减少nleft的值
    ptr += nwritten;				//更新ptr指针的值
  }
  //判断writeSum是否等于sbuff的长度
  if (writeSum == static_cast<int>(sbuff.size()))
    sbuff.clear();					//如果相等，则将sbuff清空
  else
    sbuff = sbuff.substr(writeSum);	//否则将sbuff从writeSum位置截断
  return writeSum;					//返回writeSum的值
}

/**
 * 处理SIGPIPE信号,
 * 它使用sigaction函数将信号处理函数设置为SIG_IGN,
 * 即忽略此信号
 */
void handle_for_sigpipe() {		//
  struct sigaction sa;
  memset(&sa, '\0', sizeof(sa));
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  if (sigaction(SIGPIPE, &sa, NULL)) return;
}

/**
 * 设置socket的非阻塞模式
 * @param fd
 * @return
 */
int setSocketNonBlocking(int fd) {		//
  int flag = fcntl(fd, F_GETFL, 0);//通过fcntl函数获取socket的状态标志
  if (flag == -1) return -1;

  flag |= O_NONBLOCK;					//将O_NONBLOCK添加到标志中
  //通过fcntl函数设置socket的状态标志
  if (fcntl(fd, F_SETFL, flag) == -1)
	return -1;
  return 0;
}

/**
 * 设置描述符fd的TCP连接的Nagle算法,
 * 即关闭Nagle算法
 * @param fd
 */
void setSocketNodelay(int fd) {		//
  int enable = 1;
  //使用setsockopt函数设置fd描述符的IP协议TCP的TCP_NODELAY参数为1，即关闭Nagle算法
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
}

/**
 * 在调用close()关闭socket之后，对描述符fd的操作
 * @param fd
 */
void setSocketNoLinger(int fd) {		//
  struct linger linger_;
  linger_.l_onoff = 1;		//设置linger_的l_onoff参数为1，表示关闭socket时，如果还有数据没有发送完，则立即返回
  linger_.l_linger = 30;	//将l_linger参数设置的时间内发送剩余数据
  setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&linger_,
             sizeof(linger_));
}

/**
 * 关闭指定的文件描述符（fd）上的写入，
 * 从而禁止向该文件描述符发送数据
 * @param fd
 */
void shutDownWR(int fd) {		//
  shutdown(fd, SHUT_WR);	//SHUT_WR表示要停止的操作（写入）
  // printf("shutdown\n");
}

/**
 * 创建TCP socket并绑定和监听指定的端口
 * @param port
 * @return
 */
int socket_bind_listen(int port) {		//
  //检查端口号是否处于合理范围（0~65535）
  if (port < 0 || port > 65535) return -1;

  // 创建socket(IPv4 + TCP)，返回监听描述符
  int listen_fd = 0;
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return -1;

  // 消除bind时"Address already in use"错误
  int optval = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval,
                 sizeof(optval)) == -1) {
    close(listen_fd);
    return -1;
  }

  // 设置服务器IP和Port，和监听描述副绑定
  struct sockaddr_in server_addr;
  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons((unsigned short)port);
  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    close(listen_fd);
    return -1;
  }

  // 开始监听，最大等待队列长为LISTENQ
  if (listen(listen_fd, 2048) == -1) {
    close(listen_fd);
    return -1;
  }

  // 无效监听描述符
  if (listen_fd == -1) {
    close(listen_fd);
    return -1;
  }
  return listen_fd;	//返回有效的监听描述符
}