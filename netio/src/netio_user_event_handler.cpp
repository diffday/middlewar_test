/*
 * netio_user_event_handler.cpp
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */
#include "netio_user_event_handler.h"
#include "assert.h"
#include <stdlib.h>
#include "cmd_obj.h"
#include "reactor.h"
//#include <sys/socket.h>
#include <arpa/inet.h>

int CNetIOUserEventHandler::CheckEvent(void* pvParam) {
	return 0;
}

int CNetIOUserEventHandler::OnEventFire(void* pvParam) {
	CMsgQueue* rpMsgq;
	int iRet = m_pMQManager->GetMsgQueue(NET_IO_BACK_MSQ_KEY, rpMsgq);
	assert(iRet == 0);

	MsgBuf_T stMsg;
	stMsg.Reset();
	stMsg.lType=RESPONSE;
	int Len = 0;
	iRet = rpMsgq->GetMsg(&stMsg,Len);
	if (iRet == 0 && Len == 0) {
		//printf("no msg to send now\n");
		return 0;
	}

	printf("---RESPONSE--- svc Msg lenth:%d,data is %s\n",Len,stMsg.sBuf);
	CCmd oCmd;
	printf("the cmd to string is:%s\n",oCmd.ToString().c_str());
	oCmd.InitCCmd(stMsg.sBuf);
	snprintf(stMsg.sBuf,sizeof(stMsg.sBuf),"%s",oCmd.sData.c_str());
	/*
	map<string,string> mapPara;
	strPairAppendToMap(stMsg.sBuf,mapPara);
	int ifd = static_cast<int>(atoll(mapPara.find("fd")->second.c_str()));
	snprintf(stMsg.sBuf,sizeof(stMsg.sBuf),"%s",mapPara.find("resp")->second.c_str());*/
	m_pReactor->m_arrMsg[oCmd.iFd] = stMsg;

	stTcpSockItem stSock;
	stSock.fd = oCmd.iFd;
	stSock.enEventFlag = TCP_SERVER_SEND;
	stSock.stSockAddr_in.sin_port = oCmd.iPort;
	stSock.stSockAddr_in.sin_addr.s_addr = inet_addr(oCmd.sClientIp.c_str());
	stSock.stSockAddr_in.sin_family = oCmd.ifamily;
	/*
	stSock.fd = ifd;
	stSock.enEventFlag = TCP_SERVER_SEND;
	string clientIp = mapPara.find("cliIp")->second;
	int family = atoi(mapPara.find("family")->second.c_str());
	int port = atoi(mapPara.find("cliPort")->second.c_str());
	stSock.stSockAddr_in.sin_port = port;
	stSock.stSockAddr_in.sin_addr.s_addr = inet_addr(clientIp.c_str());
	stSock.stSockAddr_in.sin_family = family;*/

	m_pReactor->AddToWatchList(oCmd.iFd,TCP_SERVER_SEND,&stSock);
	//snprintf(stMsg.sBuf,strlen(stMsg.sBuf)+1,"%s",buf);

	return 0;
}

int CNetIOUserEventHandler::RegisterMqManager(CMsgQManager* pMQManager) {
	m_pMQManager = pMQManager;
}
