/******************************************************************************
�ļ�:	encode.h
����:	�����������صĺ����������ȣ����ַ�����ת��������������ת��ȡ�
����:	wu.xiongxing
����:	wxxweb@gmail.com
ʱ��:	2014-01-23
�޸�:	2014-12-08
*******************************************************************************/

#ifndef __W2X_COMMON_ENCODE_H__
#define __W2X_COMMON_ENCODE_H__

#include <WTypes.h>
#include <memory>
#include "macros.h"
#include "string_util.h"

W2X_NAME_SPACE_BEGIN
W2X_DEFINE_NAME_SPACE_BEGIN(encode)


enum EncodeType {
	kAscii,
	kUnicode,
	kUtf8,
};


/** �����Ŀ���һ���ַ�����ջ�У�������� NULL�����ش��������Ŀ��ַ��� */
W2X_COMMON_API LPSTR AllocStringA(LPCSTR _mbs, int _bytes = -1);
W2X_COMMON_API LPWSTR AllocStringW(LPCWSTR _wcs, int _char_count = -1);

/** �ͷ��� AllocStringXXX ����Ķ��ֽ��ַ���ջ�ռ䣬���� UTF-8 */
W2X_COMMON_API bool FreeStringA(LPSTR* _mbs_pptr);
W2X_COMMON_API bool FreeStringA(LPCSTR* _mbs_pptr);

/** �ͷ��� AllocStringXXX ����Ŀ��ֽ��ַ���ջ�ռ� */
W2X_COMMON_API bool FreeStringW(LPWSTR* _wcs_pptr);
W2X_COMMON_API bool FreeStringW(LPCWSTR* _wcs_pptr);

/** ��ָ������ҳ�Ķ��ֽ��ַ���ת��Ϊ���ֽ��ַ��� */
W2X_COMMON_API LPWSTR AllocStringM2W(
	UINT _code_page,
	LPCSTR _mbs,
	int _mbs_bytes
	);

/** ����ǰϵͳ����ҳ�Ķ��ֽ��ַ���ת��Ϊ���ֽ��ַ��� */
W2X_COMMON_API LPWSTR AllocStringA2W(
	LPCSTR _mbs,
	int _mbs_bytes = -1
);

/** �����ֽ��ַ���ת��Ϊָ������ҳ�Ķ��ֽ��ַ��� */
W2X_COMMON_API LPSTR AllocStringW2M(
	UINT _code_page,
	LPCWSTR _wcs,
	int _char_count = -1
	);

/** �����ֽ��ַ���ת��Ϊ��ǰϵͳ����ҳ�Ķ��ֽ��ַ��� */
W2X_COMMON_API LPSTR AllocStringW2A(
	LPCWSTR wcs,
	int _char_count = -1
	);

/** ����ǰϵͳ����ҳ�Ķ��ֽ��ַ���ת��Ϊ GBK �ַ��� */
W2X_COMMON_API LPSTR AllocStringA2GBK(
	LPCSTR _mbs,
	int _mbs_bytes = -1
	);

/** �� GBK �ַ���ת��Ϊ��ǰϵͳ����ҳ�Ķ��ֽ��ַ��� */
W2X_COMMON_API LPSTR AllocStringGBK2A(
	LPCSTR _gbk,
	int _gbk_bytes = -1
	);

/** �����ֽ��ַ�������ת��Ϊ GBK �ַ������� */
W2X_COMMON_API LPSTR AllocStringW2GBK(
	LPCWSTR _wcs,
	int _wcs_char_count = -1
	);

/** �� GBK �ַ�������ת��Ϊ���ֽ��ַ������� */
W2X_COMMON_API LPWSTR AllocStringGBK2W(
	LPCSTR _gbk,
	int _gbk_bytes = -1
	);

/** �� UTF-8 �ַ���ת��Ϊ GBK �ַ��� */
W2X_COMMON_API LPSTR AllocStringUTF2GBK(
	LPCSTR _utf8,
	int _utf8_bytes = -1
	);

