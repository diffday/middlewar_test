/*
 * inf_service.h
 *
 *  Created on: 2016年2月25日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_INTF_SERVICE_H_
#define NETIO_INCLUDE_INTF_SERVICE_H_
#include "cmd_obj.h"

class IService {
public:
	virtual int Execute(CCmd oCmd) = 0;
	virtual ~IService(){};
};

class IServiceFactory {
public:
	virtual IService* Create()=0;
};

#endif /* NETIO_INCLUDE_INTF_SERVICE_H_ */
