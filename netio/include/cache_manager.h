/*
 * cache_manager.h
 *
 *  Created on: 2016年3月25日
 *      Author: chenclyde
 */

#ifndef NETIO_INCLUDE_CACHE_MANAGER_H_
#define NETIO_INCLUDE_CACHE_MANAGER_H_
#include "mod_hash_v1.h"
#include "global_define.h"
using namespace std;
class CCacheManager
{
	private:
	char* m_pStart;
        int m_id;
        CModHashV1<string,int> NetIoStat;
        CModHashV1<string,int> ContainerOneStat;
		CModHashV1<string,int> ContainerTwoStat;
		static CCacheManager* pCacheManager;

	    CCacheManager():m_pStart(NULL),m_id(0){};
public:
    virtual ~CCacheManager();
	static CCacheManager* GetInstance() {
		if (pCacheManager == NULL) {
			pCacheManager = new CCacheManager;
			pCacheManager->Initalize();
		}
		return pCacheManager;
	}

        virtual int Initalize(int dwCacheSize=CACHE_SIZE);//Total Cache size是一个较好的策略
    public:
        virtual int   Set(string& sKey, int iValue);
        virtual int   Get(string& sKey, int& iValue);
        virtual int   Incr(const string& sKey, int iValue);
        virtual int   Delete(string& sKey);
        virtual void PrintAll();
};



#endif /* NETIO_INCLUDE_CACHE_MANAGER_H_ */
