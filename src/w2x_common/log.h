/*****************************************************************************
文件：log.h
描述：日志系统, 用于管理日志文件的生成，接收日志消息缓存到消息队列，打印日志等.
	 CLog类不可示例化，为单一实例, 可通过静态成员函数调用其功能。日志模块对象
	 在主程序启动时WinMain被调用之前创建并初始化，在主程序退出时，WinMain返回
	 后被释放，这些过程都是自动完成的。
作者：wu.xiongxing
邮箱：wxxweb@gmail.com
日期：2013-12-24
******************************************************************************/

#ifndef __W2X_COMMON_LOG_H__
#define __W2X_COMMON_LOG_H__

#include "exports.h"
#include <WTypes.h>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(log)

const size_t MAX_LOG_INFO = 1024; // 单条日志信息的缓冲区最大字符数

// 日志内容优先级
enum EPriority {
	kPriorityNormal,	// 正常
	kPriorityUrgent,	// 紧急
	kPriorityHigh,		// 高
	kPriorityLow,		// 低
};

// 日志内容分类
enum ECategory {
	kCategoryInfo,		// 信息, 一般日志信息, 不弹框提示
	kCategoryWarn,		// 警告, 在Debug模式下会弹框提示
	kCategoryError,		// 出错, 在Debug模式下会弹框提示
	kCategoryDebug,		// 调试, 在Debug模式下会弹框提示, Release模式会被过滤掉
};

typedef UINT DirId;

// 日志定制信息结构
struct Custom
{
	EPriority priority;
	ECategory category;
	DirId work_dir_id;
	bool is_immediately;// 是否立即打印日志，跳过日志排队
	bool is_resue_file;	// 是否复用刚创建的日志文件, 不再重现创建, 必须在打第一条日志前设置才有效
	Custom(DirId dir_id = 0) 
		: priority(kPriorityNormal)
		, category(kCategoryInfo)
		, work_dir_id(dir_id)
		, is_immediately(false)
		, is_resue_file(false) {}
};


// 设置全局日志定制信息, 未做特殊定制的日志均采用默认定制,
// 当custom_ref.szDirName值为空时, 使用默认目录名称“default”
W2X_COMMON_API bool SetGlobal(const Custom& _custom_ref);

// 获取当前的全局日志定制信息
W2X_COMMON_API void GetGlobal(Custom& _custom_ref);

// 重置全局日志信息为最初状态
W2X_COMMON_API void ResetGlobal(void);

// 获取日志根目录路径
W2X_COMMON_API bool GetRootDir(LPTSTR _dir_path, size_t _size_in_words);

// 添加一个日志工作目录, 该目录可用于存放特定的日志文件, 添加成功后返回目录ID
// ID值大于 0, 若值为 0 则表示添加失败使用默认工作目录"default"
W2X_COMMON_API DirId AddWorkDir(LPCTSTR _dir_name);

// 获取指定工作目录下正在使用的日志文件路径
W2X_COMMON_API bool GetLogFile(
	LPTSTR _file_path, 			   
	size_t _size_in_words, 			  
	DirId _work_dir_id
);

// 获取日志文件编码头部字节数
W2X_COMMON_API DWORD GetLogFileHeaderSize(void);

// 获取日志文件编码头部, 当缓存区尺寸太小, size_in_bytes被赋予日志文件编码头部字节数
W2X_COMMON_API bool GetLogFileHeader(
	PBYTE _byte_buffer, 
	IN OUT DWORD& _size_in_bytes
);

// 当 custom_ptr=NULL 时, 该条日志采用默认定制, 否则使用特殊定制
W2X_COMMON_API bool Log(
	const Custom* _custom_ptr, 
	LPCTSTR _format_str, 
	...
);

// 该函数为兼容 CFunctions::BIRSI_DBGMSG()
W2X_COMMON_API bool Log(
	const Custom* _custom_ptr, 
	LPCTSTR _format_str, 
	va_list& _arg_list_ref
);

// 以下四个函数对日志内容分类（ECategory）采用特殊定制
W2X_COMMON_API bool LogInfo(LPCTSTR _format_str, ...);
W2X_COMMON_API bool LogWarn(LPCTSTR _format_str, ...);
W2X_COMMON_API bool LogError(LPCTSTR _format_str, ...);
W2X_COMMON_API bool LogDebug(LPCTSTR _format_str, ...);


W2X_DEFINE_NAME_SPACE_END(log)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_LOG_H__ */