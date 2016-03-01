/*
 * service_dispatcher.h
 *
 *  Created on: 2016年3月1日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_SERVICE_DISPATCHER_H_
#define NETIO_INCLUDE_SERVICE_DISPATCHER_H_
#include <vector>
#include "intf_service.h"
#include "cmd_obj.h"
using namespace std;
class CServiceDispatcher {
private:
	vector<IService*> vecStatefulSvcQueue;
	vector<IService*> vecStatelessSvcQueue;
	int m_iCmd;

	//处理结果是直接写入队列的，所以可以设计为没有通用传出参数
	int Dispatch(CCmd oCmd);//调用IService对象的Execute函数
};

#endif /* NETIO_INCLUDE_SERVICE_DISPATCHER_H_ */
