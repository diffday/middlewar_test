#include "msgq_manager.h"
#include "service_loader.h"
#include "reactor.h"
#include <unistd.h>
#include <iostream>
#include "netio_user_event_handler.h"
using namespace std;


int main(int argc, char** argv)
{
	CMsgQManager* oCMQManager = CMsgQManager::GetInstance();
	oCMQManager->AddMsgQueue(NET_IO_BACK_MSQ_KEY);


	map<int,const char*>::const_iterator it =g_mapCmdDLL.begin();
	for (;it!=g_mapCmdDLL.end();++it) {
		oCMQManager->AddMsgQueue(it->first);
	}

	//CServiceLoader oServiceLoader;
	//oServiceLoader.LoadServices();
	//oServiceLoader.CleanServices();

	CReactor oReactor;
	int iRet = oReactor.Init(NETIO_PORT, NET_IO_USOCK_PATH);

	CTcpNetHandler* oTcpNetHandler = new CTcpNetHandler;
	oTcpNetHandler->RegisterMqManager(oCMQManager);

	CUSockUdpHandler* oUsockUdpHandler = new CUSockUdpHandler;
	//oUsockUdpHandler->RegisterMqManager(&oCMQManager);
	CNetIOUserEventHandler* oNetIOUserHandler = new CNetIOUserEventHandler;
	oNetIOUserHandler->RegisterMqManager(oCMQManager);

	oReactor.RegisterTcpNetHandler(oTcpNetHandler);
	oReactor.RegisterUSockUdpHandler(oUsockUdpHandler);
	oReactor.RegisterUserEventHandler(oNetIOUserHandler);
	cout<<"All init result:"<<iRet<<endl;
	oReactor.RunEventLoop();

	/*
	if ( fork() == 0 ) {
		//sleep();
		//子进程程序
		//CMsgQueue* pMsgq = NULL;
		//oCMQManager.GetMsgQueue(0xcccde,pMsgq);
		oCMQManager.delMsgQueue(0xcccde);
	}
	else {
		//父进程程序
		oCMQManager.delMsgQueue(0xcccde);
	}*/


	return 0;
}
