/******************************************************************************
�ļ�:	error.h
����:	�Զ��������룬���ṩ�Դ��������и�ʽ�������
����:	wu.xiongxing					
����:	wxxweb@gmail.com
����:	2014-04-01
�޸�:	2014-05-21
*******************************************************************************/

#ifndef __W2X_COMMON_ERROR_H__
#define __W2X_COMMON_ERROR_H__

#include "macros.h"


W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(debug)


/* ��ϵͳ�����ĳ������ _error_code ��ʽ���ɳ���������Ϣ��
 * �������ַ������� _msg_buffer �У��ɹ��򷵻ر��������
 * ��Ϣ���ַ������������� NULL�������򷵻� 0��
 */
W2X_COMMON_API size_t FormatError(
	LPTSTR _msg_buffer,
	size_t _size_in_words,
	DWORD _error_code
);


W2X_DEFINE_NAME_SPACE_END(debug)
W2X_NAME_SPACE_END


#endif /* __W2X_COMMON_ERROR_H__ */