/*
 * intf_user_event_handler.h
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_INTF_USER_EVENT_HANDLER_H_
#define NETIO_INCLUDE_INTF_USER_EVENT_HANDLER_H_

class CReactor;

class CUserEventHandler {
public:
	virtual ~CUserEventHandler() {};
	virtual int OnEventFire(void* pvParam = 0) = 0;
	virtual int CheckEvent(void* pvParam = 0) = 0;
	CReactor* m_pReactor;
};



#endif /* NETIO_INCLUDE_INTF_USER_EVENT_HANDLER_H_ */
