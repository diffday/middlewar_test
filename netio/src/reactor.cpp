/*
 * reactor.cpp

 *
 *  Created on: 2015年12月23日
 *      Author: chenzhuo
 */
#include "reactor.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include "global_define.h"
#include "assert.h"
#include "string_helper.h"


int CContainerEventHandler::CheckEvent(void* pvParam) {
	return 0;
}

int CContainerEventHandler::OnEventFire(void* pvParam) {
	return 0;
}


int CNetIOUserEventHandler::CheckEvent(void* pvParam) {
	return 0;
}

int CNetIOUserEventHandler::OnEventFire(void* pvParam) {
	CMsgQueue* rpMsgq;
	int iRet = m_pMQManager->GetMsgQueue(NET_IO_BACK_MSQ_KEY, rpMsgq);
	assert(iRet == 0);

	MsgBuf_T stMsg;
	stMsg.lType=RESPONSE;
	int Len;
	rpMsgq->GetMsg(&stMsg,Len);
	printf("get Msg lenth:%d,data is %s\n",Len,stMsg.sBuf);

	map<string,string> mapPara;
	strPairAppendToMap(stMsg.sBuf,mapPara);
	int ifd = static_cast<int>(atoll(mapPara.find("fd")->second.c_str()));

	m_pReactor->AddToWatchList(ifd,TCP_SERVER_SEND);
		//snprintf(stMsg.sBuf,strlen(stMsg.sBuf)+1,"%s",buf);


	return 0;
}

int CNetIOUserEventHandler::RegisterMqManager(CMsgQManager* pMQManager) {
	m_pMQManager = pMQManager;
}

CNetHandler::~CNetHandler() { }

//=====CTcpNetHandler start
CTcpNetHandler::CTcpNetHandler() : m_pReactor(NULL) {}
CTcpNetHandler::~CTcpNetHandler() {}

int CTcpNetHandler::HandleEvent(int iConn, int iType) {
	switch (iType) {
	case TCP_SERVER_ACCEPT:
		DoConn(iConn);
		break;
	case TCP_SERVER_READ:
		DoRecv(iConn);
		break;
	case TCP_SERVER_SEND:
		DoSend(iConn);
		break;
	case TCP_SERVER_CLOSE:
		DoClose(iConn);
		break;
	}
	return 0;
}


int CTcpNetHandler::DoConn(int iConn) {
	struct sockaddr_in stCliAddr;
	socklen_t socklen = sizeof(stCliAddr);
	int iConnfd = accept(iConn, (struct sockaddr *)&stCliAddr,&socklen);
	if (iConnfd < 0)
	{
	    perror("accept error");
	    return -1;
	}
	printf("accept form %s:%d\n", inet_ntoa(stCliAddr.sin_addr), stCliAddr.sin_port);

	int iCurrentFlag = 0;
	iCurrentFlag = fcntl(iConnfd, F_GETFL, 0);
	if (iCurrentFlag == -1) {
		close(iConnfd);
		return FCNTL_SOCKET_FAILED;
	}

	int iRet = fcntl(iConnfd, F_SETFL, iCurrentFlag | O_NONBLOCK); //设置非阻塞
	if (iRet < 0) {
		close(iConnfd);
		return FCNTL_SOCKET_FAILED;
	}

	printf("==do on conn operation==, the conn fd is :%d\n",iConnfd);
	m_pReactor->AddToWatchList(iConnfd, TCP_SERVER_READ);

	return 0;
}

