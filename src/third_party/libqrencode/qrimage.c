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

#if HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdlib.h>
#include <string.h>
#include "png.h"
#include "qrencode.h"
#include "qrimage.h"

#define INCHES_PER_METER (100.0/2.54)

static const int size = 3;
static const int margin = 2;
static const int dpi = 72;
static const unsigned int fg_color[4] = {0, 0, 0, 255};
static const unsigned int bg_color[4] = {255, 255, 255, 255};

static QRimageResultType writePNG(QRcode *qrcode, const char *outfile)
{
	static FILE *fp; // avoid clobbering by setjmp.
	png_structp png_ptr;
	png_infop info_ptr;
	png_colorp palette;
	png_byte alpha_values[2];
	unsigned char *row, *p, *q;
	int x, y, xx, yy, bit;
	int realwidth;

	realwidth = (qrcode->width + margin * 2) * size;
	row = (unsigned char *)malloc((realwidth + 7) / 8);
	if(row == NULL) {
		return QR_IMG_FAILED_ALLOCATE_MEMORY;
	}

	fp = fopen(outfile, "wb");
	if(fp == NULL) {
		free(row);
		return QR_IMG_FAILED_CREATE_FILE;
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(png_ptr == NULL) {
		free(row);
		fclose(fp);
		return QR_IMG_FAILED_INIT_PNG_WRITER;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(info_ptr == NULL) {
		free(row);
		fclose(fp);
		return QR_IMG_FAILED_INIT_PNG_WRITER;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		free(row);
		fclose(fp);
		return QR_IMG_FAILED_WRITE_PNG_IMAGE;
	}

	palette = (png_colorp) malloc(sizeof(png_color) * 2);
	if(palette == NULL) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		free(row);
		fclose(fp);
		return QR_IMG_FAILED_ALLOCATE_MEMORY;
	}
	palette[0].red   = fg_color[0];
	palette[0].green = fg_color[1];
	palette[0].blue  = fg_color[2];
	palette[1].red   = bg_color[0];
	palette[1].green = bg_color[1];
	palette[1].blue  = bg_color[2];
	alpha_values[0] = fg_color[3];
	alpha_values[1] = bg_color[3];
	png_set_PLTE(png_ptr, info_ptr, palette, 2);
	png_set_tRNS(png_ptr, info_ptr, alpha_values, 2, NULL);

	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr,
		realwidth, realwidth,
		1,
		PNG_COLOR_TYPE_PALETTE,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	png_set_pHYs(png_ptr, info_ptr,
		dpi * INCHES_PER_METER,
		dpi * INCHES_PER_METER,
		PNG_RESOLUTION_METER);
	png_write_info(png_ptr, info_ptr);

	/* top margin */
	memset(row, 0xff, (realwidth + 7) / 8);
	for(y=0; y<margin * size; y++) {
		png_write_row(png_ptr, row);
	}

	/* data */
	p = qrcode->data;
	for(y=0; y<qrcode->width; y++) {
		bit = 7;
		memset(row, 0xff, (realwidth + 7) / 8);
		q = row;
		q += margin * size / 8;
		bit = 7 - (margin * size % 8);
		for(x=0; x<qrcode->width; x++) {
			for(xx=0; xx<size; xx++) {
				*q ^= (*p & 1) << bit;
				bit--;
				if(bit < 0) {
					q++;
					bit = 7;
				}
			}
			p++;
		}
		for(yy=0; yy<size; yy++) {
			png_write_row(png_ptr, row);
		}
	}
	/* bottom margin */
	memset(row, 0xff, (realwidth + 7) / 8);
	for(y=0; y<margin * size; y++) {
		png_write_row(png_ptr, row);
	}

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);
	free(row);
	free(palette);

	return QR_IMG_SUCCESS;
}


static QRimageResultType writeEPS(QRcode *qrcode, const char *outfile)
{
	FILE *fp;
	unsigned char *row, *p;
	int x, y, yy;
	int realwidth;

	fp = fopen(outfile, "wb");
	if(fp == NULL) {
		return QR_IMG_FAILED_CREATE_FILE;
	}
   
	realwidth = (qrcode->width + margin * 2) * size;
	/* EPS file header */
	fprintf(fp, "%%!PS-Adobe-2.0 EPSF-1.2\n"
				"%%%%BoundingBox: 0 0 %d %d\n"
				"%%%%Pages: 1 1\n"
				"%%%%EndComments\n", realwidth, realwidth);
	/* draw point */
	fprintf(fp, "/p { "
				"moveto "
				"0 1 rlineto "
				"1 0 rlineto "
				"0 -1 rlineto "
				"fill "
				"} bind def "
				"%d %d scale ", size, size);
	
	/* data */
	p = qrcode->data;
	for(y=0; y<qrcode->width; y++) {
		row = (p+(y*qrcode->width));
		yy = (margin + qrcode->width - y - 1);
		
		for(x=0; x<qrcode->width; x++) {
			if(*(row+x)&0x1) {
				fprintf(fp, "%d %d p ", margin + x,  yy);
			}
		}
	}

	fprintf(fp, "\n%%%%EOF\n");
	fclose(fp);

	return QR_IMG_SUCCESS;
}

