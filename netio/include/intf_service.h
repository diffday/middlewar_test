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
	void process_service(void *IService); //TODO 可以公共实现然后挪到container的编译结果中，然后so编译不用关心
#ifdef __cplusplus
}
#endif

class IService {
public:
	int m_iCmd;
private:
	pth_uctx_t m_uctx;
	char* m_pUCTXStack;
	int m_iUCTXStackSize;

public:
	virtual int Execute(CCmd& oCmd) = 0; //执行结果，这里也用简化cmd来传递

	virtual ~IService(){};

	IService();

	void Schedule();

	pth_uctx_t& GetUCTX();

};

class IServiceFactory {
public:
	virtual IService* Create()=0;
};

#endif /* NETIO_INCLUDE_INTF_SERVICE_H_ */
