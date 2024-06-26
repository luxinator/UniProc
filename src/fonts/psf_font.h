//
// Created by Lucas van Oosterhout on 2/11/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_FONTS_PSF_FON_H_
#define SRC_FONTS_PSF_FON_H_
#include <stdint.h>
#include <stddef.h>

typedef struct {
  uint32_t height;
  uint32_t width;
  uint32_t bytes_per_glyph;
  uint32_t bytes_per_line;
} glyph_info;

void psf_init(void);
uint8_t *psf_get_glyph(unsigned char c);
glyph_info psf_get_info(void);

#endif //SRC_FONTS_PSF_FON_H_
