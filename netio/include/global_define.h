/*
 * global_consts.h
 *
 *  Created on: 2015年12月15日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_GLOBAL_DEFINE_H_
#define NETIO_INCLUDE_GLOBAL_DEFINE_H_

#define MAX_MSG_SIZE 1024
#define MAX_QBYTES 1024* 100;
#define DEFAULT_EPOLL_WAIT_TIME 10 //毫秒
#define PTH_UCTX_STACK_SIZE 128*1024; //字节数
#define UNFINISH_TASK_RET_FLAG 2000000000
#include <map>

using namespace std;

enum errcode {
	CREATE_MSGQ_FAILED = 1,
	CNTL_MSGQ_BYTES_FAILED,
	MSGQ_NO_EXIST,
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
	CREATE_USOCK_FAILED,
	SET_USOCKOPT_FAILED,
	BIND_USOCKET_FAILED,
	FCNTL_USOCKET_FAILED,
	NO_FREE_SVC_HANDLER,
};

typedef enum {
	REQUEST = 1, //msg的type不能接受0，所以用1起始
	RESPONSE = 2,
	APP = 3,
} MSGTYPE_t;

static const int NET_IO_BACK_MSQ_KEY = 0xccccc;
static const char* NET_IO_USOCK_PATH = "/tmp/app/0001/0001_0";
static const int NETIO_PORT = 6789;

static const std::map<int, const char*>::value_type init_cmd_dll_value[] =
{
//std::map<int, const char*>::value_type( 0xccccd, "/home/chenzhuo/test/middlewar_test/netio/so/libverto.so.0.0"),
//std::map<int, const char*>::value_type( 0xcccce, "/home/chenzhuo/test/middlewar_test/netio/so/libXxf86vm.so.1.0.0")

std::map<int, const char*>::value_type( 0xccccd, "./so/svc_test.so"),
std::map<int, const char*>::value_type( 0xcccce, "./so/svc_test2.so")
};

static const std::map<int, const char*> g_mapCmdDLL(init_cmd_dll_value, init_cmd_dll_value+sizeof(init_cmd_dll_value)/sizeof(init_cmd_dll_value[0]));

#endif /* NETIO_INCLUDE_GLOBAL_DEFINE_H_ */
