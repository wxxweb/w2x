/*****************************************************************************
文件名：		exports.h
描述：		定义 network 模块的导入导出宏.
作者：		wu.xiongxing					
邮箱：		wxxweb@gmail.com
日期：		2013-12-10
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


#endif /* __W2X_NETWORK_EXPORTS_H__ */