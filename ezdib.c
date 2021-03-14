/*------------------------------------------------------------------
// Copyright (c) 1997 - 2012
// Robert Umbehant
// ezdib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "ezdib.h"
#include "ezdibconf.h"

//------------------------------------------------------------------
// Data structures
//------------------------------------------------------------------

#if !defined( EZD_NOPACK )
#	pragma pack( push, 1 )
#endif

/// DIB file magic number
#define EZD_MAGIC_NUMBER	0x4d42

/// Header for a standard dib file (.bmp)
typedef struct _SDIBFileHeader
{
	/// Magic number, must be 0x42 0x4D (BM)
	unsigned short	uMagicNumber;

	/// Size of the file in bytes
	unsigned int	uSize;

	/// Reserved
	unsigned short	uReserved1;

	/// Reserved
	unsigned short	uReserved2;

	/// Offset to start of image data
	unsigned int	uOffset;

} SDIBFileHeader;

/// Standard bitmap structure
typedef struct _SBitmapInfoHeader
{
	/// Size of this structure
	unsigned int			biSize;

	/// Image width
	int						biWidth;

	/// Image height
	int						biHeight;

	/// Number of bit planes in the image
	unsigned short			biPlanes;

	/// Bits per pixel / color depth
	unsigned short			biBitCount;

	/// Type of compression used
	unsigned int			biCompression;

	/// The total size of the image data
	unsigned int			biSizeImage;

	/// Horizontal resolution in pixels per meter
	int						biXPelsPerMeter;

	/// Vertical resolution in pixels per meter
	int						biYPelsPerMeter;

	/// Total number of colors actually used in the image,
	/// zero for all colors used.
	unsigned int			biClrUsed;

	/// Number of colors required for displaying the image,
	/// zero for all colors required.
	unsigned int			biClrImportant;

} SBitmapInfoHeader;

#	define EZD_FLAG_FREE_BUFFER		0x00010000

// Returns non-zero if any color components are greater than the threshold
#	define EZD_COMPARE_THRESHOLD( c, t ) ( ( c & 0xff ) > t \
										 || ( ( c >> 8 ) & 0xff ) > t \
										 || ( ( c >> 16 ) & 0xff ) > t )

// This structure contains the memory image
typedef struct _SImageData
{
	/// Windows compatible image information
	SBitmapInfoHeader		bih;

	/// Color palette for 1 bit images
	int						colPalette[ 2 ];

	/// Threshold color for 1 bit images
	int						colThreshold;

	/// Image flags
	unsigned int			uFlags;

	/// User set pixel callback function
	t_ezd_set_pixel			pfSetPixel;

	/// User data passed to set pixel callback function
	void					*pSetPixelUser;

	/// User image pointer
	unsigned char			*pImage;

	/// Image data
	unsigned char			pBuffer[ 4 ];

} SImageData;

#if !defined( EZD_NOPACK )
#	pragma pack( pop )
#endif

void ezd_destroy( HEZDIMAGE x_hDib )
{
#if !defined( EZD_NO_ALLOCATION )
	if ( x_hDib )
	{	SImageData *p = (SImageData*)x_hDib;
		if ( EZD_FLAG_FREE_BUFFER & p->uFlags )
			EZD_free( (SImageData*)x_hDib );
	} // end if
#endif
}

int ezd_header_size()
{
	return sizeof( SImageData );
}

HEZDIMAGE ezd_initialize( void *x_pBuffer, int x_nBuffer, int x_lWidth, int x_lHeight, int x_lBpp, unsigned int x_uFlags )
{
	int nImageSize;
	SImageData *p;

	// Ensure the user buffer is acceptable
	if ( !x_pBuffer || ( 0 < x_nBuffer && sizeof( SImageData ) > x_nBuffer ) )
		return _ERR( (HEZDIMAGE)0, "Invalid header buffer" );

	// Sanity check
	if ( !x_lWidth || !x_lHeight )
		return _ERR( (HEZDIMAGE)0, "Invalid parameters" );

	// Calculate image size
	nImageSize = EZD_IMAGE_SIZE( x_lWidth, x_lHeight, x_lBpp, 4 );
	if ( 0 >= nImageSize )
		return _ERR( (HEZDIMAGE)0, "Invalid bits per pixel" );

	// Point to users buffer
	p = (SImageData*)x_pBuffer;

	// Initialize the memory
	EZD_MEMSET( (char*)p, 0, sizeof( SImageData ) );

	// Initialize image metrics
	p->bih.biSize = sizeof( SBitmapInfoHeader );
	p->bih.biWidth = x_lWidth;
	p->bih.biHeight = x_lHeight;
	p->bih.biPlanes = 1;
	p->bih.biBitCount = x_lBpp;
	p->bih.biSizeImage = nImageSize;

	// Initialize color palette
	if ( 1 == x_lBpp )
	{	p->bih.biClrUsed = 2;
		p->bih.biClrImportant = 2;
		p->colPalette[ 0 ] = 0;
		p->colPalette[ 1 ] = 0xffffff;
	} // end if

	// Point image buffer
	p->pImage = ( EZD_FLAG_USER_IMAGE_BUFFER & x_uFlags ) ? 0 : p->pBuffer;

	// Save the flags
	p->uFlags = x_uFlags;

	return (HEZDIMAGE)p;
}


HEZDIMAGE ezd_create( int x_lWidth, int x_lHeight, int x_lBpp, unsigned int x_uFlags )
{
#if defined( EZD_NO_ALLOCATION )
	return 0;
#else
	int nImageSize;
	SImageData *p;

	// Make sure the caller isn't stepping on our internal flags
	if ( 0xffff0000 & x_uFlags )
		return _ERR( (HEZDIMAGE)0, "You have specified invalid flags" );

	// Sanity check
	if ( !x_lWidth || !x_lHeight )
		return _ERR( (HEZDIMAGE)0, "Invalid image width or height" );

	// Calculate image size
	nImageSize = EZD_IMAGE_SIZE( x_lWidth, x_lHeight, x_lBpp, 4 );
	if ( 0 >= nImageSize )
		return _ERR( (HEZDIMAGE)0, "Invalid bits per pixel" );

	// Allocate memory
	p = (SImageData*)EZD_malloc( sizeof( SImageData )
								 + ( ( EZD_FLAG_USER_IMAGE_BUFFER & x_uFlags ) ? 0 : nImageSize ) );

	if ( !p )
		return 0;

	// Initialize the header
	return ezd_initialize( p, sizeof( SImageData ), x_lWidth, x_lHeight, x_lBpp, x_uFlags | EZD_FLAG_FREE_BUFFER );
#endif
}

int ezd_set_image_buffer( HEZDIMAGE x_hDib, void *x_pImg, int x_nImg )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
	{	_MSG( "Invalid parameters" ); return 0; }

	// Verify image buffer size if needed
	if ( x_pImg && 0 < x_nImg && x_nImg < (int)p->bih.biSizeImage )
	{	_MSG( "Invalid user image buffer size" ); return 0; }

	// Save user image pointer
	p->pImage = ( !x_pImg && !( EZD_FLAG_USER_IMAGE_BUFFER & p->uFlags ) )
				? p->pBuffer : x_pImg;
	return 1;
}

int ezd_set_pixel_callback( HEZDIMAGE x_hDib, t_ezd_set_pixel x_pf, void *x_pUser )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	// Save user callback info
	p->pfSetPixel = x_pf;
	p->pSetPixelUser = x_pUser;

	return 1;
}


int ezd_set_palette_color( HEZDIMAGE x_hDib, int x_idx, int x_col )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	if ( 0 > x_idx || 1 < x_idx )
		return _ERR( 0, "Palette index out of range" );

	// Set this palette color
	p->colPalette[ x_idx ] = x_col;

	return 1;
}

int ezd_get_palette_color( HEZDIMAGE x_hDib, int x_idx, int x_col )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	if ( 0 > x_idx || 1 < x_idx )
		return _ERR( 0, "Palette index out of range" );

	// Return this palette color
	return p->colPalette[ x_idx ];
}

int* ezd_get_palette( HEZDIMAGE x_hDib )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( (int*)0, "Invalid parameters" );

	// Return a pointer to the palette
	return p->colPalette;
}

int ezd_get_palette_size( HEZDIMAGE x_hDib )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	switch( p->bih.biBitCount )
	{
		case 1 :
			return 2;
	} // end switch

	return 0;
}

int ezd_set_color_threshold( HEZDIMAGE x_hDib, int x_col )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	// Calculate scan width
	p->colThreshold = x_col;

	return 1;
}

int ezd_get_width( HEZDIMAGE x_hDib )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	// Calculate scan width
	return p->bih.biWidth;
}

int ezd_get_height( HEZDIMAGE x_hDib )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	// Calculate scan width
	return p->bih.biHeight;
}

int ezd_get_bpp( HEZDIMAGE x_hDib )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	// Calculate scan width
	return p->bih.biBitCount;
}

int ezd_get_image_size( HEZDIMAGE x_hDib )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( 0, "Invalid parameters" );

	// Calculate scan width
	return p->bih.biSizeImage;
}


void* ezd_get_image_ptr( HEZDIMAGE x_hDib )
{
	SImageData *p = (SImageData*)x_hDib;
	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize )
		return _ERR( (void*)0, "Invalid parameters" );

	// Calculate scan width
	return p->pImage;
}


int ezd_save( HEZDIMAGE x_hDib, const char *x_pFile )
{
#if defined( EZD_NO_FILES )
	return 0;
#else
	FILE *fh;
	int palette_size = 0;
	SDIBFileHeader dfh;
	SImageData *p = (SImageData*)x_hDib;

	// Sanity checks
	if ( !x_pFile || !*x_pFile || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize || !p->pImage )
		return _ERR( 0, "Invalid parameters" );

	// Ensure packing is ok
	if ( sizeof( SDIBFileHeader ) != 14 )
		return _ERR( 0, "Structure packing for DIB header is incorrect" );

	// Ensure packing is ok
	if ( sizeof( SBitmapInfoHeader ) != 40 )
		return _ERR( 0, "Structure packing for BITMAP header is incorrect" );

	// Add palettte size
	if ( 1 == p->bih.biBitCount )
		palette_size = sizeof( p->colPalette[ 0 ] ) * 2;

	// Attempt to open the output file
	fh = fopen ( x_pFile, "wb" );
	if ( !fh )
		return _ERR( 0, "Failed to open DIB file for writing" );

	// Fill in header info
	dfh.uMagicNumber = EZD_MAGIC_NUMBER;
	dfh.uSize = sizeof( SDIBFileHeader ) + p->bih.biSize + p->bih.biSizeImage;
	dfh.uReserved1 = 0;
	dfh.uReserved2 = 0;
	dfh.uOffset = sizeof( SDIBFileHeader ) + p->bih.biSize + palette_size;

	// Write the header
	if ( sizeof( dfh ) != fwrite( &dfh, 1, sizeof( dfh ), fh ) )
	{	fclose( fh ); return _ERR( 0, "Error writing DIB header" ); }

	// Write the Bitmap header
	if ( p->bih.biSize != fwrite( &p->bih, 1, p->bih.biSize, fh ) )
	{	fclose( fh ); return _ERR( 0, "Error writing bitmap header" ); }

	// Write the color palette if needed
	if ( 0 < palette_size )
		if ( sizeof( p->colPalette ) != fwrite( p->colPalette, 1, palette_size, fh ) )
		{	fclose( fh ); return _ERR( 0, "Error writing palette" ); }

	// Write the Image data
	if ( p->bih.biSizeImage != fwrite( p->pImage, 1, p->bih.biSizeImage, fh ) )
	{	fclose( fh ); return _ERR( 0, "Error writing image data" ); }

	// Close the file handle
	fclose( fh );

	return 1;
#endif
}

int ezd_fill( HEZDIMAGE x_hDib, int x_col )
{
	int w, h, sw, pw, x, y;
	SImageData *p = (SImageData*)x_hDib;

	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize
		 || ( !p->pImage && !p->pfSetPixel ) )
		return _ERR( 0, "Invalid parameters" );

	// Calculate image metrics
	w = EZD_ABS( p->bih.biWidth );
	h = EZD_ABS( p->bih.biHeight );

	// Check for user callback function
	if ( p->pfSetPixel )
	{
		// Fill each pixel
		for ( y = 0; y < h; y++ )
			for( x = 0; x < w; x++ )
				if ( !p->pfSetPixel( p->pSetPixelUser, x, y, x_col, 0 ) )
					return 0;

		return 1;

	} // end if

	// Pixel and scan widths
	pw = EZD_FITTO( p->bih.biBitCount, 8 );
	sw = EZD_SCANWIDTH( w, p->bih.biBitCount, 4 );

	// Set the first line
	switch( p->bih.biBitCount )
	{
		case 1 :
			EZD_MEMSET( p->pImage, EZD_COMPARE_THRESHOLD( x_col, p->colThreshold ) ? 0xff : 0, sw );
			break;

		case 24 :
		{
			// Color values
			unsigned char r = x_col & 0xff;
			unsigned char g = ( x_col >> 8 ) & 0xff;
			unsigned char b = ( x_col >> 16 ) & 0xff;
			unsigned char *pImg = p->pImage;

			// Set the first line
			for( x = 0; x < w; x++, pImg += pw )
				pImg[ 0 ] = r, pImg[ 1 ] = g, pImg[ 2 ] = b;

		} break;

		case 32 :
		{
			// Set the first line
			int *pImg = (int*)p->pImage;
			for( x = 0; x < w; x++, pImg++ )
				*pImg = x_col;

		} break;

		default :
			return 0;

	} // end switch

	// Copy remaining lines
	for( y = 1; y < h; y++ )
		EZD_MEMCPY( &p->pImage[ y * sw ], p->pImage, sw );

	return 1;
}

int ezd_set_pixel( HEZDIMAGE x_hDib, int x, int y, int x_col )
{
	int w, h, sw, pw;
	SImageData *p = (SImageData*)x_hDib;

	if ( !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize
		 || ( !p->pImage && !p->pfSetPixel ) )
		return _ERR( 0, "Invalid parameters" );

	// Calculate image metrics
	w = EZD_ABS( p->bih.biWidth );
	h = EZD_ABS( p->bih.biHeight );

	// Ensure pixel is within the image
	if ( 0 > x || x >= w || 0 > y || y >= h )
	{	_SHOW( "Point out of range : %d,%d : %dx%d ", x, y, w, h );
		return 0;
	} // en dif

	// Set the specified pixel
	if ( p->pfSetPixel )
		return p->pfSetPixel( p->pSetPixelUser, x, y, x_col, 0 );

	// Pixel and scan width
	pw = EZD_FITTO( p->bih.biBitCount, 8 );
	sw = EZD_SCANWIDTH( w, p->bih.biBitCount, 4 );

	// Set the first line
	switch( p->bih.biBitCount )
	{
		case 1 :
			p->pImage[ y * sw + ( x >> 3 ) ] |= 0x80 >> ( x & 7 );
			break;

		case 24 :
		{
			// Color values
			unsigned char r = x_col & 0xff;
			unsigned char g = ( x_col >> 8 ) & 0xff;
			unsigned char b = ( x_col >> 16 ) & 0xff;
			unsigned char *pImg = &p->pImage[ y * sw + x * pw ];

			// Set the pixel color
			pImg[ 0 ] = r, pImg[ 1 ] = g, pImg[ 2 ] = b;

		} break;

		case 32 :
			*(unsigned int*)&p->pImage[ y * sw + x * pw ] = x_col;
			break;

		default :
			return 0;

	} // end switch

	return 1;
}

int ezd_get_pixel( HEZDIMAGE x_hDib, int x, int y )
{
	int w, h, sw, pw;
	SImageData *p = (SImageData*)x_hDib;

	if ( !p || !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize || !p->pImage )
		return _ERR( 0, "Invalid parameters" );

	// Calculate image metrics
	w = EZD_ABS( p->bih.biWidth );
	h = EZD_ABS( p->bih.biHeight );

	// Ensure pixel is within the image
	if ( 0 > x || x >= w || 0 > y || y >= h )
	{	_SHOW( "Point out of range : %d,%d : %dx%d ", x, y, w, h );
		return 0;
	} // en dif

	// Pixel and scan width
	pw = EZD_FITTO( p->bih.biBitCount, 8 );
	sw = EZD_SCANWIDTH( w, p->bih.biBitCount, 4 );

	// Set the first line
	switch( p->bih.biBitCount )
	{
		case 1 :
			return p->colPalette[ ( p->pImage[ y * sw + ( x >> 3 ) ] & ( 0x80 >> ( x & 7 ) ) ) ? 1 : 0 ];

		case 24 :
		{
			// Return the color of the specified pixel
			unsigned char *pImg = &p->pImage[ y * sw + x * pw ];
			return pImg[ 0 ] | ( pImg[ 1 ] << 8 ) | ( pImg[ 2 ] << 16 );

		} break;

		case 32 :
			return *(unsigned int*)&p->pImage[ y * sw + x * pw ];

	} // end switch

	return 0;
}

int ezd_line( HEZDIMAGE x_hDib, int x1, int y1, int x2, int y2, int x_col )
{
	int w, h, sw, pw, xd, yd, xl, yl, done = 0;
	SImageData *p = (SImageData*)x_hDib;

	if ( !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize
		 || ( !p->pImage && !p->pfSetPixel ) )
		return _ERR( 0, "Invalid parameters" );

	// Calculate image metrics
	w = EZD_ABS( p->bih.biWidth );
	h = EZD_ABS( p->bih.biHeight );

	// Determine direction and distance
	xd = ( x1 < x2 ) ? 1 : -1;
	yd = ( y1 < y2 ) ? 1 : -1;
	xl = ( x1 < x2 ) ? ( x2 - x1 ) : ( x1 - x2 );
	yl = ( y1 < y2 ) ? ( y2 - y1 ) : ( y1 - y2 );

	// Check for user callback function
	if ( p->pfSetPixel )
	{
		int mx = 0, my = 0, c = EZD_COMPARE_THRESHOLD( x_col, p->colThreshold );
		static unsigned char xm[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

		// Draw the line
		while ( !done )
		{
			if ( x1 == x2 && y1 == y2 )
				done = 1;

			// Plot pixel
			if ( 0 <= x1 && x1 < w && 0 <= y1 && y1 < h )
				if ( !p->pfSetPixel( p->pSetPixelUser, x1, y1, x_col, 0 ) )
					return 0;

			mx += xl;
			if ( x1 != x2 && mx > yl )
				x1 += xd, mx -= yl;

			my += yl;
			if ( y1 != y2 && my > xl )
				y1 += yd, my -= xl;

		} // end while

		return 1;

	} // end if

	// Pixel and scan width
	pw = EZD_FITTO( p->bih.biBitCount, 8 );
	sw = EZD_SCANWIDTH( w, p->bih.biBitCount, 4 );

	// Set the first line
	switch( p->bih.biBitCount )
	{
		case 1 :
		{
			int mx = 0, my = 0, c = EZD_COMPARE_THRESHOLD( x_col, p->colThreshold );
			static unsigned char xm[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

			// Draw the line
			while ( !done )
			{
				if ( x1 == x2 && y1 == y2 )
					done = 1;

				// Plot pixel
				if ( 0 <= x1 && x1 < w && 0 <= y1 && y1 < h )
				{
					if ( c )
						p->pImage[ y1 * sw + ( x1 >> 3 ) ] |= xm[ x1 & 7 ];
					else
						p->pImage[ y1 * sw + ( x1 >> 3 ) ] &= ~xm[ x1 & 7 ];

				} // end if

				mx += xl;
				if ( x1 != x2 && mx > yl )
					x1 += xd, mx -= yl;

				my += yl;
				if ( y1 != y2 && my > xl )
					y1 += yd, my -= xl;

			} // end while

		} break;

		case 24 :
		{
			// Color values
			unsigned char r = x_col & 0xff;
			unsigned char g = ( x_col >> 8 ) & 0xff;
			unsigned char b = ( x_col >> 16 ) & 0xff;
			unsigned char *pImg;
			int mx = 0, my = 0;

			while ( !done )
			{
				if ( x1 == x2 && y1 == y2 )
					done = 1;

				// Plot pixel
				if ( 0 <= x1 && x1 < w && 0 <= y1 && y1 < h )
				{	pImg = &p->pImage[ y1 * sw + x1 * pw ];
					pImg[ 0 ] = r, pImg[ 1 ] = g, pImg[ 2 ] = b;
				} // end if

				mx += xl;
				if ( x1 != x2 && mx > yl )
					x1 += xd, mx -= yl;

				my += yl;
				if ( y1 != y2 && my > xl )
					y1 += yd, my -= xl;

			} // end while

		} break;

		case 32 :
		{
			// Color values
			int mx = 0, my = 0;

			// Draw the line
			while ( !done )
			{
				if ( x1 == x2 && y1 == y2 )
					done = 1;

				// Plot pixel
				if ( 0 <= x1 && x1 < w && 0 <= y1 && y1 < h )
					*(unsigned int*)&p->pImage[ y1 * sw + x1 * pw ] = x_col;

				mx += xl;
				if ( x1 != x2 && mx > yl )
					x1 += xd, mx -= yl;

				my += yl;
				if ( y1 != y2 && my > xl )
					y1 += yd, my -= xl;

			} // end while

		} break;

		default :
			return 0;

	} // end switch

	return 1;
}

int ezd_rect( HEZDIMAGE x_hDib, int x1, int y1, int x2, int y2, int x_col )
{
	// Draw rectangle
	return 		ezd_line( x_hDib, x1, y1, x2, y1, x_col )
		   && 	ezd_line( x_hDib, x2, y1, x2, y2, x_col )
		   &&	ezd_line( x_hDib, x2, y2, x1, y2, x_col )
		   &&	ezd_line( x_hDib, x1, y2, x1, y1, x_col );
}

#define EZD_PI		( (double)3.141592654 )
#define EZD_PI2		( EZD_PI * (double)2 )
#define EZD_PI4		( EZD_PI * (double)4 )

int ezd_arc( HEZDIMAGE x_hDib, int x, int y, int x_rad, double x_dStart, double x_dEnd, int x_col )
{
#if defined( EZD_NO_MATH )
	return 0;
#else
	double arc;
	int i, w, h, sw, pw, px, py;
	int res = (int)( (double)x_rad * EZD_PI4 ), resdraw;
	unsigned char *pImg;
	SImageData *p = (SImageData*)x_hDib;

	if ( !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize
		 || ( !p->pImage && !p->pfSetPixel ) )
		return _ERR( 0, "Invalid parameters" );

	// Dont' draw null arc
	if ( x_dStart == x_dEnd )
		return 1;

	// Ensure correct order
	else if ( x_dStart > x_dEnd )
	{	double t = x_dStart;
		x_dStart = x_dEnd;
		x_dEnd = t;
	} // end if

	// Get arc size
	arc = x_dEnd - x_dStart;

	// How many points to draw
	resdraw = ( EZD_PI2 <= arc ) ? res : (int)( arc * (double)res / EZD_PI2 );

	// Calculate image metrics
	w = EZD_ABS( p->bih.biWidth );
	h = EZD_ABS( p->bih.biHeight );

	// Ensure pixel is within the image
	if ( 0 > x || x >= w || 0 > y || y >= h )
	{	_SHOW( "Point out of range : %d,%d : %dx%d ", x, y, w, h );
		return 0;
	} // en dif

	// Check for user callback function
	if ( p->pfSetPixel )
	{
		// Draw the circle
		for ( i = 0; i < resdraw; i++ )
		{
			// Offset for this pixel
			px = x + (int)( (double)x_rad * cos( x_dStart + (double)i * EZD_PI2 / (double)res ) );
			py = y + (int)( (double)x_rad * sin( x_dStart + (double)i * EZD_PI2 / (double)res ) );

			// Plot pixel
			if ( 0 <= px && px < w && 0 <= py && py < h )
				if ( !p->pfSetPixel( p->pSetPixelUser, px, py, x_col, 0 ) )
					return 0;

		} // end while

		return 1;

	} // end if

	// Pixel and scan width
	pw = EZD_FITTO( p->bih.biBitCount, 8 );
	sw = EZD_SCANWIDTH( w, p->bih.biBitCount, 4 );

	switch( p->bih.biBitCount )
	{
		case 1:
		{
			int c = EZD_COMPARE_THRESHOLD( x_col, p->colThreshold );
			static unsigned char xm[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

			// Draw the circle
			for ( i = 0; i < resdraw; i++ )
			{
				// Offset for this pixel
				px = x + (int)( (double)x_rad * cos( x_dStart + (double)i * EZD_PI2 / (double)res ) );
				py = y + (int)( (double)x_rad * sin( x_dStart + (double)i * EZD_PI2 / (double)res ) );

				// Plot pixel
				if ( 0 <= px && px < w && 0 <= py && py < h )
				{
					if ( c )
						p->pImage[ py * sw + ( px >> 3 ) ] |= xm[ px & 7 ];
					else
						p->pImage[ py * sw + ( px >> 3 ) ] &= ~xm[ px & 7 ];

				} // end if

			} // end while

		} break;

		case 24 :
		{
			// Color values
			unsigned char r = x_col & 0xff;
			unsigned char g = ( x_col >> 8 ) & 0xff;
			unsigned char b = ( x_col >> 16 ) & 0xff;
			for ( i = 0; i < resdraw; i++ )
			{
				// Offset for this pixel
				px = x + (int)( (double)x_rad * cos( x_dStart + (double)i * EZD_PI2 / (double)res ) );
				py = y + (int)( (double)x_rad * sin( x_dStart + (double)i * EZD_PI2 / (double)res ) );

				// If it falls on the image
				if ( 0 <= px && px < w && 0 <= py && py < h )
				{	pImg = &p->pImage[ py * sw + px * pw ];
					pImg[ 0 ] = r, pImg[ 1 ] = g, pImg[ 2 ] = b;
				} // end if

			} // end for

		} break;

		case 32 :
			for ( i = 0; i < resdraw; i++ )
			{
				// Offset for this pixel
				px = x + (int)( (double)x_rad * sin( (double)i * EZD_PI2 / (double)res ) );
				py = y + (int)( (double)x_rad * cos( (double)i * EZD_PI2 / (double)res ) );

				// If it falls on the image
				if ( 0 <= px && px < w && 0 <= py && py < h )
					*(unsigned int*)&p->pImage[ py * sw + px * pw ] = x_col;

			} // end for

			break;

		default :
			return 0;

	} // end switch

	return 1;
#endif
}


int ezd_circle( HEZDIMAGE x_hDib, int x, int y, int x_rad, int x_col )
{
	return ezd_arc( x_hDib, x, y, x_rad, 0, EZD_PI2, x_col );
}

int ezd_fill_rect( HEZDIMAGE x_hDib, int x1, int y1, int x2, int y2, int x_col )
{
	int w, h, x, y, sw, pw, fw, fh;
	unsigned char *pStart, *pPos;
	SImageData *p = (SImageData*)x_hDib;

	if ( !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize
		 || ( !p->pImage && !p->pfSetPixel ) )
		return _ERR( 0, "Invalid parameters" );

	// Calculate image metrics
	w = EZD_ABS( p->bih.biWidth );
	h = EZD_ABS( p->bih.biHeight );

	// Swap coords if needed
	if ( x1 > x2 ) { int t = x1; x1 = x2; x2 = t; }
	if ( y1 > y2 ) { int t = y1; y1 = y2; y2 = t; }

	// Clip
	if ( 0 > x1 ) x1 = 0; else if ( x1 >= w ) x1 = w - 1;
	if ( 0 > y1 ) y1 = 0; else if ( y1 >= h ) y1 = h - 1;
	if ( 0 > x2 ) x2 = 0; else if ( x2 >= w ) x2 = w - 1;
	if ( 0 > y2 ) y2 = 0; else if ( y2 >= h ) y2 = h - 1;

	// Fill width and height
	fw = x2 - x1;
	fh = y2 - y1;

	// Are we left with a valid region
	if ( 0 > fw || 0 > fh )
	{	_SHOW( "Invalid fill rect : %d,%d -> %d,%d : %dx%d ",
			   x1, y1, x2, y2, w, h );
		return 0;
	} // en dif

	// Check for user callback function
	if ( p->pfSetPixel )
	{
		// Fill each pixel
		for ( y = y1; y < y2; y++ )
			for( x = x1; x < x2; x++ )
				if ( 0 <= x && x < w && 0 <= y && y < h )
					if ( !p->pfSetPixel( p->pSetPixelUser, x, y, x_col, 0 ) )
						return 0;

		return 1;

	} // end if

	// Pixel and scan width
	pw = EZD_FITTO( p->bih.biBitCount, 8 );
	sw = EZD_SCANWIDTH( w, p->bih.biBitCount, 1 );

	// Set the first line
	switch( p->bih.biBitCount )
	{
		case 1 :
		{
			int c = EZD_COMPARE_THRESHOLD( x_col, p->colThreshold );
			static unsigned char xm[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

			// Just fill it in pixel by pixel for 1 bit images
			for ( y = y1; y < y2; y++ )
				for( x = x1; x < x2; x++ )
					if ( 0 <= x && x < w && 0 <= y && y < h )
					{
						if ( c )
							p->pImage[ y * sw + ( x >> 3 ) ] |= xm[ x & 7 ];
						else
							p->pImage[ y * sw + ( x >> 3 ) ] &= ~xm[ x & 7 ];

					} // end if

			return 1;

		} break;

		case 24 :
		{
			// Color values
			unsigned char r = x_col & 0xff;
			unsigned char g = ( x_col >> 8 ) & 0xff;
			unsigned char b = ( x_col >> 16 ) & 0xff;
			pStart = pPos = &p->pImage[ y1 * sw + x1 * pw ];

			// Set the first line
			for( x = 0; x < fw; x++, pPos += pw )
				pPos[ 0 ] = r, pPos[ 1 ] = g, pPos[ 2 ] = b;

		} break;

		case 32 :
		{
			// Set the first line
			pStart = pPos = &p->pImage[ y1 * sw + x1 * pw ];
			for( x = 0; x < fw; x++, pPos += pw )
				*(unsigned int*)pPos = x_col;

		} break;

		default :
			return 0;

	} // end switch

	// Copy remaining lines
	pPos = pStart;
	for( y = 1; y < fh; y++ )
	{
		// Skip to next line
		pPos += sw;
		EZD_MEMCPY( pPos, pStart, fw * pw );

	} // end for

	return 1;
}

int ezd_flood_fill( HEZDIMAGE x_hDib, int x, int y, int x_bcol, int x_col )
{
#if defined( EZD_NO_ALLOCATION )
	return 0;
#else
	int ok, n, i, ii, w, h, sw, pw, bc;
	unsigned char r, g, b, br, bg, bb;
	unsigned char *pImg, *map;
	SImageData *p = (SImageData*)x_hDib;

	if ( !p || sizeof( SBitmapInfoHeader ) != p->bih.biSize || !p->pImage )
		return _ERR( 0, "Invalid parameters" );

	// +++ Fix ezd_flood_fill() for 1 bpp
	if ( 1 == p->bih.biBitCount )
		return 0; // _ERR( 0, "Invalid pixel depth" );

	// Calculate image metrics
	w = EZD_ABS( p->bih.biWidth );
	h = EZD_ABS( p->bih.biHeight );

	// Ensure pixel is within the image
	if ( 0 > x || x >= w || 0 > y || y >= h )
	{	_SHOW( "Point out of range : %d,%d : %dx%d ", x, y, w, h );
		return 0;
	} // en dif

	// Pixel and scan width
	pw = EZD_FITTO( p->bih.biBitCount, 8 );
	sw = EZD_SCANWIDTH( w, p->bih.biBitCount, 4 );

	// Set the image pointer
	pImg = p->pImage;

	// Allocate space for fill map
	map = (unsigned char*)EZD_calloc( w * h, 1 );
	if ( !map )
		return 0;

	// Prepare 24 bit color components
	r = x_col & 0xff; g = ( x_col >> 8 ) & 0xff; b = ( x_col >> 16 ) & 0xff;
	br = x_bcol & 0xff; bg = ( x_bcol >> 8 ) & 0xff; bb = ( x_bcol >> 16 ) & 0xff;

	// Initialize indexes
	i = y * w + x;
	ii = y * sw + x * pw;

	// Save away bit count
	bc = p->bih.biBitCount;

	// Crawl the map
	while ( ( map[ i ] & 0x0f ) <= 3 )
	{

		if ( ( map[ i ] & 0x0f ) == 0 )
		{
			// In the name of simplicity
			switch( bc )
			{
				case 24 :
					pImg[ ii ] = r;
					pImg[ ii + 1 ] = g;
					pImg[ ii + 2 ] = b;
					break;

				case 32 :
					*(unsigned int*)&p->pImage[ ii ] = x_col;
					break;

			} // end switch

			// Point to next direction
			map[ i ] &= 0xf0, map[ i ] |= 1;

			// Can we go up?
			if ( y < ( h - 1 ) )
			{
				n = ( y + 1 ) * sw + x * pw;
				switch( bc )
				{	case 24 :
						ok = pImg[ n ] != r || pImg[ n + 1 ] != g || pImg[ n + 2 ] != b;
						if ( ok ) ok = pImg[ n ] != br || pImg[ n + 1 ] != bg || pImg[ n + 2 ] != bb;
						break;
					case 32 :
						ok = *(unsigned int*)&pImg[ n ] != x_col;
						if ( ok ) ok = *(unsigned int*)&pImg[ n ] != x_bcol;
						break;
				} // end switch

				if ( ok )
				{	y++;
					i = y * w + x;
					map[ i ] = 0x10;
					ii = n;
				} // end if

			} // end if

		} // end if

		if ( ( map[ i ] & 0x0f ) == 1 )
		{
			// Point to next direction
			map[ i ] &= 0xf0, map[ i ] |= 2;

			// Can we go right?
			if ( x < ( w - 1 ) )
			{
				n = y * sw + ( x + 1 ) * pw;
				switch( bc )
				{	case 24 :
						ok = pImg[ n ] != r || pImg[ n + 1 ] != g || pImg[ n + 2 ] != b;
						if ( ok ) ok = pImg[ n ] != br || pImg[ n + 1 ] != bg || pImg[ n + 2 ] != bb;
						break;
					case 32 :
						ok = *(unsigned int*)&pImg[ n ] != x_col;
						if ( ok ) ok = *(unsigned int*)&pImg[ n ] != x_bcol;
						break;
				} // end switch

				if ( ok )
				{	x++;
					i = y * w + x;
					map[ i ] = 0x20;
					ii = n;
				} // end if

			} // end if

		} // end if

		if ( ( map[ i ] & 0x0f ) == 2 )
		{
			// Point to next direction
			map[ i ] &= 0xf0, map[ i ] |= 3;

			// Can we go down?
			if ( y > 0 )
			{
				n = ( y - 1 ) * sw + x * pw;
				switch( bc )
				{	case 24 :
						ok = pImg[ n ] != r || pImg[ n + 1 ] != g || pImg[ n + 2 ] != b;
						if ( ok ) ok = pImg[ n ] != br || pImg[ n + 1 ] != bg || pImg[ n + 2 ] != bb;
						break;
					case 32 :
						ok = *(unsigned int*)&pImg[ n ] != x_col;
						if ( ok ) ok = *(unsigned int*)&pImg[ n ] != x_bcol;
						break;
				} // end switch

				if ( ok )
				{	y--;
					i = y * w + x;
					map[ i ] = 0x30;
					ii = n;
				} // end if

			} // end if

		} // end if

		if ( ( map[ i ] & 0x0f ) == 3 )
		{
			// Point to next
			map[ i ] &= 0xf0, map[ i ] |= 4;

			// Can we go left
			if ( x > 0 )
			{
				n = y * sw + ( x - 1 ) * pw;
				switch( bc )
				{	case 24 :
						ok = pImg[ n ] != r || pImg[ n + 1 ] != g || pImg[ n + 2 ] != b;
						if ( ok ) ok = pImg[ n ] != br || pImg[ n + 1 ] != bg || pImg[ n + 2 ] != bb;
						break;
					case 32 :
						ok = *(unsigned int*)&pImg[ n ] != x_col;
						if ( ok ) ok = *(unsigned int*)&pImg[ n ] != x_bcol;
						break;
				} // end switch

				if ( ok )
				{	x--;
					i = y * w + x;
					map[ i ] = 0x40;
					ii = n;
				} // end if

			} // end if

		} // end if

		// Time to backup?
		while ( ( map[ i ] & 0xf0 ) > 0 && ( map[ i ] & 0x0f ) > 3 )
		{
			// Go back
			if ( ( map[ i ] & 0xf0 ) == 0x10 ) y--;
			else if ( ( map[ i ] & 0xf0 ) == 0x20 ) x--;
			else if ( ( map[ i ] & 0xf0 ) == 0x30 ) y++;
			else if ( ( map[ i ] & 0xf0 ) == 0x40 ) x++;

			// Set indexes
			i = y * w + x;
			ii = y * sw + x * pw;

		} // end while

	} // end if

	EZD_free( map );

	return 1;
#endif
}

static void ezd_draw_bmp_cb( unsigned char *pImg, int x, int y, int sw, int pw, 
							 int inv, int bw, int bh, const unsigned char *pBmp, 
							 int col, int ch, t_ezd_set_pixel pf, void *pUser )
{
	int w, h, lx = x;
	unsigned char m = 0x80;
	unsigned char r = col & 0xff;
	unsigned char g = ( col >> 8 ) & 0xff;
	unsigned char b = ( col >> 16 ) & 0xff;
	static unsigned char xm[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	// Draw the glyph
	for( h = 0; h < bh; h++ )
	{
		// Draw horz line
		for( w = 0; w < bw; w++ )
		{
			// Next glyph byte?
			if ( !m )
				m = 0x80, pBmp++;

			// Is this pixel on?
			if ( *pBmp & m )
				if ( !pf( pUser, lx, y, col, ch ) )
					return;

			// Next bmp bit
			m >>= 1;

			// Next x pixel
			lx++;

		} // end for

		// Reset x
		lx = x;

		// Reset y
		y++;

	} // end for

}

static void ezd_draw_bmp_1( unsigned char *pImg, int x, int y, int sw, int pw, 
							int inv, int bw, int bh, const unsigned char *pBmp, int col )
{
	int w, h, lx = x;
	unsigned char m = 0x80;
	unsigned char r = col & 0xff;
	unsigned char g = ( col >> 8 ) & 0xff;
	unsigned char b = ( col >> 16 ) & 0xff;
	static unsigned char xm[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

	// Draw the glyph
	for( h = 0; h < bh; h++ )
	{
		// Draw horz line
		for( w = 0; w < bw; w++ )
		{
			// Next glyph byte?
			if ( !m )
				m = 0x80, pBmp++;

			// Is this pixel on?
			if ( *pBmp & m )
			{
				if ( col )
					pImg[ y * sw + ( lx >> 3 ) ] |= xm[ lx & 7 ];
				else
					pImg[ y * sw + ( lx >> 3 ) ] &= ~xm[ lx & 7 ];

			} // end if

			// Next bmp bit
			m >>= 1;

			// Next x pixel
			lx++;

		} // end for

		// Reset x
		lx = x;

		// Reset y
		y++;

	} // end for

}

static void ezd_draw_bmp_24( unsigned char *pImg, int sw, int pw, int inv,
							 int bw, int bh, const unsigned char *pBmp, int col )
{
	int w, h;
	unsigned char m = 0x80;
	unsigned char r = col & 0xff;
	unsigned char g = ( col >> 8 ) & 0xff;
	unsigned char b = ( col >> 16 ) & 0xff;

	// Draw the glyph
	for( h = 0; h < bh; h++ )
	{
		// Draw horz line
		for( w = 0; w < bw; w++ )
		{
			// Next glyph byte?
			if ( !m )
				m = 0x80, pBmp++;

			// Is this pixel on?
			if ( *pBmp & m )
				pImg[ 0 ] = r, pImg[ 1 ] = g, pImg[ 2 ] = b;

			// Next bmp bit
			m >>= 1;

			// Next pixel
			pImg += pw;

		} // end for

		// Next image line
		if ( 0 < inv )
			pImg += sw - ( bw * pw );
		else
			pImg -= sw + ( bw * pw );

	} // end for

}

static void ezd_draw_bmp_32( unsigned char *pImg, int sw, int pw, int inv,
							 int bw, int bh, const unsigned char *pBmp, int col )
{
	int w, h;
	unsigned char m = 0x80;

	// Draw the glyph
	for( h = 0; h < bh; h++ )
	{
		// Draw horz line
		for( w = 0; w < bw; w++ )
		{
			// Next glyph byte?
			if ( !m )
				m = 0x80, pBmp++;

			// Is this pixel on?
			if ( *pBmp & m )
				*(unsigned int*)pImg = col;

			// Next bmp bit
			m >>= 1;

			// Next pixel
			pImg += pw;

		} // end for

		// Next image line
		if ( 0 < inv )
			pImg += sw - ( bw * pw );
		else
			pImg -= sw + ( bw * pw );

	} // end for

}

int ezd_text(HEZDIMAGE x_hDib, HEZDFONT x_hFont, const char *x_pText, int x_nTextLen, int x, int y, int x_col)
{
	int w, h, sw, pw, inv, i, mh = 0, lx = x;
	const tGlyph *_pGlyph;
	SImageData *p = (SImageData*)x_hDib;

#if !defined( EZD_STATIC_FONTS )
	SFontData *f = (SFontData*)x_hFont;
	if (!f)
		return _ERR(0, "Invalid parameters");
#endif

	// Sanity checks
	if (!p || sizeof(SBitmapInfoHeader) != p->bih.biSize
		|| (!p->pImage && !p->pfSetPixel))
		return _ERR(0, "Invalid parameters");

	// Calculate image metrics
	w = EZD_ABS(p->bih.biWidth);
	h = EZD_ABS(p->bih.biHeight);

	// Invert font?
	inv = ((0 < p->bih.biHeight ? 1 : 0)
#if !defined( EZD_STATIC_FONTS )
		^ ((f->uFlags & EZD_FONT_FLAG_INVERT) ? 1 : 0)
#endif
		) ? -1 : 1;

	// Pixel and scan width
	pw = EZD_FITTO(p->bih.biBitCount, 8);
	sw = EZD_SCANWIDTH(w, p->bih.biBitCount, 4);

	// For each character in the string
	for (i = 0; i < x_nTextLen || (0 > x_nTextLen && x_pText[i]); i++)
	{
		// Get the specified glyph
		_pGlyph = ezd_find_glyph(x_hFont, x_pText[i]);

		// CR, just go back to starting x pos
		if ('\r' == x_pText[i])
			lx = x;

		// LF - Back to starting x and next line
		else if ('\n' == x_pText[i])
			lx = x, y += inv * (1 + mh), mh = 0;

		// Other characters
		else
		{
			int gWidth = (int)(_pGlyph->bbox.width) + (int)(_pGlyph->bbox.xoffset);
			int gHeight = (int)(_pGlyph->bbox.height) + (int)(_pGlyph->bbox.yoffset);
			int baselineAKAOriginY = (int)(f->bbox.height) + (int)(f->bbox.yoffset);
			int bitmapTop = baselineAKAOriginY - gHeight;
			_SHOW("Glyph '%c' (w,h):%d,%d bl:%d top:%d\n", _pGlyph->encoding, gWidth, gHeight, baselineAKAOriginY, bitmapTop);
			// Draw this glyph if it's completely on the screen
			// Let user pfSetPixel to draw outside
			if ((gWidth && gHeight) && ((p->pfSetPixel != NULL) ||
				(0 <= lx && (lx + gWidth) < w
				&& 0 <= y && (y + f->bbox.height) <= h)))
			{
				int originX = lx + (int)(_pGlyph->bbox.xoffset);
				int originY = y + bitmapTop;
				// Check for user callback function
				if (p->pfSetPixel)
					ezd_draw_bmp_cb(p->pImage, originX, originY, sw, pw, inv,
						_pGlyph->bbox.width, _pGlyph->bbox.height, (const unsigned char*)(_pGlyph + 1), // -> not pointing to next glyph but the data
						x_col, x_pText[i], p->pfSetPixel, p->pSetPixelUser);

				else switch (p->bih.biBitCount)
				{
				case 1:
					ezd_draw_bmp_1(p->pImage, originX, originY, sw, pw, inv,
						_pGlyph->bbox.width, _pGlyph->bbox.height, (const unsigned char*)(_pGlyph + 1), // -> not pointing to next glyph but the data
						EZD_COMPARE_THRESHOLD(x_col, p->colThreshold));
					break;

				case 24:
					ezd_draw_bmp_24(&p->pImage[y * sw + lx * pw], sw, pw, inv,
						_pGlyph->bbox.width, _pGlyph->bbox.height, (const unsigned char*)(_pGlyph + 1), // -> not pointing to next glyph but the data
						x_col);
					break;

				case 32:
					ezd_draw_bmp_32(&p->pImage[y * sw + lx * pw], sw, pw, inv,
						_pGlyph->bbox.width, _pGlyph->bbox.height, (const unsigned char*)(_pGlyph + 1), // -> not pointing to next glyph but the data
						x_col);
					break;
				} // end switch

			} // end if

			  // Next character position
			lx += f->spacing + _pGlyph->xoffsetnext;

			// Track max height
			mh = (gHeight > mh) ? gHeight : mh;

		} // end else

	} // end for

	return 1;
}

#define EZD_CNVTYPE( t, c ) case EZD_TYPE_##t : return oDst + ( (double)( ((c*)pData)[ i ] ) - oSrc ) * rDst / rSrc;
double ezd_scale_value( int i, int t, void *pData, double oSrc, double rSrc, double oDst, double rDst )
{
	switch( t )
	{
		EZD_CNVTYPE( CHAR,	 		char );
		EZD_CNVTYPE( UCHAR,			unsigned char );
		EZD_CNVTYPE( SHORT, 		short );
		EZD_CNVTYPE( USHORT,		unsigned short );
		EZD_CNVTYPE( INT, 			int );
		EZD_CNVTYPE( UINT, 			unsigned int );
		EZD_CNVTYPE( LONGLONG, 		long long );
		EZD_CNVTYPE( ULONGLONG,		unsigned long long );
		EZD_CNVTYPE( FLOAT, 		float );
		EZD_CNVTYPE( DOUBLE, 		double );
//		EZD_CNVTYPE( LONGDOUBLE,	long double );

		default :
			break;

	} // end switch

	return 0;
}

double ezd_calc_range( int t, void *pData, int nData, double *pMin, double *pMax, double *pTotal )
{
	int i;
	double v;

	// Sanity checks
	if ( !pData || 0 >= nData )
		return 0;

	// Starting point
	v = ezd_scale_value( 0, t, pData, 0, 1, 0, 1 );

	if ( pMin )
		*pMin = v;

	if ( pMax )
		*pMax = v;

	if ( pTotal )
		*pTotal = 0;

	// Figure out the range
	for ( i = 1; i < nData; i++ )
	{
		// Get element value
		v = ezd_scale_value( i, t, pData, 0, 1, 0, 1 );

		// Track minimum
		if ( pMin && v < *pMin )
			*pMin = v;

		// Track maximum
		if ( pMax && v > *pMax )
			*pMax = v;

		// Accumulate total
		if ( pTotal )
			*pTotal += v;

	} // end for

	return 1;
}

