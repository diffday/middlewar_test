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
#include "global_define.h"
using namespace std;
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
	//int Fini();
private:
	int Fini();

private:
	int m_iMsgQId;
	key_t m_iKey;
	string m_sLastErrMsg;
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
