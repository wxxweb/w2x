/**
 * qrimage - QR Code Image Output
 *
 * Copyright (C) 2015 Atypiape <wxxweb@163.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __QRIMAGE_H__
#define __QRIMAGE_H__

#include "qrencode.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * Export image type.
 */
typedef enum {
	QR_IMG_PNG,
	QR_IMG_EPS,
	QR_IMG_SVG,
	QR_IMG_ANSI,
	QR_IMG_ANSI256,
	QR_IMG_ASCII,
	QR_IMG_ASCIIi,
	QR_IMG_UTF8,
	QR_IMG_ANSIUTF8
} QRimageType;

/**
 * Result type of call.
 */
typedef enum {
	QR_IMG_SUCCESS,
	QR_IMG_UNKNOW_IMAGE_TYPE,
	QR_IMG_QRCODE_IS_NULL,
	QR_IMG_FAILED_ALLOCATE_MEMORY,
	QR_IMG_FAILED_CREATE_FILE,
	QR_IMG_FAILED_INIT_PNG_WRITER,
	QR_IMG_FAILED_WRITE_PNG_IMAGE
} QRimageResultType;

/**
 * The params struct of QRimage_encode.
 */
typedef struct {
	const unsigned char *data;	///< input data
	QRecLevel level;			///< error correction level
	QRencodeMode hint;			///< tell the library how Japanese Kanji characters should be encoded
	int size;					///< size of the input 8-bit data, not input string length
	int version;				///< version (size) of QR-code symbol (0 = auto) 
	int casesensitive;			///< case-sensitive(1) or not(0)
	int eightbit;				///< encode entire data in 8-bit mode(1) or string(0)
	int micro;					///< Micro QR Code version(1) or not(0)
} QRimageEncodeParams;

/**
 * Initialize QRimageEncodeParams object.
 * @param params QRimageEncodeParams object pointer.
 * @return the entrie QRimageEncodeParams object (initialized).
 */
extern QRimageEncodeParams* QRimage_initEncodeParams(QRimageEncodeParams* params);

/**
 * Create a symbol from the data or string. The library automatically parses the
 *  input data or string and encodes in a QR Code symbol.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param params input QRimageEncodeParams object, \see QRimageEncodeParams.
 * @return an instance of QRcode class. The version of the result QRcode may
 *         be larger than the designated version. On error, NULL is returned,
 *         and errno is set to indicate the error. See Exceptions for the
 *         details.
 * @throw EINVAL invalid input object.
 * @throw ENOMEM unable to allocate memory for input objects.
 * @throw ERANGE input data is too large.
 */
extern QRcode* QRimage_encode(QRimageEncodeParams* params);

/**
 * Free the instance of QRcode class.
 * @param qrcode an instance of QRcode class.
 */
extern void QRimage_free(QRcode *qrcode);

/**
 * Input QR Code and save as a PNG or EPS image.
 * @warning This function is THREAD UNSAFE when pthread is disabled.
 * @param qrcode input an instance of QRcode class.
 * @param outfile write image to outfile.
 * @param imagetype specify the type of the generated image.
 * @return an result code.
 */
extern QRimageResultType QRimage_writeImage(
	QRcode *qrcode,
	const char *outfile,
	QRimageType imagetype
	);

#if defined(__cplusplus)
}
#endif

#endif /* __QRIMAGE_H__ */