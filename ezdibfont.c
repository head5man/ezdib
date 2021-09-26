#include "ezdib.h"

// A small font map
static const unsigned char font_map_small[] =
{
	// bbox
	5, 6, 6, 0,
	// Default glyph
	'.', 1, 0, 1, 6, 0, 0, 0x08,

	// Tab width
	'\t', 8, 0, 0, 0, 0, 0, 

	// Space
	' ', 3, 0, 0, 0, 0, 0, 

	'!', 1, 0, 1, 6, 0, 0, 0xea,
	'+', 3, 0, 3, 6, 0, 0, 0x0b, 0xa0, 0x00,
	'-', 3, 0, 3, 6, 0, 0, 0x03, 0x80, 0x00,
	'/', 3, 0, 3, 6, 0, 0, 0x25, 0x48, 0x00,
	'*', 3, 0, 3, 6, 0, 0, 0xab, 0xaa, 0x00,
	'@', 4, 0, 4, 6, 0, 0, 0x69, 0xbb, 0x87,
	':', 1, 0, 1, 6, 0, 0, 0x52,
	'=', 3, 0, 3, 6, 0, 0, 0x1c, 0x70, 0x00,
	'?', 4, 0, 4, 6, 0, 0, 0x69, 0x24, 0x04,
	'%', 3, 0, 3, 6, 0, 0, 0x85, 0x28, 0x40,
	'^', 3, 0, 3, 6, 0, 0, 0x54, 0x00, 0x00,
	'#', 5, 0, 5, 6, 0, 0, 0x57, 0xd5, 0xf5, 0x00,
	'$', 5, 0, 5, 6, 0, 0, 0x23, 0xe8, 0xe2, 0xf8,
	'~', 4, 0, 4, 6, 0, 0, 0x05, 0xa0, 0x00,

	'0', 3, 0, 3, 6, 0, 0, 0x56, 0xd4, 0x31,
	'1', 2, 0, 2, 6, 0, 0, 0xd5, 0x42,
	'2', 4, 0, 4, 6, 0, 0, 0xe1, 0x68, 0xf0,
	'3', 4, 0, 4, 6, 0, 0, 0xe1, 0x61, 0xe0,
	'4', 4, 0, 4, 6, 0, 0, 0x89, 0xf1, 0x10,
	'5', 4, 0, 4, 6, 0, 0, 0xf8, 0xe1, 0xe0,
	'6', 4, 0, 4, 6, 0, 0, 0x78, 0xe9, 0x60,
	'7', 4, 0, 4, 6, 0, 0, 0xf1, 0x24, 0x40,
	'8', 4, 0, 4, 6, 0, 0, 0x69, 0x69, 0x60,
	'9', 4, 0, 4, 6, 0, 0, 0x69, 0x71, 0x60,

	'A', 4, 0, 4, 6, 0, 0, 0x69, 0xf9, 0x90,
	'B', 4, 0, 4, 6, 0, 0, 0xe9, 0xe9, 0xe0,
	'C', 4, 0, 4, 6, 0, 0, 0x78, 0x88, 0x70,
	'D', 4, 0, 4, 6, 0, 0, 0xe9, 0x99, 0xe0,
	'E', 4, 0, 4, 6, 0, 0, 0xf8, 0xe8, 0xf0,
	'F', 4, 0, 4, 6, 0, 0, 0xf8, 0xe8, 0x80,
	'G', 4, 0, 4, 6, 0, 0, 0x78, 0xb9, 0x70,
	'H', 4, 0, 4, 6, 0, 0, 0x99, 0xf9, 0x90,
	'I', 3, 0, 3, 6, 0, 0, 0xe9, 0x2e, 0x00,
	'J', 4, 0, 4, 6, 0, 0, 0xf2, 0x2a, 0x40,
	'K', 4, 0, 4, 6, 0, 0, 0x9a, 0xca, 0x90,
	'L', 3, 0, 3, 6, 0, 0, 0x92, 0x4e, 0x00,
	'M', 5, 0, 5, 6, 0, 0, 0x8e, 0xeb, 0x18, 0x80,
	'N', 4, 0, 4, 6, 0, 0, 0x9d, 0xb9, 0x90,
	'O', 4, 0, 4, 6, 0, 0, 0x69, 0x99, 0x60,
	'P', 4, 0, 4, 6, 0, 0, 0xe9, 0xe8, 0x80,
	'Q', 4, 0, 4, 6, 0, 0, 0x69, 0x9b, 0x70,
	'R', 4, 0, 4, 6, 0, 0, 0xe9, 0xea, 0x90,
	'S', 4, 0, 4, 6, 0, 0, 0x78, 0x61, 0xe0,
	'T', 3, 0, 3, 6, 0, 0, 0xe9, 0x24, 0x00,
	'U', 4, 0, 4, 6, 0, 0, 0x99, 0x99, 0x60,
	'V', 4, 0, 4, 6, 0, 0, 0x99, 0x96, 0x60,
	'W', 5, 0, 5, 6, 0, 0, 0x8c, 0x6b, 0x55, 0x00,
	'X', 4, 0, 4, 6, 0, 0, 0x99, 0x69, 0x90,
	'Y', 3, 0, 3, 6, 0, 0, 0xb5, 0x24, 0x00,
	'Z', 4, 0, 4, 6, 0, 0, 0xf2, 0x48, 0xf0,

	'a', 4, 0, 4, 6, 0, 0, 0x69, 0xf9, 0x90,
	'b', 4, 0, 4, 6, 0, 0, 0xe9, 0xe9, 0xe0,
	'c', 4, 0, 4, 6, 0, 0, 0x78, 0x88, 0x70,
	'd', 4, 0, 4, 6, 0, 0, 0xe9, 0x99, 0xe0,
	'e', 4, 0, 4, 6, 0, 0, 0xf8, 0xe8, 0xf0,
	'f', 4, 0, 4, 6, 0, 0, 0xf8, 0xe8, 0x80,
	'g', 4, 0, 4, 6, 0, 0, 0x78, 0xb9, 0x70,
	'h', 4, 0, 4, 6, 0, 0, 0x99, 0xf9, 0x90,
	'i', 3, 0, 3, 6, 0, 0, 0xe9, 0x2e, 0x00,
	'j', 4, 0, 4, 6, 0, 0, 0xf2, 0x2a, 0x40,
	'k', 4, 0, 4, 6, 0, 0, 0x9a, 0xca, 0x90,
	'l', 3, 0, 3, 6, 0, 0, 0x92, 0x4e, 0x00,
	'm', 5, 0, 5, 6, 0, 0, 0x8e, 0xeb, 0x18, 0x80,
	'n', 4, 0, 4, 6, 0, 0, 0x9d, 0xb9, 0x90,
	'o', 4, 0, 4, 6, 0, 0, 0x69, 0x99, 0x60,
	'p', 4, 0, 4, 6, 0, 0, 0xe9, 0xe8, 0x80,
	'q', 4, 0, 4, 6, 0, 0, 0x69, 0x9b, 0x70,
	'r', 4, 0, 4, 6, 0, 0, 0xe9, 0xea, 0x90,
	's', 4, 0, 4, 6, 0, 0, 0x78, 0x61, 0xe0,
	't', 3, 0, 3, 6, 0, 0, 0xe9, 0x24, 0x00,
	'u', 4, 0, 4, 6, 0, 0, 0x99, 0x99, 0x60,
	'v', 4, 0, 4, 6, 0, 0, 0x99, 0x96, 0x60,
	'w', 5, 0, 5, 6, 0, 0, 0x8c, 0x6b, 0x55, 0x00,
	'x', 4, 0, 4, 6, 0, 0, 0x99, 0x69, 0x90,
	'y', 3, 0, 3, 6, 0, 0, 0xb5, 0x24, 0x00,
	'z', 4, 0, 4, 6, 0, 0, 0xf2, 0x48, 0xf0,
	0,
};

