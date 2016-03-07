/*
 * usock_udp_handler.h
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_USOCK_UDP_HANDLER_H_
#define NETIO_INCLUDE_USOCK_UDP_HANDLER_H_
#include "intf_net_handler.h"
class CReactor;

class CUSockUdpHandler : public CNetHandler {
public:
	CUSockUdpHandler();
	~CUSockUdpHandler();
public:
	int HandleEvent(int iConn, int iType);
private:
	int DoConn(int iConn);
	int DoRecv(int iConn);
	int DoSend(int iConn);
	int DoClose(int iConn);

public:
	CReactor* m_pReactor;
};



#endif /* NETIO_INCLUDE_USOCK_UDP_HANDLER_H_ */
