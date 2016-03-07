/*
 * service_dispatcher.h
 *
 *  Created on: 2016年3月1日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_SERVICE_DISPATCHER_H_
#define NETIO_INCLUDE_SERVICE_DISPATCHER_H_
#include <list>
#include "intf_service.h"
#include "cmd_obj.h"
#include "pth.h"
using namespace std;
class CServiceDispatcher {
public:
	map<int,IService*> m_mapStatefulSvcQueue;
	map<int,IService*> m_mapStatelessSvcQueue;
	int m_iCmd;

	//int

	//队列认为是container关心的，分配器不拥有队列相关信息，交由container回写response到netio的队列
	int Dispatch(CCmd& oCmd);//调用IService对象的Execute函数

	int AddSvcHandler(IService*);
	CServiceDispatcher() :m_count(0),m_iCmd(0) {};
	~CServiceDispatcher();
private:
	int m_count;
};

#endif /* NETIO_INCLUDE_SERVICE_DISPATCHER_H_ */
