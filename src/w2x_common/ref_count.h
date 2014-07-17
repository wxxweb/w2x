/******************************************************************************
 * 文件:		ref_count.h
 * 描述:		类 CRefCount 实现了原子级别的引用计数。
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 创建:		2014-02-23
 * 修改:		2014-05-21
 ******************************************************************************/

#ifndef __W2X_COMMON_REF_COUNT_H__
#define __W2X_COMMON_REF_COUNT_H__

#include "macros.h"

W2X_NAME_SPACE_BEGIN


class W2X_COMMON_API CRefCount
{
public:
	CRefCount() : m_ref_count(0) {}

	/** 原子级别的引用计数值递增。*/
	int w2xAddRef() {
		return InterlockedIncrement(&m_ref_count);
	}

	/** 原子级别的引用计数值递减。当不再被引用时删除对象。*/
	int w2xRelease() {
		return InterlockedDecrement(&m_ref_count);
	}

	/** 返回当前的引用计数值。*/
	int w2xGetRefCount() { 
		return m_ref_count; 
	}

private:
	long m_ref_count;
};


/**
 * 该宏为继承 IBase 的类提供引用计数的代码实现。
 */
#define W2X_IMPLEMENT_REFCOUNTING(class_name)                   \
public:                                                         \
    /** 谨慎调用该函数，尽量使用智能指针 CRefPtr 的 attach() 替代 */  \
	int w2xAddRef(void) {                                       \
        return m_ref_count.w2xAddRef();                         \
     }                                                          \
	/** 谨慎调用该函数，尽量使用智能指针 CRefPtr 的 detach() 替代 */  \
	int w2xRelease(void) {                                      \
		int retval = m_ref_count.w2xRelease();                  \
		if (0 == retval) { delete this; }                       \
		return retval;                                          \
	}                                                           \
	int w2xGetRefCount(void) {                                  \
        return m_ref_count.w2xGetRefCount();                    \
    }                                                           \
private:                                                        \
    w2x::CRefCount m_ref_count;


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_REF_COUNT_H__ */