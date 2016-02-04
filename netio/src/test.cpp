#include "msgq_manager.h"
#include "service_loader.h"
#include "reactor.h"
#include <unistd.h>
#include <iostream>
using namespace std;


int main(int argc, char** argv)
{
	CMsgQManager oCMQManager;

	map<int,const char*>::const_iterator it =g_mapCmdDLL.begin();
	for (;it!=g_mapCmdDLL.end();++it) {
		oCMQManager.AddMsgQueue(it->first);
	}

	CServiceLoader oServiceLoader;
	oServiceLoader.LoadServices();
	oServiceLoader.CleanServices();

	CReactor oReactor;
	int iRet = oReactor.Init(7890);
	cout<<iRet<<endl;

	/*
	if ( fork() == 0 ) {
		//sleep();
		//�ӽ��̳���
		//CMsgQueue* pMsgq = NULL;
		//oCMQManager.GetMsgQueue(0xcccde,pMsgq);
		oCMQManager.delMsgQueue(0xcccde);
	}
	else {
		//�����̳���
		oCMQManager.delMsgQueue(0xcccde);
	}*/


	return 0;
}