static void writeSVG_writeRect(FILE *fp, int x, int y, int width, char* col, float opacity)
{
	if(fg_color[3] != 255) {
		fprintf(fp, "\t\t\t<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"1\" "\
				"fill=\"#%s\" fill-opacity=\"%f\" />\n", 
				x, y, width, col, opacity );
	} else {
		fprintf(fp, "\t\t\t<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"1\" "\
				"fill=\"#%s\" />\n", 
				x, y, width, col );
	}
}

static QRimageResultType writeSVG( QRcode *qrcode, const char *outfile )
{
	FILE *fp;
	unsigned char *row, *p;
	int rle = 0;
	int x, y, x0, pen;
	int symwidth, realwidth;
	float scale;
	char fg[7], bg[7];
	float fg_opacity;
	float bg_opacity;

	fp = fopen(outfile, "wb");
	if(fp == NULL) {
		return QR_IMG_FAILED_CREATE_FILE;
	}

	scale = dpi * INCHES_PER_METER / 100.0;

	symwidth = qrcode->width + margin * 2;
	realwidth = symwidth * size;

	_snprintf(fg, 7, "%02x%02x%02x", fg_color[0], fg_color[1],  fg_color[2]);
	_snprintf(bg, 7, "%02x%02x%02x", bg_color[0], bg_color[1],  bg_color[2]);
	fg_opacity = (float)fg_color[3] / 255;
	bg_opacity = (float)bg_color[3] / 255;

	/* XML declaration */
	fputs( "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n", fp );

	/* DTD 
	   No document type specified because "while a DTD is provided in [the SVG] 
	   specification, the use of DTDs for validating XML documents is known to be 
	   problematic. In particular, DTDs do not handle namespaces gracefully. It 
	   is *not* recommended that a DOCTYPE declaration be included in SVG 
	   documents." 
	   http://www.w3.org/TR/2003/REC-SVG11-20030114/intro.html#Namespace
	*/

	/* Vanity remark */
	fprintf( fp, "<!-- Created with qrencode %s (http://fukuchi.org/works/qrencode/index.html.en) -->\n", 
			QRcode_APIVersionString() );

	/* SVG code start */
	fprintf( fp, "<svg width=\"%0.2fcm\" height=\"%0.2fcm\" viewBox=\"0 0 %d %d\""\
			" preserveAspectRatio=\"none\" version=\"1.1\""\
			" xmlns=\"http://www.w3.org/2000/svg\">\n", 
			realwidth / scale, realwidth / scale, symwidth, symwidth
		   );

	/* Make named group */
	fputs( "\t<g id=\"QRcode\">\n", fp );

	/* Make solid background */
	if(bg_color[3] != 255) {
		fprintf(fp, "\t\t<rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#%s\" fill-opacity=\"%f\" />\n", symwidth, symwidth, bg, bg_opacity);
	} else {
		fprintf(fp, "\t\t<rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#%s\" />\n", symwidth, symwidth, bg);
	}

	/* Create new viewbox for QR data */
	fputs( "\t\t<g id=\"Pattern\">\n", fp);

	/* Write data */
	p = qrcode->data;
	for(y=0; y<qrcode->width; y++) {
		row = (p+(y*qrcode->width));

		if( !rle ) {
			/* no RLE */
			for(x=0; x<qrcode->width; x++) {
				if(*(row+x)&0x1) {
					writeSVG_writeRect(fp,	margin + x,
								margin + y, 1,
								fg, fg_opacity);
				}
			}
		} else {
			/* simple RLE */
			pen = 0;
			x0  = 0;
			for(x=0; x<qrcode->width; x++) {
				if( !pen ) {
					pen = *(row+x)&0x1;
					x0 = x;
				} else {
					if(!(*(row+x)&0x1)) {
						writeSVG_writeRect(fp, x0 + margin, y + margin, x-x0, fg, fg_opacity);
						pen = 0;
					}
				}
			}
			if( pen ) {
				writeSVG_writeRect(fp, x0 + margin, y + margin, qrcode->width - x0, fg, fg_opacity);
			}
		}
	}
	/* Close QR data viewbox */
	fputs( "\t\t</g>\n", fp );

	/* Close group */
	fputs( "\t</g>\n", fp );

	/* Close SVG code */
	fputs( "</svg>\n", fp );
	fclose( fp );

	return QR_IMG_SUCCESS;
}

