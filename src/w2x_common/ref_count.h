/******************************************************************************
 * 文件：	ref_count.h
 * 描述：	类 CRefCount 实现了原子级别的引用计数。
 * 作者：	wu.xiongxing					
 * 邮箱：	wxxweb@gmail.com
 * 日期：	2014-02-23
*******************************************************************************/

#ifndef __W2X_COMMON_REF_COUNT_H__
#define __W2X_COMMON_REF_COUNT_H__

#include "mutex.h"


W2X_NAME_SPACE_BEGIN


class W2X_COMMON_API CRefCount 
{
public:
	CRefCount() : m_ref_count(0) {}

	/*
	 * 原子级别的引用计数值递增。
	 */
	int w2xAddRef() {
		return W2X_ATOMIC_INCREMENT(&m_ref_count);
	}

	/*
	 * 原子级别的引用计数值递减。当不再被引用时删除对象。
	 */
	int w2xRelease() {
		return W2X_ATOMIC_DECREMENT(&m_ref_count);
	}

	/*
	 * 返回当前的引用计数值。
	 */
	int w2xGetRefCount() { 
		return m_ref_count; 
	}

private:
	long m_ref_count;
};


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_REF_COUNT_H__ */