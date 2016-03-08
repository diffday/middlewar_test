/*
 * intf_service.cpp
 *
 *  Created on: 2016年3月8日
 *      Author: chenzhuo
 */

#include "intf_service.h"
#include "service_dispatcher.h"


IService::IService() {
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

		//int pth_uctx_make(pth_uctx_t uctx, char *sk_addr, size_t sk_size, const sigset_t *sigmask, void (*start_func)(void *), void *start_arg, pth_uctx_t uctx_after);
		//创建一份
		//pth_uctx_make(m_uctx, m_pUCTX, m_nUCtxSize, NULL, process_service, (void *)this, CAPP->UCTX());
	}

int IService::Schedule(CCmd& oCmd){
		//int pth_uctx_make(pth_uctx_t uctx, char *sk_addr, size_t sk_size, const sigset_t *sigmask, void (*start_func)(void *), void *start_arg, pth_uctx_t uctx_after);
		pth_uctx_make(m_uctx,m_pUCTXStack,m_iUCTXStackSize,NULL,process_service,(void*)&oCmd,CServiceDispatcher::Instance()->GetUCTX());

		return 0;
}
