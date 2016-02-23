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

	CMsgQueue* rpMsgq;
	oCMQManager.GetMsgQueue(0xccccd,rpMsgq);
	MsgBuf_T stMsg;
	stMsg.lType = REQUEST;
	int Len;
	rpMsgq->GetMsg(&stMsg,Len);
	printf("get Msg lenth:%d\n",Len);

	oCMQManager.GetMsgQueue(0xcccce,rpMsgq);
	MsgBuf_T stMsg2;
	stMsg2.lType = RESPONSE;
	string strResponse = "This is the response";
	snprintf(stMsg2.sBuf,strResponse.length()+1,"%s",strResponse.c_str());
	rpMsgq->PutMsg(&stMsg2,sizeof(stMsg2.lType) + strResponse.length());



	//cout<<iRet<<endl;
	//oReactor.RunEventLoop();

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
