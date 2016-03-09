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
class CServiceDispatcher { //做成单例类

private:
	int m_count;
	pth_uctx_t m_uctx;
	//int m_iCmd;
	static CServiceDispatcher* pServiceDispatcher;
	CCmd m_oCurCmdObj;


	map<int,IService*> m_mapStatefulSvcQueue;
	map<int,IService*> m_mapStatelessSvcQueue;

public:
	//队列认为是container关心的，分配器不拥有队列相关信息，交由container回写response到netio的队列
	int Dispatch(CCmd& oCmd);//调用IService对象的Execute函数

	int AddSvcHandler(IService*);
	~CServiceDispatcher() {
			map<int, IService*>::iterator it = m_mapStatelessSvcQueue.begin();
			for (; it != m_mapStatelessSvcQueue.end();) {
				delete (it->second);
				m_mapStatelessSvcQueue.erase(it++);
			}

			it = m_mapStatefulSvcQueue.begin();
			for (; it != m_mapStatefulSvcQueue.end();) {
				delete (it->second);
				m_mapStatefulSvcQueue.erase(it++);
			}
			pth_uctx_destroy(m_uctx);
	}

	pth_uctx_t GetUCTX(){
		return m_uctx;
	}

	CCmd& GetCmdObj() {
		return m_oCurCmdObj;
	}

	static CServiceDispatcher* Instance() {
		if (pServiceDispatcher == NULL) {
			pServiceDispatcher = new CServiceDispatcher();
		}
		return pServiceDispatcher;
	}

private:
	CServiceDispatcher() :m_count(0) {
		pth_uctx_create((pth_uctx_t *)&m_uctx);
	};
};

#endif /* NETIO_INCLUDE_SERVICE_DISPATCHER_H_ */
