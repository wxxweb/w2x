/*****************************************************************************
�ļ�����		exports.h
������		���� network ģ��ĵ��뵼����.
���ߣ�		wu.xiongxing					
���䣺		wxxweb@gmail.com
���ڣ�		2013-12-10
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