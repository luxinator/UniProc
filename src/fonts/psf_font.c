//
// Created by Lucas van Oosterhout on 2/11/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include "psf_font.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "font.h"

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
  uint16_t magic; // Magic bytes for identification.
  uint8_t fontMode; // PSF font mode.
  uint8_t characterSize; // PSF character size.
} PSF1_Header;

#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
  uint32_t magic;         /* magic bytes to identify PSF */
  uint32_t version;       /* zero */
  uint32_t headersize;    /* offset of bitmaps in file, 32 */
  uint32_t flags;         /* 0 if there's no unicode table */
  uint32_t numglyph;      /* number of glyphs */
  uint32_t bytesperglyph; /* size of each glyph */
  uint32_t height;        /* height in pixels */
  uint32_t width;         /* width in pixels */
} PSF_font;
static PSF_font *font_info = NULL;

void psf_init(void) {
  font_info = (PSF_font *) &font_data;
}

/**
 *            padding
 *    Font data    |
 *   +----------+ +--+
 *   000001100000 0000
 *   000011110000 0000
 *   000110011000 0000
 *   001100001100 0000
 *   011000000110 0000
 *   110000000011 0000
 *   111111111111 0000
 *   111111111111 0000
 *   110000000011 0000
 *   110000000011 0000
 *   110000000011 0000
 *   110000000011 0000
 */
uint8_t *psf_get_glyph(unsigned char c) {
  return (uint8_t *) &font_data + font_info->headersize + c * font_info->bytesperglyph;
}

glyph_info psf_get_info(void) {
  PSF_font *font = (PSF_font *) &font_data;
  return (glyph_info) {
      .height = font->height,
      .width = font->width,
      .bytes_per_glyph = font->bytesperglyph,
      .bytes_per_line = font->bytesperglyph / font->height
  };
}
