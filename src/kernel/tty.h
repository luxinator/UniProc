#ifndef KERNEL_TTY_H
#define KERNEL_TTY_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint16_t char_height;
  uint16_t char_width;
  uint16_t tty_width;
  uint16_t tty_height;
  bool serial;
} tty_specs;

void terminal_initialize(void);
void clear_terminal(void);
void terminal_putchar(char c);
void terminal_write(const char *data, size_t size);

tty_specs get_terminal_specs(void);

void send_to_serial(bool);

#endif