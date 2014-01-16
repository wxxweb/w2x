/*****************************************************************************
文件名：		exports.h
描述：		定义 common 模块的导入导出宏.
作者：		wu.xiongxing					
邮箱：		wxxweb@gmail.com
日期：		2013.12.10
******************************************************************************/

#ifndef __W2X_COMMON_EXPORTS_H__
#define __W2X_COMMON_EXPORTS_H__


#ifdef W2X_COMMON_EXPORTS
#  ifndef  W2X_COMMON_API
#    define W2X_COMMON_API __declspec(dllexport)
#  endif
#else
#  ifndef W2X_COMMON_API
#    define W2X_COMMON_API __declspec(dllimport)
#  endif
#endif


/* 定义 w2x 命名空间 */
#ifndef W2X_NAMESPACE_BEGIN
#  define W2X_NAMESPACE_BEGIN namespace w2x {
#endif
#ifndef W2X_NAMESPACE_END
#  define W2X_NAMESPACE_END };
#endif


#endif /* __W2X_COMMON_EXPORTS_H__ */