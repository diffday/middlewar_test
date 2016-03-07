/*
 * netio_user_event_handler.h
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_NETIO_USER_EVENT_HANDLER_H_
#define NETIO_INCLUDE_NETIO_USER_EVENT_HANDLER_H_

#include "intf_user_event_handler.h"
#include "msgq_manager.h"

class CNetIOUserEventHandler : public CUserEventHandler {
public:
	int OnEventFire(void* pvParam= 0);
	int CheckEvent(void* pvParam= 0);
	CMsgQManager* m_pMQManager;

	int RegisterMqManager(CMsgQManager* m_pMQManager);
	~CNetIOUserEventHandler(){};
};

#endif /* NETIO_INCLUDE_NETIO_USER_EVENT_HANDLER_H_ */