/** �� GBK �ַ���ת��Ϊ UTF-8 �ַ��� */
W2X_COMMON_API LPSTR AllocStringGBK2UTF(
	LPCSTR _gbk,
	int _gbk_bytes = -1
	);

/** ����ǰϵͳ����ҳ�Ķ��ֽ��ַ���ת��Ϊ UTF-8 �ַ��� */
W2X_COMMON_API LPSTR AllocStringA2UTF(
	LPCSTR _mbs,
	int _mbs_bytes = -1
	);

/** �����ֽ��ַ���ת��Ϊ UTF-8 �ַ��� */
W2X_COMMON_API LPSTR AllocStringW2UTF(
	LPCTSTR _wcs,
	int _wcs_char_count = -1
	);

/** �� UTF-8 �ַ���ת��Ϊ��ǰϵͳ����ҳ�Ķ��ֽ��ַ��� */
W2X_COMMON_API LPSTR AllocStringUTF2A(
	LPCSTR _utf8_str,
	int _utf8_bytes = -1
	);

/** �� UTF-8 �ַ���ת��Ϊ���ֽ��ַ��� */
W2X_COMMON_API LPWSTR AllocStringUTF2W(
	LPCSTR _utf8_str,
	int _utf8_bytes = -1
	);

/** URL ���룺���ֽ��ַ������� UTF-8 ���� */
W2X_COMMON_API LPSTR AllocStringUrlEncodeA(LPCSTR _mbs);

/** URL ���룺���ֽ��ַ������� UTF-8 ���� */
W2X_COMMON_API LPWSTR AllocStringUrlEncodeW(LPCWSTR _wcs);

/** URL ���룺UTF-8 �ַ������� UTF-8 ���� */
W2X_COMMON_API LPSTR AllocStringUrlEncodeUTF8(LPCSTR _utf8);

/** URL ���룺���ֽ��ַ������� UTF-8 ���� */
W2X_COMMON_API LPSTR AllocStringUrlDecodeA(LPCSTR _mbs);

/** URL ���룺���ֽ��ַ������� UTF-8 ���� */
W2X_COMMON_API LPWSTR AllocStringUrlDecodeW(LPCWSTR _wcs);

/** URL ���룺UTF-8 �ֽ��ַ������� UTF-8 ���� */
W2X_COMMON_API LPSTR AllocStringUrlDecodeUTF8(LPCSTR _utf8);

inline std::wstring _W2X_A2W(LPCSTR _mbs)
{
	LPWSTR wcs = w2x::encode::AllocStringA2W(_mbs);
	std::wstring str = wcs;
	w2x::encode::FreeStringW(&wcs);
	return str;
}

inline std::string _W2X_W2A(LPCWSTR _wcs)
{
	LPSTR mbs = w2x::encode::AllocStringW2A(_wcs);
	std::string str = mbs;
	w2x::encode::FreeStringA(&mbs);
	return str;
}

inline std::string _W2X_GBK2A(LPCSTR _gbk)
{
	LPSTR mbs = w2x::encode::AllocStringGBK2A(_gbk);
	std::string str = mbs;
	w2x::encode::FreeStringA(&mbs);
	return str;
}

inline std::string _W2X_A2GBK(LPCSTR _mbs)
{
	LPSTR gbk = w2x::encode::AllocStringA2GBK(_mbs);
	std::string str = gbk;
	w2x::encode::FreeStringA(&gbk);
	return str;
}

inline std::wstring _W2X_GBK2W(LPCSTR _gbk)
{
	LPWSTR wcs = w2x::encode::AllocStringGBK2W(_gbk);
	std::wstring str = wcs;
	w2x::encode::FreeStringW(&wcs);
	return str;
}

