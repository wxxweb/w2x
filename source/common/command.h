/******************************************************************************
 * �ļ���	command.h
 * �������������࣬��ʹ�������з�ʽ�����ӽ��̣���ͨ�������ܵ���ȡ�ӽ��������������������
 *      �����ģ�ֻ�е��ӽ��������������߳�ʱ��ɱ���󣬺��� Execute() �Ż᷵�أ���ͨ����
 *      �ú��� GetOutput() ��ȡ�ӽ�����������
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ʱ�䣺	2013-01-21
 ******************************************************************************/

#ifndef __W2X_COMMON_COMMAND_H__
#define __W2X_COMMON_COMMAND_H__

#include "exports.h"
#include "macros.h"
#include <WTypes.h>
#include <string>

W2X_NAME_SPACE_BEGIN

class W2X_COMMON_API CCommand
{
public:
	enum EExecuteStatus {
		ES_SUCCESSED			= 0,	// �ӽ��̳ɹ�����
		ES_CREATE_PIPE_FAILD	= 1,	// �����ܵ�ʧ��
		ES_CREATE_PROCESS		= 2,	// �����ӽ���ʧ��
		ES_INVALID_ARG			= 3,	// ��Ч����
		ES_READ_FAILD			= 4,	// ���ܵ�ʧ��
	};

public:
	CCommand(void);
	virtual ~CCommand(void);

W2X_DISALLOW_COPY_AND_ASSIGN(CCommand);

public:
	EExecuteStatus Execute(
		LPCTSTR _app_path,			// �����еĳ���·��
		LPCTSTR _app_args,			// �������в���
		DWORD _time_out = 5000,		// �������еĵȴ���ʱʱ�䣬��λ����
		bool _is_save_output = true	// �Ƿ񱣴����������
	);

	EExecuteStatus Execute(
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