/*
 * service_dispatcher.cpp
 *
 *  Created on: 2016年3月2日
 *      Author: chenzhuo
 */

#include "service_dispatcher.h"
#include "global_define.h"

int CServiceDispatcher::Dispatch(CCmd& oCmd) {
	int iRet = 0;
	if (oCmd.iSvcSerialNo) {
		map<int,IService*>::iterator it = m_mapStatefulSvcQueue.begin();
		for (;it != m_mapStatefulSvcQueue.begin(); ++it) {
			if (oCmd.iSvcSerialNo == it->first) {
				IService* pSvcHandler = it->second;
				iRet = pSvcHandler->Execute(oCmd);
				m_mapStatefulSvcQueue.erase(it);
				m_mapStatelessSvcQueue[oCmd.iSvcSerialNo] = pSvcHandler;
				return iRet;
			}
		}
	}

	if (m_mapStatelessSvcQueue.size() == 0) {
		return NO_FREE_SVC_HANDLER;
	}


	map<int,IService*>::iterator it = m_mapStatelessSvcQueue.begin();
	IService* pSvcHandler = it->second;
	oCmd.iSvcSerialNo = it->first;
	m_mapStatelessSvcQueue.erase(it);

	m_mapStatefulSvcQueue[oCmd.iSvcSerialNo] = pSvcHandler;
	iRet = pSvcHandler->Execute(oCmd);
	m_mapStatelessSvcQueue[oCmd.iSvcSerialNo] = pSvcHandler;
	return iRet;
}

int CServiceDispatcher::AddSvcHandler(IService* pSvcHandler) {
	m_mapStatelessSvcQueue[m_count++] = pSvcHandler;
}
