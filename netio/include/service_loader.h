/*
 * service_loader.h
 *
 *  Created on: 2015年12月21日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_SERVICE_LOADER_H_
#define NETIO_INCLUDE_SERVICE_LOADER_H_
#include <map>
#include <string>
using namespace std;

class CServiceLoader {
public:
	CServiceLoader();
	~CServiceLoader();

public:
	int LoadServices();
	int CleanServices();

private:
	int LoadSercie_i(int iCmdId,const char* pszDLLName);

private:
	map<int,void*> m_mapHandlers; //每个命令字对应的DLL的打开句柄
};


#endif /* NETIO_INCLUDE_SERVICE_LOADER_H_ */
