/******************************************************************************
 * �ļ���	version.h
 * ������	������汾��صĺ����������ȣ������ϵͳ�汾��������汾�ȡ�
 * ���ߣ�	wu.xiongxing
 * ���䣺	wxxweb@gmail.com
 * ʱ�䣺	2014-01-23
 ******************************************************************************/

#ifndef __W2X_COMMON_VERSION_H__
#define __W2X_COMMON_VERSION_H__

#include "exports.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(version)


// ����ϵͳ�汾����
// �����ɲ���ϵͳ���汾�š��ΰ汾�ź� Service Package �����汾�š��ΰ汾����ɡ�
// ���� OS_VER_WIN_XP_SP3 = 0x05010300 ��ʾ�汾���� ��Windows XP����
// ����ϵͳ���汾�� 05���ΰ汾�� 01��Service Package ���汾�� 03 �ΰ汾�� 00��
enum EOsVersion {
	OS_VER_EARLIER					= 0x00000000,
	OS_VER_LASTER					= 0x09999999,
	OS_VER_WIN_2000					= 0x05000000,
	OS_VER_WIN_2000_SP1				= 0x05000100,
	OS_VER_WIN_2000_SP2				= 0x05000200,
	OS_VER_WIN_2000_SP3				= 0x05000300,
	OS_VER_WIN_2000_SP4				= 0x05000400,
	OS_VER_WIN_XP					= 0x05010000,
	OS_VER_WIN_XP_SP1				= 0x05010100,
	OS_VER_WIN_XP_SP2				= 0x05010200,
	OS_VER_WIN_XP_SP3				= 0x05010300,
	OS_VER_WIN_XP_SP4				= 0x05010400,
	OS_VER_WIN_SERVER_2003			= 0x05020000,
	OS_VER_WIN_SERVER_2003_SP1		= 0x05020100,
	OS_VER_WIN_SERVER_2003_SP2		= 0x05020200,
	OS_VER_WIN_SERVER_2003_SP3		= 0x05020300,
	OS_VER_WIN_SERVER_2003_SP4		= 0x05020400,
	OS_VER_WIN_VISTA				= 0x06000000,
	OS_VER_WIN_VISTA_SP1			= 0x06000100,
	OS_VER_WIN_VISTA_SP2			= 0x06000200,
	OS_VER_WIN_VISTA_SP3			= 0x06000300,
	OS_VER_WIN_VISTA_SP4			= 0x06000400,
	OS_VER_WIN_SERVER_2008			= 0x06000000,
	OS_VER_WIN_SERVER_2008_SP1		= 0x06000100,
	OS_VER_WIN_SERVER_2008_SP2		= 0x06000200,
	OS_VER_WIN_SERVER_2008_SP3		= 0x06000300,
	OS_VER_WIN_SERVER_2008_SP4		= 0x06000400,
	OS_VER_WIN_SERVER_2008_R2		= 0x06010000,
	OS_VER_WIN_7					= 0x06010000,
	OS_VER_WIN_7_SP1				= 0x06010100,
	OS_VER_WIN_7_SP2				= 0x06010200,
	OS_VER_WIN_7_SP3				= 0x06010300,
	OS_VER_WIN_7_SP4				= 0x06010400,
	OS_VER_WIN_8					= 0x06020000,
	OS_VER_WIN_8_SP1				= 0x06020100,
	OS_VER_WIN_8_SP2				= 0x06020200,
	OS_VER_WIN_8_SP3				= 0x06020300,
	OS_VER_WIN_8_SP4				= 0x06020400,
};

// �����ļ��汾��Ϣ
struct ModuleVersionInfo {
	WORD major;						// ���汾��
	WORD minor;						// �ΰ汾��
	WORD revision;					// �����汾��
	WORD build;						// ����汾��
	WORD language_id;				// ���Ա��
	WORD code_page;					// ���Դ���ҳ
	TCHAR language_name[32];		// ��������
	TCHAR company_name[64];			// ��˾����
	TCHAR file_description[128];	// �ļ�����
	TCHAR internal_name[32];		// �ڲ�����
	TCHAR legal_copyright[64];		// ��Ȩ��Ϣ
	TCHAR original_file_name[32];	// ԭ�ļ���
	TCHAR product_name[32];			// ��Ʒ����
};

// ��ȡ����ϵͳ�汾���ţ���ȡ�ɹ����� true��
// ʧ�ܷ��� false �������������ֵ��Ϊ 0
W2X_COMMON_API EOsVersion GetOsVersion(void);


// �жϲ���ϵͳ�Ƿ�Ϊ 64 λ
// WoW64 (Windows 32-bit on Windows 64-bit) is a subsystem of the Windows
// operating system capable of running 32-bit applications and is included
// on all 64-bit versions of Windows��including Windows XP Professional x64
// Edition, IA-64 and x64 versions of Windows Server 2003, as well as 64-bit
// versions of Windows Vista, Windows Server 2008, Windows 7 and Windows 8.
// In Windows Server 2008 R2 Server Core, it is an optional component.
W2X_COMMON_API bool IsWow64(void);


// ��ȡĳ��ģ��İ汾��Ϣ
W2X_COMMON_API bool GetModuleVersionInfo(
	OUT ModuleVersionInfo& _module_ver_info_ref,
	LPCTSTR _module_path,
	size_t _language_index = 0
);


W2X_DEFINE_NAME_SPACE_END(version)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_VERSION_H__ */