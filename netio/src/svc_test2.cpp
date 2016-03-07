/*
 * svc_test.cpp
 *
 *  Created on: 2016年3月1日
 *      Author: chenzhuo
 */

#include "svc_base.h"
#include "intf_service.h"
#include "cmd_obj.h"

class CSvcTest2: public IService {
public:
	CSvcTest2() {
		printf("CSvcTest2::construct\n");
	}

	~CSvcTest2() {
		printf("CSvcTest2::destruct\n");
	}
	int Execute(CCmd& oCmd);
};

class CSvcTest2Factory : public IServiceFactory{
	IService* Create(){
		return (IService*)new CSvcTest2;
	}
};

int CSvcTest2::Execute(CCmd& oCmd) {
	printf("CSvcTest2::Execute\n");
	return 0;
}


IServiceFactory* InitSvrObjFactory(void) {
	return (IServiceFactory*)new CSvcTest2Factory;
}

void process_service(void *cmdObj){}
