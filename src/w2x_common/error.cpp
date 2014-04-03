/******************************************************************************
 * �ļ�:		error.cpp
 * ����:		�μ� error.h
 * ����:		wu.xiongxing					
 * ����:		wxxweb@gmail.com
 * ����:		2014-04-01
 ******************************************************************************/

#include "stdafx.h"
#include "error.h"



W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(debug)



W2X_COMMON_API size_t FormatError(
LPTSTR _str_buffer,
size_t _size_in_words,
DWORD _error_code
)
{
	if (NULL == _str_buffer)
	{
		ASSERT(NULL != _str_buffer);
		return 0;
	}
	memset(_str_buffer, 0, _size_in_words * sizeof(TCHAR));

	if (1 >= _size_in_words)
	{
		ASSERT(1 < _size_in_words);
		return 0;
	}

	PVOID msg_buf_ptr = NULL;

	size_t chars_stored = FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER |			// �Զ����仺��
		FORMAT_MESSAGE_FROM_SYSTEM |				// ϵͳ��Ϣ
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		_error_code,								// �������
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	// Ĭ������
		reinterpret_cast<LPTSTR>(&msg_buf_ptr), _size_in_words, NULL);

	size_t max_words = _size_in_words - 1;
	max_words = (chars_stored >= max_words) ? max_words : chars_stored;

	_tcsncpy_s(_str_buffer, _size_in_words,  
		reinterpret_cast<LPTSTR>(msg_buf_ptr), max_words);

	::LocalFree(msg_buf_ptr);	// �ͷ��ڴ�
	msg_buf_ptr = NULL;

	return max_words;
}


W2X_DEFINE_NAME_SPACE_END(debug)
W2X_NAME_SPACE_END