int CTcpNetHandler::DoRecv(int iConn) {
	int nread;
	char buf[1000] = {0};
	int iRet = 0;
	nread = read(iConn, buf, 1000);//读取客户端socket流

	if (nread == 0) {
	   printf("client close the connection\n");
	   close(iConn);
	   return -1;
	}
	if (nread < 0) {
	   perror("read error");
	   close(iConn);
	   return -1;
	}

	printf("the recv info is : %s\n",buf);
	CMsgQueue* rpMsgq;
	/*
	map<string,string> mapPara;
	strPairAppendToMap(buf,mapPara);
	map<string,string>::iterator it = mapPara.begin();
	for (;it!=mapPara.end();++it){
		printf("%s=%s\n",(it->first).c_str(),(it->second).c_str());
	}*/

	map<string,string> mapPara;
	strPairAppendToMap(buf,mapPara);
	if (mapPara.find("cmd") != mapPara.end()) {
		uint32_t dwCmd = static_cast<uint32_t>(atoll(mapPara.find("cmd")->second.c_str()));
		int iIndex = dwCmd % g_mapCmdDLL.size();
		int iCount = 0;
		for (std::map<int, const char*>::const_iterator it=g_mapCmdDLL.begin();it!=g_mapCmdDLL.end();++it) {
			if (iIndex == iCount) {
				iRet = m_pMQManager->GetMsgQueue(it->first, rpMsgq);
				printf("cmd %d will put request into MSGQ %x\n",dwCmd,it->first);
				assert(iRet == 0);
				break;
			}
			++iCount;
		}
	}

	/*
	iRet = m_pMQManager->GetMsgQueue(NET_IO_BACK_MSQ_KEY, rpMsgq);
	assert(iRet == 0);*/
	//printf("GetMsgQueue %x, %d\n",NET_IO_BACK_MSQ_KEY,iRet);

	MsgBuf_T stMsg;
	stMsg.lType=REQUEST;
	snprintf(stMsg.sBuf,strlen(stMsg.sBuf)+16,"fd=%d&%s",buf);

	iRet = rpMsgq->PutMsg(&stMsg,strlen(stMsg.sBuf) + sizeof(stMsg.lType));
	if (0 != iRet) {
		printf("putinto msgq failed,%d,%s\n",iRet,rpMsgq->m_sLastErrMsg.c_str());
	}

	//m_arrEpollItem[iFd].enEventFlag = TCP_SERVER_SEND; //

	//m_pReactor->AddToWatchList();

	write(iConn, buf, nread);//响应客户端
	m_pReactor->RemoveFromWatchList(iConn);
	return 0;
}

int CTcpNetHandler::DoSend(int iConn) {
	return 0;
}

int CTcpNetHandler::DoClose(int iConn) {
	return 0;
}

int CTcpNetHandler::RegisterMqManager(CMsgQManager* pMQManager) {
	m_pMQManager = pMQManager;
}

//=====CTcpNetHandler end

//=====CUSockUdpHandler start
CUSockUdpHandler::CUSockUdpHandler() {}
CUSockUdpHandler::~CUSockUdpHandler() {}
/*
CUSockUdpHandler::CUSockUdpHandler(CReactor* pReactor) {
	m_pReactor = pReactor;
};*/

int CUSockUdpHandler::HandleEvent(int iConn, int iType) {
	switch (iType) {
	case UDP_READ:
		DoRecv(iConn);
		break;
	case UDP_SEND:
		DoSend(iConn);
		break;
	}
	return 0;
}

int CUSockUdpHandler::DoConn(int iConn) {
	return 0;
}

int CUSockUdpHandler::DoRecv(int iConn) {
	return 0;
}

int CUSockUdpHandler::DoSend(int iConn) {
	return 0;
}

int CUSockUdpHandler::DoClose(int iConn) {
	return 0;
}

//=====CUSockUdpHandler end

CReactor::CReactor() : m_pTcpNetHandler(NULL),m_pUSockUdpHandler(NULL),m_iSvrFd(0),m_iUSockFd(0),m_iEpollSucc(0),m_pUserEventHandler(NULL) {
	m_pszBuf=new char[10240];
	m_iEvents = 0;

}
CReactor::~CReactor() {
	delete m_pszBuf;
	close(m_iSvrFd);
	close(m_iUSockFd);
	if (m_pTcpNetHandler) {
		delete m_pTcpNetHandler;
	}
	if (m_pUSockUdpHandler) {
		delete m_pUSockUdpHandler;
	}
}

