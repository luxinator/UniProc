#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#endif

int putchar(int ic) {
#if defined(__is_libk)
  char c = (char)ic;
  terminal_write(&c, sizeof(c));
#else
  // TODO: Implement stdio and the write system call.
#endif
  return ic;
}

static bool print(const char *data, size_t length) {
  const unsigned char *bytes = (const unsigned char *) data;
  for (size_t i = 0; i < length; i++)
    if (putchar(bytes[i]) == EOF) return false;
  return true;
}

//// Taken and changed from:
///https://github.com/mit-pdos/xv6-public/blob/master/console.c
static int printint(int xx, int base, int sgn) {
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  unsigned int x;

  neg = 0;
  if (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);
  if (neg) buf[i++] = '-';

  int written = 0;
  while (--i >= 0) {
    written++;
    putchar(buf[i]);
  }

  return written;
}

int printf(const char *restrict format, ...) {
  va_list parameters;
  va_start(parameters, format);

  size_t written = 0;

  while (*format != '\0') {
    size_t maxrem = INT_MAX - written;

    if (format[0] != '%' || format[1] == '%') {
      if (format[0] == '%') format++;
      size_t amount = 1;
      while (format[amount] && format[amount] != '%') amount++;
      if (maxrem < amount) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(format, amount)) return -1;
      format += amount;
      written += amount;
      continue;
    }

    const char *format_begun_at = format++;

    if (*format == 'c') {
      format++;
      char c = (char) va_arg(parameters, int /* char promotes to int */);
      if (!maxrem) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(&c, sizeof(c))) return -1;
      written++;
    } else if (*format == 's') {
      format++;
      const char *str = va_arg(parameters, const char *);
      size_t len = strlen(str);
      if (maxrem < len) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(str, len)) return -1;
      written += len;
    } else if (*format == 'd') {
      format++;
      int number = va_arg(parameters, int);
      int int_written = printint(number, 10, 1);
      written += int_written;
    } else if (*format == 'x') {
      format++;
      print("0x", 2);
      int number = va_arg(parameters, int);
      int int_written = printint(number, 16, 0);
      written += int_written + 2;
    } else if (*format == 'b') {
      format++;
      print("0b", 2);
      int number = va_arg(parameters, int);
      int int_written = printint(number, 2, 0);
      written += int_written + 2;

    } else {
      format = format_begun_at;
      size_t len = strlen(format);
      if (maxrem < len) {
        // TODO: Set errno to EOVERFLOW.
        return -1;
      }
      if (!print(format, len)) return -1;
      written += len;
      format += len;
    }
  }

  va_end(parameters);
  return written;
}