/*
 * cache_manager.cpp
 *
 *  Created on: 2016年3月25日
 *      Author: chenclyde
 */
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
//#include "global_define.h"
#include "cache_manager.h"

CCacheManager* CCacheManager::pCacheManager = NULL;

int CCacheManager::Initalize(int dwCacheSize) {
	key_t key;
	printf("ftok_path:%s\n",CACHE_PATH);
	key = ftok(CACHE_PATH, 0); //文件要已存在，否则ftok会失败
	if(key == -1) {
		perror("ftok error");
		exit(1);
	}
	else /* This is not needed, just for success message*/
	{
	    printf("ftok success\n");
	}


	//int shmget(key_t key, size_t size, int shmflg);
	m_id = shmget(key,dwCacheSize,IPC_CREAT | 0666); //IPC_CREAT | IPC_EXCL 当碰到已创建的shm，返回失败
	if(m_id == -1) {
		perror("shmget error");
		return -1;
	}

	if (m_pStart == NULL) {
		//void *shmat(int shmid, const void *shmaddr,int shmflg);
		m_pStart = (char*)shmat(m_id, NULL,0);
		printf("Init pCache address:%p\n",m_pStart);
	}

	int iInit = *((int *)m_pStart);
	//内存区放一个魔术标记表示已初始化完成
	if (iInit == 0) {
		//创建netio 缓存区, 1个节点
		char* pNetIoCacheStart = m_pStart + sizeof(iInit);
		NetIoStat.InitalizeByNodeCount(pNetIoCacheStart,1);

		//创建container1缓存区，多进程，初始化100个节点,按进程号模100记录历史处理计数
		char* pContainerOneCacheStart = pNetIoCacheStart + NetIoStat.GetTotalSize();
		ContainerOneStat.InitalizeByNodeCount(pContainerOneCacheStart,100);
		//创建container2缓存区，100个节点
		char* pContainerTwoCacheStart = pContainerOneCacheStart + ContainerOneStat.GetTotalSize();
		ContainerTwoStat.InitalizeByNodeCount(pContainerTwoCacheStart,100);

		*((int *)m_pStart) = 1;
	}
	else {
		printf("=======The cache area has init======\n");
		//将头区的信息读出设置进对象，关键是重新计算索引指针值。因为每次重启，指针值都不一样，但偏移关系是稳定的（头部及迭代器设计为保存的是偏移值，则更方便）。这里因为是demo程序，所以重新初始化一遍头部，没影响到存储的值内容就好
		char* pNetIoCacheStart = m_pStart + sizeof(iInit);
		NetIoStat.InitalizeByNodeCount(pNetIoCacheStart, 1);

		//创建container1缓存区，多进程，初始化100个节点,按进程号模100记录历史处理计数
		char* pContainerOneCacheStart = pNetIoCacheStart + NetIoStat.GetTotalSize();
		ContainerOneStat.InitalizeByNodeCount(pContainerOneCacheStart, 100);
		//创建container2缓存区，100个节点
		char* pContainerTwoCacheStart = pContainerOneCacheStart	+ ContainerOneStat.GetTotalSize();
		ContainerTwoStat.InitalizeByNodeCount(pContainerTwoCacheStart, 100);


	}

	return 0;
}

CCacheManager::~CCacheManager() {
	if (m_pStart && (shmdt(m_pStart) == -1)) {
		perror(" detach error ");
	}
}

int CCacheManager::Set(string& sKey, int iValue) {
	printf("The cache key is:%s\n",sKey.c_str());
	CModHashV1<string,int>* hashObj;
	if (sKey.find("c1_") == 0) {
		hashObj = &ContainerOneStat;
	}
	else if (sKey.find("c2_") == 0) {
		hashObj = &ContainerTwoStat;
	}
	else if (sKey.find("netio_") == 0) {
		hashObj = &NetIoStat;
	}
	else {
		return -1;//invalid key
	}


	return hashObj->Add(sKey,iValue);
}

int CCacheManager::Get(string& sKey, int& iValue) {
	CModHashV1<string, int>* hashObj;
	if (sKey.find("c1_") == 0) {
		hashObj = &ContainerOneStat;
	} else if (sKey.find("c2_") == 0) {
		hashObj = &ContainerTwoStat;
	} else if (sKey.find("netio_") == 0) {
		hashObj = &NetIoStat;
	} else {
		return -1; //invalid key
	}
	return hashObj->Find(sKey,iValue);
	//return 0;
}

int CCacheManager::Delete(string& sKey) {
	CModHashV1<string, int>* hashObj;
	if (sKey.find("c1_") == 0) {
		hashObj = &ContainerOneStat;
	} else if (sKey.find("c2_") == 0) {
		hashObj = &ContainerTwoStat;
	} else if (sKey.find("netio_") == 0) {
		hashObj = &NetIoStat;
	} else {
		return -1; //invalid key
	}
	return	hashObj->Delete(sKey);
}

int CCacheManager::Incr(const string& sKey, int iValue) {
	CModHashV1<string, int>* hashObj;
	if (sKey.find("c1_") == 0) {
		hashObj = &ContainerOneStat;
	} else if (sKey.find("c2_") == 0) {
		hashObj = &ContainerTwoStat;
	} else if (sKey.find("netio_") == 0) {
		hashObj = &NetIoStat;
	} else {
		return -1; //invalid key
	}

	int iTemp = 0;
	int i = hashObj->Find(sKey,iTemp);
	if (i == 0) {
		return hashObj->Add(sKey,iTemp + iValue);
	}
	else {
		return hashObj->Add(sKey,iValue);
	}


	return 0;
}

void CCacheManager::PrintAll() {
	printf("=======Print netio stat cache======\n");
	CModHashV1Iterator<string,int>* it = NetIoStat.Begin();
	int i = 0;
	for (;it->Compare(NetIoStat.End())<=0;it->Next(),++i) {
		printf("%d statinfo:%d\n",i,it->GetCurrentNode());
	}
	printf("=======Print container1 stat cache======\n");
	it = ContainerOneStat.Begin();
	i=0;
	for (;it->Compare(ContainerOneStat.End()) <=0;it->Next(),++i) {
		printf("%d statinfo:%d\n",i,it->GetCurrentNode());
	}

	printf("=======Print container2 stat cache======\n");
	it = ContainerTwoStat.Begin();
	i=0;
	for (;it->Compare(ContainerTwoStat.End())<=0;it->Next(),++i) {
		printf("%d statinfo:%d\n",i,it->GetCurrentNode());
	}

	//it = ContainerTwoStat.End();
	//printf("%d\n", (*it)<=ContainerTwoStat.End()); //运算符重载是作用在对象上才能调用
}

