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
	CTcpNetHandler(CReactor* pReactor);
public:
	int HandleEvent(int iConn, int iType);
private:
	int DoConn(int iConn);
	int DoRecv(int iConn);
	int DoSend(int iConn);
	int DoClose(int iConn);

private:
	CReactor* m_pReactor;
};

class CReactor {
	friend class CNetHandler;
public:
	CReactor();
	~CReactor();
public:
	int Init(int iPort); //此处我们默认初始化tcp svr服务
	int RegisterTcpNetHandler(CTcpNetHandler* pTcpNetHandler);

	void RunEventLoop();
protected:
	int CheckEvents();
	int ProcessSocketEvent();
private:
	int AddToWatchList(int iFd, int type);
	int RemoveFromWatchList(int iFd, int type);

private:
	vector<int> m_vecFds;
	int m_iSvrFd;
	CTcpNetHandler* m_pTcpNetHandler;
	char* m_pszBuf;
	int m_iBufLen;
	bool m_bInit;
	static const int MAX_EPOLL_EVENT_NUM = 4096;
	static const int DEFAULT_EPOLL_WAIT_TIME = 10;//毫秒
	int m_iEpFd;
	epoll_event* m_aEpollEvents;
	int m_nEvents;
};



#endif /* NETIO_INCLUDE_REACTOR_H_ */
