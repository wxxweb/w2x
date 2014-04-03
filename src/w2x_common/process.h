/******************************************************************************
 * 文件:		process.h
 * 描述:		该文件中定义了进程相关的一些帮助器，如互斥、同步等.
 * 作者:		wu.xiongxing					
 * 邮箱:		wxxweb@gmail.com
 * 日期:		2014-03-27
 ******************************************************************************/


#ifndef __W2X_COMMON_PROCESS_H__
#define __W2X_COMMON_PROCESS_H__

#include "exports.h"
#include "macros.h"

W2X_NAME_SPACE_BEGIN


/**
 * 初始化用于进程间同步的命名互斥体，它使用 GUID 做为其命名的组成部分。
 * 该互斥体建立在全局命名空间中，可用于实现程序只允许运行一个实例。
 * 可以调用函数 UninitProcessMutex 提前释放该互斥体，或者等到程序退出后自动释放。
 * 如果已经有一个进程持有该命名互斥体返回 false，否则返回 true。
 */
W2X_COMMON_API bool InitProcessMutex(GUID _guid);

/** 释放调用函数 InitProcessMutex 创建的全局命名互斥体。 */
W2X_COMMON_API void UninitProcessMutex(void);

/** 获取调用函数 InitProcessMutex 创建的全局命名互斥体的名称。 */
W2X_COMMON_API LPCTSTR GetProcessMutex(void);


W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_PROCESS_H__ */