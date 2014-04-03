/*****************************************************************************
�ļ���log.h
��������־ϵͳ, ���ڹ�����־�ļ������ɣ�������־��Ϣ���浽��Ϣ���У���ӡ��־��.
	 CLog�಻��ʾ������Ϊ��һʵ��, ��ͨ����̬��Ա���������书�ܡ���־ģ�����
	 ������������ʱWinMain������֮ǰ��������ʼ�������������˳�ʱ��WinMain����
	 ���ͷţ���Щ���̶����Զ���ɵġ�
���ߣ�wu.xiongxing
���䣺wxxweb@gmail.com
���ڣ�2013-12-24
******************************************************************************/

#ifndef __W2X_COMMON_LOG_H__
#define __W2X_COMMON_LOG_H__

#include "exports.h"
#include <WTypes.h>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(log)

const size_t MAX_LOG_INFO = 1024; // ������־��Ϣ�Ļ���������ַ���

// ��־���ݷ���
enum ECategory {
	kCategoryInfo,		// ��Ϣ, һ����־��Ϣ, ��������ʾ
	kCategoryWarn,		// ����, ��Debugģʽ�»ᵯ����ʾ
	kCategoryError,		// ����, ��Debugģʽ�»ᵯ����ʾ
	kCategoryDebug,		// ����, ��Debugģʽ�»ᵯ����ʾ, Releaseģʽ�ᱻ���˵�
};

typedef UINT DirId;

// ��־������Ϣ�ṹ
struct Custom
{
	ECategory category;
	DirId work_dir_id;
	bool is_immediately;// �Ƿ�������ӡ��־��������־�Ŷ�
	bool is_resue_file;	// �Ƿ��øմ�������־�ļ�, �������ִ���, �����ڴ��һ����־ǰ���ò���Ч
	Custom(DirId dir_id = 0) 
		: category(kCategoryInfo)
		, work_dir_id(dir_id)
		, is_immediately(false)
		, is_resue_file(false) {}
};


// ����ȫ����־������Ϣ, δ�����ⶨ�Ƶ���־������Ĭ�϶���,
// ��custom_ref.szDirNameֵΪ��ʱ, ʹ��Ĭ��Ŀ¼���ơ�default��
W2X_COMMON_API bool SetGlobal(const Custom& _custom_ref);

// ��ȡ��ǰ��ȫ����־������Ϣ
W2X_COMMON_API void GetGlobal(Custom& _custom_ref);

// ����ȫ����־��ϢΪ���״̬
W2X_COMMON_API void ResetGlobal(void);

// ��ȡ��־��Ŀ¼·��
W2X_COMMON_API bool GetRootDir(LPTSTR _dir_path, size_t _size_in_words);

// ���һ����־����Ŀ¼, ��Ŀ¼�����ڴ���ض�����־�ļ�, ��ӳɹ��󷵻�Ŀ¼ID
// IDֵ���� 0, ��ֵΪ 0 ���ʾ���ʧ��ʹ��Ĭ�Ϲ���Ŀ¼"default"
W2X_COMMON_API DirId AddWorkDir(LPCTSTR _dir_name);

// ��ȡָ������Ŀ¼������ʹ�õ���־�ļ�·��
W2X_COMMON_API bool GetLogFile(
	LPTSTR _file_path, 			   
	size_t _size_in_words, 			  
	DirId _work_dir_id
);

// ��ȡ��־�ļ�����ͷ���ֽ���
W2X_COMMON_API DWORD GetLogFileHeaderSize(void);

// ��ȡ��־�ļ�����ͷ��, ���������ߴ�̫С, size_in_bytes��������־�ļ�����ͷ���ֽ���
W2X_COMMON_API bool GetLogFileHeader(
	PBYTE _byte_buffer, 
	IN OUT DWORD& _size_in_bytes
);

// �� custom_ptr=NULL ʱ, ������־����Ĭ�϶���, ����ʹ�����ⶨ��
W2X_COMMON_API bool Log(
	const Custom* _custom_ptr, 
	LPCTSTR _format_str, 
	...
);

// �ú���Ϊ���� CFunctions::BIRSI_DBGMSG()
W2X_COMMON_API bool Log(
	const Custom* _custom_ptr, 
	LPCTSTR _format_str, 
	va_list& _arg_list_ref
);

// �����ĸ���������־���ݷ��ࣨECategory���������ⶨ��
W2X_COMMON_API bool LogInfo(LPCTSTR _format_str, ...);
W2X_COMMON_API bool LogWarn(LPCTSTR _format_str, ...);
W2X_COMMON_API bool LogError(LPCTSTR _format_str, ...);
W2X_COMMON_API bool LogDebug(LPCTSTR _format_str, ...);


W2X_DEFINE_NAME_SPACE_END(log)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_LOG_H__ */