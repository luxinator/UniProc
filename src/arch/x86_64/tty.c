#include <kernel/tty.h>
#include <string.h>
#include <fonts/psf_font.h>
#include <kernel/framebuffer.h>

#include "serial.h"

static size_t terminal_row;
static size_t terminal_column;
static uint32_t terminal_fg_color = 0xffffff;
static uint32_t terminal_bg_color = 0x000000;

bool print_to_serial = false;
bool serial_initialized = false;
const uint16_t com_port = COM_PORT_1;
glyph_info char_info;

void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  // Always initialize the serial
  send_to_serial(true);

  psf_init();
  char_info = psf_get_info();
}

void clear_terminal(void) {
  fb_clear();
}

void terminal_put_entry_at(unsigned char c, size_t x, size_t y) {
  const size_t screen_x = x * char_info.width;
  const size_t screen_y = y * char_info.height;

  uint8_t *bytes = psf_get_glyph(c);

  // We write a char line by line to the fb
  for (uint32_t cy = 0; cy < char_info.height; cy++) {
    uint16_t line_data = bytes[cy * char_info.bytes_per_line];
    if (char_info.bytes_per_line == 2) {
      line_data = (line_data << (char_info.width - 8)) | bytes[cy * char_info.bytes_per_line + 1];
    }

    // 0b100000000 selects the first bit of the line
    uint16_t mask = 1 << char_info.width;
    for (uint32_t cx = 0; cx < char_info.width; cx++) {
      // if this bit in the line_data is 1 we pick the fg else bg
      uint32_t clr = line_data & mask ? terminal_fg_color : terminal_bg_color;
      fb_set_pixel(screen_x + cx, screen_y + cy, clr);
      //shifts the mask one to the right
      mask = mask >> 1;
    }
  }
}

void delete_line(size_t line) {
  fb_specs fb = fb_get_specs();
  const size_t start = line * char_info.height;
  const size_t end = start + char_info.height;

  for (size_t y = start; y < end; y++) {
    for (size_t x = 0; x < fb.width; x++) {
      fb_set_pixel(x, y, terminal_bg_color);
    }
  }
}

void terminal_scroll(size_t line) {
//  for (size_t row = 1; row < line; row++) {
  fb_scroll_up_by(char_info.height);
//  }
  delete_line(line);
}

void terminal_putchar(char c) {
  uint8_t uc = c;
  if (print_to_serial) {
    write_serial(com_port, uc);
  }

  if (!fb_available())
    return;

  if (c == '\n') {
    terminal_row += 1;
    terminal_column = 0;
  } else {
    terminal_put_entry_at(uc, terminal_column, terminal_row);
    terminal_column += 1;
  }

  fb_specs fb = fb_get_specs();
  if (fb.height == 0 || fb.width == 0 || char_info.width == 0 || char_info.height == 0)
    return;

  const size_t terminal_width = fb.width / char_info.width;
  if (terminal_column > terminal_width) {
    terminal_column = 0;
    terminal_row += 1;
  }

  const size_t terminal_height = (fb.height / char_info.height) - 1;
  if (terminal_row > terminal_height) {
    terminal_scroll(terminal_height);
    terminal_row = terminal_height - 1;
  }
}

void terminal_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++) terminal_putchar(data[i]);
}

void terminal_writestring(const char *data) {
  terminal_write(data, strlen(data));
}

void send_to_serial(bool b) {
  if (b && !serial_initialized) {
    serial_initialized = init_serial(com_port);
    write_serial(com_port, 'o');
    write_serial(com_port, 'k');
    write_serial(com_port, '\n');
  }

  print_to_serial = b;
}

tty_specs get_terminal_specs(void) {
  tty_specs specs = {
      .char_height = 0,
      .char_width = 0,
      .tty_height = 0,
      .tty_width = 0,
      .serial = serial_initialized
  };
  return specs;
}