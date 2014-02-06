/*****************************************************************************
文件名：		exports.h
描述：		定义 network 模块的导入导出宏.
作者：		wu.xiongxing					
邮箱：		wxxweb@gmail.com
日期：		2013.12.10
******************************************************************************/

#ifndef __W2X_NETWORK_EXPORTS_H__
#define __W2X_NETWORK_EXPORTS_H__


#ifdef W2X_NETWORK_EXPORTS
#  ifndef  W2X_NETWORK_API
#    define W2X_NETWORK_API __declspec(dllexport)
#  endif
#else
#  ifndef W2X_NETWORK_API
#    define W2X_NETWORK_API __declspec(dllimport)
#  endif
#endif


/* 定义命名空间并标记其开始和结束位置 */
#ifndef W2X_DEFINE_NAME_SPACE_BEGIN
#  define W2X_DEFINE_NAME_SPACE_BEGIN(name) namespace name {
#endif
#ifndef W2X_DEFINE_NAME_SPACE_END
#  define W2X_DEFINE_NAME_SPACE_END(name) };
#endif


/* 定义 w2x 命名空间 */
#ifndef W2X_NAME_SPACE_BEGIN
#  define W2X_NAME_SPACE_BEGIN namespace w2x {
#endif
#ifndef W2X_NAME_SPACE_END
#  define W2X_NAME_SPACE_END };
#endif


#ifdef W2X_NETWORK_EXPORTS
	W2X_NAME_SPACE_BEGIN
	W2X_DEFINE_NAME_SPACE_BEGIN(network)
	W2X_DEFINE_NAME_SPACE_END(network)
	W2X_NAME_SPACE_END
	namespace internal = w2x::network;
#endif


#endif /* __W2X_NETWORK_EXPORTS_H__ */