/*
 * tcp_net_handler.h
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_TCP_NET_HANDLER_H_
#define NETIO_INCLUDE_TCP_NET_HANDLER_H_

#include "intf_net_handler.h"
#include "msgq_manager.h"

class CReactor;

class CTcpNetHandler : public CNetHandler {
public:
	CTcpNetHandler();
	~CTcpNetHandler();
public:
	int HandleEvent(int iConn, int iType);
	int RegisterMqManager(CMsgQManager* m_pMQManager);
private:
	int DoConn(int iConn);
	int DoRecv(int iConn);//mqmanager应该是reactor回调的成员，因为框架不关心事件处理细节
	int DoSend(int iConn);
	int DoClose(int iConn);

public:
	CReactor* m_pReactor;
	CMsgQManager* m_pMQManager;
};



#endif /* NETIO_INCLUDE_TCP_NET_HANDLER_H_ */
