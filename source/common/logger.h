/******************************************************************************
 * 文件：	logger.h
 * 描述：	日志记录器
 * 作者：	wu.xiongxing
 * 时间：	2013-05-25
 ******************************************************************************/

#ifndef W2X_COMMON_LOGGER_H_
#define W2X_COMMON_LOGGER_H_


#ifndef COMMON_API
#  ifdef COMMON_EXPORTS
#    define COMMON_API __declspec(dllexport)
#else
#    define COMMON_API __declspec(dllimport)
#  endif
#endif


/* 定义 w2x 命名空间 */
#ifndef W2X_NAMESPACE_BEGIN
#define W2X_NAMESPACE_BEGIN namespace w2x {
#endif

#ifndef W2X_NAMESPACE_END
#define W2X_NAMESPACE_END }
#endif

W2X_NAMESPACE_BEGIN

class CLogger
{
private:
	/* 用于自动初始化 */
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
	static COMMON_API void log(LPCTSTR msg);

private:
	static LPCTSTR sm_kSysLogDir;
	static CHelper sm_helper;
};

W2X_NAMESPACE_END

#endif /* W2X_COMMON_LOGGER_H_ */