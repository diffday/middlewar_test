#include "service_loader.h"
#include "global_define.h"
#include <vector>
#include <dlfcn.h>
#include <stdio.h>
#include <iostream>
#include "intf_service.h"
CServiceLoader::CServiceLoader(){}
CServiceLoader::~CServiceLoader(){
	CleanServices();
}

typedef IServiceFactory* (*pfnSvcInitFunction_T)(void);
const char* SVC_INIT_FUNC = "InitSvrObjFactory";

int CServiceLoader::LoadServices(){
	map<int,const char*>::const_iterator it = g_mapCmdDLL.begin();
	for (;it!=g_mapCmdDLL.end();++it) {
		printf("start load dll %s\n",it->second);
		LoadSercie_i(it->first,it->second);
	}

	return 0;
}

int CServiceLoader::LoadSercie_i(int iCmdId,const char* pszDLLName) {
	void* pHandle = dlopen(pszDLLName, RTLD_NOW | RTLD_GLOBAL);//RTLD_NOW 当你容器加载的so在最后的话，可以用now，这样还可更快检查出编译的库是否有缺失，不推迟到运行时，若后面还要动态加载其它库，则用RTLD_LAZY更好
	if (NULL == pHandle) {
		printf("load dll %s failed,msg: %s\n",pszDLLName,dlerror());
		return OPEN_DLL_FAILED;
	}
	else {
		printf("load dll %s succ!\n",pszDLLName);
	}

	pfnSvcInitFunction_T pfnSvcInitFunction = (pfnSvcInitFunction_T)dlsym(pHandle, SVC_INIT_FUNC); //函数指针
	if (dlerror() != NULL) {
		printf("dlsym (%s) Error,dll %s msg: %s\n",SVC_INIT_FUNC,pszDLLName,dlerror());
		return OPEN_DLL_FAILED;
	}
	IServiceFactory* pSvcFactory = (*pfnSvcInitFunction)();
	//pSvcFactory->Create();

	m_mapHandlers[iCmdId] = pHandle;
	m_mapServiceFactory[iCmdId] = pSvcFactory;
}

int CServiceLoader::CleanServices() {
	map<int,void*>::iterator it = m_mapHandlers.begin();
	for (;it!=m_mapHandlers.end();++it) {
		printf("clean dll %x\n",it->first);
		dlclose(it->second);
	}
	m_mapHandlers.clear();
}

int CServiceLoader::GetServiceFactory(int iCmdId, IServiceFactory*& pIServiceFactory) {
	map<int,IServiceFactory*>::iterator it = m_mapServiceFactory.find(iCmdId);
	if (it != m_mapServiceFactory.end()) {
		pIServiceFactory = it->second;
		return 0;
	}
	return -1;
}
