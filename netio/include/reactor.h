/*
 * reactor.h
 *
 *  Created on: 2015年12月22日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_REACTOR_H_
#define NETIO_INCLUDE_REACTOR_H_
#include <vector>
#include <sys/epoll.h>
#include <stdio.h>
#include <netinet/in.h>
#include "intf_user_event_handler.h"
#include "tcp_net_handler.h"
#include "usock_udp_handler.h"
using namespace std;

class CReactor;

typedef enum {
	NONE_FLAG = 0,
	TCP_CLIENT = 1, //无实际意义
	TCP_SERVER_ACCEPT,
	TCP_SERVER_READ,
	TCP_SERVER_SEND,
	TCP_SERVER_CLOSE,
	UDP_READ,
	UDP_SEND,
} EventFlag_t;

//Epoll结构中传入的数据信息
struct stTcpSockItem {
	int fd;
	EventFlag_t enEventFlag;
	sockaddr_in stSockAddr_in;

	/*
	stEpollItem(const stEpollItem& stOther) {
		printf("call stEpollItem copy construct\n");
		fd = stOther.fd;
		enEventFlag = stOther.enEventFlag;
		pData = stOther.pData;
	}

	stEpollItem& operator = (stEpollItem& stOther) {
		printf("call operator =\n");
		fd = stOther.fd;
		enEventFlag = stOther.enEventFlag;
		pData = stOther.pData;
		return *this;
	}*/

	stTcpSockItem() {
		fd=0;
		enEventFlag = NONE_FLAG;
	}

	void Reset() {
		fd=0;
		enEventFlag = NONE_FLAG;
	}
};

class CReactor {
	friend class CTcpNetHandler;
	friend class CUSockUdpHandler;
	friend class CNetIOUserEventHandler;
	friend class CContainerEventHandler;
public:
	CReactor();
	~CReactor();
public:
	int Init(int iTcpSvrPort, const char* pszUSockPath); //当iTcpSvrPort=0时，不启用tcp svr监听服务。当pszUSockPath=NULL(0)时，不启用usock的udp svr服务
	int RegisterTcpNetHandler(CTcpNetHandler* pTcpNetHandler);
	//USock默认用UDP SVR服务
	int RegisterUSockUdpHandler(CUSockUdpHandler* pUSockUdpHandler);
	int RegisterUserEventHandler(CUserEventHandler* pUserEventHandler);

	void RunEventLoop();
protected:
	int CheckEvents();
	int ProcessEvent();
private:
	//Epoll事件，加入或更新
	int AddToWatchList(int iFd, EventFlag_t type, void* pData = NULL,bool bCheckSock = true);
	int RemoveFromWatchList(int iFd);
	int InitTcpSvr(int iTcpSvrPort);
	int InitUSockUdpSvr(const char* pszUSockPath);
	const char* GetEventFlag(int iFlag);

private:
	vector<int> m_vecFds;//clear和erase不释放空间，只是析构

	int m_iSvrFd; //接收外部请求的socket
	int m_iUSockFd; //接收容器回包唤醒的Usock
	CTcpNetHandler* m_pTcpNetHandler;
	CUSockUdpHandler* m_pUSockUdpHandler;
	CUserEventHandler* m_pUserEventHandler;
	char* m_pszBuf;
	int m_iBufLen;

	bool m_bInit;
	static const int MAX_EPOLL_EVENT_NUM = 4096;
	//static const int DEFAULT_EPOLL_WAIT_TIME = 10;//毫秒
	int m_iEpFd;
	epoll_event* m_arrEpollEvents; //用于方法间参数传递
	int m_iEvents;
	int m_iEpollSucc;//每次Epoll_event的handle处理结果，用于在方法间传递信息

	stTcpSockItem m_arrTcpSock[MAX_EPOLL_EVENT_NUM]; //用于epoll信息的保存
	MsgBuf_T m_arrMsg[MAX_MSG_SIZE];
	//stEpollItem m_MSG[MAX_EPOLL_EVENT_NUM];

	const char* m_pszFlag[8];
};



#endif /* NETIO_INCLUDE_REACTOR_H_ */
