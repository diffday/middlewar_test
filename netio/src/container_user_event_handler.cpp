/*
 * container_user_event_handler.cpp
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */
#include "container_user_event_handler.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int CContainerEventHandler::CheckEvent(void* pvParam) {
	return 0;
}


int CContainerEventHandler::RespNotify() {
	int iSockfd = ::socket(PF_LOCAL, SOCK_DGRAM, 0);
	printf("create fd:%d\n", iSockfd);
	if (iSockfd < 0) {
		printf("USockUDPSendTo-create socket failed\n");
		return -1;
	}

	char pSendBuf[2] = { 'w', '\0' };
	// Make Peer Addr
	struct sockaddr_un stUNIXAddr;
	memset(&stUNIXAddr, 0, sizeof(stUNIXAddr));
	stUNIXAddr.sun_family = AF_LOCAL;
	//StrMov(stUNIXAddr.sun_path, pszSockPath); // "/tmp/pipe_channel.sock"
	strncpy(stUNIXAddr.sun_path, NET_IO_USOCK_PATH,	sizeof(stUNIXAddr.sun_path));

	// Send Buffer
	int iBytesSent = ::sendto(iSockfd, pSendBuf, 1, 0, (struct sockaddr *) &(stUNIXAddr), sizeof(struct sockaddr_un));

	if (iBytesSent == -1 || static_cast<uint32_t>(iBytesSent) != 1) {
		printf("USockUDPSendTo-send notify failed\n");

		return -2;
	}

	printf("close fd:%d,ret:%d\n", iSockfd, close(iSockfd)); //udp同样要关闭

	return 0;
}

int CContainerEventHandler::OnEventFire(void* pvParam) {
	//printf("CContainerEventHandler::OnEventFire come in \n");
	CMsgQueue* rpMsgq;
	m_pMQManager->GetMsgQueue(m_iMqKey,rpMsgq);

	MsgBuf_T stMsg;
	stMsg.Reset();
	stMsg.lType = REQUEST;

	int Len = 0;
	int iret = rpMsgq->GetMsg(&stMsg,Len);
	if (Len == 0) {
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = DEFAULT_EPOLL_WAIT_TIME;

		select(0,NULL,NULL,NULL,&tv); //纯纯的sleep
		return 0;
	}
	else {
		printf("get Msg lenth:%d,data is %s\n",Len,stMsg.sBuf);
	}
	CCmd oCmd;
	oCmd.InitCCmd(stMsg.sBuf);

	map<int,CServiceDispatcher*>::iterator it = m_mapPSvcDispatcher.find(oCmd.iCmd);
	if (it != m_mapPSvcDispatcher.end()) {
		int iRet = it->second->Dispatch(oCmd);
		if (iRet != 0 && iRet != UNFINISH_TASK_RET_FLAG) {
			oCmd.iRet = iRet;
			oCmd.sData = "resp=Err happend!";
		}
		else if (iRet == UNFINISH_TASK_RET_FLAG) { //未做完的任务等待再次被唤醒继续完成
			return 0;
		}

		stringstream ss;
		ss<<"resp=This is the resp from "<<oCmd.iCmd<< " by pid:" << getpid();

		oCmd.sData = ss.str();

		m_pMQManager->GetMsgQueue(NET_IO_BACK_MSQ_KEY,rpMsgq);
		MsgBuf_T stMsg2;
		stMsg.Reset();
		stMsg2.lType = RESPONSE;

		oCmd.ToString(stMsg2.sBuf,sizeof(stMsg2.sBuf));
		rpMsgq->PutMsg(&stMsg2,strlen(stMsg2.sBuf));

		RespNotify();
	}
	else {
		printf("can't find:%d dispatcher\n",oCmd.iCmd);
	}

	return 0;
}


int CContainerEventHandler::RegisterMqInfo(CMsgQManager* pMQManager, key_t iMqKey) {
	m_pMQManager = pMQManager;
	m_iMqKey = iMqKey;
}

int CContainerEventHandler::RegisterSvcDispatcher(int iCmd,CServiceDispatcher* pSvcDispatcher) {
	m_mapPSvcDispatcher[iCmd] = pSvcDispatcher;
}
