/******************************************************************************
 * �ļ���	debug.h
 *
 * ������	����һ��
 *          ������ DEBUG ģʽ�¶�������Ч�ԡ����ڴ�й©�Ƚ��м�⣬�������ⱨ�档
 *		    �����ó����� Debug ģʽ�¼��й©��������棬�뽫��ͷ�ļ� include �� 
 *		    ϵͳͷ�ļ�֮���û�ͷ�ļ�֮ǰ�� ����� stdafx.h���ɷ�����֮��
 *		    ��Ҫ���ľ����ڳ�����ڴ����� w2x::debug::EnableLeakCheck()��
 *          ���Ե���w2x::debug::SetReportMode() ��������������Ŀ��λ�á�
 *
 *          ���ܶ���
 *          �ڳ�������쳣ʱ���в������� dump �ļ�������ϱ���ʱ���ɵ� pdb �ļ�
 *			�Գ�����е�������λ�����㡣ͬʱ��֧���û����ûص����������ڻص�������
 *          �����Ѻ���ʾ��������ļ�����Ϣ�ϴ��������������ڴ�����Ҫʹ�ô˹��ܣ�
 *          ���ڳ�����ڵ���� w2x::debug::EnableExcptionHandle()
 *
 * ���䣺	wxxweb@gmail.com
 * ���ߣ�	wu.xiongxing
 * ʱ�䣺	2013-05-24
 ******************************************************************************/

#ifndef __W2X_COMMON_DEBUG_H__
#define __W2X_COMMON_DEBUG_H__

#include "exports.h"


// ���� DEBUG ģʽ�¶��ڴ�й©���
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <crtdbg.h>


/* �� new �滻�ɴ��ļ������кŲ����� new */
#ifdef _DEBUG
#  ifndef DEBUG_NEW 
#    define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#    define new DEBUG_NEW
#  else // ���ڼ���ڴ�й©�� new �������� MFC ����Ӧ�Ķ���
#    pragma message("DEBUG_NEW is defined in MFC.")
#  endif /* DEBUG_NEW */
#endif /* _DEBUG */


/*
 * ģ�� MFC �Ķ��ԣ������ʽΪ��ʱ���������ļ������кźͱ��ʽ����ʾ��
 */
#ifndef ASSERT
#  ifdef _DEBUG
#    define ASSERT(expression) (void) \
	   ( (!!(expression)) || \
	     (_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, #expression)) && \
		 ([](){__asm int 3}(), 0) \
       )
#  else
#    define ASSERT(expression) ((void)0)
#  endif
#endif /* ASSERT */


/*
 * ģ�� MFC �� TRACE
 */
#ifndef TRACE
#  ifdef _DEBUG
#    define TRACE(format,...) \
       _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, NULL, format, __VA_ARGS__)
#  else
#    define TRACE(format) ((void)0)
#  endif
#endif

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(debug)

/*
* Debug ���Ա���ģʽ
*/
enum EReportMode {
	kModeDebug,		// ���� Debug �Ի���
	kModeStderr,	// �ض��򵽱�׼�������
};

/*
* �쳣����������ֵ
*/
enum EExcptionReturn {
	kPassToNextHandler,	// ���쳣���ݸ���һ��������
	kEndProcess,		// ������������
};

/*
* �����쳣��Ϣ�ṹ
*/
struct ExcptionInfo {
	LPCTSTR module_name;
	PVOID excption_address;
	DWORD excption_code;
	DWORD excption_flags;
};

/*
* �쳣�������������ͣ���������ֵ�� EExcptionReturn
*/
typedef EExcptionReturn (CALLBACK* FExcptionHandler)(const ExcptionInfo&);

/*
* ���������ⱨ���Ŀ�꣺VS �� Debug ������ڣ������Ǳ�׼������� stderr
*/
W2X_COMMON_API void SetReportMode(EReportMode mode);

/*
* �ú������ڳ�����ڴ������ڿ���ջ�ڴ�й©��⣬�������ڴ�й©��������ڴ�й©
* �������Ϣ�����£�
* 
*	1.���з���й©���ڴ���ļ�����
*	2.�ļ��ڵ��кţ��������У���
*	3.�ڴ�����ţ��ڴ������ڣ���
*	4.�����ͣ���ͨ���ͻ��˻� CRT����
*	5.ʮ��������ʽ���ڴ�λ�ã�
*	6.���ֽ�Ϊ��λ�Ŀ��С��
*	7.ǰ 16 �ֽڵ����ݣ���Ϊʮ�����ƣ���
*/
W2X_COMMON_API void EnableLeakCheck(bool delay = false);


/*
* �ú������ڳ�����ڴ������ڿ������̶����쳣�����������쳣ʱ���������쳣���������������
* δ�����Զ����쳣���������� _handler_ptr����ʹ��Ĭ���쳣���������������򱨸��쳣��Ϣ��
* �쳣�����Ϣ���£�
* 
*	1.�����쳣��ģ��ȫ·����
*	2.�쳣��ţ�
*	3.�쳣��ַ��
*	4.�쳣��ʶ��
*/
W2X_COMMON_API void EnableExcptionHandle(FExcptionHandler _handler_ptr = NULL);


W2X_DEFINE_NAME_SPACE_END(debug)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_DEBUG_H__ */