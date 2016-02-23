#include "msgq_manager.h"
#include "reactor.h"
#include "service_loader.h"
#include "reactor.h"
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
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
	printf("get Msg lenth:%d,data is %s\n",Len,stMsg.sBuf);
	map<string,string> mapPara;
	strPairAppendToMap(stMsg.sBuf,mapPara);
	int ifd = static_cast<int>(atoll(mapPara.find("fd")->second.c_str()));

	oCMQManager.GetMsgQueue(NET_IO_BACK_MSQ_KEY,rpMsgq);
	MsgBuf_T stMsg2;
	stMsg2.lType = RESPONSE;
	string strResponse = "This is the response";
	snprintf(stMsg2.sBuf,strResponse.length()+30,"fd=%d&resp=%s",ifd, strResponse.c_str());
	rpMsgq->PutMsg(&stMsg2,sizeof(stMsg2.lType) + strResponse.length());

	/*
	struct sockaddr_in addr;
	int sockfd, len = 0;
	int addr_len = sizeof(struct sockaddr_in);*/


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
