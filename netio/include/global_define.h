/*
 * global_consts.h
 *
 *  Created on: 2015��12��15��
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_GLOBAL_DEFINE_H_
#define NETIO_INCLUDE_GLOBAL_DEFINE_H_

#define MAX_MSG_SIZE 1024
#define MAX_QBYTES 1024* 100;

#include <map>
using namespace std;

/**
 * ��Ϣ�����еĽṹ����
 */
typedef struct tagMsgBuf { //������Ĭ��ֵ����ǰ��ipcs -l �鿴����
	long lType;
	char sBuf[MAX_MSG_SIZE];
} MsgBuf_T;

enum errcode {
	CREATE_MSGQ_FAILED = 1,
	CNTL_MSGQ_BYTES_FAILED,
	MSG_TOO_LONG,
	SEND_MSG_FAILED,
	RECV_MSG_FAILED,
	DELETE_MSGQ_FAILED,
	OPEN_DLL_FAILED,
	CREATE_SOCKET_FAILED,
	SET_SOCKOPT_FAILED,
	BIND_SOCKET_FAILED,
	LISTEN_SOCKET_FAILED,
	FCNTL_SOCKET_FAILED,
	EPOLL_CREATE_FAILED,
	EPOLL_CNTL_FAILED,
	EPOLL_WAIT_FAILED,
};

static const std::map<int, const char*>::value_type init_cmd_dll_value[] =
{
std::map<int, const char*>::value_type( 0xccccd, "/home/chenzhuo/test/middlewar_test/netio/so/libverto.so.0.0"),
std::map<int, const char*>::value_type( 0xcccce, "/home/chenzhuo/test/middlewar_test/netio/so/libXxf86vm.so.1.0.0")
};

static const std::map<int, const char*> g_mapCmdDLL(init_cmd_dll_value, init_cmd_dll_value+sizeof(init_cmd_dll_value)/sizeof(init_cmd_dll_value[0]));

#endif /* NETIO_INCLUDE_GLOBAL_DEFINE_H_ */