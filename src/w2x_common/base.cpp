/******************************************************************************
文件:	base.cpp
描述:	参见 base.h
作者:	wu.xiongxing					
邮箱:	wxxweb@gmail.com
创建:	2014-02-23
修改:	2014-05-21
 ******************************************************************************/

#include "stdafx.h"
#include "base.h"
#include "mutex.h"
#include "log.h"


W2X_NAME_SPACE_BEGIN


class CBasePtrManager
{
	enum {
		GC_INTERVAL_TIME = 60000,	// 两次垃圾回收的时间间隔
	};

	typedef std::set<IBase*> BasePtrSet;

protected:
	CBasePtrManager(void);
	~CBasePtrManager(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CBasePtrManager)
W2X_IMPLEMENT_LOCKING(CBasePtrManager, CAutoLock)

public:
	static CBasePtrManager* GetInstance(void)
	{
		static CBasePtrManager* s_instance_ptr = NULL;
		if (NULL == s_instance_ptr)
		{
			CAutoLock class_lock(NULL);
			if (NULL == s_instance_ptr)
			{
				s_instance_ptr = new CBasePtrManager();
			}
		}
		return s_instance_ptr;
	}

	inline bool Add(IBase* ptr) 
	{
		IF_NULL_RETURN_VALUE(ptr, false);
		CAutoLock this_lock(this);
		m_ptr_set.insert(ptr);
		return true;
	}

	inline bool Remove(IBase* ptr)
	{
		IF_NULL_RETURN_VALUE(ptr, false);
		CAutoLock this_lock(this);
		m_ptr_set.erase(ptr);
		return true;
	}

private:
	BasePtrSet	m_ptr_set;
};

W2X_IMPLEMENT_LOCKING_CLASS(CBasePtrManager, CAutoLock)


CBasePtrManager::CBasePtrManager(void)
{
}

CBasePtrManager::~CBasePtrManager(void) 
{
}

IBase::IBase(void)
{
	//CBasePtrManager::GetInstance()->Add(this);
}


IBase::~IBase(void)
{
	//CBasePtrManager::GetInstance()->Remove(this);
}


W2X_NAME_SPACE_END