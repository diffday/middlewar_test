/*
 * msgq_manager.h
 *
 *  Created on: 2015年12月15日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_MSGQ_MANAGER_H_
#define NETIO_INCLUDE_MSGQ_MANAGER_H_

#include <map>
#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <cstring>
#include "global_define.h"
using namespace std;

typedef enum {
	REQUEST = 1, //msg的type不能接受0，所以用1起始
	RESPONSE = 2,
} MSGTYPE_t;

/**
 * 消息队列中的结构定义
 */
typedef struct tagMsgBuf { //允许的默认值请提前用ipcs -l 查看上限
	long lType;
	char sBuf[MAX_MSG_SIZE - 8];

	void Reset() {
		lType = 0l;
		memset((void*)sBuf,0,sizeof(sBuf));
	}

} MsgBuf_T;

class CMsgQManager;

class CMsgQueue{
	friend class CMsgQManager;
public:
	CMsgQueue();
	~CMsgQueue();

public:
	int Init(key_t iKey);
	int PutMsg(const MsgBuf_T *pBuf, int dwBufLen);
	int GetMsg(MsgBuf_T *pBuf, int& dwBufLen);
	int GetMsgQKey() { return m_iKey;}
	string m_sLastErrMsg;
	//int Fini();
private:
	int Fini();

private:
	int m_iMsgQId;
	key_t m_iKey;
};

class CMsgQManager {
public:
	CMsgQManager();
	~CMsgQManager();

public:
	int GetMsgQueue(int dwGroupId, CMsgQueue*& rpMsgq);
	int delMsgQueue(int dwGroupId);
	int AddMsgQueue(int dwGroupId);

private:
	map<int, CMsgQueue*> m_mapGroupMsgqs;

};

#endif /* NETIO_INCLUDE_MSGQ_MANAGER_H_ */
