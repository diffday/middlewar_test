/*
 * cmd_obj.h
 *
 *  Created on: 2016年2月29日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_CMD_OBJ_H_
#define NETIO_INCLUDE_CMD_OBJ_H_
#include <string>
#include <stdio.h>
#include <sstream>
#include "string_helper.h"
#include <stdlib.h>

class CCmd {
public:
	int iIndex; //IO进程的内部标示，当前版本没用到
	int iSvcSerialNo;//SVC进程的内部表示，用以状态性的指派恢复
	int      iFd;
	int      iCmd;
	int ifamily;
	std::string sClientIp;
	int    iPort;
	std::string sData;
	int iRet;

	//返回格式化后的长度，iLen为buf的总长度
	int ToString(char* pBuf, int iLen) {
		return snprintf(pBuf,iLen,"ret=%d&index=%d&serialno=%d&cmd=%d&fd=%d&family=%d&cliIp=%s&cliPort=%d&%s",iRet, iIndex,iSvcSerialNo,iCmd,iFd,ifamily,sClientIp.c_str(),iPort,sData.c_str());
	}

	CCmd():iIndex(0), iSvcSerialNo(0), iFd(0),iCmd(0),ifamily(0),iPort(0),iRet(0){}

	std::string ToString() {
		stringstream ss;
		ss<<"ret="<<iRet;
		ss<<"&index="<<iIndex;
		ss<<"&serialno="<<iSvcSerialNo;
		ss<<"&cmd="<<iCmd;
		ss<<"&fd="<<iFd;
		ss<<"&family="<<ifamily;
		ss<<"&cliIp="<<sClientIp;
		ss<<"&cliPort="<<iPort;
		ss<<"&"<<sData;

		return string(ss.str());

	}

	int InitCCmd(char* pBuf) { //返回值可以用于区分是否字段齐全，demo实现就不搞那么严谨了
		map<string,string> mapPara;
		strPairAppendToMap(pBuf,mapPara);

		if (mapPara.find("ret") != mapPara.end()) {
			int iData = static_cast<int>(atoll(mapPara.find("ret")->second.c_str()));
			iRet = iData;
			mapPara.erase(mapPara.find("ret"));
		}

		if (mapPara.find("cmd") != mapPara.end()) {
			int iData = static_cast<int>(atoll(mapPara.find("cmd")->second.c_str()));
			iCmd = iData;
			mapPara.erase(mapPara.find("cmd"));
		}

		if (mapPara.find("fd") != mapPara.end()) {
			int iData = static_cast<int>(atoll(mapPara.find("fd")->second.c_str()));
			iFd = iData;
			mapPara.erase(mapPara.find("fd"));
		}

		if (mapPara.find("index") != mapPara.end()) {
			int iData = static_cast<int>(atoll(mapPara.find("index")->second.c_str()));
			iIndex = iData;
			mapPara.erase(mapPara.find("index"));
		}


		if (mapPara.find("serialno") != mapPara.end()) {
			int iData = static_cast<int>(atoll(mapPara.find("serialno")->second.c_str()));
			iSvcSerialNo = iData;
			mapPara.erase(mapPara.find("serialno"));
		}

		if (mapPara.find("family") != mapPara.end()) {
			int iData = static_cast<int>(atoll(mapPara.find("family")->second.c_str()));
			ifamily = iData;
			mapPara.erase(mapPara.find("family"));
		}

		if (mapPara.find("cliPort") != mapPara.end()) {
			int iData = static_cast<int>(atoll(mapPara.find("cliPort")->second.c_str()));
			iPort = iData;
			mapPara.erase(mapPara.find("cliPort"));
		}

		if (mapPara.find("cliIp") != mapPara.end()) {
			sClientIp = mapPara.find("cliIp")->second;
			mapPara.erase(mapPara.find("cliIp"));
		}

		map<string,string>::iterator it = mapPara.begin();
		stringstream ss;
		for (;it != mapPara.end();++it) {
			if (it != mapPara.begin()) {
				ss<<"&";
			}
			ss<<it->first<<"="<<it->second;
		}
		sData = ss.str();
	}

};

#endif /* NETIO_INCLUDE_CMD_OBJ_H_ */
