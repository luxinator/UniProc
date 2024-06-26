//
// Created by Lucas van Oosterhout on 2/11/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_KERNEL_FRAMEBUFFER_H_
#define SRC_KERNEL_FRAMEBUFFER_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint64_t width;
  uint64_t height;
  uint64_t pitch;
  uint16_t bbp;
} fb_specs;

bool fb_init(void);
bool fb_available(void);

fb_specs fb_get_specs(void);
void fb_clear(void);
void fb_set_pixel(size_t x, size_t y, uint32_t color);
void fb_set_pixel_rgb(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b);
void fb_scroll_up_by(size_t pixels);

#endif //SRC_KERNEL_FRAMEBUFFER_H_
