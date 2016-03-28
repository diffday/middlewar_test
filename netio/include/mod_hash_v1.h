/*
 * mod_hash.h
 *
 *  Created on: 2015年5月26日
 *      Author: chenzhuo
 */

#ifndef PAIPAI_PROMOTE_CACHE_INDEX_CHUNKED_MOD_HASH_H_
#define PAIPAI_PROMOTE_CACHE_INDEX_CHUNKED_MOD_HASH_H_
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include "intf_chunkedindex.h"
#include "assert.h"
#include <vector>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <typeinfo>
//#include "include/error_code.h"


/*#ifndef __MODHASH_DEBUG_PRINT
	#define __MODHASH_DEBUG_PRINT(format,args...) \
	fprintf(stderr, "[FUNCITON:%s,LINE:%d]\t",__FUNCTION__,__LINE__); \
	fprintf(stderr, format, ##args); \
	fprintf(stderr, "\n");
#else*/
	#define __MODHASH_DEBUG_PRINT(format,args...)
//#endif

using namespace std;



template<typename K, typename T> class CModHashV1Iterator;
/**
 * 此类实现的特征为，hash冲突用下挂链表桶的方案来解决，不实施二次散列
 */
template<typename K, typename T>
class CModHashV1: public IChunkedIndex<K, T> {
	friend class CModHashV1Iterator<K, T>;

private:
	struct stBucketMeta {
		int iBuckNum;
		int iLock;
		int iHasSet;
		int iInvalid;
		uint32_t iFirstSetTime;
		uint32_t iLastSetTime;
		uint32_t iLastAccessTime;
		uint32_t dwAccessCount;
	};

	//设计的大缺陷是没保存原始key的值
	//K,T本身应该就是定长chunk块，可容纳的空间是确定有限的，而不是当前这种类似于传统stl的new(T)的处理办法。
	struct stModHashV1Bucket {
		stBucketMeta stBMeta;
		T node;
	};

	struct stHead {
		uint32_t dwSpecifier;
		int iInited;
		int iBuckNums;
		int iGlobalLock;
		int iVersion;
		uint32_t dwAccessCount;
		uint32_t dwCreateTime;
		uint32_t dwLastResetTime;
		int iTotalSize;
		int iFreeNodeCount;
		void* pCache;
		stModHashV1Bucket* pPucketsStart;
		stModHashV1Bucket* pPBucketsEnd;
	} m_stHead;

	typedef CModHashV1Iterator<K, T>* iterator;
	const static int SUCCESS = 0;
	const static int SPECIFIER = 17607181;

	iterator m_pIteratorStart;
	iterator m_pIteratorEnd;

	int m_dwNeedSynchronized;

	/**
	 * 判断索引区是否为空
	 */
	int IsEmpty();

	inline void MustInit() {
		assert(m_stHead.iInited);
	}

	/**
	 * 将主键转换成
	 */
	int ModToUint(const K& nodeKey);
	uint32_t GetTimeNow();
	int StoreHead();
	int ReadAndCheckHead(char* pCache);

public:
	//typedef CModHashV1Iterator<K, T>* iterator;
	//typedef stModHashV1Bucket* iterator;
	/**
	 * 查找索引记录
	 * @param const char* pszIndexkey: 索引的key
	 * @param T& pIndexNode: 索引值的指针
	 *
	 * @return  int : 0(成功) 其它失败
	 */
	int Find(const K& sIndexkey, T& indexNode);

	int GetFreeNodeCount();
	int GetTotalNodeCount();
	int GetTotalSize();
	int GetUnitChunkSize();
	/**
	 * 对管理的索引数据进行清空
	 */
	int Reset();
	int DumpToFile(const char* pszFilePath) {
		MustInit();
		return SUCCESS;
	}
	int LoadFromFile(const char* pszFilePath) {
		MustInit();
		return SUCCESS;
	}
	int InitalizeByCacheSize(char* pCache, int dwCacheLength);

	int Add(const K& sIndexkey, const T& indexNode);

	int Delete(const K& sIndexkey);

	/**
	 * 获取索引chunk区外保留占用空间，一般用于存放索引区元信息
	 *
	 * @return int 总大小字节数，负数为出错
	 */
	int GetHeadReservedSize();

	/**
	 * 获取索引的节点数
	 *
	 * @return int 索引节点数，负数为出错
	 */
	int GetIndexedNodeCount();


	iterator Begin() {
		MustInit();
		return m_pIteratorStart;
	}


	iterator End() {
		MustInit();
		return m_pIteratorEnd;
	}

	int InitalizeByNodeCount(char* pCache, int dwNodeCount);

	int LockKey(const K& keyNode) {
		MustInit();
		//高效的办法可以引用apache的一段汇编级锁定代码
		return SUCCESS;
	}

