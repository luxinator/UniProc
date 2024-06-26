#include <kernel/framebuffer.h>
#include <stddef.h>
#include "limine.h"

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, in C, they should
// NOT be made "static".
struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

struct limine_framebuffer *framebuffer = NULL;

bool fb_init(void) {
  // Ensure we got a framebuffer.
  if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count == 0) {
    return false;
  }

  framebuffer = framebuffer_request.response->framebuffers[0];
  return true;
}

void fb_clear(void) {
  if (framebuffer == NULL)
    return;

  for (size_t y = 0; y < framebuffer->height; y++) {
    for (size_t x = 0; x < framebuffer->width; x++) {
      volatile uint32_t *fb_ptr = framebuffer->address;
      const size_t index = x + y * (framebuffer->pitch / 4);
      fb_ptr[index] = 0x0;
    }
  }
}

fb_specs fb_get_specs(void) {
  if (framebuffer == NULL)
    return (fb_specs) {
        .bbp = 0,
        .pitch = 0,
        .width = 0,
        .height = 0,
    };

  fb_specs specs = {
      .bbp = framebuffer->bpp,
      .pitch = framebuffer->pitch,
      .width = framebuffer->width,
      .height = framebuffer->height
  };
  return specs;
}

void fb_set_pixel(size_t x, size_t y, uint32_t color) {
  if (framebuffer == NULL)
    return;

  volatile uint32_t *fb_ptr = framebuffer->address;
  const size_t index = x + y * (framebuffer->pitch / 4);
  fb_ptr[index] = color;
}

void fb_set_pixel_rgb(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b) {
  const uint32_t v = (r << 16) | (g << 8) | b;
  fb_set_pixel(x, y, v);
}

bool fb_available(void) {
  return framebuffer != NULL;
}

void fb_scroll_up_by(const size_t pixels) {
  if (framebuffer == NULL)
    return;

  for (size_t y = pixels; y < framebuffer->height; y++) {
    for (size_t x = 0; x < framebuffer->width; x++) {
      volatile uint32_t *fb_ptr = framebuffer->address;
      const size_t src_index = x + y * (framebuffer->pitch / 4);
      const size_t dest_index = x + (y - pixels) * (framebuffer->pitch / 4);
      fb_ptr[dest_index] = fb_ptr[src_index];
    }
  }
}
