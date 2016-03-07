/*
 * container_event_handler.h
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_CONTAINER_USER_EVENT_HANDLER_H_
#define NETIO_INCLUDE_CONTAINER_USER_EVENT_HANDLER_H_

#include "intf_user_event_handler.h"
#include "msgq_manager.h"
#include "service_dispatcher.h"
class CContainerEventHandler : public CUserEventHandler {
public:
	int OnEventFire(void* pvParam= 0);
	int CheckEvent(void* pvParam= 0);
	CMsgQManager* m_pMQManager;
	key_t m_iMqKey;
	map<int,CServiceDispatcher*> m_mapPSvcDispatcher;
	int RegisterMqInfo(CMsgQManager* pMQManager, key_t iMqKey);
	int RegisterSvcDispatcher(int iCmd,CServiceDispatcher* pSvcDispatcher);
	int RespNotify();
	~CContainerEventHandler(){};
};




#endif /* NETIO_INCLUDE_CONTAINER_USER_EVENT_HANDLER_H_ */