// A medium font map
static const unsigned char font_map_medium[] =
{
	// bbox
	7, 10, 10, 0,
	
	// Default glyph
	'.', 2, 0, 2, 10, 0, 0,	0x00, 0x3c, 0x00,

	// Tab width
	'\t', 10, 0, 0, 0, 0, 0,

	// Space
	' ', 2, 0, 0, 0, 0, 0,

	'!', 1, 0, 1, 10, 0, 0, 0xf6, 0x00,
	'(', 3, 0, 3, 10, 0, 0, 0x2a, 0x48, 0x88, 0x00,
	')', 3, 0, 3, 10, 0, 0, 0x88, 0x92, 0xa0, 0x00,
	',', 2, 0, 2, 10, 0, 0, 0x00, 0x16, 0x00,
	'-', 3, 0, 3, 10, 0, 0, 0x00, 0x70, 0x00, 0x00,
	'/', 3, 0, 3, 10, 0, 0, 0x25, 0x25, 0x20, 0x00,
	'@', 6, 0, 6, 10, 0, 0, 0x7a, 0x19, 0x6b, 0x9a, 0x07, 0x80, 0x00, 0x00,
	'$', 5, 0, 5, 10, 0, 0, 0x23, 0xab, 0x47, 0x16, 0xae, 0x20, 0x00,
	'#', 6, 0, 6, 10, 0, 0, 0x49, 0x2f, 0xd2, 0xfd, 0x24, 0x80, 0x00, 0x00,
	'%', 7, 0, 7, 10, 0, 0, 0x43, 0x49, 0x20, 0x82, 0x49, 0x61, 0x00, 0x00, 0x00,
	':', 2, 0, 2, 10, 0, 0, 0x3c, 0xf0, 0x00,
	'^', 3, 0, 3, 10, 0, 0, 0x54, 0x00, 0x00, 0x00,
	'~', 5, 0, 5, 10, 0, 0, 0x00, 0x11, 0x51, 0x00, 0x00, 0x00, 0x00,

	'0', 5, 0, 5, 10, 0, 0, 0x74, 0x73, 0x59, 0xc5, 0xc0, 0x00, 0x00,
	'1', 3, 0, 3, 10, 0, 0, 0xc9, 0x24, 0xb8, 0x00,
	'2', 5, 0, 5, 10, 0, 0, 0x74, 0x42, 0xe8, 0x43, 0xe0, 0x00, 0x00,
	'3', 5, 0, 5, 10, 0, 0, 0x74, 0x42, 0xe0, 0xc5, 0xc0, 0x00, 0x00,
	'4', 5, 0, 5, 10, 0, 0, 0x11, 0x95, 0x2f, 0x88, 0x40, 0x00, 0x00,
	'5', 5, 0, 5, 10, 0, 0, 0xfc, 0x3c, 0x10, 0xc5, 0xc0, 0x00, 0x00,
	'6', 5, 0, 5, 10, 0, 0, 0x74, 0x61, 0xe8, 0xc5, 0xc0, 0x00, 0x00,
	'7', 5, 0, 5, 10, 0, 0, 0xfc, 0x44, 0x42, 0x10, 0x80, 0x00, 0x00,
	'8', 5, 0, 5, 10, 0, 0, 0x74, 0x62, 0xe8, 0xc5, 0xc0, 0x00, 0x00,
	'9', 5, 0, 5, 10, 0, 0, 0x74, 0x62, 0xf0, 0xc5, 0xc0, 0x00, 0x00,

	'A', 6, 0, 6, 10, 0, 0, 0x31, 0x28, 0x7f, 0x86, 0x18, 0x40, 0x00, 0x00,
	'B', 6, 0, 6, 10, 0, 0, 0xfa, 0x18, 0x7e, 0x86, 0x1f, 0x80, 0x00, 0x00,
	'C', 6, 0, 6, 10, 0, 0, 0x7a, 0x18, 0x20, 0x82, 0x17, 0x80, 0x00, 0x00,
	'D', 6, 0, 6, 10, 0, 0, 0xfa, 0x18, 0x61, 0x86, 0x1f, 0x80, 0x00, 0x00,
	'E', 6, 0, 6, 10, 0, 0, 0xfe, 0x08, 0x3c, 0x82, 0x0f, 0xc0, 0x00, 0x00,
	'F', 6, 0, 6, 10, 0, 0, 0xfe, 0x08, 0x3c, 0x82, 0x08, 0x00, 0x00, 0x00,
	'G', 6, 0, 6, 10, 0, 0, 0x7a, 0x18, 0x27, 0x86, 0x17, 0xc0, 0x00, 0x00,
	'H', 6, 0, 6, 10, 0, 0, 0x86, 0x18, 0x7f, 0x86, 0x18, 0x40, 0x00, 0x00,
	'I', 3, 0, 3, 10, 0, 0, 0xe9, 0x24, 0xb8, 0x00,
	'J', 6, 0, 6, 10, 0, 0, 0xfc, 0x41, 0x04, 0x12, 0x46, 0x00, 0x00, 0x00,
	'K', 5, 0, 5, 10, 0, 0, 0x8c, 0xa9, 0x8a, 0x4a, 0x20, 0x00, 0x00,
	'L', 4, 0, 4, 10, 0, 0, 0x88, 0x88, 0x88, 0xf0, 0x00,
	'M', 6, 0, 6, 10, 0, 0, 0x87, 0x3b, 0x61, 0x86, 0x18, 0x40, 0x00, 0x00,
	'N', 5, 0, 5, 10, 0, 0, 0x8e, 0x6b, 0x38, 0xc6, 0x20, 0x00, 0x00,
	'O', 6, 0, 6, 10, 0, 0, 0x7a, 0x18, 0x61, 0x86, 0x17, 0x80, 0x00, 0x00,
	'P', 5, 0, 5, 10, 0, 0, 0xf4, 0x63, 0xe8, 0x42, 0x00, 0x00, 0x00,
	'Q', 6, 0, 6, 10, 0, 0, 0x7a, 0x18, 0x61, 0x86, 0x57, 0x81, 0x00, 0x00,
	'R', 5, 0, 5, 10, 0, 0, 0xf4, 0x63, 0xe8, 0xc6, 0x20, 0x00, 0x00,
	'S', 6, 0, 6, 10, 0, 0, 0x7a, 0x18, 0x1e, 0x06, 0x17, 0x80, 0x00, 0x00,
	'T', 3, 0, 3, 10, 0, 0, 0xe9, 0x24, 0x90, 0x00,
	'U', 6, 0, 6, 10, 0, 0, 0x86, 0x18, 0x61, 0x86, 0x17, 0x80, 0x00, 0x00,
	'V', 6, 0, 6, 10, 0, 0, 0x86, 0x18, 0x61, 0x85, 0x23, 0x00, 0x00, 0x00,
	'W', 7, 0, 7, 10, 0, 0, 0x83, 0x06, 0x4c, 0x99, 0x35, 0x51, 0x00, 0x00, 0x00,
	'X', 5, 0, 5, 10, 0, 0, 0x8c, 0x54, 0x45, 0x46, 0x20, 0x00, 0x00,
	'Y', 5, 0, 5, 10, 0, 0, 0x8c, 0x54, 0x42, 0x10, 0x80, 0x00, 0x00,
	'Z', 6, 0, 6, 10, 0, 0, 0xfc, 0x10, 0x84, 0x21, 0x0f, 0xc0, 0x00, 0x00,

	'a', 4, 0, 4, 10, 0, 0, 0x00, 0x61, 0x79, 0x70, 0x00,
	'b', 4, 0, 4, 10, 0, 0, 0x88, 0xe9, 0x99, 0xe0, 0x00,
	'c', 4, 0, 4, 10, 0, 0, 0x00, 0x78, 0x88, 0x70, 0x00,
	'd', 4, 0, 4, 10, 0, 0, 0x11, 0x79, 0x99, 0x70, 0x00,
	'e', 4, 0, 4, 10, 0, 0, 0x00, 0x69, 0xf8, 0x60, 0x00,
	'f', 4, 0, 4, 10, 0, 0, 0x25, 0x4e, 0x44, 0x40, 0x00,
	'g', 4, 0, 4, 10, 0, 0, 0x00, 0x79, 0x99, 0x71, 0x60,
	'h', 4, 0, 4, 10, 0, 0, 0x88, 0xe9, 0x99, 0x90, 0x00,
	'i', 1, 0, 1, 10, 0, 0, 0xbe, 0x00,
	'j', 2, 0, 2, 10, 0, 0, 0x04, 0x55, 0x80,
	'k', 4, 0, 4, 10, 0, 0, 0x89, 0xac, 0xca, 0x90, 0x00,
	'l', 3, 0, 3, 10, 0, 0, 0xc9, 0x24, 0x98, 0x00,
	'm', 5, 0, 5, 10, 0, 0, 0x00, 0x15, 0x5a, 0xd6, 0x20, 0x00, 0x00,
	'n', 4, 0, 4, 10, 0, 0, 0x00, 0xe9, 0x99, 0x90, 0x00,
	'o', 4, 0, 4, 10, 0, 0, 0x00, 0x69, 0x99, 0x60, 0x00,
	'p', 4, 0, 4, 10, 0, 0, 0x00, 0xe9, 0x99, 0xe8, 0x80,
	'q', 4, 0, 4, 10, 0, 0, 0x00, 0x79, 0x97, 0x11, 0x10,
	'r', 3, 0, 3, 10, 0, 0, 0x02, 0xe9, 0x20, 0x00,
	's', 4, 0, 4, 10, 0, 0, 0x00, 0x78, 0x61, 0xe0, 0x00,
	't', 3, 0, 3, 10, 0, 0, 0x4b, 0xa4, 0x88, 0x00,
	'u', 4, 0, 4, 10, 0, 0, 0x00, 0x99, 0x99, 0x70, 0x00,
	'v', 4, 0, 4, 10, 0, 0, 0x00, 0x99, 0x99, 0x60, 0x00,
	'w', 5, 0, 5, 10, 0, 0, 0x00, 0x23, 0x1a, 0xd5, 0x40, 0x00, 0x00,
	'x', 5, 0, 5, 10, 0, 0, 0x00, 0x22, 0xa2, 0x2a, 0x20, 0x00, 0x00,
	'y', 4, 0, 4, 10, 0, 0, 0x00, 0x99, 0x99, 0x71, 0x60,
	'z', 4, 0, 4, 10, 0, 0, 0x00, 0xf1, 0x24, 0xf0, 0x00,

	0,

};

