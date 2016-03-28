#ifndef PAIPAI_PROMOTE_CACHE_ICHUNKEDINDEX_H
#define PAIPAI_PROMOTE_CACHE_ICHUNKEDINDEX_H

#include <string>

template<typename K, typename T> class IChunkedIndexIterator;

template<typename K, typename T>
class IChunkedIndex {

	friend class IChunkedIndexIterator<K, T> ;

protected:
	int m_dwNeedSynchronized;

public:
	virtual ~IChunkedIndex() {
	};
	IChunkedIndex(int dwNeedSynchronized) {
		m_dwNeedSynchronized = dwNeedSynchronized;
	};
	IChunkedIndex() {
		m_dwNeedSynchronized = 0;
	};
	typedef IChunkedIndexIterator<K, T>* iterator;

public:
	/**
	 * 查找索引记录
	 * @param K& keyNode: 索引的key
	 * @param T& indexNode: 索引值的指针
	 *
	 * @return  int : 0(成功) 其它失败
	 */
	virtual int Find(const K& keyNode, T& indexNode) = 0;

	/**
	 * 获取空闲节点数
	 *
	 * @return int 负数为出凑
	 */
	virtual int GetFreeNodeCount() = 0;

	/**
	 * 获取可索引的节点总数
	 *
	 * @return int 负数为出凑
	 */
	virtual int GetTotalNodeCount() = 0;

	/**
	 * 获取已索引的节点总数
	 *
	 * @return int 负数为出凑
	 */
	virtual int GetIndexedNodeCount() = 0;

	/**
	 * 获取管理的索引区总大小
	 *
	 * @return int 总大小字节数，负数为出错
	 */
	virtual int GetTotalSize() = 0;

	/**
	 * 获取单索引节点占用空间
	 *
	 * @return int 总大小字节数，负数为出错
	 */
	virtual int GetUnitChunkSize() = 0;

	/**
	 * 获取索引chunk区外保留占用空间，一般用于存放索引区元信息
	 *
	 * @return int 总大小字节数，负数为出错
	 */
	virtual int GetHeadReservedSize() = 0;

	/**
	 * 对管理的索引数据进行清空
	 *
	 * @return int 非0为出错
	 */
	virtual int Reset() = 0;

	virtual int DumpToFile(const char* pszFilePath) = 0;

	virtual int LoadFromFile(const char* pszFilePath) = 0;

	/**
	 * 通过指定的指针起始和索引区总长度，初始化索引区，对于已初始过化的索引区，此结果将导致原管理区数据的失效
	 * @param void char* pCache: 索引区起始
	 * @param uint32_t dwCacheLength: 索引区总长度
	 *
	 * @return int 小于0为出错
	 */
	virtual int InitalizeByCacheSize(char* pCache, int dwCacheLength) = 0;

	/**
	 * 通过指定的指针起始和节点数，初始化索引区。对于已初始过化的索引区，此结果将导致原管理区数据的失效
	 * @param void char* pCache: 索引区起始
	 * @param int dwNodeCount: 要索引的节点总个数
	 *
	 * @return int 需要占用的缓冲区大小，小于0出错
	 */
	virtual int InitalizeByNodeCount(char* pCache, int dwNodeCount) = 0;

	/**
	 * 添加索引节点
	 * @param k& keyNode: 索引的key
	 * @param T& indexNode: 索引要存储的信息，从性能上考虑，建议存储指针类型
	 *
	 * @return int 非0为出错
	 */
	virtual int Add(const K& keyNode, const T& indexNode) = 0;

	/**
	 * 对指定key的索引节点进行释放/回收删除
	 * @param K& keyNode: 索引的key
	 *
	 * @return  int : 0(成功) 其它失败
	 */
	virtual int Delete(const K& keyNode) = 0;

	/**
	 * 对索引记录节点加锁，此处不实现锁的超时释放等复杂机制
	 * @param K& keyNode: 索引的key
	 *
	 * @return  int : 0(成功) 其它失败
	 */
	virtual int LockKey(const K& keyNode) = 0;

	/**
	 * 对索引记录节点接锁,此处不实现锁的超时释放等复杂机制
	 * @param K& keyNode: 索引的key
	 *
	 * @return  int : 0(成功) 其它失败
	 */
	virtual int UnLockKey(const K& keyNode) = 0;

	/**
	 * 返回指向存储区头的迭代器
	 *
	 * @return IChunkedIndexIterator* 迭代器
	 */
	virtual iterator Begin() = 0;

	/**
	 * 返回指向存储区尾的迭代器
	 *
	 * @return IChunkedIndexIterator* 迭代器
	 */
	virtual iterator End() = 0;

	/**
	 * 设置数据操作对象管理的索引起始点
	 *
	 * @return int 非0为失败
	 */
	virtual int SetIndexPointer(char* pIndexPointer) = 0;
};

#endif /*PAIPAI_PROMOTE_CACHE_ICHUNKEDINDEX_H*/
#include "intf_chunkedindexIterator.h"
