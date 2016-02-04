#include "service_loader.h"
#include "global_define.h"
#include <vector>
#include <dlfcn.h>
#include <stdio.h>
#include <iostream>
CServiceLoader::CServiceLoader(){}
CServiceLoader::~CServiceLoader(){
	CleanServices();
}


int CServiceLoader::LoadServices(){
	map<int,const char*>::const_iterator it = g_mapCmdDLL.begin();
	for (;it!=g_mapCmdDLL.end();++it) {
		printf("load dll %s\n",it->second);
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

	m_mapHandlers[iCmdId] = pHandle;
}

int CServiceLoader::CleanServices() {
	map<int,void*>::iterator it = m_mapHandlers.begin();
	for (;it!=m_mapHandlers.end();++it) {
		printf("clean dll %x\n",it->first);
		dlclose(it->second);
	}
	m_mapHandlers.clear();
}
