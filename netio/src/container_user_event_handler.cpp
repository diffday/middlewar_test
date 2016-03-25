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

	//每轮循环中，尝试取两个消息。一个是简单的REQUEST类型。
	//另一个是跟本进程相关的msgq消息，对应于本机的服务间互调。
	MsgBuf_T stMsg;
	stMsg.Reset();
	stMsg.lType = REQUEST;
	CCmd oCmd;
	int Len = 0;
	int iret = rpMsgq->GetMsg(&stMsg,Len);
	if (Len == 0) {
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = DEFAULT_EPOLL_WAIT_TIME;

		select(0,NULL,NULL,NULL,&tv); //纯纯的sleep
		//return 0;

		//stMsg.Reset();
			int iPid = getpid();
			stMsg.lType = iPid * 10 + APP;
			//printf("try to check app reponse_%d\n", stMsg.lType);
			//Len = 0;
			iret = rpMsgq->GetMsg(&stMsg,Len);
			if (Len == 0) {
				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = DEFAULT_EPOLL_WAIT_TIME;

				select(0,NULL,NULL,NULL,&tv); //纯纯的sleep
				return 0;
			}
			else {
				printf("get APP_RESPONSE_%d Msg lenth:%d,data is %s\n", iPid,Len,stMsg.sBuf);
			}
			//CCmd oCmd;
			oCmd.InitCCmd(stMsg.sBuf);

			ProcessMsg(oCmd);
			return 0;
	}
	else {
		printf("get REQUEST Msg lenth:%d,data is %s\n",Len,stMsg.sBuf);
	}
	//CCmd oCmd;
	oCmd.InitCCmd(stMsg.sBuf);
	if (oCmd.iSvcSerialNo) {
		printf("-----It's a Request from app %d----\n",oCmd.iSvcSerialNo);
	}

	ProcessMsg(oCmd);


	/*
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
		ss<<oCmd.sData<<" by pid:" << getpid();

		oCmd.sData = ss.str();

		m_pMQManager->GetMsgQueue(NET_IO_BACK_MSQ_KEY,rpMsgq);
		MsgBuf_T stMsg2;
		stMsg2.Reset();
		stMsg2.lType = RESPONSE;

		oCmd.iType = RESPONSE; //这里的设置没什么意义，因为netio不认这字段。走到这一步，都是方法调用全部做完了。
		oCmd.ToString(stMsg2.sBuf,sizeof(stMsg2.sBuf));
		rpMsgq->PutMsg(&stMsg2,strlen(stMsg2.sBuf));

		RespNotify();
	}
	else {
		printf("can't find:%d dispatcher\n",oCmd.iCmd);
	}*/

	return 0;
}


int CContainerEventHandler::RegisterMqInfo(CMsgQManager* pMQManager, key_t iMqKey) {
	m_pMQManager = pMQManager;
	m_iMqKey = iMqKey;
}

int CContainerEventHandler::RegisterSvcDispatcher(int iCmd,CServiceDispatcher* pSvcDispatcher) {
	m_mapPSvcDispatcher[iCmd] = pSvcDispatcher;
}

int CContainerEventHandler::ProcessMsg(CCmd& oCmd) {
	if (oCmd.iSvcSerialNo) {
		printf("===start to process app request %d\n",oCmd.iSvcSerialNo);
	}
	//当前的container进程初始化的dispatcher无需按命令路由
	//map<int,CServiceDispatcher*>::iterator it = m_mapPSvcDispatcher.find(oCmd.iCmd);
	map<int,CServiceDispatcher*>::iterator it = m_mapPSvcDispatcher.begin();
	//it = m_mapPSvcDispatcher.find(oCmd.iCmd);
		//if (it != m_mapPSvcDispatcher.end()) {
			int iRet = it->second->Dispatch(oCmd);
			if (iRet != 0 && iRet != UNFINISH_TASK_RET_FLAG) {
				oCmd.iRet = iRet;
				char szBuf [256] = {0};
				snprintf(szBuf,sizeof(szBuf),"errCode=%d&resp=Err happend!",oCmd.iRet);
				oCmd.sData = szBuf;
			}
			else if (iRet == UNFINISH_TASK_RET_FLAG) { //未做完的任务等待再次被唤醒继续完成
				return 0;
			}

			CMsgQueue* rpMsgq;
			if (oCmd.iSvcSerialNo != 0) { //服务间的互调，结果不是回到netio的队列中
				printf("===put app reponse to msgq for request %d,cmd:%d\n",oCmd.iSvcSerialNo,oCmd.iCmd);
				//oCmd中的icmd在request请求中代表目标cmd，用于netio路由
				//但在App请求中代表请求方的cmd，用于回包路由,放入对应服务的消息队列中
				int iCount = 1; //更改为命令值对应so的编号的下标，这里当然容错没处理
				for (std::map<int, const char*>::const_iterator it=g_mapCmdDLL.begin();it!=g_mapCmdDLL.end();++it) {
					if (oCmd.iCmd == iCount) {
						iRet = m_pMQManager->GetMsgQueue(it->first, rpMsgq);
						printf("APP reponse cmd %d will put request into MSGQ %x\n",oCmd.iCmd,it->first);
						//assert(iRet == 0);
						break;
					}
					++iCount;
				}
				MsgBuf_T stMsg2;
				stMsg2.Reset();
				//iPid * 10 + APP
				int iPid = (oCmd.iSvcSerialNo/100);
				printf("calc msgtype pid:%d\n",iPid);
				stMsg2.lType = iPid * 10 + APP;

				oCmd.iType = RESPONSE;
				oCmd.ToString(stMsg2.sBuf,sizeof(stMsg2.sBuf));
				rpMsgq->PutMsg(&stMsg2,strlen(stMsg2.sBuf));
				printf("finish one app request,from cmd:%d,iSvcSerialNo:%d,msg type:%d\n",oCmd.iCmd,oCmd.iSvcSerialNo,stMsg2.lType);
			}
			else {
				stringstream ss;
				ss<<oCmd.sData<<" by pid:" << getpid();

				oCmd.sData = ss.str();

				m_pMQManager->GetMsgQueue(NET_IO_BACK_MSQ_KEY,rpMsgq);
				MsgBuf_T stMsg2;
				stMsg2.Reset();
				stMsg2.lType = RESPONSE;

				oCmd.iType = RESPONSE; //这里的设置没什么意义，因为netio不认这字段。走到这一步，都是方法调用全部做完了。
				oCmd.ToString(stMsg2.sBuf,sizeof(stMsg2.sBuf));
				rpMsgq->PutMsg(&stMsg2,strlen(stMsg2.sBuf));

				RespNotify();
				printf("finish one normal request,from ip:%s,port:%d\n",oCmd.sClientIp.c_str(),oCmd.iPort);
			}
		//}
		/*
		else {
			printf("can't find:%d dispatcher\n",oCmd.iCmd);
		}*/

		return 0;
}