int CReactor::InitTcpSvr(int iTcpSvrPort) {
	/*
	 * struct sockaddr_in

	 {

	 short sin_family;/Address family一般来说AF_INET（地址族）PF_INET（协议族）

	 unsigned short sin_port;//Port number(必须要采用网络数据格式,普通数字可以用htons()函数转换成网络数据格式的数字)

	 struct in_addr sin_addr;//IP address in network byte order（Internet address）

	 unsigned char sin_zero[8];//Same size as struct sockaddr没有实际意义,只是为了　跟SOCKADDR结构在内存中对齐

	 };
	 */
	struct sockaddr_in stServaddr;
	int iRet = 0;
	m_iSvrFd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_iSvrFd < 0) {
		return CREATE_SOCKET_FAILED;
	}
	bzero(&stServaddr, sizeof(stServaddr));
	stServaddr.sin_family = AF_INET;
	stServaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	stServaddr.sin_port = htons(iTcpSvrPort);
	int opt = 1;
	//SO_REUSEADDR 决定time_wait的socket能否被立即重新绑定。同时允许其它若干非常规重复绑定行为，不详细展开了
	iRet = setsockopt(m_iSvrFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (iRet < 0) {
		close(m_iSvrFd);
		return SET_SOCKOPT_FAILED;
	}

	iRet = bind(m_iSvrFd, (struct sockaddr *) &stServaddr, sizeof(stServaddr));
	if (0 != iRet) {
		close(m_iSvrFd);
		return BIND_SOCKET_FAILED;
	}

	//SO_LINGER 决定系统如何处理残存在套接字发送队列中的数据。处理方式无非两种，丢弃和发送到对端后优雅关闭
	/*typedef struct linger {
	 u_short l_onoff; //是否开启，默认是0表示关闭
	 u_short l_linger; //优雅关闭最长时限
	 }
	 当l_onoff为0时，整个结构无意义，表示socket继续沿用默认行为。closesocket立即返回，发送队列系统底层保持至发送到对端完成
	 l_onoff非0，l_linger=0 表示立即丢弃数据，直接发送rst包，自身复位
	 l_onoff非0，l_linger非0，当是阻塞式socket时，closesocket阻塞到l_linger时间超时或数据发送完成。超时后，发送队列还是会被丢弃
	 */
	linger stLinger;
	stLinger.l_onoff = 1;
	stLinger.l_linger = 0;
	iRet = setsockopt(m_iSvrFd, SOL_SOCKET, SO_LINGER, &stLinger, sizeof(stLinger));
	if (iRet < 0) {
		close(m_iSvrFd);
		return SET_SOCKOPT_FAILED;
	}

	int iBackLog = 20;
	iRet = listen(m_iSvrFd, iBackLog);
	if (iRet < 0) {
		close(m_iSvrFd);
		return LISTEN_SOCKET_FAILED;
	}

	int iCurrentFlag = 0;
	iCurrentFlag = fcntl(m_iSvrFd, F_GETFL, 0);
	if (iCurrentFlag == -1) {
		close(m_iSvrFd);
		return FCNTL_SOCKET_FAILED;
	}

	iRet = fcntl(m_iSvrFd, F_SETFL, iCurrentFlag | O_NONBLOCK); //设置非阻塞
	if (iRet < 0) {
		close(m_iSvrFd);
		return FCNTL_SOCKET_FAILED;
	}

	return 0;
}

