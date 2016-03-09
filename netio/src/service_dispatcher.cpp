/*
 * service_dispatcher.cpp
 *
 *  Created on: 2016年3月2日
 *      Author: chenzhuo
 */

#include "service_dispatcher.h"
#include "global_define.h"
#include "assert.h"
CServiceDispatcher* CServiceDispatcher::pServiceDispatcher = NULL;

int CServiceDispatcher::Dispatch(CCmd& oCmd) {
	m_oCurCmdObj = oCmd;

	int iRet = 0;
	int iStateFulContinue = 0;
	IService* pSvcHandler = NULL;
	if (oCmd.iSvcSerialNo) {
		map<int,IService*>::iterator it = m_mapStatefulSvcQueue.begin();
		for (;it != m_mapStatefulSvcQueue.begin(); ++it) {
			if (oCmd.iSvcSerialNo == it->first) {
				iStateFulContinue = 1;
				pSvcHandler = it->second;
				printf("++stateful service find:%d\n",it->first);
				assert(pth_uctx_switch(m_uctx, pSvcHandler->GetUCTX()));
				//iRet = pSvcHandler->Execute(oCmd);
				//m_mapStatefulSvcQueue.erase(it);
				//m_mapStatelessSvcQueue[oCmd.iSvcSerialNo] = pSvcHandler;
				//return iRet;
			}
		}
	}

	if (m_mapStatelessSvcQueue.size() == 0 && iStateFulContinue == 0) {
		return NO_FREE_SVC_HANDLER;
	}
	else {
		map<int,IService*>::iterator it = m_mapStatelessSvcQueue.begin();
		pSvcHandler = it->second;
		oCmd.iSvcSerialNo = it->first;
		m_mapStatelessSvcQueue.erase(it);
		printf("--stateless service find:%d\n",it->first);
		m_mapStatefulSvcQueue[oCmd.iSvcSerialNo] = pSvcHandler;
		assert(pth_uctx_switch(m_uctx, pSvcHandler->GetUCTX()));
	}


	//int pth_uctx_switch(pth_uctx_t uctx_from, pth_uctx_t uctx_to);

	//iRet = pSvcHandler->Execute(oCmd);
	if (oCmd.iRet != UNFINISH_TASK_RET_FLAG) {
		map<int,IService*>::iterator it = m_mapStatefulSvcQueue.find(oCmd.iSvcSerialNo);
		m_mapStatefulSvcQueue.erase(it);

		m_mapStatelessSvcQueue[oCmd.iSvcSerialNo] = pSvcHandler;
	}

	return oCmd.iRet;
	//return iRet;
}

int CServiceDispatcher::AddSvcHandler(IService* pSvcHandler) {
	m_mapStatelessSvcQueue[m_count++] = pSvcHandler;
}
