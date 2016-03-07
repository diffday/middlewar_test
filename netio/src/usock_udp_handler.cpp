/*
 * usock_udp_handler.cpp
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */

#include "usock_udp_handler.h"
#include "reactor.h"
#include <arpa/inet.h>

//=====CUSockUdpHandler start
CUSockUdpHandler::CUSockUdpHandler() {}
CUSockUdpHandler::~CUSockUdpHandler() {}
/*
CUSockUdpHandler::CUSockUdpHandler(CReactor* pReactor) {
	m_pReactor = pReactor;
};*/

int CUSockUdpHandler::HandleEvent(int iConn, int iType) {
	switch (iType) {
	case UDP_READ:
		DoRecv(iConn);
		break;
	case UDP_SEND:
		DoSend(iConn);
		break;
	}
	return 0;
}

int CUSockUdpHandler::DoConn(int iConn) {
	return 0;
}

int CUSockUdpHandler::DoRecv(int iConn) {
	char buf[1000] = {0};
	struct sockaddr_in m_l_stFromAddr;
	socklen_t m_l_iFromAddrLen = sizeof(sockaddr_in);
	int m_iRecvBufLen = recvfrom(
			iConn,
			buf,
	        sizeof(buf),
	        0,
	        (struct sockaddr*)&m_l_stFromAddr,
	        &m_l_iFromAddrLen);

	    if(m_iRecvBufLen <= 0)
	        printf("err when recv from\n");

	   //long l_uiHost = ntohl(m_l_stFromAddr.sin_addr.s_addr);

	   long l_ushPort = ntohs(m_l_stFromAddr.sin_port);
	   printf("UDP-info %s get from recv from %s:%d\n",buf,inet_ntoa(m_l_stFromAddr.sin_addr),l_ushPort,l_ushPort);
	return 0;
}

int CUSockUdpHandler::DoSend(int iConn) {
	return 0;
}

int CUSockUdpHandler::DoClose(int iConn) {
	return 0;
}

//=====CUSockUdpHandler end