int CReactor::InitUSockUdpSvr(const char* pszUSockPath) {
	//printf("InitUSockUdpSvr\n");
	/*
	 * AF 表示ADDRESS FAMILY 地址族;PF 表示PROTOCL FAMILY 协议族
	 * AF_UNIX=AF_LOCAL, PF_UNIX=PF_LOCAL.如果看到usock初始化写法时用_UNIX也不用惊奇。不同的unix分支与linux可能有所区别
	 * 但可以通过公共网络库的来处理跨平台的问题，而不是如下这样绑定在linux中
	 *
	 * 一般的建议是
	 * 对于socketpair与socket的domain参数,使用PF_LOCAL系列,而在初始化套接口地址结构时,则使用AF_LOCAL.
	 */
	m_iUSockFd = ::socket(PF_LOCAL, SOCK_DGRAM, 0);
	if (m_iUSockFd < 0) {
		return CREATE_USOCK_FAILED;
	}

	mode_t old_mod = umask(S_IRWXO); // Set umask = 002
	/*
	 * struct sockaddr_un {
               sa_family_t sun_family;               // AF_UNIX
               char        sun_path[108];            // pathname
       };
	 */
	struct sockaddr_un stServaddr;
	bzero(&stServaddr, sizeof(stServaddr));
	stServaddr.sun_family = AF_LOCAL;
	strncpy(stServaddr.sun_path, pszUSockPath, sizeof(stServaddr.sun_path));

	int opt = 1;
	int iRet = 0;
	iRet = setsockopt(m_iUSockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (iRet < 0) {
		close(m_iUSockFd);
		return SET_USOCKOPT_FAILED;
	}
	unlink(pszUSockPath); // unlink it if exist
	//printf("unlink-iRet:%d\n",iRet);


	/*
	 * #define SUN_LEN(su) \
        (sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))
       因为sockaddr与sockaddr_un的顶部结构是兼容的，所以这个预定义长度不对等的转换也是没问题的
	 */
	iRet = bind(m_iUSockFd, (struct sockaddr *) &stServaddr, SUN_LEN(&stServaddr));
	if (0 != iRet) {
		close(m_iUSockFd);
		printf("iRet:%d,errno:%d,info:%s\n",iRet,errno,strerror(errno));
		return BIND_USOCKET_FAILED;
	}
	umask(old_mod);

	int iCurrentFlag = 0;
	iCurrentFlag = fcntl(m_iUSockFd, F_GETFL, 0);
	if (iCurrentFlag == -1) {
		close(m_iUSockFd);
		return FCNTL_SOCKET_FAILED;
	}

	iRet = fcntl(m_iUSockFd, F_SETFL, iCurrentFlag | O_NONBLOCK); //设置非阻塞
	if (iRet < 0) {
		close(m_iUSockFd);
		return FCNTL_SOCKET_FAILED;
	}

	return 0;
}

int CReactor::Init(int iTcpSvrPort, const char* pszUSockPath) {
	int iRet = 0;

	if (iTcpSvrPort) {
		iRet = InitTcpSvr(iTcpSvrPort);
		if (iRet) {
			return iRet;
		}
	}

	if (NULL != pszUSockPath) {
		iRet = InitUSockUdpSvr(pszUSockPath);
		if (iRet) {
			return iRet;
		}
	}

	//初始化Epoll数据部分
	m_iEpFd = ::epoll_create(MAX_EPOLL_EVENT_NUM); //加上空::表明是系统库函数或全局变量/函数，和成员函数明确区分开来
	if (-1 == m_iEpFd) {
		if (ENOSYS == errno) { //系统不支持
			assert(0);
		}
		return EPOLL_CREATE_FAILED;
	}

	m_arrEpollEvents = new epoll_event[MAX_EPOLL_EVENT_NUM];
	memset(m_arrEpollEvents, 0, MAX_EPOLL_EVENT_NUM * sizeof(epoll_event));

	m_bInit = 1;

	return 0;
}

int CReactor::AddToWatchList(int iFd, EventFlag_t emTodoOp, void* pData) {
	/*
	 *
	 typedef union epoll_data { //一般填一个fd参数即可
	 	 void        *ptr;
	 	 int          fd;
	 	 uint32_t     u32;
	 	 uint64_t     u64;
	 } epoll_data_t;

 	 struct epoll_event {
          uint32_t     events;      // Epoll events
          epoll_data_t  data;        //User data variable
     };
	 */

	struct epoll_event event;
	/*
	 * EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
EPOLLOUT：表示对应的文件描述符可以写；
EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
EPOLLERR：表示对应的文件描述符发生错误；
EPOLLHUP：表示对应的文件描述符被挂断；
EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于默认的水平触发(Level Triggered)来说的。ET对一个事件（如可读），只会通知一次。但LT当事件持续（一直可读），会一直通知
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里
	 */
	event.events = EPOLLIN | EPOLLET;
	stEpollItem stSockItem;
	stSockItem.fd = iFd;
	stSockItem.enEventFlag = emTodoOp;
	stSockItem.pData = pData;

	m_arrEpollItem[iFd] = stSockItem;

	//event.data.fd = iFd; //用自定义结构的ptr代替传统的fd来区分信息
	event.data.ptr = reinterpret_cast<void*>(&m_arrEpollItem[iFd]);
	/*
	EPOLL_CTL_ADD：注册新的fd到epfd中；
	EPOLL_CTL_MOD：修改已经注册的fd的监听事件；
	EPOLL_CTL_DEL：从epfd中删除一个fd；
	 */
	int op = EPOLL_CTL_ADD;
	for(vector<int>::iterator it=m_vecFds.begin();it != m_vecFds.begin(); ++it) {
		if ((*it) == iFd) {
			op = EPOLL_CTL_MOD;
			break;
		}
	}
	/*int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)*/
	int iRet = epoll_ctl(m_iEpFd, op, iFd, &event);
	if (iRet <  0) {
		printf("%d,error:%d,info:%s\n",iRet,errno,strerror(errno));
		return EPOLL_CNTL_FAILED;
	}

	if (EPOLL_CTL_MOD != op) {
		++m_iEvents;
		m_vecFds.push_back(iFd);
	}
	printf("add fd,iEventCount:%d,op:%d\n",m_iEvents, emTodoOp);

	return 0;
}

int  CReactor::RemoveFromWatchList(int iFd) {
	int iRet = epoll_ctl(m_iEpFd, EPOLL_CTL_DEL, iFd, NULL);
	if (iRet <  0) {
		return EPOLL_CNTL_FAILED;
	}
	vector<int>::iterator it = m_vecFds.begin();
	for (;it!=m_vecFds.end();++it) {
		if (iFd == (*it)) {
			m_vecFds.erase(it);
			--m_iEvents;
			break;
		}
	}

	return 0;
}

int CReactor::CheckEvents() {
	if (m_pUserEventHandler) {
		int iRet = m_pUserEventHandler->CheckEvent();
		if (iRet != 0) {
			printf("error happed when checkUserEvent %d\n",iRet);
		}
	}

	if (m_iEvents > 0) {//进程间通信也放入监听。当container有数据返回到来时，可发数据包及时唤醒
		m_iEpollSucc = epoll_wait(m_iEpFd, m_arrEpollEvents, m_iEvents, DEFAULT_EPOLL_WAIT_TIME); //超时时间单位是毫秒
		//printf("event count %d\n",m_iEpollSucc);
		if (m_iEpollSucc < 0) { //出错的时候返回-1，可通过errno查看具体错误.否则返回可处理的IO个数
			return EPOLL_WAIT_FAILED;
		}
		else if (m_iEpollSucc > 0){
			printf("event count %d\n",m_iEpollSucc);
		}
	}
	else {
		printf("nothing to do,m_nEvent:%d\n",m_iEvents);
	}

	return 0;
}

int CReactor::ProcessEvent() {
	/*
	 *
	typedef union epoll_data {
		void        *ptr;
		int          fd;
		uint32_t     u32;
		uint64_t     u64;
	} epoll_data_t;

	struct epoll_event {
		uint32_t     events;      // Epoll events--EPOLLIN(for read) EPOLLOUT(for write) 等等一堆事件的掩码值
		epoll_data_t data;        //User data variable
	};
	 */
	if (m_pUserEventHandler) {
		m_pUserEventHandler->OnEventFire();
	}

	for(int i = 0; i < m_iEpollSucc; ++i)
	{
		stEpollItem* stItem= reinterpret_cast<stEpollItem*>(m_arrEpollEvents[i].data.ptr);
		if (stItem->fd == m_iSvrFd) {
			int iRet = m_pTcpNetHandler->HandleEvent(m_iSvrFd, stItem->enEventFlag);
			if (0 != iRet) {
				return iRet;
			}
		}
		else if (stItem->enEventFlag == TCP_SERVER_READ) {
			int iRet = m_pTcpNetHandler->HandleEvent(stItem->fd, stItem->enEventFlag);
			if (0 != iRet ) {
				return iRet;
			}
		}
		else if (stItem->enEventFlag == UDP_READ) {
			int iRet = m_pTcpNetHandler->HandleEvent(stItem->fd, stItem->enEventFlag);
			if (0 != iRet ) {
				return iRet;
			}
		}
		else if (stItem->enEventFlag == TCP_SERVER_SEND) {
			int iRet = m_pTcpNetHandler->HandleEvent(stItem->fd, stItem->enEventFlag);
			if (0 != iRet ) {
				return iRet;
			}
		}
		else  {
			printf("无效的请求应不处理丢弃\n");//socket事件不对
		}
		/* 传统只根据Fd进行处理的办法，无法适应复杂的代码编写结构
		 if (m_arrEpollEvents[i].data.fd == m_iSvrFd) {
			 int iRet = m_pTcpNetHandler->HandleEvent(m_iSvrFd,1);
			 if (0 != iRet ) {
				 return iRet;
			 }
		 }
		 else {
			 int iRet = m_pTcpNetHandler->HandleEvent(m_arrEpollEvents[i].data.fd,2);
			 if (0 != iRet ) {
			 	return iRet;
			 }
		 }*/

	}

	m_iEpollSucc = 0;
	return 0;
}

void CReactor::RunEventLoop() {
	int iRet = 0;
	AddToWatchList(m_iSvrFd, TCP_SERVER_ACCEPT); //将两大IO通道加入监听
	AddToWatchList(m_iUSockFd, TCP_SERVER_ACCEPT);
	while (1) {
		iRet = this->CheckEvents();
		if (0 == iRet) { //没有任何东西要处理时，小小暂停一下
			/**
			 * 不用sleep是因为sleep会让整个进程切换状态，等待操作系统的唤醒调用，最差的情况可能要10s钟的时间
			 * 并不能精确的做到短时间段的sleep任务
			 *
			 * 不加等待机制（阻塞）的逻辑也是不对的，这样进程会占住cpu过多的时间才放开。不利于多进程间cpu时间的有效利用
			 */
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = DEFAULT_EPOLL_WAIT_TIME;
			/*int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* errorfds, struct timeval* timeout);*/
			select(0,NULL,NULL,NULL,&tv); //纯纯的sleep
			//printf("just sleep a little\n");
		}

		iRet = this->ProcessEvent();
		if (iRet) {
			printf("something error happen!%d\n",iRet);
			break;
		}
	}
}

int CReactor::RegisterTcpNetHandler(CTcpNetHandler* pTcpNetHandler) {
	m_pTcpNetHandler = pTcpNetHandler;
	m_pTcpNetHandler->m_pReactor = this;
	return 0;
}

int CReactor::RegisterUSockUdpHandler(CUSockUdpHandler* pUSockUdpHandler) {
	m_pUSockUdpHandler = pUSockUdpHandler;
	m_pUSockUdpHandler->m_pReactor = this;
	return 0;
}

int CReactor::RegisterUserEventHandler(CUserEventHandler* pUserEventHandler) {
	m_pUserEventHandler = pUserEventHandler;
	m_pUserEventHandler->m_pReactor = this;
	return 0;
}

