#ifndef PAIPAI_PROMOTE_CACHE_ICHUNKEDINDEX_ITERATOR_H
#define PAIPAI_PROMOTE_CACHE_ICHUNKEDINDEX_ITERATOR_H

#include "intf_chunkedindex.h"

template<typename K, typename T>
class IChunkedIndexIterator {
public:
	virtual ~IChunkedIndexIterator() {};
	IChunkedIndexIterator(IChunkedIndex<K,T>* IChunkedIndex) {};
	IChunkedIndexIterator() {};
public:
   /**
	* 迭代器后移
	*
	* @return int 负数为出错
	*/
	virtual int Next()  = 0;

	/**
	 * 迭代器前移
	 *
	 * @return int 负数为出错
	 */
	virtual int Prev()  = 0;

	/**
	 * 获取迭代器指向的值，直接返回容器中存储的内容
	 * 类似于stl的迭代器，取值之前要注意容器中是否为空，否则可能得到无意义的值
	 *
	 * 此方法实现体可以考虑加断言，来控制遍历空容器不被调用
	 */
	virtual T& GetCurrentNode() = 0;

	/**
	 * 将容器中的内容拷贝至传入的node节点存储空间中
	 * 注意对于T为指针类型或原生基础类型，此方法与上一个方法相当于一样，多列一个只是照顾更多的习惯
	 * 当容器为空时，以错误码表示返回结果
	 *
	 * @return int 非0表示出错
	 */
	virtual int GetCurrentNode(T& node) = 0;

	/**
	 * 比较两个迭代器
	 * 当小于入参迭代器时，返回-1
	 * 大于入参迭代器时，返回1
	 * 当相等时，返回0
	 *
	 * @return 其余值表示失败
	 *
	 */
	virtual int Compare(IChunkedIndexIterator<K,T>* it) = 0;
};


#endif /*PAIPAI_PROMOTE_CACHE_ICHUNKEDINDEX_ITERATOR_H*/
