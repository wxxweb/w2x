/******************************************************************************
�ļ�:	encode.h
����:	�����������صĺ����������ȣ����ַ�����ת��������������ת��ȡ�
����:	wu.xiongxing
����:	wxxweb@gmail.com
ʱ��:	2014-01-23
�޸�:	2014-05-21
*******************************************************************************/

#ifndef __W2X_COMMON_ENCODE_H__
#define __W2X_COMMON_ENCODE_H__

#include "macros.h"
#include <WTypes.h>
#include <string>
#include <memory>

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(encode)


enum EncodeType {
	kAscii,
	kUnicode,
	kUtf8,
};


/** �����Ŀ���һ���ַ�����ջ�У�������� NULL�����ش��������Ŀ��ַ��� */
W2X_COMMON_API LPTSTR AllocString(LPCTSTR _t_str);
W2X_COMMON_API LPSTR AllocStringA(LPCSTR _a_str);
W2X_COMMON_API LPTSTR AllocStringW(LPCWSTR _w_str);

#define W2X_ALLOC_STR_A(s)	w2x::encode::AllocStringA(s)
#define W2X_ALLOC_STR_W(s)	w2x::encode::AllocStringW(s)

#ifdef UNICODE
#  define W2X_ALLOC_STR(s)	w2x::encode::AllocStringW(s)
#else
#  define W2X_ALLOC_STR(s)	w2x::encode::AllocStringA(s)
#endif

/** �ͷ��� AllocStringXXX ����Ķ��ֽ��ַ���ջ�ռ� */
W2X_COMMON_API bool FreeStringA(LPSTR* _a_pptr);
W2X_COMMON_API bool FreeStringA(LPCSTR* _a_pptr);

/** �ͷ��� AllocStringXXX ����Ŀ��ֽ��ַ���ջ�ռ� */
W2X_COMMON_API bool FreeStringW(LPWSTR* _w_pptr);
W2X_COMMON_API bool FreeStringW(LPCWSTR* _w_pptr);

#define W2X_FREE_STR_A(p)	w2x::encode::FreeStringA(&(p))
#define W2X_FREE_STR_W(p)	w2x::encode::FreeStringW(&(p))

#ifdef UNICODE
#  define W2X_FREE_STR(p)	w2x::encode::FreeStringW(&(p))
#else
#  define W2X_FREE_STR(p)	w2x::encode::FreeStringA(&(p))
#endif


/** �� ASCII �ַ�����ת��Ϊ UNICODE �ַ������� */
W2X_COMMON_API LPWSTR AllocStringA2W(
	LPCSTR _a_str,
	int _mbs_bytes = -1
);

#define W2X_ALLOC_STR_A2W		w2x::encode::AllocStringA2W

#ifdef UNICODE
#  define W2X_ALLOC_STR_A2T		w2x::encode::AllocStringA2W
#else
#  define W2X_ALLOC_STR_A2T(s)	(s)
#endif

#ifdef UNICODE
#  define W2X_ALLOC_STR_T2W(s)	(s)
#else
#  define W2X_ALLOC_STR_T2W(s)	w2x::encode::AllocStringA2W(s)
#endif


__forceinline std::wstring _W2X_A2W(LPCSTR pszAscii)
{
	LPWSTR pszWide = w2x::encode::AllocStringA2W(pszAscii);
	std::wstring strWide = pszWide;
	w2x::encode::FreeStringW(&pszWide);
	return strWide;
}

#define W2X_A2W(s)		w2x::encode::_W2X_A2W(s)

#ifdef UNICODE
#  define W2X_A2T(s)	W2X_A2W(s)
#else
#  define W2X_A2T(s)	(s)
#endif

#ifdef UNICODE
#  define W2X_T2W(s)	(s)
#else
#  define W2X_T2W(s)	W2X_A2W(s)
#endif


/** �� UNICODE �ַ�����ת��Ϊ ASCII �ַ������� */
W2X_COMMON_API LPSTR AllocStringW2A(
	LPCWSTR _w_str,
	int _wbs_bytes = -1
);

#define W2X_ALLOC_STR_W2A		w2x::encode::AllocStringW2A

#ifdef UNICODE
#  define W2X_ALLOC_STR_T2A		w2x::encode::AllocStringW2A
#else
#  define W2X_ALLOC_STR_T2A(s)	(s)
#endif

__forceinline std::string _W2X_W2A(LPCWSTR pszWide)
{
	LPSTR pszAscii = w2x::encode::AllocStringW2A(pszWide);
	std::string strAscii = pszAscii;
	w2x::encode::FreeStringA(&pszAscii);
	return strAscii;
}

#define W2X_W2A(s)		w2x::encode::_W2X_W2A(s)

#ifdef UNICODE
#  define W2X_T2A(s)	W2X_W2A(s)
#else
#  define W2X_T2A(s)	(s)
#endif