	int UnLockKey(const K& keyNode) {
		MustInit();
		return SUCCESS;
	}

	int SetIndexPointer(char* pIndexPointer);
	int FullFillValue(vector<T>& vecT);

public:
	~CModHashV1() {

		if (m_pIteratorStart != NULL) {
			printf("delete m_pIteratorStart\n");
			delete m_pIteratorStart;
		}
		if (m_pIteratorEnd != NULL) {
			printf("delete m_pIteratorEnd\n");
			delete m_pIteratorEnd;
		}
	}

	CModHashV1(int dwNeedSynchronized=0) {
		//初始化头部区
		m_stHead.dwSpecifier=SPECIFIER;
		m_stHead.iInited = 0;
		m_stHead.iBuckNums = 0;
		m_stHead.iVersion = 0;
		m_stHead.dwAccessCount = 0;
		m_stHead.dwCreateTime = 0;
		m_stHead.dwLastResetTime = 0;
		m_stHead.iTotalSize = 0;
		m_stHead.iFreeNodeCount = 0;
		m_stHead.pCache = 0;
		m_stHead.pPucketsStart = 0;
		m_stHead.pPBucketsEnd = 0;
		m_stHead.iGlobalLock = 0;

		m_dwNeedSynchronized = dwNeedSynchronized;

		m_pIteratorStart = 0;
		m_pIteratorEnd = 0;

	}



};

template<typename K, typename T>
int CModHashV1<K,T>::IsEmpty() {
	MustInit();
	return m_stHead.iFreeNodeCount == m_stHead.iBuckNums;
}

template<typename K, typename T>
uint32_t CModHashV1<K,T>::GetTimeNow() {
	return time(0);
}


template<typename K, typename T>
int CModHashV1<K,T>::InitalizeByCacheSize(char* pCache, int dwCacheLength) {
	int iHeadSize = sizeof(m_stHead);
	int iUintBuckSize = sizeof(stModHashV1Bucket);

	int iLeftLength = dwCacheLength-iHeadSize;

	m_stHead.iBuckNums = iLeftLength / iUintBuckSize;
	iLeftLength -= (m_stHead.iBuckNums*iUintBuckSize);

	//m_stHead.dwSpecifier=SPECIFIER;
	m_stHead.iInited = 0;
	m_stHead.dwAccessCount = 0;
	m_stHead.iGlobalLock = 0;
	m_stHead.dwCreateTime = GetTimeNow();
	m_stHead.dwLastResetTime = GetTimeNow();
	m_stHead.iTotalSize = dwCacheLength;
	m_stHead.iFreeNodeCount = m_stHead.iBuckNums;
	m_stHead.pCache = pCache;
	m_stHead.pPucketsStart = (stModHashV1Bucket*)(((char*)pCache) + iHeadSize);
	m_stHead.pPBucketsEnd = (stModHashV1Bucket*)(((char*)pCache) + dwCacheLength - iLeftLength - iUintBuckSize);
	m_pIteratorStart = new CModHashV1Iterator<K, T>(this,m_stHead.pPucketsStart);
	m_pIteratorEnd = new CModHashV1Iterator<K, T>(this,m_stHead.pPBucketsEnd);

	m_stHead.iInited = 1;



	__MODHASH_DEBUG_PRINT("All size:%d", m_stHead.iTotalSize);
	__MODHASH_DEBUG_PRINT("Bucks size:%d", m_stHead.iBuckNums*iUintBuckSize);
	__MODHASH_DEBUG_PRINT("Head size:%d", iHeadSize);
	__MODHASH_DEBUG_PRINT("uintBuck size:%d", iUintBuckSize);
	__MODHASH_DEBUG_PRINT("left size:%d", iLeftLength);
	__MODHASH_DEBUG_PRINT("bucks num:%d", m_stHead.iBuckNums);
	__MODHASH_DEBUG_PRINT("First BuckAdress:%p", m_stHead.pPucketsStart);
	__MODHASH_DEBUG_PRINT("Last BuckAdress:%p", m_stHead.pPBucketsEnd);
	__MODHASH_DEBUG_PRINT("CreateTime:%u", m_stHead.dwCreateTime);

	StoreHead();

	return SUCCESS;
}

