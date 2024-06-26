//
// Created by Lucas van Oosterhout on 2/18/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include "keyboard.h"
#include "io.h"

int kb_get_scancode(void) {
  // taken from: https://github.com/mit-pdos/xv6-public/blob/eeb7b415dbcb12cc362d0783e41c3d1f44066b17/kbd.c#L11
  static uint32_t shift;
  static uint8_t *charcode[4] = {
      normalmap, shiftmap, ctlmap, ctlmap
  };

  uint32_t st = inb(KBSTATP);
  if ((st & KBS_DIB) == 0)
    return -1;
  uint32_t data = inb(KBDATAP);

  if (data == 0xE0) {
    shift |= E0ESC;
    return 0;
  } else if (data & 0x80) {
    // Key released
    data = (shift & E0ESC ? data : data & 0x7F);
    shift &= ~(shiftcode[data] | E0ESC);
    return 0;
  } else if (shift & E0ESC) {
    // Last character was an E0 escape; or with 0x80
    data |= 0x80;
    shift &= ~E0ESC;
  }

  shift |= shiftcode[data];
  shift ^= togglecode[data];
  int c = charcode[shift & (CTL | SHIFT)][data];
  if (shift & CAPSLOCK) {
    if ('a' <= c && c <= 'z')
      c += 'A' - 'a';
    else if ('A' <= c && c <= 'Z')
      c += 'a' - 'A';
  }
  return c;
}
