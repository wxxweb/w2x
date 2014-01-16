/******************************************************************************
 * �ļ���	logger.h
 * ������	��־��¼��
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2013-05-25
 ******************************************************************************/

#ifndef W2X_COMMON_LOGGER_H_
#define W2X_COMMON_LOGGER_H_

#include "exports.h"
#include "thread.h"

W2X_NAMESPACE_BEGIN

class CLogger
{
private:
	/* �����Զ���ʼ�� */
	class CHelper
	{
	public:
		CHelper(void);
		~CHelper(void);
	};

private:
	CLogger(void);
	~CLogger(void);

public:
	static W2X_COMMON_API void log(LPCTSTR msg);

private:
	static LPCTSTR sm_kSysLogDir;
	static CHelper sm_helper;

W2X_IMPLEMENT_LOCKING(CLogger, CAutoLock)
};

W2X_NAMESPACE_END

#endif /* W2X_COMMON_LOGGER_H_ */