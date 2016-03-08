#include "pth.h"
#include <stdio.h>

pth_uctx_t uctx;
pth_uctx_t uctxMain;

void Process(void* ctx)
{
	printf("5\n");
	char* szValue = reinterpret_cast<char*>(ctx);
	pth_uctx_switch(uctx, uctxMain);
	printf("%s\n",szValue);

	printf("6\n");
}

int main()
{

	pth_uctx_create((pth_uctx_t *)&uctx);
	pth_uctx_create((pth_uctx_t *)&uctxMain);

	printf("1\n");
	char* szAddr = NULL;
	char szName[6] ={'a','b'};
	int ret = pth_uctx_make(uctx, szAddr, 256000, NULL, Process, (void*)szName, uctxMain);
	printf("2\n");
	pth_uctx_switch(uctxMain, uctx);
	printf("3\n");
	printf("4\n");

	pth_uctx_switch(uctxMain, uctx);

	return 0;
}