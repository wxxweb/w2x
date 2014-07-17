/******************************************************************************
 * 文件:		base.h
 * 描述:		IBase 接口类定义了引用计数的实现方法。为了避免不小心导致的内存泄露等问题，凡
 *          框架类都有必要继承该接口。务必通过宏 W2X_IMPLEMENT_REFCOUNTING 在 IBase
 *			的子类中实现引用计数相关接口函数。应尽量避免手动调用 AddRef() 和 Release()
 *			来修改引用计数值，最好的办法是使用智能指针类 CRefPtr 来管理 IBase 子类对象
 *			的指针。不要直接操作 IBase 子类对象的指针，而是将指针赋给 CRefPtr 对象，通过
 *			CRefPtr 对象来间接操作。请参见 ref_ptr.h
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 创建:		2014-02-23
 * 修改:		2014-04-19
 ******************************************************************************/

#ifndef __W2X_COMMON_BASE_H__
#define __W2X_COMMON_BASE_H__

#include "macros.h"
#include "ref_count.h"
#include "ref_ptr.h"

W2X_NAME_SPACE_BEGIN


class W2X_COMMON_API IBase
{
public:
	IBase(void);

protected:
	/*
	 * 凡是继承了该接口的类对象，都无法直接通过 delete 来手动释放对象内存，使用
	 * delete 将会出现编译错误，想要释放对象内存请使用对象的 w2xRelease() 函数来实现。
	 */
	virtual ~IBase(void);

public:
	/**
	 * 使对象的引用计数值递增。
	 * @return 返回引用计数的结果值，该值应当只用于诊断或测试。
	 * @note 每当获取对象指针的新副本，都必须调用该函数。
	 */
	virtual int w2xAddRef(void) = 0;

	/**
	 * 使对象的引用计数值递减。
	 * @return 引用计数的结果值将被返回，该值应当只用于诊断或测试。
	 * @note 当对象的引用计数值降到 0 时，必须将对象从内存中释放掉。
	 */
	virtual int w2xRelease(void) = 0;

	/**
	 * @return 返回对象当前引用计数值。
	 */
	virtual int w2xGetRefCount(void) = 0;
};


W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_BASE_H__ */