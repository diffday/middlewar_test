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
using namespace std;

class CReactor;

typedef enum {
	TCP_CLIENT = 1,
	TCP_SERVER_READ,
	TCP_SERVER_ACCEPT,
	UDP
} EventFlag_t;

struct stSocketEvent {
	int fd;
	EventFlag_t enEventFlag;
	void* pData;
};

class CNetHandler {
public:
	virtual ~CNetHandler() = 0;
public:
	virtual int HandleEvent(int iConn, int iType) = 0;
};

class CTcpNetHandler : public CNetHandler {
public:
	CTcpNetHandler();
	~CTcpNetHandler();
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

	void RunEventLoop();
protected:
	int CheckEvents();
	int ProcessSocketEvent();
private:
	int AddToWatchList(int iFd, EventFlag_t type, void* pData = NULL);
	int RemoveFromWatchList(int iFd);
	int InitTcpSvr(int iTcpSvrPort);
	int InitUSockUdpSvr(const char* pszUSockPath);

private:
	vector<int> m_vecFds;
	int m_iSvrFd; //接收外部请求的socket
	int m_iUSockFd; //接收容器回包唤醒的Usock
	CTcpNetHandler* m_pTcpNetHandler;
	CUSockUdpHandler* m_pUSockUdpHandler;
	char* m_pszBuf;
	int m_iBufLen;
	bool m_bInit;
	static const int MAX_EPOLL_EVENT_NUM = 4096;
	static const int DEFAULT_EPOLL_WAIT_TIME = 10;//毫秒
	int m_iEpFd;
	epoll_event* m_arrEpollEvents;
	int m_iEvents;
	int m_iEpollSucc;
};



#endif /* NETIO_INCLUDE_REACTOR_H_ */
