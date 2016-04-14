/*
 * svc_base.h
 *
 *  Created on: 2016年2月25日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_SVC_BASE_H_
#define NETIO_INCLUDE_SVC_BASE_H_
#include "intf_service.h"
#ifdef __cplusplus
//创造版本冲突，无法自动合并

extern "C"
{
#endif
	IServiceFactory* InitSvrObjFactory(void);
#ifdef __cplusplus
}
#endif



#endif /* NETIO_INCLUDE_SVC_BASE_H_ */