template<typename K, typename T>
int CModHashV1<K,T>::InitalizeByNodeCount(char* pCache, int dwNodeCount) {
	int iHeadSize = sizeof(m_stHead);
	int iUintBuckSize = sizeof(stModHashV1Bucket);

	m_stHead.iBuckNums = dwNodeCount;
	int iBucksSize = m_stHead.iBuckNums*iUintBuckSize;

	//m_stHead.dwSpecifier=SPECIFIER;
	m_stHead.iInited = 0;
	m_stHead.dwAccessCount = 0;
	m_stHead.iGlobalLock = 0;
	m_stHead.dwCreateTime = GetTimeNow();
	m_stHead.dwLastResetTime = GetTimeNow();
	m_stHead.iTotalSize = iBucksSize+iHeadSize;
	m_stHead.iFreeNodeCount = m_stHead.iBuckNums;
	m_stHead.pCache = pCache;
	m_stHead.pPucketsStart = (stModHashV1Bucket*)(((char*)pCache) + iHeadSize);
	m_stHead.pPBucketsEnd = (stModHashV1Bucket*)(((char*)pCache) + iHeadSize + iBucksSize - iUintBuckSize);
	m_pIteratorStart = new CModHashV1Iterator<K, T>(this,m_stHead.pPucketsStart);
	m_pIteratorEnd = new CModHashV1Iterator<K, T>(this,m_stHead.pPBucketsEnd);

	m_stHead.iInited = 1;

	__MODHASH_DEBUG_PRINT("All size:%d", m_stHead.iTotalSize);
	__MODHASH_DEBUG_PRINT("Bucks size:%d", m_stHead.iBuckNums*iUintBuckSize);
	__MODHASH_DEBUG_PRINT("Head size:%d", iHeadSize);
	__MODHASH_DEBUG_PRINT("uintBuck size:%d", iUintBuckSize);
	__MODHASH_DEBUG_PRINT("bucks num:%d", m_stHead.iBuckNums);
	__MODHASH_DEBUG_PRINT("First BuckAdress:%p", m_stHead.pPucketsStart);
	__MODHASH_DEBUG_PRINT("Last BuckAdress:%p", m_stHead.pPBucketsEnd);
	__MODHASH_DEBUG_PRINT("CreateTime:%u", m_stHead.dwCreateTime);

	StoreHead();

	return SUCCESS;
}

template<typename K, typename T>
int CModHashV1<K,T>::StoreHead() {
	MustInit();
	memcpy(m_stHead.pCache, &m_stHead, sizeof(m_stHead));
	__MODHASH_DEBUG_PRINT("Store Head to mem, %s!", "done!");
	return 0;
}

template<typename K, typename T>
int CModHashV1<K,T>::ReadAndCheckHead(char* pCache) {
	if ((pCache == m_stHead.pCache) && (pCache !=0) ) {
		return SUCCESS;
	}

	//检查存储区头部信息
	stHead stHeadTemp;

	memcpy(&stHeadTemp, pCache, sizeof(m_stHead));

	if (stHeadTemp.dwSpecifier != SPECIFIER) {
		__MODHASH_DEBUG_PRINT("ERR:%d", m_stHead.iTotalSize);
		//return INDEX_MOD_HASH_ERRCODE_WRONG_POINTER;
		return 1000;
	}

	if (stHeadTemp.iInited != 0) {
		//return INDEX_MOD_HASH_ERRCODE_UNINIT_AREA;
		return 1001;
	}

	//memcpy(&m_stHead, m_stHead.pCache, sizeof(m_stHead));
	memcpy(&m_stHead, &stHeadTemp, sizeof(m_stHead));

	return SUCCESS;
}


template<typename K, typename T>
int CModHashV1<K,T>::SetIndexPointer(char* pIndexPointer) {
	return ReadAndCheckHead(pIndexPointer);
}

template<typename K, typename T>
int CModHashV1<K,T>::GetIndexedNodeCount() {
	MustInit();
	return m_stHead.iBuckNums-m_stHead.iFreeNodeCount;
}

template<typename K, typename T>
int CModHashV1<K,T>::GetHeadReservedSize() {
	return sizeof(m_stHead);
}

template<typename K, typename T>
int CModHashV1<K,T>::GetTotalSize() {
	MustInit();
	return m_stHead.iTotalSize;
}

template<typename K, typename T>
int CModHashV1<K, T>::GetUnitChunkSize() {
	return sizeof(stModHashV1Bucket);
}

template<typename K, typename T>
int CModHashV1<K, T>::GetTotalNodeCount() {
	MustInit();
	return m_stHead.iBuckNums;
}

template<typename K, typename T>
int CModHashV1<K, T>::GetFreeNodeCount() {
	MustInit();
	return m_stHead.iFreeNodeCount;
}