const unsigned char* ezd_next_glyph(const unsigned char* pGlyph)
{
	int sz;

	// Last glyph?
	if (!pGlyph || !*pGlyph)
		return 0;
	tGlyph* _pGlyph = (tGlyph*)pGlyph;
	// keeping with the old packing (not as BDF where width bits padded to even byte)
	sz = ((_pGlyph->bbox.width * _pGlyph->bbox.height) + 7) / 8;
	//sz = pGlyph[1] * pGlyph[2];

	// Return a pointer to the next glyph
	return &pGlyph[sizeof(tGlyph) + sz];
}

const void* ezd_find_glyph(HEZDFONT x_pFt, const unsigned char ch)
{
#if !defined( EZD_STATIC_FONTS )

	SFontData *f = (SFontData*)x_pFt;

	// Ensure valid font pointer
	if (!f)
		return 0;

	// Get a pointer to the glyph
	return f->pIndex[ch];
#else

	const unsigned char* pGlyph = (const signed char*)x_pFt;

	// Find the glyph
	while (pGlyph && *pGlyph)
		if (ch == *pGlyph)
			return pGlyph;
		else
			pGlyph = ezd_next_glyph(pGlyph);

	// First glyph is the default
	return (const char*)x_pFt;

#endif
}

font_ident_t* ezd_get_font_id(HEZDFONT hFont)
{
	SFontData* data = (SFontData*)hFont;
	return &(data->ID);
}

