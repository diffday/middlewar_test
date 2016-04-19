/*
 * service_dispatcher.cpp
 *
 *  Created on: 2016年3月2日
 *      Author: chenzhuo
 */

#include "service_dispatcher.h"
#include "global_define.h"
#include "assert.h"
#include <unistd.h>
CServiceDispatcher* CServiceDispatcher::pServiceDispatcher = NULL;

int CServiceDispatcher::Dispatch(CCmd& oCmd) {
	m_oCurCmdObj = oCmd;
	//printf("++org serno :%d\n",oCmd.iSvcSerialNo);
	//printf("--new serno :%d\n",m_oCurCmdObj.iSvcSerialNo);

	int iRet = 0;
	int iStateFulContinue = 0;
	IService* pSvcHandler = NULL;
	if (m_oCurCmdObj.iType == RESPONSE) {
	//if (oCmd.iSvcSerialNo) {
		map<int,IService*>::iterator it = m_mapStatefulSvcQueue.begin();
		//printf("stateful-size-%d\n",m_mapStatefulSvcQueue.size());
		for (;it != m_mapStatefulSvcQueue.end(); ++it) {
			printf("stateful-find-%d\n",it->first);
			if (m_oCurCmdObj.iSvcSerialNo == it->first) {
				iStateFulContinue = 1;
				pSvcHandler = it->second;
				printf("++stateful service find:%d\n",pSvcHandler->m_iIndex);
				assert(pth_uctx_switch(m_uctx, pSvcHandler->GetUCTX()));
				//iRet = pSvcHandler->Execute(oCmd);
				//m_mapStatefulSvcQueue.erase(it);
				//m_mapStatelessSvcQueue[oCmd.iSvcSerialNo] = pSvcHandler;
				//return iRet;
			}
		}
		if (!iStateFulContinue) {
			printf("can't find stateful %d in this process %d, stateful queue size:%d\n",m_oCurCmdObj.iSvcSerialNo,getpid(),m_mapStatefulSvcQueue.size());
		}
		assert(iStateFulContinue);
	}
	else {
		if (m_mapStatelessSvcQueue.size() == 0 && iStateFulContinue == 0) {
				return NO_FREE_SVC_HANDLER;
		}
		else {
			map<int,IService*>::iterator it = m_mapStatelessSvcQueue.begin();
			pSvcHandler = it->second;
				//m_oCurCmdObj.iSvcSerialNo = it->first;
			m_mapStatelessSvcQueue.erase(it);
			printf("--stateless service find:%d\n",pSvcHandler->m_iIndex);
				//printf("--stateless service find:%d\n",pSvcHandler->m_iIndex);
			m_mapStatefulSvcQueue[pSvcHandler->m_iIndex] = pSvcHandler;

			assert(pth_uctx_switch(m_uctx, pSvcHandler->GetUCTX()));
		}
	}

	printf("****back to main process******,data:%s\n",m_oCurCmdObj.ToString().c_str());
	//int pth_uctx_switch(pth_uctx_t uctx_from, pth_uctx_t uctx_to);

	//iRet = pSvcHandler->Execute(oCmd);
	if (m_oCurCmdObj.iRet != UNFINISH_TASK_RET_FLAG) {
		if (pSvcHandler) {
			map<int,IService*>::iterator it = m_mapStatefulSvcQueue.find(pSvcHandler->m_iIndex);
			if (it != m_mapStatefulSvcQueue.end()) {
				m_mapStatefulSvcQueue.erase(it);
			}
			pSvcHandler->ResetUCTX();
			printf("--statful back stateless service :%d\n",pSvcHandler->m_iIndex);
			m_mapStatelessSvcQueue[pSvcHandler->m_iIndex] = pSvcHandler;
		}

	}

	oCmd = m_oCurCmdObj;
	return oCmd.iRet;
	//return iRet;
}

int CServiceDispatcher::AddSvcHandler(IService* pSvcHandler) {
	m_mapStatelessSvcQueue[pSvcHandler->m_iIndex] = pSvcHandler;
	m_count++;
}

CServiceDispatcher* CServiceDispatcher::Instance() {
	if (pServiceDispatcher == NULL) {
		pServiceDispatcher = new CServiceDispatcher();
	}
	return pServiceDispatcher;
}