inline std::string _W2X_W2GBK(LPCWSTR _wcs)
{
	LPSTR gbk = w2x::encode::AllocStringW2GBK(_wcs);
	std::string str = gbk;
	w2x::encode::FreeStringA(&gbk);
	return str;
}

inline std::string _W2X_UTF2A(LPCSTR _utf8)
{
	LPSTR mbs = w2x::encode::AllocStringUTF2A(_utf8);
	std::string str = mbs;
	w2x::encode::FreeStringA(&mbs);
	return str;
}

inline std::string _W2X_A2UTF(LPCSTR _mbs)
{
	LPSTR utf8 = w2x::encode::AllocStringA2UTF(_mbs);
	std::string str = utf8;
	w2x::encode::FreeStringA(&utf8);
	return str;
}

inline std::wstring _W2X_UTF2W(LPCSTR _utf8)
{
	LPWSTR wcs = w2x::encode::AllocStringUTF2W(_utf8);
	std::wstring str = wcs;
	w2x::encode::FreeStringW(&wcs);
	return str;
}

inline std::string _W2X_W2UTF(LPCWSTR _wcs)
{
	LPSTR utf8 = w2x::encode::AllocStringW2UTF(_wcs);
	std::string str = utf8;
	w2x::encode::FreeStringA(&utf8);
	return str;
}

inline std::string _W2X_GBK2UTF(LPCSTR _gbk)
{
	LPSTR utf8 = w2x::encode::AllocStringGBK2UTF(_gbk);
	std::string str = utf8;
	w2x::encode::FreeStringA(&utf8);
	return str;
}

inline std::string _W2X_UTF2GBK(LPCSTR _utf8)
{
	LPSTR gbk = w2x::encode::AllocStringUTF2GBK(_utf8);
	std::string str = gbk;
	w2x::encode::FreeStringA(&gbk);
	return str;
}

inline std::string _W2X_URL_ENCODE_A(LPCSTR _mbs)
{
	LPSTR encode_mbs = w2x::encode::AllocStringUrlEncodeA(_mbs);
	std::string ret_str = encode_mbs;
	w2x::encode::FreeStringA(&encode_mbs);
	return ret_str;
}

inline std::wstring _W2X_URL_ENCODE_W(LPCWSTR _wcs)
{
	LPWSTR encode_wcs = w2x::encode::AllocStringUrlEncodeW(_wcs);
	std::wstring ret_str = encode_wcs;
	w2x::encode::FreeStringW(&encode_wcs);
	return ret_str;
}

inline std::string _W2X_URL_ENCODE_UTF8(LPCSTR _utf8)
{
	LPSTR encode_utf8 = w2x::encode::AllocStringUrlEncodeUTF8(_utf8);
	std::string ret_str = encode_utf8;
	w2x::encode::FreeStringA(&encode_utf8);
	return ret_str;
}

inline std::string _W2X_URL_DECODE_A(LPCSTR _mbs)
{
	LPSTR decode_mbs = w2x::encode::AllocStringUrlDecodeA(_mbs);
	std::string ret_str = decode_mbs;
	w2x::encode::FreeStringA(&decode_mbs);
	return ret_str;
}

inline std::wstring _W2X_URL_DECODE_W(LPCWSTR _wcs)
{
	LPWSTR decode_wcs = w2x::encode::AllocStringUrlDecodeW(_wcs);
	std::wstring ret_str = decode_wcs;
	w2x::encode::FreeStringW(&decode_wcs);
	return ret_str;
}

inline std::string _W2X_URL_DECODE_UTF8(LPCSTR _utf8)
{
	LPSTR decode_utf8 = w2x::encode::AllocStringUrlDecodeA(_utf8);
	std::string ret_str = decode_utf8;
	w2x::encode::FreeStringA(&decode_utf8);
	return ret_str;
}

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

/** �����ļ� MD5 ��ϢժҪ */
W2X_COMMON_API bool GenerateFileMD5(
	OUT TSTDSTR _md5_digest,
	LPCTSTR _file_path
);