int ezd_font_pixel_size(HEZDFONT hFont)
{
	font_ident_t* ident = ezd_get_font_id(hFont);
	// hacking: compare ascents, as we have Capital only font in the mix
	return ident->bbx_height + ident->bbx_yoffset;
}

void ezd_font_id_string(char* buffer, HEZDFONT hFont)
{
	SFontData* font = (SFontData*)hFont;
	char avgString[4] = "?";
	if (font->ID.average_width_tenths != -1)
		sprintf(avgString, "%d", font->ID.average_width_tenths);
	// generate id from [filename:4]|[bbx:height]|[bbx:yoffset]|[(avglower100+avgupper100)/2]
	sprintf(buffer, "%s;%d;%d;%s", font->ID.fileID, font->ID.bbx_height, font->ID.bbx_yoffset, avgString);
}

int ezd_compare_fonts(HEZDFONT a, HEZDFONT b)
{
	int ret = 0;
	int sizeA = 0;
	int sizeB = 0;
	font_ident_t* identA = ezd_get_font_id(a);
	font_ident_t* identB = ezd_get_font_id(b);
	sizeA = identA->bbx_height + identA->bbx_yoffset;
	sizeB = identB->bbx_height + identB->bbx_yoffset;
	ret = (sizeA - sizeB);
	if (ret == 0)
	{
		ret = identA->average_width_tenths - identB->average_width_tenths;
	}
	return ret;
}

