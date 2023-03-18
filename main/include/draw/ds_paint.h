
#ifndef _DS_PAINT_H_
#define _DS_PAINT_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iconv.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/stat.h> 
#include "ds_font.h"


#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

typedef struct {
    UBYTE *_Image;
    UWORD Width;
    UWORD Height;
    UWORD WidthMemory;
    UWORD HeightMemory;
    UWORD Color;
    UWORD Rotate;
    UWORD Mirror;
    UWORD WidthByte;
    UWORD HeightByte;
    UWORD Scale;
} PAINT;

typedef struct _tFont
{    
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
} sFONT;


#define WHITE          0xFF
#define BLACK          0x00
#define RED            WHITE

#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

typedef enum {
    ROTATE_0  = 0x00,
    ROTATE_90 = 0x01,
    ROTATE_180 = 0x02,
    ROTATE_270 = 0x03,
} ROTATE_IMAGE;



/**
 * The size of the point
**/
typedef enum {
    DOT_PIXEL_1X1  = 1,    // 1 x 1
    DOT_PIXEL_2X2  ,    // 2 X 2
    DOT_PIXEL_3X3  ,    // 3 X 3
    DOT_PIXEL_4X4  ,    // 4 X 4
    DOT_PIXEL_5X5  ,    // 5 X 5
    DOT_PIXEL_6X6  ,    // 6 X 6
    DOT_PIXEL_7X7  ,    // 7 X 7
    DOT_PIXEL_8X8  ,    // 8 X 8
} DOT_PIXEL;
  
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex
/**
 * Point size fill style
**/
typedef enum {
    DOT_FILL_AROUND  = 1,    // dot pixel 1 x 1
    DOT_FILL_RIGHTUP  ,     // dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
**/
typedef enum {
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} LINE_STYLE;

typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;
#define MIRROR_IMAGE_DFT MIRROR_NONE

/**
 * Whether the graphic is filled
**/
typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
} DRAW_FILL;


void Paint_NewImage(UBYTE *_image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color);
void Paint_SelectImage(UBYTE *_image);
void Paint_Clear(UWORD Color);
void Paint_DrawString_CN(UWORD Xstart, UWORD Ystart, const char * pString,
                        UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawString_piture(UWORD Xstart, UWORD Ystart,const unsigned char *pbuf ,int plen,int pxsize,
                        UWORD Color_Foreground, UWORD Color_Background);
void ds_paint_image(void);
void ds_paint_image_new(void);
void ds_paint_image_copy(void);
void ds_paint_last_page(void);
#endif