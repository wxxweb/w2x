/*****************************************************************************
文件名：		exports.h
描述：		定义 cef 模块的导入导出宏.
作者：		wu.xiongxing					
邮箱：		wxxweb@gmail.com
日期：		2014-05-21
******************************************************************************/

#ifndef __W2X_CEF_EXPORTS_H__
#define __W2X_CEF_EXPORTS_H__


#ifdef W2X_CEF_EXPORTS
#  ifndef  W2X_CEF_API
#    define W2X_CEF_API __declspec(dllexport)
#  endif
#else
#  ifndef W2X_CEF_API
#    define W2X_CEF_API __declspec(dllimport)
#  endif
#endif


#endif ///< __W2X_CEF_EXPORTS_H__