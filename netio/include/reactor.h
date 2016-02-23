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
#include "msgq_manager.h"
using namespace std;

class CReactor;

typedef enum {
	NONE_FLAG = 0,
	TCP_CLIENT = 1,
	TCP_SERVER_READ,
	TCP_SERVER_ACCEPT,
	TCP_SERVER_SEND,
	TCP_SERVER_CLOSE,
	UDP_READ,
	UDP_SEND,
} EventFlag_t;

//Epoll结构中传入的数据信息
struct stEpollItem {
	int fd;
	EventFlag_t enEventFlag;
	void* pData;

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

	stEpollItem() {
		fd=0;
		enEventFlag = NONE_FLAG;
		pData = NULL;
	}

	void Reset() {
		fd=0;
		enEventFlag = NONE_FLAG;
		pData = NULL;
	}
};

class CUserEventHandler {
public:
	virtual ~CUserEventHandler() {};
	virtual int OnEventFire(void* pvParam = 0) = 0;
	virtual int CheckEvent(void* pvParam = 0) = 0;
};

class CContainerEventHandler : public CUserEventHandler {
public:
	int OnEventFire(void* pvParam= 0);
	int CheckEvent(void* pvParam= 0);
	CMsgQManager* m_pMQManager;
	key_t m_iMqKey;
	int RegisterMqInfo(CMsgQManager* m_pMQManager, key_t iMqKey);
	~CContainerEventHandler(){};
};

class CNetIOUserEventHandler : public CUserEventHandler {
public:
	int OnEventFire(void* pvParam= 0);
	int CheckEvent(void* pvParam= 0);
	CMsgQManager* m_pMQManager;
	int RegisterMqManager(CMsgQManager* m_pMQManager);
	~CNetIOUserEventHandler(){};
};


class CNetHandler {
public:
	virtual ~CNetHandler() = 0;//定义虚析构是一个好习惯，好让父类有虚函数表入口。否则向上转型利用时，将不能享受多态的好处
public:
	virtual int HandleEvent(int iConn, int iType) = 0;
};

class CTcpNetHandler : public CNetHandler {
public:
	CTcpNetHandler();
	~CTcpNetHandler();
public:
	int HandleEvent(int iConn, int iType);
	int RegisterMqManager(CMsgQManager* m_pMQManager);
private:
	int DoConn(int iConn);
	int DoRecv(int iConn);//mqmanager应该是reactor回调的成员，因为框架不关心事件处理细节
	int DoSend(int iConn);
	int DoClose(int iConn);

public:
	CReactor* m_pReactor;
	CMsgQManager* m_pMQManager;
};

class CUSockUdpHandler : public CNetHandler {
public:
	CUSockUdpHandler();
	~CUSockUdpHandler();
public:
	int HandleEvent(int iConn, int iType);
private:
	int DoConn(int iConn);
	int DoRecv(int iConn);
	int DoSend(int iConn);
	int DoClose(int iConn);

public:
	CReactor* m_pReactor;
};

class CReactor {
	friend class CTcpNetHandler;
	friend class CUSockUdpHandler;
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
	int AddToWatchList(int iFd, EventFlag_t type, void* pData = NULL);
	int RemoveFromWatchList(int iFd);
	int InitTcpSvr(int iTcpSvrPort);
	int InitUSockUdpSvr(const char* pszUSockPath);

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
	static const int DEFAULT_EPOLL_WAIT_TIME = 10;//毫秒
	int m_iEpFd;
	epoll_event* m_arrEpollEvents; //用于方法间参数传递
	int m_iEvents;
	int m_iEpollSucc;//每次Epoll_event的handle处理结果，用于在方法间传递信息

	stEpollItem m_arrEpollItem[MAX_EPOLL_EVENT_NUM]; //用于epoll信息的保存
	//key_t iBackMsgKey;
};



#endif /* NETIO_INCLUDE_REACTOR_H_ */