HEZDFONT ezd_load_font(const void *x_pFt, int x_nFtSize, unsigned int x_uFlags, font_ident_t* x_pIdent)
{
#if !defined( EZD_STATIC_FONTS )

	int i, sz;
	SFontData *p;
	const bbxFont* pBbx = NULL;
	const unsigned char* pGlyph = NULL;
	const unsigned char *pFt = (const unsigned char*)x_pFt;

	// Font parameters
	if (!pFt)
		return _ERR((HEZDFONT)0, "Invalid parameters");

	// Check for built in small font
	if (EZD_FONT_TYPE_SMALL == pFt)
		pFt = font_map_medium, x_nFtSize = sizeof(font_map_medium) - sizeof(bbxFont);

	// Check for built in large font
	else if (EZD_FONT_TYPE_MEDIUM == pFt)
		pFt = font_map_medium, x_nFtSize = sizeof(font_map_medium) - sizeof(bbxFont);

	// Check for built in large font
	else if (EZD_FONT_TYPE_LARGE == pFt)
		pFt = font_map_medium, x_nFtSize = sizeof(font_map_medium) - sizeof(bbxFont);

	pBbx = (bbxFont*)pFt;
	pGlyph = pFt + sizeof(bbxFont);
	/// Null terminated font buffer?
	if (0 >= x_nFtSize)
	{
		x_nFtSize = 0;
		while (pGlyph[x_nFtSize])
		{
			tGlyph* _pGlyph = (tGlyph*)(&pGlyph[x_nFtSize]);
			sz = ((_pGlyph->bbox.width * _pGlyph->bbox.height) + 7) / 8;
			x_nFtSize += sizeof(tGlyph) + sz;
		} // end while
	} // end if

	  // Sanity check
	if (0 >= x_nFtSize)
		return _ERR((HEZDFONT)0, "Empty font table");

	// Allocate space for font buffer
	p = (SFontData*)EZD_malloc(sizeof(SFontData) + x_nFtSize);
	if (!p)
		return 0;

	// Copy the font bitmaps
	EZD_MEMCPY(p->pGlyph, pGlyph, x_nFtSize);
	EZD_MEMCPY((char*)p, (const char*)pBbx, sizeof(bbxFont));
	// Save font flags
	p->uFlags = x_uFlags;
	p->spacing = (x_uFlags >> EZD_FONT_FLAG_SPACING_POS) & EZD_FONT_FLAG_SPACING_MASK;
		
	if (x_pIdent)
	{
		strcpy(p->ID.fileID, x_pIdent->fileID);
		p->ID.bbx_height = x_pIdent->bbx_height;
		p->ID.bbx_yoffset = x_pIdent->bbx_yoffset;
		p->ID.average_width_tenths = x_pIdent->average_width_tenths;
	}
	else
	{
		int dftID = (int)((long)x_pFt);
		if (dftID >= 10)
			dftID = 0;
		sprintf(p->ID.fileID, "DFT%d", dftID);
		p->ID.bbx_height = p->bbox.height;
		p->ID.bbx_yoffset = p->bbox.yoffset;
		p->ID.average_width_tenths = -1;
	}
	
	// Use the first character as the default glyph
	for (i = 0; i < 256; i++)
		p->pIndex[i] = p->pGlyph;

	// Index the glyphs
	pGlyph = p->pGlyph;
	do // first glyph encoding can be '\0' thus do-while
	{
		p->pIndex[*pGlyph] = pGlyph;
		pGlyph = ezd_next_glyph(pGlyph);
	} while (pGlyph && *pGlyph != 0);
	
	// Return the font handle
	return (HEZDFONT)p;

#else

	// Convert type
	const unsigned char *pFt = (const unsigned char*)x_pFt;

	// Font parameters
	if (!pFt)
		return _ERR((HEZDFONT)0, "Invalid parameters");

	// Check for built in small font
	if (EZD_FONT_TYPE_SMALL == pFt)
		return (HEZDFONT)font_map_medium;

	// Check for built in large font
	else if (EZD_FONT_TYPE_MEDIUM == pFt)
		return (HEZDFONT)font_map_medium;

	// Check for built in large font
	else if (EZD_FONT_TYPE_LARGE == pFt)
		return (HEZDFONT)font_map_medium;

	// Just use the users raw font table pointer
	else
		return (HEZDFONT)x_pFt;

#endif
}

