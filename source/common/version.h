/******************************************************************************
 * 文件：	version.h
 * 描述：	定义与版本相关的函数、常量等，如操作系统版本、浏览器版本等。
 * 作者：	wu.xiongxing
 * 邮箱：	wxxweb@gmail.com
 * 时间：	2014-01-23
 ******************************************************************************/

#ifndef __W2X_COMMON_VERSION_H__
#define __W2X_COMMON_VERSION_H__

#include "exports.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(version)


// 操作系统版本代号
// 常量由操作系统主版本号、次版本号和 Service Package 的主版本号、次版本号组成。
// 例如 OS_VER_WIN_XP_SP3 = 0x05010300 表示版本代号 “Windows XP”，
// 操作系统主版本号 05、次版本号 01，Service Package 主版本号 03 次版本号 00。
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

// 保存文件版本信息
struct ModuleVersionInfo {
	WORD major;						// 主版本号
	WORD minor;						// 次版本号
	WORD revision;					// 修正版本号
	WORD build;						// 编译版本号
	WORD language_id;				// 语言编号
	WORD code_page;					// 语言代码页
	TCHAR language_name[32];		// 语言名称
	TCHAR company_name[64];			// 公司名称
	TCHAR file_description[128];	// 文件描述
	TCHAR internal_name[32];		// 内部名称
	TCHAR legal_copyright[64];		// 版权信息
	TCHAR original_file_name[32];	// 原文件名
	TCHAR product_name[32];			// 产品名称
};

// 获取操作系统版本代号，获取成功返回 true，
// 失败返回 false 且两个输出参数值均为 0
W2X_COMMON_API EOsVersion GetOsVersion(void);


// 判断操作系统是否为 64 位
// WoW64 (Windows 32-bit on Windows 64-bit) is a subsystem of the Windows
// operating system capable of running 32-bit applications and is included
// on all 64-bit versions of Windows—including Windows XP Professional x64
// Edition, IA-64 and x64 versions of Windows Server 2003, as well as 64-bit
// versions of Windows Vista, Windows Server 2008, Windows 7 and Windows 8.
// In Windows Server 2008 R2 Server Core, it is an optional component.
W2X_COMMON_API bool IsWow64(void);


// 获取某个模块的版本信息
W2X_COMMON_API bool GetModuleVersionInfo(
	OUT ModuleVersionInfo& _module_ver_info_ref,
	LPCTSTR _module_path,
	size_t _language_index = 0
);


W2X_DEFINE_NAME_SPACE_END(version)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_VERSION_H__ */