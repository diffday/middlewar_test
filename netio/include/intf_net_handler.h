/*
 * intf_net_handler.h
 *
 *  Created on: 2016年3月7日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_INTF_NET_HANDLER_H_
#define NETIO_INCLUDE_INTF_NET_HANDLER_H_
class CNetHandler {
public:
	virtual ~CNetHandler() {}//定义虚析构是一个好习惯，好让父类有虚函数表入口。否则向上转型利用时，将不能享受多态的好处
public:
	virtual int HandleEvent(int iConn, int iType) = 0;
};

#endif /* NETIO_INCLUDE_INTF_NET_HANDLER_H_ */
