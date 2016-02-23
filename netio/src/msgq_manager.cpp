/*
 * msgq_manager.cpp
 *
 *  Created on: 2015年12月15日
 *      Author: chenzhuo
 */
#include "msgq_manager.h"
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


CMsgQueue::CMsgQueue() : m_iMsgQId(0),m_iKey(0) {}

CMsgQueue::~CMsgQueue() {

}

int CMsgQueue::GetMsg(MsgBuf_T *pBuf, int& dwBufLen) {
	int iRet = 0;
	/*
	 *struct msgbuf {
		long mtype; // type of message
		char mtext[1]; //message text,利用指针的转型来强制解释type后存放的任意数据类型
	};
	 */
	iRet = msgrcv(m_iMsgQId, (struct msgbuf*) pBuf,	MAX_MSG_SIZE, pBuf->lType, IPC_NOWAIT);
	if (iRet < 0) {
		m_sLastErrMsg = strerror(errno);
		return RECV_MSG_FAILED;
	}

	dwBufLen = iRet;

	return 0;
}


int CMsgQueue::Init(key_t iKey) {
	int iFlag = 0666;
	int iRet = 0;

	int iMsgId= msgget(iKey, iFlag|IPC_CREAT);
	if(iMsgId == -1) {
		m_sLastErrMsg = strerror(errno);
		return CREATE_MSGQ_FAILED;
	}

	struct msqid_ds buf;
	iRet = msgctl(iMsgId, IPC_STAT, (struct msqid_ds *)&buf);

	buf.msg_qbytes = MAX_QBYTES;
	iRet = msgctl(iMsgId, IPC_SET, (struct msqid_ds *)&buf);
	if (iRet == -1) {
		m_sLastErrMsg = strerror(errno);
		return CNTL_MSGQ_BYTES_FAILED;
	}

	m_iMsgQId = iMsgId;
	m_iKey = iKey;

	return 0;
}

/**
 * 因为无法获知一个消息队列中到底还有多少进程挂载在上面，其删除操作，只适合确定性了解回收时机时启用。作为中间件，更适合将消息队列删除操作留给shell脚本或管理员命令行
 */
int CMsgQueue::Fini() {
	int iRet=msgctl(m_iMsgQId,IPC_RMID,NULL);//删除消息队列
	if(iRet < 0)
	{
	    printf("unlink msg queue error\n");
	    return DELETE_MSGQ_FAILED;
	}

	return 0;
}

int CMsgQueue::PutMsg(const MsgBuf_T *pBuf, int dwBufLen) {
	if (dwBufLen > MAX_MSG_SIZE) {
		return MSG_TOO_LONG;
	}
	int iRet = 0;
	iRet = msgsnd(m_iMsgQId, (struct msgbuf*)pBuf, dwBufLen,IPC_NOWAIT);

	if (iRet == -1) {
		m_sLastErrMsg = strerror(errno);
		return SEND_MSG_FAILED;
	}

	return 0;
}

int CMsgQManager::AddMsgQueue(int dwGroupId) {
	int iRet = 0;
	map<int, CMsgQueue*>::const_iterator it = m_mapGroupMsgqs.find(dwGroupId);
	if (it != m_mapGroupMsgqs.end()) {
		printf("this msgq has been created\n");
	}
	else {
		CMsgQueue* pMsgQueue = new  CMsgQueue();
		iRet = pMsgQueue->Init(dwGroupId);
		if (iRet != 0) {
			printf("create msgq %x error,msg:%s\n",dwGroupId,pMsgQueue->m_sLastErrMsg.c_str());
		}
		m_mapGroupMsgqs[dwGroupId] = pMsgQueue;
		printf("Init msgq %x Succ!\n",dwGroupId);
	}
	return 0;
}

int CMsgQManager::delMsgQueue(int dwGroupId) {
	map<int, CMsgQueue*>::iterator it = m_mapGroupMsgqs.begin();
	for (;it!=m_mapGroupMsgqs.end();++it) {
		if (it->first == dwGroupId ) {
			it->second->Fini();
		}
	}
	return 0;
}

int CMsgQManager::GetMsgQueue(int dwGroupId, CMsgQueue*& rpMsgq) {
	int iRet = MSGQ_NO_EXIST;
	map<int, CMsgQueue*>::iterator it = m_mapGroupMsgqs.begin();
	for (; it != m_mapGroupMsgqs.end(); ++it) {
		if (it->first == dwGroupId) {
			iRet = 0;
			rpMsgq = it->second;
			return iRet;
		}
	}
	return iRet;
}

CMsgQManager::CMsgQManager() {}
CMsgQManager::~CMsgQManager(){
	map<int, CMsgQueue*>::iterator it = m_mapGroupMsgqs.begin();
	for (;it!=m_mapGroupMsgqs.end();++it) {
		//it->second->Fini();
	}
	m_mapGroupMsgqs.clear();
}
