/*
 * svc_test.cpp
 *
 *  Created on: 2016年3月1日
 *      Author: chenzhuo
 */
#include "intf_service.h"
#include "svc_base.h"
#include "cmd_obj.h"
#include "cache_manager.h"

class CSvcTest: public IService {
public:
	~CSvcTest() {
		//printf("CSvcTest::destruct\n");
	}

	CSvcTest() {
		m_iCmd = 1;
		//printf("CSvcTest::construct\n");
	}
	int Execute(CCmd& oCmd);
};

class CSvcTestFactory : public IServiceFactory{
	IService* Create(){
		return (IService*)new CSvcTest;
	}
};

int CSvcTest::Execute(CCmd& oCmd) {
	printf("CSvcTest %d::Execute serno:%d data:%s\n",m_iIndex,oCmd.iSvcSerialNo,oCmd.sData.c_str());
	CCacheManager* pCm =  CCacheManager::GetInstance();
		int processStatIndex = (m_iIndex / 100)%100; //取进程号的后两尾数
		stringstream ss;
		ss<<"c1_"<<processStatIndex;
		pCm->Incr(ss.str(),1);

	oCmd.sData = "resp1=r1";
	oCmd.iType = RESPONSE;
	return 0;
}

IServiceFactory* InitSvrObjFactory(void) {
	return (IServiceFactory*)new CSvcTestFactory;
}