static void writeANSI_margin(FILE* fp, int realwidth,
                             char* buffer, int buffer_s,
                             char* white, int white_s )
{
	int y;

	strncpy_s(buffer, buffer_s, white, white_s);
	memset(buffer + white_s, ' ', realwidth * 2);
	strcpy(buffer + white_s + realwidth * 2, "\033[0m\n"); // reset to default colors
	for(y=0; y<margin; y++ ){
		fputs(buffer, fp);
	}
}

static QRimageResultType writeANSI(QRcode *qrcode, const char *outfile, int ansi256)
{
	FILE *fp;
	unsigned char *row, *p;
	int x, y;
	int realwidth;
	int last;

	char *white, *black, *buffer;
	int white_s, black_s, buffer_s;

	if( ansi256 ){
		/* codes for 256 color compatible terminals */
		white = "\033[48;5;231m";
		white_s = 11;
		black = "\033[48;5;16m";
		black_s = 10;
	} else {
		white = "\033[47m";
		white_s = 5;
		black = "\033[40m";
		black_s = 5;
	}

	fp = fopen(outfile, "wb");
	if(fp == NULL) {
		return QR_IMG_FAILED_CREATE_FILE;
	}

	realwidth = (qrcode->width + margin * 2);
	buffer_s = ( realwidth * white_s ) * 2;
	buffer = (char *)malloc( buffer_s );
	if(buffer == NULL) {
		return QR_IMG_FAILED_ALLOCATE_MEMORY;
	}

	/* top margin */
	writeANSI_margin(fp, realwidth, buffer, buffer_s, white, white_s);

	/* data */
	p = qrcode->data;
	for(y=0; y<qrcode->width; y++) {
		row = (p+(y*qrcode->width));

		memset( buffer, 0, buffer_s );
		strncpy_s( buffer, buffer_s, white, white_s );
		for(x=0; x<margin; x++ ){
			strncat_s( buffer, buffer_s, "  ", 2 );
		}
		last = 0;

		for(x=0; x<qrcode->width; x++) {
			if(*(row+x)&0x1) {
				if( last != 1 ){
					strncat_s( buffer, buffer_s, black, black_s );
					last = 1;
				}
			} else {
				if( last != 0 ){
					strncat_s( buffer, buffer_s, white, white_s );
					last = 0;
				}
			}
			strncat_s( buffer, buffer_s, "  ", 2 );
		}

		if( last != 0 ){
			strncat_s( buffer, buffer_s, white, white_s );
		}
		for(x=0; x<margin; x++ ){
			strncat_s( buffer, buffer_s, "  ", 2 );
		}
		strncat_s( buffer, buffer_s, "\033[0m\n", 5 );
		fputs( buffer, fp );
	}

	/* bottom margin */
	writeANSI_margin(fp, realwidth, buffer, buffer_s, white, white_s);

	fclose(fp);
	free(buffer);

	return QR_IMG_SUCCESS;
}

static void writeUTF8_margin(FILE* fp, int realwidth,
			     const char* white, const char *reset,
			     int use_ansi)
{
	int x, y;

	for (y = 0; y < margin/2; y++) {
		fputs(white, fp);
		for (x = 0; x < realwidth; x++)
			fputs("\342\226\210", fp);
		fputs(reset, fp);
		fputc('\n', fp);
	}
}

static QRimageResultType writeUTF8(QRcode *qrcode, const char *outfile, int use_ansi)
{
	FILE *fp;
	int x, y;
	int realwidth;
	const char *white, *reset;

	if (use_ansi){
		white = "\033[40;37;1m";
		reset = "\033[0m";
	} else {
		white = "";
		reset = "";
	}

	fp = fopen(outfile, "wb");
	if(fp == NULL) {
		return QR_IMG_FAILED_CREATE_FILE;
	}

	realwidth = (qrcode->width + margin * 2);

	/* top margin */
	writeUTF8_margin(fp, realwidth, white, reset, use_ansi);

	/* data */
	for(y = 0; y < qrcode->width; y += 2) {
		unsigned char *row1, *row2;
		row1 = qrcode->data + y*qrcode->width;
		row2 = row1 + qrcode->width;

		fputs(white, fp);

		for (x = 0; x < margin; x++)
			fputs("\342\226\210", fp);

		for (x = 0; x < qrcode->width; x++) {
			if(row1[x] & 1) {
				if(y < qrcode->width - 1 && row2[x] & 1) {
					fputc(' ', fp);
				} else {
					fputs("\342\226\204", fp);
				}
			} else {
				if(y < qrcode->width - 1 && row2[x] & 1) {
					fputs("\342\226\200", fp);
				} else {
					fputs("\342\226\210", fp);
				}
			}
		}

		for (x = 0; x < margin; x++)
			fputs("\342\226\210", fp);

		fputs(reset, fp);
		fputc('\n', fp);
	}

	/* bottom margin */
	writeUTF8_margin(fp, realwidth, white, reset, use_ansi);

	fclose(fp);

	return QR_IMG_SUCCESS;
}

