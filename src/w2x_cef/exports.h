/*****************************************************************************
�ļ�����		exports.h
������		���� cef ģ��ĵ��뵼����.
���ߣ�		wu.xiongxing					
���䣺		wxxweb@gmail.com
���ڣ�		2014-05-21
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