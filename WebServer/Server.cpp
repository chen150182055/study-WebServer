// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "Server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include "Util.h"
#include "base/Logging.h"

/**
 * 构造函数
 * @param loop
 * @param threadNum 线程池中线程的数目
 * @param port 要监听的端口号
 */
Server::Server(EventLoop *loop, int threadNum, int port)
	: loop_(loop),
	  threadNum_(threadNum),
	  eventLoopThreadPool_(new EventLoopThreadPool(loop_,threadNum)),
	  started_(false),
	  acceptChannel_(new Channel(loop_)),
	  port_(port),
	  listenFd_(socket_bind_listen(port_)) {
  acceptChannel_->setFd(listenFd_);
  //调用handle_for_sigpipe函数和setSocketNonBlocking函数将socket设置成非阻塞模式
  handle_for_sigpipe();
  if (setSocketNonBlocking(listenFd_) < 0) {
	perror("set socket non block failed");
	abort();
  }
}

/**
 * 启动函数
 */
void Server::start() {
  //利用eventLoopThreadPool_开启一组I/O线程
  eventLoopThreadPool_->start();
  // acceptChannel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
  //设置acceptChannel_的事件类型为EPOLLIN、EPOLLET
  acceptChannel_->setEvents(EPOLLIN | EPOLLET);
  //设置读取处理程序为handNewConn函数
  acceptChannel_->setReadHandler(bind(&Server::handNewConn, this));
  //连接处理程序为handThisConn函数
  acceptChannel_->setConnHandler(bind(&Server::handThisConn, this));
  //将acceptChannel_添加到loop_的轮询器
  loop_->addToPoller(acceptChannel_, 0);
  //将started_设置为true，表示服务器已经启动
  started_ = true;
}

/**
 * 接收新的连接
 */
void Server::handNewConn() {
  //存储客户端的地址信息
  struct sockaddr_in client_addr;
  memset(&client_addr, 0, sizeof(struct sockaddr_in));   //初始化结构client_addr
  socklen_t client_addr_len = sizeof(client_addr);
  int accept_fd = 0;
  while ((accept_fd = accept(listenFd_, (struct sockaddr *) &client_addr, &client_addr_len)) > 0) {   //关键1,accept
	//使用getNextLoop函数获取一个EventLoop对象
	EventLoop *loop = eventLoopThreadPool_->getNextLoop();
	//使用日志系统记录来自客户端的IP地址和端口号
	LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":"
		<< ntohs(client_addr.sin_port);
	// cout << "new connection" << endl;
	// cout << inet_ntoa(client_addr.sin_addr) << endl;
	// cout << ntohs(client_addr.sin_port) << endl;
	/*
	// TCP的保活机制默认是关闭的
	int optval = 0;
	socklen_t len_optval = 4;
	getsockopt(accept_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
	cout << "optval ==" << optval << endl;
	*/
	//创建一个HttpData对象，然后将新连接的文件描述符和EventLoop对象传递给HttpData
	// 限制服务器的最大并发连接数
	if (accept_fd >= MAXFDS) {
	  close(accept_fd);    //如果超过了最大值，则会关闭该连接
	  continue;
	}
	// 设为非阻塞模式
	if (setSocketNonBlocking(accept_fd) < 0) {
	  LOG << "Set non block failed!";
	  // perror("Set non block failed!");
	  return;
	}

	setSocketNodelay(accept_fd);
	// setSocketNoLinger(accept_fd);

	//创建一个HttpData对象，然后将新连接的文件描述符和EventLoop对象传递给HttpData
	shared_ptr<HttpData> req_info(new HttpData(loop, accept_fd));
	req_info->getChannel()->setHolder(req_info);
	//调用queueInLoop函数来把新创建的HttpData对象放入EventLoop的消息队列
	loop->queueInLoop(std::bind(&HttpData::newEvent, req_info));
  }
  acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}