static void writeASCII_margin(FILE* fp, int realwidth, char* buffer, int buffer_s, int invert)
{
	int y, h;

	h = margin;

	memset(buffer, (invert?'#':' '), realwidth);
	buffer[realwidth] = '\n';
	buffer[realwidth + 1] = '\0';
	for(y=0; y<h; y++ ){
		fputs(buffer, fp);
	}
}

static QRimageResultType writeASCII(QRcode *qrcode, const char *outfile, int invert)
{
	FILE *fp;
	unsigned char *row;
	int x, y;
	int realwidth;
	char *buffer, *p;
	int buffer_s;
	char black = '#';
	char white = ' ';

	if(invert) {
		black = ' ';
		white = '#';
	}

	fp = fopen(outfile, "wb");
	if(fp == NULL) {
		return QR_IMG_FAILED_CREATE_FILE;
	}

	realwidth = (qrcode->width + margin * 2) * 2;
	buffer_s = realwidth + 2;
	buffer = (char *)malloc( buffer_s );
	if(buffer == NULL) {
		return QR_IMG_FAILED_ALLOCATE_MEMORY;
	}

	/* top margin */
	writeASCII_margin(fp, realwidth, buffer, buffer_s, invert);

	/* data */
	for(y=0; y<qrcode->width; y++) {
		row = qrcode->data+(y*qrcode->width);
		p = buffer;

		memset(p, white, margin * 2);
		p += margin * 2;

		for(x=0; x<qrcode->width; x++) {
			if(row[x]&0x1) {
				*p++ = black;
				*p++ = black;
			} else {
				*p++ = white;
				*p++ = white;
			}
		}

		memset(p, white, margin * 2);
		p += margin * 2;
		*p++ = '\n';
		*p++ = '\0';
		fputs( buffer, fp );
	}

	/* bottom margin */
	writeASCII_margin(fp, realwidth, buffer, buffer_s, invert);

	fclose(fp);
	free(buffer);

	return QR_IMG_SUCCESS;
}

QRimageEncodeParams* QRimage_initEncodeParams(QRimageEncodeParams* params)
{
	if (NULL == params) {
		return NULL;
	}

	memset(params, 0, sizeof(QRimageEncodeParams));

	params->hint = QR_MODE_8;
	params->level = QR_ECLEVEL_L;

	return params;
}

QRcode* QRimage_encode(QRimageEncodeParams* params)
{
	QRcode *code;

	if (NULL == params) {
		return NULL;
	}

	if(params->micro) {
		if(params->eightbit) {
			code = QRcode_encodeDataMQR(params->size, 
				params->data, params->version, params->level);
		} else {
			code = QRcode_encodeStringMQR((char *)params->data, params->version,
				params->level, params->hint, params->casesensitive);
		}
	} else {
		if(params->eightbit) {
			code = QRcode_encodeData(params->size, 
				params->data, params->version, params->level);
		} else {
			code = QRcode_encodeString((char *)params->data, params->version,
				params->level, params->hint, params->casesensitive);
		}
	}
	return code;
}

void QRimage_free(QRcode *qrcode)
{
	QRcode_free(qrcode);
}

QRimageResultType QRimage_writeImage(
	QRcode *qrcode,
	const char *outfile,
	QRimageType imagetype
	)
{
	if (!qrcode) {
		return QR_IMG_QRCODE_IS_NULL;
	}
	switch(imagetype) {
		case QR_IMG_PNG:
			return writePNG(qrcode, outfile);
		case QR_IMG_EPS:
			return writeEPS(qrcode, outfile);
		case QR_IMG_SVG:
			return writeSVG(qrcode, outfile);
		case QR_IMG_ANSI:
			return writeANSI(qrcode, outfile, 0);
		case QR_IMG_ANSI256:
			return writeANSI(qrcode, outfile, 1);
		case QR_IMG_ASCIIi:
			return writeASCII(qrcode, outfile,  1);
		case QR_IMG_ASCII:
			return writeASCII(qrcode, outfile,  0);
		case QR_IMG_UTF8:
			return writeUTF8(qrcode, outfile, 0);
		case QR_IMG_ANSIUTF8:
			return writeUTF8(qrcode, outfile, 1);
		default:
			return QR_IMG_UNKNOW_IMAGE_TYPE;
	}
	return QR_IMG_SUCCESS;
}