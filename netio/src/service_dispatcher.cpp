/*
 * service_dispatcher.cpp
 *
 *  Created on: 2016年3月2日
 *      Author: chenzhuo
 */

#include "service_dispatcher.h"
#include "global_define.h"

CServiceDispatcher::CServiceDispatcher() : m_iCmd(0){}
CServiceDispatcher::~CServiceDispatcher() {
	list<IService*>::iterator it = listStatelessSvcQueue.begin();
	for (;it != listStatelessSvcQueue.end(); ++it) {
		delete (*it);
	}

	it = listStatefulSvcQueue.begin();
	for (; it != listStatefulSvcQueue.end(); ++it) {
		delete (*it);
	}
}

int CServiceDispatcher::Dispatch(CCmd& oCmd) {
	//listStatefulSvcQueue.
	if (listStatelessSvcQueue.size() == 0) {
		return NO_FREE_SVC_HANDLER;
	}
	IService* pSvcHandler = listStatelessSvcQueue.front();
	listStatelessSvcQueue.pop_front();

	return pSvcHandler->Execute(oCmd);
}

int CServiceDispatcher::AddSvcHandler(IService* pSvcHandler) {
	listStatelessSvcQueue.push_back(pSvcHandler);
}