/// Releases the specified font
void ezd_destroy_font(HEZDFONT x_hFont)
{
#if !defined( EZD_STATIC_FONTS )

	if (x_hFont)
		EZD_free((SFontData*)x_hFont);

#endif
}

int ezd_text_size(HEZDFONT x_hFont, const char *x_pText, int x_nTextLen, int *pw, int *ph)
{
	int i, w, h, lw = 0, lh = 0;
	const tGlyph* _pGlyph;

	// Sanity check
	if (!x_hFont || !pw || !ph)
		return _ERR(0, "Invalid parameters");

	// Set all sizes to zero
	*pw = *ph = 0;

	// For each character in the string
	for (i = 0; i < x_nTextLen || (0 > x_nTextLen && x_pText[i]); i++)
	{
		// Get the specified glyph
		_pGlyph = (tGlyph*)ezd_find_glyph(x_hFont, x_pText[i]);

		switch (x_pText[i])
		{
			// CR
		case '\r':

			// Reset width, and grab current height
			w = 0; //h = lh;
			i += ezd_text_size(x_hFont, &x_pText[i + 1], x_nTextLen - i - 1, &w, &lh);

			// Take the largest width / height
			*pw = (*pw > w) ? *pw : w;
			//lh = ( lh > h ) ? lh : h;

			break;

			// LF
		case '\n':

			// New line
			w = 0; h = 0;
			i += ezd_text_size(x_hFont, &x_pText[i + 1], x_nTextLen - i - 1, &w, &h);

			// Take the longest width
			*pw = (*pw > w) ? *pw : w;

			// Add the height
			*ph += h;

			break;

			// Regular character
		default:

			// Accumulate width / height
			lw += !lw ? _pGlyph->xoffsetnext : (((SFontData*)x_hFont)->spacing + _pGlyph->xoffsetnext),
				lh = (((_pGlyph->bbox.height + abs(_pGlyph->bbox.yoffset)) > lh) ? (_pGlyph->bbox.height + abs(_pGlyph->bbox.yoffset)) : lh);

			break;

		} // end switch

	} // end for

	  // Take the longest width
	*pw = (*pw > lw) ? *pw : lw;

	// Add our line height
	*ph += lh;

	return i;
}
