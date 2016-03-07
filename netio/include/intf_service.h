/*
 * inf_service.h
 *
 *  Created on: 2016年2月25日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_INTF_SERVICE_H_
#define NETIO_INCLUDE_INTF_SERVICE_H_
#include "cmd_obj.h"
#include "pth.h"
#include "global_define.h"

#ifdef __cplusplus
extern "C"
{
#endif
	void process_service(void *cmdObj);
#ifdef __cplusplus
}
#endif

class IService {
private:
	int m_iCmd;
	pth_uctx_t m_uctx;
	char* m_pUCTXStack;
	int m_iUCTXStackSize;

public:
	virtual int Execute(CCmd& oCmd) = 0; //执行结果，这里也用简化cmd来传递

	virtual ~IService(){};

	IService() {
		m_iCmd = 0;
		m_iUCTXStackSize = PTH_UCTX_STACK_SIZE;

		/*
		 *
typedef pth_uctx_st *pth_uctx_t;
struct pth_uctx_st {
	int         uc_stack_own; // whether stack were allocated by us
	char       *uc_stack_ptr; // pointer to start address of stack area
	size_t      uc_stack_len; // size of stack area
	int         uc_mctx_set;  // whether uc_mctx is set
	pth_mctx_t  uc_mctx;      // saved underlying machine context
};

typedef struct pth_mctx_st pth_mctx_t;
struct pth_mctx_st {
#if PTH_MCTX_MTH(mcsc)
    ucontext_t uc;
#elif PTH_MCTX_MTH(sjlj)
    pth_sigjmpbuf jb;
#else
#error "unknown mctx method"
#endif
    sigset_t sigs;
#if PTH_MCTX_DSP(sjlje)
    sigset_t block;
#endif
    int error;
};

		 */
		//int pth_uctx_create(pth_uctx_t *uctx);
		pth_uctx_create((pth_uctx_t *)&m_uctx);
		m_pUCTXStack = (char*)malloc(m_iUCTXStackSize);
	}

	int Schedule(CCmd& oCmd) {
		//int pth_uctx_make(pth_uctx_t uctx, char *sk_addr, size_t sk_size, const sigset_t *sigmask, void (*start_func)(void *), void *start_arg, pth_uctx_t uctx_after);
		//pth_uctx_make(m_uctx,m_pUCTXStack,PTH_UCTX_STACK_SIZE,NULL,process_service,&oCmd,);

		return 0;
	}


};

class IServiceFactory {
public:
	virtual IService* Create()=0;
};

#endif /* NETIO_INCLUDE_INTF_SERVICE_H_ */