#define W2X_ALLOC_STR_A					w2x::encode::AllocStringA
#define W2X_ALLOC_STR_W					w2x::encode::AllocStringW
#define W2X_FREE_STR_A(p)				w2x::encode::FreeStringA(&(p))
#define W2X_FREE_STR_W(p)				w2x::encode::FreeStringW(&(p))
#define W2X_ALLOC_STR_A2W				w2x::encode::AllocStringA2W
#define W2X_ALLOC_STR_W2A				w2x::encode::AllocStringW2A
#define W2X_ALLOC_STR_A2GBK				w2x::encode::AllocStringA2GBK
#define W2X_ALLOC_STR_GBK2A				w2x::encode::AllocStringGBK2A
#define W2X_ALLOC_STR_W2GBK				w2x::encode::AllocStringW2GBK
#define W2X_ALLOC_STR_GBK2W				w2x::encode::AllocStringGBK2W
#define W2X_ALLOC_STR_UTF2A				w2x::encode::AllocStringUTF2A
#define W2X_ALLOC_STR_UTF2W				w2x::encode::AllocStringUTF2W
#define W2X_ALLOC_STR_A2UTF				w2x::encode::AllocStringA2UTF
#define W2X_ALLOC_STR_W2UTF				w2x::encode::AllocStringW2UTF
#define W2X_ALLOC_STR_GBK2UTF			w2x::encode::AllocStringGBK2UTF
#define W2X_ALLOC_STR_UTF2GBK			w2x::encode::AllocStringUTF2GBK
#define W2X_A2W							w2x::encode::_W2X_A2W
#define W2X_W2A							w2x::encode::_W2X_W2A
#define W2X_A2GBK						w2x::encode::_W2X_A2GBK
#define W2X_GBK2A						w2x::encode::_W2X_GBK2A
#define W2X_W2GBK						w2x::encode::_W2X_W2GBK
#define W2X_GBK2W						w2x::encode::_W2X_GBK2W
#define W2X_A2UTF						w2x::encode::_W2X_A2UTF
#define W2X_UTF2A						w2x::encode::_W2X_UTF2A
#define W2X_W2UTF						w2x::encode::_W2X_W2UTF
#define W2X_UTF2W						w2x::encode::_W2X_UTF2W
#define W2X_GBK2UTF						w2x::encode::_W2X_GBK2UTF
#define W2X_UTF2GBK						w2x::encode::_W2X_UTF2GBK
#define W2X_ALLOC_STR_URL_ENCODE_A		w2x::encode::AllocStringUrlEncodeA
#define W2X_ALLOC_STR_URL_ENCODE_W		w2x::encode::AllocStringUrlEncodeW
#define W2X_ALLOC_STR_URL_ENCODE_UTF8	w2x::encode::AllocStringUrlEncodeUTF8
#define W2X_ALLOC_STR_URL_DECODE_A		w2x::encode::AllocStringUrlDecodeA
#define W2X_ALLOC_STR_URL_DECODE_W		w2x::encode::AllocStringUrlDecodeW
#define W2X_ALLOC_STR_URL_DECODE_UTF8	w2x::encode::AllocStringUrlDecodeUTF8
#define W2X_URL_ENCODE_A				w2x::encode::_W2X_URL_ENCODE_A
#define W2X_URL_ENCODE_W				w2x::encode::_W2X_URL_ENCODE_W
#define W2X_URL_ENCODE_UTF8				w2x::encode::_W2X_URL_ENCODE_UTF8
#define W2X_URL_DECODE_A				w2x::encode::_W2X_URL_DECODE_A
#define W2X_URL_DECODE_W				w2x::encode::_W2X_URL_DECODE_W
#define W2X_URL_DECODE_UTF8				w2x::encode::_W2X_URL_DECODE_UTF8