template<typename K, typename T>
int CModHashV1<K, T>::FullFillValue(vector<T>& vecX) {
	MustInit();
	int iInputSize = vecX.size();
	if (iInputSize != m_stHead.iBuckNums) {
		//return INDEX_MOD_HASH_ERRCODE_WRONG_FULLFILLSIZE;
		return 1002;
	}

	__MODHASH_DEBUG_PRINT("The fill size:%d", iInputSize);

	stModHashV1Bucket* pStBucketTemp = m_stHead.pPucketsStart;

	T* pt = &(pStBucketTemp->node);

	for (int i=0; i < iInputSize;++i) {
		new(pt) T(vecX[i]); //调用拷贝构造函数将内容复制到指定的指针位置
		pStBucketTemp->stBMeta.iHasSet = 1;
		pStBucketTemp->stBMeta.iInvalid = 0;
		++pStBucketTemp;
		pt = &(pStBucketTemp->node);
	}

	__MODHASH_DEBUG_PRINT("---Finish FullFill---");

	return SUCCESS;
}

template<typename K, typename T>
int CModHashV1<K, T>::ModToUint(const K& nodeKey) {
	__MODHASH_DEBUG_PRINT("The Hash key value:%s",nodeKey.c_str());
	//字符串hash算法，所有的key都当做字符指针看待
	int iSize = sizeof(nodeKey);
	unsigned long h=0;
	//int g=0;

	char* pChar = (char*)(&nodeKey);
	if (strcmp(typeid(nodeKey).name(),"Ss") == 0) {
		//char* pChar = (char*)(&nodeKey);
		//__MODHASH_DEBUG_PRINT("===== key type:%s",typeid(nodeKey).name());
		pChar = const_cast<char*>(nodeKey.c_str());
		//__MODHASH_DEBUG_PRINT("parse Hash key pointer:%s",pChar);
		char* pEnd = pChar+iSize;
		while (pChar <= pEnd) {
			//__MODHASH_DEBUG_PRINT(" key char value:%c",*pChar);
			h += (*pChar);
			pChar++;
			/*
			if ((g = (h & 0xF0000000))) {
				h = h ^ (g >> 24);
				h = h ^ g;
			}*/
		}
	}
	else if (strcmp(typeid(nodeKey).name(),"i") == 0) {
		//int* pInt = reinterpret_cast<int*>(pChar);
		h=*(reinterpret_cast<int*>(pChar));
	}
	else if (strcmp(typeid(nodeKey).name(),"l") == 0) {
		h=*(reinterpret_cast<long*>(pChar));
	}
	else {//数字等类型的hash
		perror("unsupport key type");
	}

	__MODHASH_DEBUG_PRINT("The Hash value before mod:%lu",h);

	int iResult = h%(m_stHead.iBuckNums);

	__MODHASH_DEBUG_PRINT("---The Buck index:%d",iResult);

	return iResult;
}

template<typename K, typename T>
int CModHashV1<K, T>::Find(const K& sIndexkey, T& indexNode) {
	MustInit();
	int iBuckIndex = ModToUint(sIndexkey);

	stModHashV1Bucket* pBuck = m_stHead.pPucketsStart + iBuckIndex;
	if (pBuck->stBMeta.iHasSet && !pBuck->stBMeta.iInvalid) {
		indexNode = pBuck->node;
		return SUCCESS;
	}

	//return INDEX_MOD_HASH_ERRCODE_KEY_NOT_FOUND;
	return 1003;
}

template<typename K, typename T>
int CModHashV1<K, T>::Delete(const K& sIndexkey) {
	MustInit();
	int iBuckIndex = ModToUint(sIndexkey);

	stModHashV1Bucket* pBuck = m_stHead.pPucketsStart + iBuckIndex;
	pBuck->stBMeta.iInvalid = 1;

	return SUCCESS;
}

template<typename K, typename T>
int CModHashV1<K, T>::Reset() {
	MustInit();

	stModHashV1Bucket* pBuck = m_stHead.pPucketsStart;
	for (;pBuck<=m_stHead.pPBucketsEnd;++pBuck) {
		pBuck->stBMeta.iInvalid = 1;
	}

	return SUCCESS;
}

template<typename K, typename T>
int CModHashV1<K, T>::Add(const K& sIndexkey, const T& indexNode) {
	MustInit();
	int iBuckIndex = ModToUint(sIndexkey);
	stModHashV1Bucket* pBuck = m_stHead.pPucketsStart + iBuckIndex;

	__MODHASH_DEBUG_PRINT("The offset is %d,pointer:%p，value pointer:%p",iBuckIndex,pBuck,&pBuck->node);
	T* pt = &(pBuck->node);
	new(pt) T(indexNode); //调用拷贝构造函数将内容复制到指定的指针位置

	pBuck->stBMeta.iHasSet = 1;
	pBuck->stBMeta.iInvalid = 0;

	return SUCCESS;
}



#endif /* PAIPAI_PROMOTE_CACHE_INDEX_CHUNKED_MOD_HASH_H_ */
#include "mod_hash_v1_iterator.h"
