/*
 * svc_test.cpp
 *
 *  Created on: 2016年3月1日
 *      Author: chenzhuo
 */

#include "svc_base.h"
#include "intf_service.h"
#include "cmd_obj.h"
#include "msgq_manager.h"

CMsgQManager* GetMQManager();

class CSvcTest2: public IService {
public:
	CSvcTest2() {
		m_iCmd = 2;
		printf("CSvcTest2::construct\n");
	}

	~CSvcTest2() {
		printf("CSvcTest2::destruct\n");
	}
	int Execute(CCmd& oCmd);
	int SendRequest(CCmd& oCmd);
	int GetResponse(CCmd& oCmd);
};

class CSvcTest2Factory : public IServiceFactory{
	IService* Create(){
		return (IService*)new CSvcTest2;
	}
};


int CSvcTest2::SendRequest(CCmd& oCmd) {
	printf("CSvcTest2::SendRequest\n");

	/*CMsgQManager* pMsgQManager = GetMQManager();
	CMsgQueue* rpMsgq;
	pMsgQManager->GetMsgQueue(0xccccd,rpMsgq);

	MsgBuf_T stMsg;
	stMsg.Reset();
	stMsg.lType=REQUEST;

	oCmd.ToString(stMsg.sBuf,sizeof(stMsg.sBuf));
	rpMsgq->PutMsg(&stMsg,strlen(stMsg.sBuf));
	//Schedule();
	//oCmd.sData = "resp1=r1_for_r2";*/
	return 0;
}

int CSvcTest2::GetResponse(CCmd& oCmd) {
	printf("CSvcTest2::GetResponse\n");


	/*CMsgQManager* pMsgQManager = GetMQManager();
	CMsgQueue* rpMsgq;
	pMsgQManager->GetMsgQueue(0xccccd,rpMsgq);

	MsgBuf_T stMsg;
	stMsg.Reset();
	stMsg.lType=RESPONSE;

	oCmd.ToString(stMsg.sBuf,sizeof(stMsg.sBuf));
	//rpMsgq->PutMsg(&stMsg,strlen(stMsg.sBuf));

	//Schedule();
	//oCmd.sData = "resp1=r1_for_r2";*/
	return 0;
}

int CSvcTest2::Execute(CCmd& oCmd) {
	printf("CSvcTest2::Execute\n");


	//oCmd.iRet = UNFINISH_TASK_RET_FLAG;
	//CCmd oCmd2;
	//SendRequest(oCmd2);
	//Schedule();
	//GetResponse(oCmd2);
	//oCmd.iRet
	//oCmd.iRet = 0;
	oCmd.sData = "resp2=r2";
	return 0;
}


IServiceFactory* InitSvrObjFactory(void) {
	return (IServiceFactory*)new CSvcTest2Factory;
}
