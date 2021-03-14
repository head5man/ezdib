#pragma once


#include "ezdibconf.h"

#if defined( __cplusplus )
extern "C"
{
#endif

#if !defined( EZD_NOPACK )
#	pragma pack( push, 1 )
#endif

	typedef struct
	{
		char fileID[5];
		int bbx_height;
		int bbx_yoffset;
		int average_width_tenths;
	} font_ident_t;

	typedef struct _bbxfont
	{
		unsigned char width;
		unsigned char height;
		signed char xoffset;
		signed char yoffset;
	} bbxFont;

	typedef struct _bbx
	{
		unsigned char width;
		unsigned char height;
		signed char xoffset;
		signed char yoffset;
	} bbxGlyph;

	typedef struct _glyph
	{
		unsigned char encoding;
		signed char xoffsetnext;
		signed char yoffsetnext;
		bbxGlyph bbox;
	} tGlyph;

#define EZD_FONT_ID_FIELD_LEN 16
#if !defined( EZD_STATIC_FONTS )

	// This structure contains the memory image
	typedef struct _SFontData
	{
		bbxFont bbox;

		unsigned int spacing;
		/// Font flags
		unsigned int			uFlags;
		font_ident_t			ID;
		/// Font index pointers
		const unsigned char		*pIndex[256];

		/// Font bitmap data
		unsigned char			pGlyph[1];

	} SFontData;

	typedef struct _ezdipFontData
	{
		bbxFont bbx;
		// followed by glyphs
	} ezdibFontData;

#endif

#if !defined( EZD_NOPACK )
#	pragma pack( pop )
#endif

#if defined( __cplusplus )
}
#endif