#ifdef UNICODE
#  define W2X_W2T(s)	(s)
#else
#  define W2X_W2T(s)	W2X_W2A(s)
#endif


/** �� UTF-8 �ַ�����ת��Ϊ ASCII �ַ������� */
W2X_COMMON_API LPSTR AllocStringUTF2A(
	LPCSTR _utf8_str,
	int _mbs_bytes = -1
);

/** �� UTF-8 �ַ�����ת��Ϊ UNICODE �ַ������� */
W2X_COMMON_API LPWSTR AllocStringUTF2W(
	LPCSTR _utf8_str,
	int _mbs_bytes = -1
);

#define W2X_ALLOC_STR_UTF2A		w2x::encode::AllocStringUTF2A
#define W2X_ALLOC_STR_UTF2W		w2x::encode::AllocStringUTF2W

#ifdef UNICODE
#  define W2X_ALLOC_STR_UTF2T	w2x::encode::AllocStringUTF2W
#else
#  define W2X_ALLOC_STR_UTF2T	w2x::encode::AllocStringUTF2A
#endif

__forceinline std::string _W2X_UTF2A(LPCSTR pszUtf8)
{
	LPSTR pszAscii = w2x::encode::AllocStringUTF2A(pszUtf8);
	std::string strAscii = pszAscii;
	w2x::encode::FreeStringA(&pszAscii);
	return strAscii;
}

#define W2X_UTF2A(s)	w2x::encode::_W2X_UTF2A(s)

__forceinline std::wstring _W2X_UTF2W(LPCSTR pszUtf8)
{
	LPWSTR pszWide = w2x::encode::AllocStringUTF2W(pszUtf8);
	std::wstring strWide = pszWide;
	w2x::encode::FreeStringW(&pszWide);
	return strWide;
}

#define W2X_UTF2W(s)	w2x::encode::_W2X_UTF2W(s)

#ifdef UNICODE
#  define W2X_UTF2T(s)	W2X_UTF2W(s)
#else
#  define W2X_UTF2T(s)	W2X_UTF2A(s)
#endif

/** �� UNICODE �ַ�����ת��Ϊ UTF-8 �ַ������� */
W2X_COMMON_API LPSTR AllocStringW2UTF(
	LPCTSTR _w_str,
	int _wbs_bytes = -1
);

/** �� ASCII �ַ�����ת��Ϊ UTF-8 �ַ������� */
W2X_COMMON_API LPSTR AllocStringA2UTF(
	LPCSTR _a_str,
	int _mbs_bytes = -1
);

#define W2X_ALLOC_STR_A2UTF		w2x::encode::AllocStringA2UTF
#define W2X_ALLOC_STR_W2UTF		w2x::encode::AllocStringW2UTF

#ifdef UNICODE
#  define W2X_ALLOC_STR_T2UTF	w2x::encode::AllocStringW2UTF
#else
#  define W2X_ALLOC_STR_T2UTF	w2x::encode::AllocStringA2UTF
#endif

__forceinline std::string _W2X_A2UTF(LPCSTR pszAscii)
{
	LPSTR pszUtf8 = w2x::encode::AllocStringA2UTF(pszAscii);
	std::string strUtf8 = pszUtf8;
	w2x::encode::FreeStringA(&pszUtf8);
	return strUtf8;
}

#define W2X_A2UTF(s)	w2x::encode::_W2X_A2UTF(s)

__forceinline std::string _W2X_W2UTF(LPCWSTR pszWide)
{
	LPSTR pszUtf8 = w2x::encode::AllocStringW2UTF(pszWide);
	std::string strUtf8 = pszUtf8;
	w2x::encode::FreeStringA(&pszUtf8);
	return strUtf8;
}

#define W2X_W2UTF(s)	w2x::encode::_W2X_W2UTF(s)

#ifdef UNICODE
#  define W2X_T2UTF(s)	W2X_W2UTF(s)
#else
#  define W2X_T2UTF(s)	W2X_A2UTF(s)
#endif


/**
 * �� 16�ֽ�(128λ) �� GUID ת�����ַ���������ʽ�����磺
 * BF3EE013-F387-4BCA-9599-581546EAB6DC��
 * ���ַ������� 36 �ַ���32 ��ʮ�������ַ� + 4 �����ַ������������� '\0'��
 */
W2X_COMMON_API bool Guid2String(
	OUT LPTSTR _str_buffer, 
	size_t _size_in_words, 
	GUID _guid
);

#ifndef W2X_GUID_STR_LEN
#  define W2X_GUID_STR_LEN	37
#endif

/*
 * �����ļ� MD5 ��ϢժҪ
 */
W2X_COMMON_API bool GenerateFileMD5(
	OUT TSTDSTR _md5_digest,
	LPCTSTR _file_path
);


W2X_DEFINE_NAME_SPACE_END(encode)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_ENCODE_H__ */