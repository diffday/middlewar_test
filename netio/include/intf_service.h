/*
 * inf_service.h
 *
 *  Created on: 2016年2月25日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_INTF_SERVICE_H_
#define NETIO_INCLUDE_INTF_SERVICE_H_
#include "cmd_obj.h"
#include "pth.h"

class IService {
public:
	virtual int Execute(CCmd& oCmd) = 0; //执行结果，这里也用简化cmd来传递

	virtual ~IService(){};
	int m_iCmd;
	pth_uctx_t m_uctx;
	int Schedule() {
		pth_uctx_create((pth_uctx_t *)&m_uctx);
	}
};

class IServiceFactory {
public:
	virtual IService* Create()=0;
};

#endif /* NETIO_INCLUDE_INTF_SERVICE_H_ */
