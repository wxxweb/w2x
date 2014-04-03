/******************************************************************************
 * �ļ�:		process.h
 * ����:		���ļ��ж����˽�����ص�һЩ���������绥�⡢ͬ����.
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-03-27
 ******************************************************************************/


#ifndef __W2X_COMMON_PROCESS_H__
#define __W2X_COMMON_PROCESS_H__

#include "exports.h"
#include "macros.h"

W2X_NAME_SPACE_BEGIN


/**
 * ��ʼ�����ڽ��̼�ͬ�������������壬��ʹ�� GUID ��Ϊ����������ɲ��֡�
 * �û����彨����ȫ�������ռ��У�������ʵ�ֳ���ֻ��������һ��ʵ����
 * ���Ե��ú��� UninitProcessMutex ��ǰ�ͷŸû����壬���ߵȵ������˳����Զ��ͷš�
 * ����Ѿ���һ�����̳��и����������巵�� false�����򷵻� true��
 */
W2X_COMMON_API bool InitProcessMutex(GUID _guid);

/** �ͷŵ��ú��� InitProcessMutex ������ȫ�����������塣 */
W2X_COMMON_API void UninitProcessMutex(void);

/** ��ȡ���ú��� InitProcessMutex ������ȫ����������������ơ� */
W2X_COMMON_API LPCTSTR GetProcessMutex(void);


W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_PROCESS_H__ */