/******************************************************************************
�ļ�:	command.h
����:	�������࣬��ʹ�������з�ʽ�����ӽ��̣���ͨ�������ܵ���ȡ�ӽ��������������������
		�����ģ�ֻ�е��ӽ��������������߳�ʱ��ɱ���󣬺��� Execute() �Ż᷵�أ���ͨ����
		�ú��� GetOutput() ��ȡ�ӽ�����������
����:	wu.xiongxing
����:	wxxweb@gmail.com
ʱ��:	2013-01-21
�޸�:	2014-05-21
 ******************************************************************************/

#ifndef __W2X_COMMON_COMMAND_H__
#define __W2X_COMMON_COMMAND_H__

#include <WTypes.h>
#include "macros.h"
#include "string_util.h"


W2X_NAME_SPACE_BEGIN

class W2X_COMMON_API CCommand
{
public:
	enum ExecuteStatus {
		kSuccessed				= 0,	// �ӽ��̳ɹ�����
		kCreatePipeFailed		= 1,	// �����ܵ�ʧ��
		kCreateProcessFailed	= 2,	// �����ӽ���ʧ��
		kInvalidArg				= 3,	// ��Ч����
		kReadFailed				= 4,	// ���ܵ�ʧ��
		kTimeout				= 5,	// �ȴ��ӽ��̳�ʱ
	};

public:
	CCommand(void);
	virtual ~CCommand(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CCommand);

public:
	ExecuteStatus Execute(
		LPCTSTR _app_path,			// �����еĳ���·��
		LPCTSTR _app_args,			// �������в���
		DWORD _time_out = 5000,		// �������еĵȴ���ʱʱ�䣬��λ����
		bool _is_save_output = true	// �Ƿ񱣴����������
	);

	ExecuteStatus Execute(
		LPCTSTR _cmd_line,			// ��������·����������������
		DWORD _time_out = 5000,		// �������еĵȴ���ʱʱ�䣬��λ����
		bool _is_save_output = true	// �Ƿ񱣴����������
	);

	// ��ȡ�ӽ���������ַ�������
	int GetOutputLength(void) const;

	// ��ȡ�ӽ���������ַ������ɹ��򷵻�ָ�� str_buffer ���ַ���ָ��
	LPTSTR GetOutput(LPTSTR _str_buffer, size_t _size_in_bytes) const;

	TSTDSTR GetOutput(void) const;

private:
	class CImpl;
	CImpl* const m_impl_ptr;
};

W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_COMMAND_H__ */