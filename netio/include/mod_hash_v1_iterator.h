#ifndef PAIPAI_PROMOTE_CACHE_MODHASHV1_ITERATOR_H
#define PAIPAI_PROMOTE_CACHE_MODHASHV1_ITERATOR_H

#include "mod_hash_v1.h"
#include <typeinfo>


template<typename K,typename T>
class CModHashV1Iterator : public IChunkedIndexIterator<K,T> {
private:
	CModHashV1<K,T>* m_pCModHashV1Index;
	typedef typename CModHashV1<K,T>::stModHashV1Bucket* stBuckIndex;
	stBuckIndex m_pBuckIndex;

public:
	~CModHashV1Iterator() {};
	CModHashV1Iterator(CModHashV1<K,T>* pCmodHashV1,stBuckIndex pBuckIndex) {
		m_pCModHashV1Index=pCmodHashV1;
		m_pBuckIndex=pBuckIndex;
		//m_pCModHashV1Index->MustInit(); //不能开启
	};

public:
   /**
	* 迭代器后移
	*
	* @return int 负数为出凑
	*/
	int Next()  {
		m_pCModHashV1Index->MustInit();
		//if ((m_pBuckIndex <= m_pCModHashV1Index->m_stHead.pPBucketsEnd)) {
			__MODHASH_DEBUG_PRINT("Iterator Next move.");
			++(m_pBuckIndex);
			//printf("%p\n",m_pBuckIndex);
		//}

		return m_pCModHashV1Index->SUCCESS;
	}

	/**
	 * 迭代器前移
	 *
	 * @return int 负数为出凑
	 */
	int Prev()  {
		m_pCModHashV1Index->MustInit();
		//if ((m_pBuckIndex >= m_pCModHashV1Index->m_stHead.pPucketsStart)) {
			__MODHASH_DEBUG_PRINT("Iterator Prev move.");
			--(m_pBuckIndex);
		//}

		return m_pCModHashV1Index->SUCCESS;
	}

	/**
	 * 获取迭代器指代的值
	 */

	T& GetCurrentNode() {
		m_pCModHashV1Index->MustInit();
		return m_pBuckIndex->node;
	}

	int GetCurrentNode(T& node) {
		m_pCModHashV1Index->MustInit();
		node=m_pBuckIndex->node;
		return m_pCModHashV1Index->SUCCESS;
	}

	bool operator<=(CModHashV1Iterator<K, T>* it2) {
		printf("m_pBuckIndex:%p - it->m_pBuckIndex:%p\n",m_pBuckIndex,it2->m_pBuckIndex);
		return (m_pBuckIndex -  it2->m_pBuckIndex) < 0;
	}

	bool operator !=(CModHashV1Iterator<K,T>* it) {
		return m_pBuckIndex ==  it->m_pBuckIndex;
	}


	int Compare(IChunkedIndexIterator<K, T>* it) {
		//__MODHASH_DEBUG_PRINT("The orginal typeid:%u",typeid(this));
		//__MODHASH_DEBUG_PRINT("The compare typeid:%u",typeid(it));
		//__MODHASH_DEBUG_PRINT("The orginal type:%s",typeid(this).name());
		//__MODHASH_DEBUG_PRINT("The compare type:%s",typeid(it).name());
		//printf("m_pBuckIndex:%p - it->m_pBuckIndex:%p\n",m_pBuckIndex,((CModHashV1Iterator<K,T>*)it)->m_pBuckIndex);
		if (typeid(this) != typeid(it)) {
			//assert(0);
			//return 1;
		}

		if (typeid(this).name() != typeid(it).name()) {
			//assert(0);
			//return 1;
		}

		int64_t ddwResult = m_pBuckIndex -  ((CModHashV1Iterator<K,T>*)it)->m_pBuckIndex;
		if (ddwResult > 0) {
			return 1;
		}
		else if (ddwResult < 0){
			return -1;
		}

		return 0;
	}
};

#endif /*PAIPAI_PROMOTE_CACHE_MODHASHV1_ITERATOR_H*/
