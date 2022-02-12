#ifndef fpng2ctx
#define fpng2ctx

#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "grx20.h"

#define ROUNDCOLORCOMP(x,n) (                                   \
    ((unsigned int)(x) >= GrColorInfo->mask[n]) ?                   \
	GrColorInfo->mask[n] :                                      \
	(((x) + GrColorInfo->round[n]) & GrColorInfo->mask[n])          \
)

struct PNGRGB {
  int r;
  int g;
  int b;
};

struct PNGScanLines {
  int pixelCount;
  int width;
  int height;
  PNGRGB *pixels;
};

static PNGScanLines readpng_lines( FILE *f, int use_alpha );

/*
** GrLoadContextFromPng - Load a context from a PNG file
**
** If context dimensions are lesser than png dimensions,
** the routine loads as much as it can
**
** If color mode is not in RGB mode, the routine allocates as
** much colors as it can
**
** Arguments:
**   grc:      Context to be loaded (NULL -> use current context)
**   pngfn:    Name of pnm file
**   use_alpha: if true, use alpha channel if available
**
** Returns  0 on success
**         -1 on error
*/

PNGScanLines LoadPngAsScanLines(char *pngfn, int use_alpha)
{
  FILE *f;
  int r;
  PNGScanLines result = {0,0,0,NULL};
  
  f = fopen( pngfn,"rb" );
  if( f == NULL ) return result;

  result = readpng_lines( f, use_alpha );

  fclose( f );

  return result;
}

/**/

//uses stricter color allocation to save time loading the image.
static PNGScanLines readpng_lines( FILE *f, int use_alpha )
{
  png_struct *png_ptr = NULL;
  png_info *info_ptr = NULL;
  png_byte buf[8];
  png_byte *png_pixels = NULL;
  png_byte **row_pointers = NULL;
  png_byte *pix_ptr = NULL;
  png_uint_32 row_bytes;
  png_uint_32 width;
  png_uint_32 height;
  int bit_depth;
  int color_type;
  int alpha_present;
  int i, x, y, r, g, b;
  int alpha = 0, ro, go, bo;
  int maxwidth, maxheight;
  PNGScanLines result = {0,0,0,NULL};
  PNGRGB* image = NULL;

  /* is it a PNG file? */
  if( fread( buf,1,8,f ) != 8 ) return result;
  if( ! png_check_sig( buf,8 ) ) return result;

  png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
  if( !png_ptr ){
    return result;
    }

  info_ptr = png_create_info_struct( png_ptr );
  if( !info_ptr ){
    png_destroy_read_struct( &png_ptr,NULL,NULL );
    return result;
    }

  if( setjmp( png_jmpbuf(png_ptr) ) ){
    png_destroy_read_struct( &png_ptr,&info_ptr,NULL );
    return result;
    }

  png_init_io( png_ptr,f );
  png_set_sig_bytes( png_ptr,8 );
  png_read_info( png_ptr,info_ptr );

  png_get_IHDR( png_ptr,info_ptr,&width,&height,&bit_depth,
                &color_type,NULL,NULL,NULL);

  /* tell libpng to strip 16 bit/color files down to 8 bits/color */
  if( bit_depth == 16 )
    png_set_strip_16( png_ptr );
  /* expand paletted colors into true RGB triplets */
  if( color_type == PNG_COLOR_TYPE_PALETTE )
    png_set_expand( png_ptr );
  /* expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
  if( color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 )
    png_set_expand( png_ptr );
  /* expand paletted or RGB images with transparency to full alpha channels
     so the data will be available as RGBA quartets. */
  if( png_get_valid( png_ptr,info_ptr,PNG_INFO_tRNS ) )
    png_set_expand( png_ptr );
  /* transform grayscale images into rgb */
  if( color_type == PNG_COLOR_TYPE_GRAY ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
    png_set_gray_to_rgb( png_ptr );

  /* we don't do gamma correction by now */

  png_read_update_info( png_ptr,info_ptr );
  png_get_IHDR( png_ptr,info_ptr,&width,&height,&bit_depth,
                &color_type,NULL,NULL,NULL);

  if( color_type == PNG_COLOR_TYPE_RGB )
    alpha_present = 0;
  else if( color_type == PNG_COLOR_TYPE_RGB_ALPHA )
    alpha_present = 1;
  else{
    png_destroy_read_struct( &png_ptr,&info_ptr,NULL );
    return result;
    }

  row_bytes = png_get_rowbytes( png_ptr,info_ptr );

  png_pixels = (png_byte *) malloc( row_bytes * height * sizeof(png_byte) );
  if( png_pixels == NULL ){
    png_destroy_read_struct( &png_ptr,&info_ptr,NULL );
    return result;
    }

  row_pointers = (png_byte **) malloc( height * sizeof(png_bytep) );
  if( row_pointers == NULL ){
    png_destroy_read_struct( &png_ptr,&info_ptr,NULL );
    free( png_pixels );
    png_pixels = NULL;
    return result;
    }

  for( i=0; i<height; i++ )
    row_pointers[i] = png_pixels + i * row_bytes;

  png_read_image (png_ptr, row_pointers);

  png_read_end (png_ptr, info_ptr);

  png_destroy_read_struct( &png_ptr,&info_ptr,NULL );

  /* write data to context */
  maxwidth = (width > GrSizeX()) ? GrSizeX() : width;
  maxheight = (height > GrSizeY()) ? GrSizeY() : height;
  int npixels = maxwidth * maxheight;
  result.pixelCount = npixels;
  result.width = maxwidth;
  result.height = maxheight;
  
  image = (PNGRGB*)malloc(npixels * sizeof(struct PNGRGB));
  if( image == NULL ){
    free( row_pointers );
    row_pointers = NULL;
    free( png_pixels );
    png_pixels = NULL;
    return result;
    }

  int rgbi = 0;
  for( y=0; y<maxheight; y++ ){
    pix_ptr = row_pointers[y];
    for( x=0; x<width; x++ ){
      r = *pix_ptr++;
      g = *pix_ptr++;
      b = *pix_ptr++;
      alpha = 0;
      if( alpha_present )
        alpha = *pix_ptr++;
      if( x < maxwidth ){
        image[rgbi] = {r,g,b};
        }
        rgbi++;
      }
    }

  if( row_pointers ) free( row_pointers );
  if( png_pixels ) free( png_pixels );

  result.pixels = image;
  return result;
}

#endif