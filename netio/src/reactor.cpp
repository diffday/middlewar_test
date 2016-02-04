/*
 * reactor.cpp

 *
 *  Created on: 2015��12��23��
 *      Author: chenzhuo
 */
#include "reactor.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "global_define.h"
CNetHandler::~CNetHandler() { }
CTcpNetHandler::CTcpNetHandler() : m_pReactor(NULL) {}
CTcpNetHandler::~CTcpNetHandler() {}
CTcpNetHandler::CTcpNetHandler(CReactor* pReactor) {
	m_pReactor = pReactor;
};

int CTcpNetHandler::HandleEvent(int iConn, int iType) {
	switch (iType) {
	case 1:
		DoConn(iConn);
		break;
	case 2:
		DoRecv(iConn);
		break;
	case 3:
		DoSend(iConn);
		break;
	case 4:
		DoClose(iConn);
		break;
	}
	return 0;
}


int CTcpNetHandler::DoConn(int iConn) {
	return 0;
}

int CTcpNetHandler::DoRecv(int iConn) {
	return 0;
}

int CTcpNetHandler::DoSend(int iConn) {
	return 0;
}

int CTcpNetHandler::DoClose(int iConn) {
	return 0;
}

CReactor::CReactor() : m_pTcpNetHandler(NULL),m_iSvrFd(0) {
	m_pszBuf=new char[10240];
}
CReactor::~CReactor() {
	delete m_pszBuf;
	close(m_iSvrFd);
	if (m_pTcpNetHandler) {
		delete m_pTcpNetHandler;
	}

}

int CReactor::Init(int iPort) {
	struct sockaddr_in servaddr;
	int iRet = 0;

	m_iSvrFd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_iSvrFd < 0) {
		return CREATE_SOCKET_FAILED;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(iPort);
	int opt = 1;
	//SO_REUSEADDR ����time_wait��socket�ܷ��������°󶨡�ͬʱ�����������ɷǳ����ظ�����Ϊ������ϸչ����
	iRet = setsockopt(m_iSvrFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (iRet < 0) {
		close(m_iSvrFd);
		return SET_SOCKOPT_FAILED;
	}

	iRet = bind(m_iSvrFd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if ( 0 != iRet ) {
		close(m_iSvrFd);
		return BIND_SOCKET_FAILED;
	}

	//SO_LINGER ����ϵͳ��δ���д����׽��ַ��Ͷ����е����ݡ�����ʽ�޷����֣������ͷ��͵��Զ˺����Źر�
	/*typedef struct linger {
		u_short l_onoff; //�Ƿ�����Ĭ����0��ʾ�ر�
		u_short l_linger; //���Źر��ʱ��
	  }
	  ��l_onoffΪ0ʱ�������ṹ�����壬��ʾsocket��������Ĭ����Ϊ��closesocket�������أ����Ͷ���ϵͳ�ײ㱣�������͵��Զ����
	  l_onoff��0��l_linger=0 ��ʾ�����������ݣ�ֱ�ӷ���rst��������λ
	  l_onoff��0��l_linger��0����������ʽsocketʱ��closesocket������l_lingerʱ�䳬ʱ�����ݷ�����ɡ���ʱ�󣬷��Ͷ��л��ǻᱻ����
	*/
	linger stLinger;
	stLinger.l_onoff=1;
	stLinger.l_linger=0;
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

	iRet = fcntl(m_iSvrFd, F_SETFL, iCurrentFlag | O_NONBLOCK); //���÷�����
	if (iRet < 0) {
		close(m_iSvrFd);
		return FCNTL_SOCKET_FAILED;
	}

	m_iEpFd = ::epoll_create(MAX_EPOLL_EVENT_NUM); //���Ͽ�::������ϵͳ�⺯����ȫ�ֱ���/�������ͳ�Ա������ȷ���ֿ���
	if (-1 == m_iEpFd) {
		if (ENOSYS == errno) { //ϵͳ��֧��

		}
		return EPOLL_CREATE_FAILED;
	}

	m_aEpollEvents = new epoll_event[MAX_EPOLL_EVENT_NUM];
	memset(m_aEpollEvents,0,MAX_EPOLL_EVENT_NUM*sizeof(epoll_event));

	m_bInit = 1;

	return 0;
}

int CReactor::AddToWatchList(int iFd, int type) {
	/*
	 *
	 typedef union epoll_data { //һ����һ��fd��������
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
	 * EPOLLIN ����ʾ��Ӧ���ļ����������Զ��������Զ�SOCKET�����رգ���
EPOLLOUT����ʾ��Ӧ���ļ�����������д��
EPOLLPRI����ʾ��Ӧ���ļ��������н��������ݿɶ�������Ӧ�ñ�ʾ�д������ݵ�������
EPOLLERR����ʾ��Ӧ���ļ���������������
EPOLLHUP����ʾ��Ӧ���ļ����������Ҷϣ�
EPOLLET�� ��EPOLL��Ϊ��Ե����(Edge Triggered)ģʽ�����������Ĭ�ϵ�ˮƽ����(Level Triggered)��˵�ġ�ET��һ���¼�����ɶ�����ֻ��֪ͨһ�Ρ���LT���¼�������һֱ�ɶ�������һֱ֪ͨ
EPOLLONESHOT��ֻ����һ���¼���������������¼�֮���������Ҫ�����������socket�Ļ�����Ҫ�ٴΰ����socket���뵽EPOLL������
	 */
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = iFd;
	/*
	 * EPOLL_CTL_ADD��ע���µ�fd��epfd�У�
		EPOLL_CTL_MOD���޸��Ѿ�ע���fd�ļ����¼���
		EPOLL_CTL_DEL����epfd��ɾ��һ��fd��
	 */
	int iRet = epoll_ctl(m_iEpFd, EPOLL_CTL_ADD, iFd, &event);
	if (iRet <  0) {
		return EPOLL_CNTL_FAILED;
	}
	++m_nEvents;

	m_vecFds.push_back(iFd);

	return 0;
}

int  CReactor::RemoveFromWatchList(int iFd, int type) {
	int iRet = epoll_ctl(m_iEpFd, EPOLL_CTL_DEL, iFd, NULL);
	if (iRet <  0) {
		return EPOLL_CNTL_FAILED;
	}
	vector<int>::iterator it = m_vecFds.begin();
	for (;it!=m_vecFds.end();++it) {
		if (iFd == (*it)) {
			m_vecFds.erase(it);
			--m_nEvents;
			break;
		}
	}

	return 0;
}

int CReactor::CheckEvents() {
	if (m_nEvents > 0) {
		int iRet = epoll_wait(m_iEpFd, m_aEpollEvents, m_nEvents, DEFAULT_EPOLL_WAIT_TIME); //��ʱʱ�䵥λ�Ǻ���
		if (iRet < 0) {
			return EPOLL_WAIT_FAILED;
		}
	}
	else {
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = DEFAULT_EPOLL_WAIT_TIME;
		//int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* errorfds, struct timeval* timeout);
		select(0,NULL,NULL,NULL,&tv);
	}


	return 0;
}

int CReactor::ProcessSocketEvent() {
	return 0;
}

void CReactor::RunEventLoop() {
	int iRet = 0;
	while (1) {
		iRet = this->CheckEvents();
		if (0 == iRet) {
			continue;
		}
		this->ProcessSocketEvent();
	}
}

int CReactor::RegisterTcpNetHandler(CTcpNetHandler* pTcpNetHandler) {
	m_pTcpNetHandler = pTcpNetHandler;
	//m_pTcpNetHandler->m_pReactor = this;
	return 0;
}
