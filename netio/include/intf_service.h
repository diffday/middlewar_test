/*
 * inf_service.h
 *
 *  Created on: 2016年2月25日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_INTF_SERVICE_H_
#define NETIO_INCLUDE_INTF_SERVICE_H_

class IService {
	virtual int Execute();
	virtual ~IService();
};



#endif /* NETIO_INCLUDE_INTF_SERVICE_H_ */
