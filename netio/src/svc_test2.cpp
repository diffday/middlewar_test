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
#include "service_dispatcher.h"
#include <unistd.h>
//#include <stdlib.h>

#include "cache_manager.h"

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

	CMsgQManager* pMsgQManager = CMsgQManager::GetInstance();
	CMsgQueue* rpMsgq;
	pMsgQManager->GetMsgQueue(0xccccd,rpMsgq);

	MsgBuf_T stMsg;
	stMsg.Reset();
	stMsg.lType=REQUEST;
	oCmd.iSvcSerialNo = m_iIndex;
	oCmd.iCmd = m_iCmd;

	oCmd.ToString(stMsg.sBuf,sizeof(stMsg.sBuf));
	rpMsgq->PutMsg(&stMsg,strlen(stMsg.sBuf));
	//Schedule();
	//oCmd.sData = "resp1=r1_for_r2";
	return 0;
}

int CSvcTest2::GetResponse(CCmd& oCmd) {

	oCmd = CServiceDispatcher::Instance()->GetCmdObj();
	printf("CSvcTest2::GetResponse,data:%s\n",oCmd.ToString().c_str());
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
	printf("CSvcTest2 %d::Execute serno:%d data:%s\n",m_iIndex,oCmd.iSvcSerialNo,oCmd.ToString().c_str());

	CCacheManager* pCm =  CCacheManager::GetInstance();
	int processStatIndex = (m_iIndex / 100)%100; //取进程号的后两尾数
	stringstream ss;
	ss<<"c2_"<<processStatIndex;
	pCm->Incr(ss.str(),1);

	//从主进程切回来，因为期间主进程收到了新的数据包，ocmd指向的已经是新数据包，而非堆栈切出去之前的数据。这也符合语言的定义，毕竟同一
	//进程中引用的存储空间内容变化了。用户态堆栈也不能打破这个逻辑或自动备份，因为那样违反了基本的编程模型和语义。

	//合理的设计方案是每一个service持有一个自己的缓冲区，用于保留请求和response对象。这样就不依赖于过深的引用传递
	//返回时，主框架把service的reponse序列化直接返回即可，然后清空service缓冲区。拷贝的语义是最简单的

	//当然针对我这个demo方案，不想有obj3这个拷贝，那就在数据包上启用index的含义，避免ip,port，family承担的意义。由index交由netio来找句柄
	CCmd obj3 = oCmd;

	oCmd.iRet = UNFINISH_TASK_RET_FLAG;
	CCmd oCmd2;
	SendRequest(oCmd2);
	Schedule();
	printf("CSvcTest2 %d::after back serno:%d data:%s\n",m_iIndex,oCmd.iSvcSerialNo,oCmd.ToString().c_str());
	GetResponse(oCmd2);
	//oCmd.iRet
	obj3.iRet = 0;
	obj3.iSvcSerialNo = 0;
	obj3.iType = RESPONSE;

	obj3.sData = oCmd2.sData+"&resp2=r2";
	oCmd = obj3;
	return 0;
}


IServiceFactory* InitSvrObjFactory(void) {
	return (IServiceFactory*)new CSvcTest2Factory;
}
