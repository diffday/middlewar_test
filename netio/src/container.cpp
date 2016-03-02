#include "msgq_manager.h"
#include "reactor.h"
#include "service_loader.h"
#include "reactor.h"
#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <sys/un.h>
#include "cmd_obj.h"

#include "string_helper.h"
using namespace std;

int main(int argc, char** argv)
{
	CMsgQManager oCMQManager;
	oCMQManager.AddMsgQueue(NET_IO_BACK_MSQ_KEY);

	map<int,const char*>::const_iterator it =g_mapCmdDLL.begin();
	for (;it!=g_mapCmdDLL.end();++it) {
		oCMQManager.AddMsgQueue(it->first);
	}

	CServiceLoader oServiceLoader;
	oServiceLoader.LoadServices();
	oServiceLoader.CleanServices();

	int count=0;
	while (count<1000000) {

	CMsgQueue* rpMsgq;
	oCMQManager.GetMsgQueue(0xcccce,rpMsgq);
	MsgBuf_T stMsg;
	stMsg.Reset();
	stMsg.lType = REQUEST;
	//stMsg.sBuf = {0};
	int Len = 0;
	int iret = rpMsgq->GetMsg(&stMsg,Len);
	++count;

	if (Len == 0) {
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 10;

		select(0,NULL,NULL,NULL,&tv); //纯纯的sleep
		continue;
	}
	else {
		printf("get Msg lenth:%d,data is %s\n",Len,stMsg.sBuf);
	}
	CCmd oCmd;
	oCmd.InitCCmd(stMsg.sBuf);

	/*
	map<string,string> mapPara;
	strPairAppendToMap(stMsg.sBuf,mapPara);
	int ifd = static_cast<int>(atoll(mapPara.find("fd")->second.c_str()));
	string clientIp = mapPara.find("cliIp")->second;
	int family = atoi(mapPara.find("family")->second.c_str());
	int port = atoi(mapPara.find("cliPort")->second.c_str());*/


	oCMQManager.GetMsgQueue(NET_IO_BACK_MSQ_KEY,rpMsgq);
	MsgBuf_T stMsg2;
	stMsg.Reset();
	stMsg2.lType = RESPONSE;
	oCmd.sData = "resp=This is the response";
	//string strResponse = "This is the response";
	oCmd.ToString(stMsg2.sBuf,sizeof(stMsg2.sBuf));
	//snprintf(stMsg2.sBuf,strResponse.length()+100,"fd=%d&family=%d&cliIp=%s&cliPort=%d&resp=%s",ifd, family,clientIp.c_str(), port, strResponse.c_str());
	rpMsgq->PutMsg(&stMsg2,strlen(stMsg2.sBuf));

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

	int iSockfd = ::socket(PF_LOCAL, SOCK_DGRAM, 0);
	printf("create fd:%d\n",iSockfd);
	if(iSockfd < 0 )
	{
		printf("USockUDPSendTo-create socket failed\n");
		continue;
	}

	char pSendBuf[2] = {'s','\0'};
		    // Make Peer Addr
	struct sockaddr_un stUNIXAddr;
	memset(&stUNIXAddr, 0, sizeof(stUNIXAddr));
	stUNIXAddr.sun_family = AF_LOCAL;
		    //StrMov(stUNIXAddr.sun_path, pszSockPath); // "/tmp/pipe_channel.sock"
	strncpy(stUNIXAddr.sun_path, NET_IO_USOCK_PATH, sizeof(stUNIXAddr.sun_path));

		    // Send Buffer
	int iBytesSent = ::sendto(
		        iSockfd,
				pSendBuf,
		        1,
		        0,
		        (struct sockaddr *)&(stUNIXAddr),
		        sizeof(struct sockaddr_un));

	if(iBytesSent == -1 || static_cast<uint32_t>(iBytesSent) != 1)
	{
		printf("USockUDPSendTo-send notify failed\n");;

	    return -1;
	}

	printf("close fd:%d,ret:%d\n",iSockfd,close(iSockfd)); //udp同样要关闭


	}
	return 0;

}