#ifdef UNICODE
#  define W2X_ALLOC_STR				w2x::encode::AllocStringW
#  define W2X_FREE_STR(p)			w2x::encode::FreeStringW(&(p))
#  define W2X_ALLOC_STR_A2T			w2x::encode::AllocStringA2W
#  define W2X_ALLOC_STR_T2A			w2x::encode::AllocStringW2A
#  define W2X_ALLOC_STR_W2T			w2x::encode::AllocStringW
#  define W2X_ALLOC_STR_T2W			w2x::encode::AllocStringW
#  define W2X_ALLOC_STR_GBK2T		w2x::encode::AllocStringGBK2W
#  define W2X_ALLOC_STR_T2GBK		w2x::encode::AllocStringW2GBK
#  define W2X_ALLOC_STR_UTF2T		w2x::encode::AllocStringUTF2W
#  define W2X_ALLOC_STR_T2UTF		w2x::encode::AllocStringW2UTF
#  define W2X_A2T					w2x::encode::_W2X_A2W
#  define W2X_T2A					w2x::encode::_W2X_W2A
#  define W2X_W2T(s)				(s)
#  define W2X_T2W(s)				(s)
#  define W2X_GBK2T					w2x::encode::_W2X_GBK2W
#  define W2X_T2GBK					w2x::encode::_W2X_W2GBK
#  define W2X_UTF2T					w2x::encode::_W2X_UTF2W
#  define W2X_T2UTF					w2x::encode::_W2X_W2UTF
#  define W2X_ALLOC_STR_URL_ENCODE	w2x::encode::AllocStringUrlEncodeW
#  define W2X_ALLOC_STR_URL_DECODE	w2x::encode::AllocStringUrlDecodeW
#  define W2X_URL_ENCODE			w2x::encode::_W2X_URL_ENCODE_W
#  define W2X_URL_DECODE			w2x::encode::_W2X_URL_DECODE_W
#else // !UNICODE
#  define W2X_ALLOC_STR				w2x::encode::AllocStringA
#  define W2X_FREE_STR(p)			w2x::encode::FreeStringA(&(p))
#  define W2X_ALLOC_STR_A2T			w2x::encode::AllocStringA
#  define W2X_ALLOC_STR_T2A			w2x::encode::AllocStringA
#  define W2X_ALLOC_STR_T2W			w2x::encode::AllocStringA2W
#  define W2X_ALLOC_STR_W2T			w2x::encode::AllocStringW2A
#  define W2X_ALLOC_STR_GBK2T		w2x::encode::AllocStringGBK2A
#  define W2X_ALLOC_STR_T2GBK		w2x::encode::AllocStringA2GBK
#  define W2X_ALLOC_STR_UTF2T		w2x::encode::AllocStringUTF2A
#  define W2X_ALLOC_STR_T2UTF		w2x::encode::AllocStringA2UTF
#  define W2X_A2T(s)				(s)
#  define W2X_T2A(s)				(s)
#  define W2X_W2T					w2x::encode::_W2X_W2A
#  define W2X_T2W					w2x::encode::_W2X_A2W
#  define W2X_GBK2T					w2x::encode::_W2X_GBK2A
#  define W2X_T2GBK					w2x::encode::_W2X_A2GBK
#  define W2X_UTF2T					w2x::encode::_W2X_UTF2A
#  define W2X_T2UTF					w2x::encode::_W2X_A2UTF
#  define W2X_ALLOC_STR_URL_ENCODE	w2x::encode::AllocStringUrlEncodeA
#  define W2X_ALLOC_STR_URL_DECODE	w2x::encode::AllocStringUrlDecodeA
#  define W2X_URL_ENCODE			w2x::encode::_W2X_URL_ENCODE_A
#  define W2X_URL_DECODE			w2x::encode::_W2X_URL_DECODE_A
#endif // UNICODE


W2X_DEFINE_NAME_SPACE_END(encode)
W2X_NAME_SPACE_END

#endif /* __W2X_COMMON_ENCODE_H__ */