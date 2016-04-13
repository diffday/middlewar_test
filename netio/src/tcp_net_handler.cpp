/*
 * tcp_net_handler.cpp
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */
#include "tcp_net_handler.h"
#include "cmd_obj.h"
#include "assert.h"
#include "reactor.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "cache_manager.h"

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
	int i = 0;
	while (1) {

	int iConnfd = accept(iConn, (struct sockaddr *)&stCliAddr,&socklen);
	if (iConnfd < 0)
	{
		if (iConnfd == -1 && (errno == EAGAIN)) {
			printf("all conn request has accepted\n");
			//all conn request has accepted
			return 0;
		}
		perror("accept error");
	    return -1;
	}
	++i;


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

	printf("###DoConn %d accept from %s:%d,the conn fd is :%d\n", i,inet_ntoa(stCliAddr.sin_addr), stCliAddr.sin_port,iConnfd);

	stTcpSockItem stTcpSock;
	stTcpSock.fd = iConnfd;
	stTcpSock.enEventFlag = TCP_SERVER_READ;
	stTcpSock.stSockAddr_in = stCliAddr;
	m_pReactor->m_arrTcpSock[iConnfd] = stTcpSock;

	m_pReactor->AddToWatchList(iConnfd, TCP_SERVER_READ, (void*)(&(m_pReactor->m_arrTcpSock[iConnfd])));

	}
	return 0;
}

int CTcpNetHandler::DoRecv(int iConn) {
	CCacheManager* pCm =  CCacheManager::GetInstance();
	stringstream ss;
	ss<<"netio_"<<0;
	pCm->Incr(ss.str(),1);

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

	CCmd oCmd;
	oCmd.InitCCmd(buf);
	assert(oCmd.iCmd);
	//int iIndex = oCmd.iCmd % g_mapCmdDLL.size();
	int iIndex = oCmd.iCmd;
	int iCount = 1; //更改为命令值对应so的编号的下标，这里当然容错没处理
	for (std::map<int, const char*>::const_iterator it=g_mapCmdDLL.begin();it!=g_mapCmdDLL.end();++it) {
		if (iIndex == iCount) {
			iRet = m_pMQManager->GetMsgQueue(it->first, rpMsgq);
			printf("cmd %d will put request into MSGQ %x\n",oCmd.iCmd,it->first);
			assert(iRet == 0);
			break;
		}
		++iCount;
	}

	/*
	uint32_t dwCmd=0;
	map<string,string> mapPara;
	strPairAppendToMap(buf,mapPara);
	if (mapPara.find("cmd") != mapPara.end()) {
		dwCmd = static_cast<uint32_t>(atoll(mapPara.find("cmd")->second.c_str()));
		//printf("cmd is %x\n",dwCmd);
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

	assert(dwCmd);*/

	//CCmd oCmd;
	//oCmd.InitCCmd(buf);
	oCmd.iFd=iConn;
	oCmd.ifamily=m_pReactor->m_arrTcpSock[iConn].stSockAddr_in.sin_family;
	oCmd.sClientIp=inet_ntoa(m_pReactor->m_arrTcpSock[iConn].stSockAddr_in.sin_addr);
	oCmd.iPort=m_pReactor->m_arrTcpSock[iConn].stSockAddr_in.sin_port;
	oCmd.iType=REQUEST;

	//printf("dump cmdobj:%s\n",oCmd.ToString().c_str());
	/*
	iRet = m_pMQManager->GetMsgQueue(NET_IO_BACK_MSQ_KEY, rpMsgq);
	assert(iRet == 0);*/
	//printf("GetMsgQueue %x, %d\n",NET_IO_BACK_MSQ_KEY,iRet);

	MsgBuf_T stMsg;
	stMsg.lType=REQUEST;

	oCmd.ToString(stMsg.sBuf,sizeof(stMsg.sBuf));

	iRet = rpMsgq->PutMsg(&stMsg,strlen(stMsg.sBuf));
	if (0 != iRet) {
		printf("putinto msgq failed,%d,%s\n",iRet,rpMsgq->m_sLastErrMsg.c_str());
	}

	m_pReactor->m_arrTcpSock[iConn].enEventFlag = TCP_SERVER_SEND;

	return 0;
}

int CTcpNetHandler::DoSend(int iConn) {
	write(iConn, m_pReactor->m_arrMsg[iConn].sBuf, strlen(m_pReactor->m_arrMsg[iConn].sBuf));//响应客户端
	DoClose(iConn);
	return 0;
}

int CTcpNetHandler::DoClose(int iConn) {
	m_pReactor->m_arrTcpSock[iConn].Reset();
	m_pReactor->RemoveFromWatchList(iConn);
	int i = close(iConn);
	printf("###DoClose %d,errcode:%d\n",iConn,i);

	return 0;
}

int CTcpNetHandler::RegisterMqManager(CMsgQManager* pMQManager) {
	m_pMQManager = pMQManager;
}

//=====CTcpNetHandler end
