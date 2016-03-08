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
#include "global_define.h"

#ifdef __cplusplus
extern "C"
{
#endif
	void process_service(void *cmdObj);
#ifdef __cplusplus
}
#endif

class IService {
private:
	int m_iCmd;
	pth_uctx_t m_uctx;
	char* m_pUCTXStack;
	int m_iUCTXStackSize;

public:
	virtual int Execute(CCmd& oCmd) = 0; //执行结果，这里也用简化cmd来传递

	virtual ~IService(){};

	IService();

	int Schedule(CCmd& oCmd);

};

class IServiceFactory {
public:
	virtual IService* Create()=0;
};

#endif /* NETIO_INCLUDE_INTF_SERVICE_H_ */
