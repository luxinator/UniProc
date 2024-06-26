//
// Created by Lucas van Oosterhout on 2/15/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//
#include "gdt.h"
#include <stdint.h>

#define GDT_SEGMENT (0b00010000)
#define GDT_PRESENT (0b10000000)
#define GDT_USER (0b01100000)
#define GDT_EXECUTABLE (0b00001000)
#define GDT_READWRITE (0b00000010)
#define GDT_LONG_MODE_GRANULARITY 0b0010

extern void load_gdt(uint64_t idt_addr);
extern void tss_update(void);

typedef struct gdt_descriptor {
  uint16_t limit;
  uint64_t offset;
} __attribute__((packed)) gdt_descriptor;

// AKA Segement Descriptor
typedef struct gdt_entry {
  uint16_t limit;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access;
  uint8_t gran;
  uint8_t base_high;
} __attribute__((packed)) gdt_entry;

typedef struct gdt_tss_entry {
  uint16_t len;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t flags1;
  uint8_t flags2;
  uint8_t base_high;
  uint32_t base_upper32;
  uint32_t reserved;
} __attribute__((packed)) gdt_tss_entry;

typedef struct gdt {
  gdt_entry entries[5];
  gdt_tss_entry tss;
} gdt;

typedef struct tss {
  uint32_t reserved;
  // stack pointers
  uint64_t rsp[3];
  uint64_t reserved0;
  // registers
  uint64_t ist[7];
  uint32_t reserved1;
  uint32_t reserved2;
  uint16_t reserved3;
  uint16_t iopb_offset;
}  __attribute__((packed)) tss;

static gdt_descriptor gdt_desc;

static gdt _gdt;
static tss _tss = {
    .reserved = 0,
    .rsp = {},
    .reserved0 = 0,
    .ist = {},
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0,
    .iopb_offset = 0,
};

void init_gdt(void) {
// Mostly taken from: https://github.com/V01D-NULL/MoonOS/tree/main/kernel/arch/x86/int

  // Null descriptor
  _gdt.entries[0].limit = 0;
  _gdt.entries[0].base_low = 0;
  _gdt.entries[0].base_mid = 0;
  _gdt.entries[0].base_high = 0;
  _gdt.entries[0].access = 0;
  _gdt.entries[0].gran = 0;

  // 64 bit kernel Code Segement
  _gdt.entries[1].limit = 0;
  _gdt.entries[1].base_low = 0;
  _gdt.entries[1].base_mid = 0;
  _gdt.entries[1].base_high = 0;
  _gdt.entries[1].access = (GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE);
  _gdt.entries[1].gran = 0xA2;

  // 64 bit kernel Data Segement
  _gdt.entries[2].limit = 0;
  _gdt.entries[2].base_low = 0;
  _gdt.entries[2].base_mid = 0;
  _gdt.entries[2].base_high = 0;
  _gdt.entries[2].access = GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE;
  _gdt.entries[2].gran = 0xA0;

  // 64 bit user Code Segement
  _gdt.entries[3].limit = 0;
  _gdt.entries[3].base_low = 0;
  _gdt.entries[3].base_mid = 0;
  _gdt.entries[3].base_high = 0;
  _gdt.entries[3].access = GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE | GDT_USER;
  _gdt.entries[3].gran = 0x20;

  // 64 bit user Data Segement
  _gdt.entries[4].limit = 0;
  _gdt.entries[4].base_low = 0;
  _gdt.entries[4].base_mid = 0;
  _gdt.entries[4].base_high = 0;
  _gdt.entries[4].access = GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_USER;
  _gdt.entries[4].gran = 0;

  // TSS: Task Switch Segement
  uintptr_t tss_ptr = (uintptr_t) &_tss;
  _gdt.tss.base_low = (uint16_t) ((tss_ptr) & 0xffff);
  _gdt.tss.base_mid = (uint8_t) ((tss_ptr >> 16) & 0xff);
  _gdt.tss.base_high = (uint8_t) ((tss_ptr >> 24) & 0xff);
  _gdt.tss.base_upper32 = tss_ptr >> 32;
  _gdt.tss.flags1 = 0b10001001;
  _gdt.tss.flags2 = 0;
  _gdt.tss.reserved = 0;
  _gdt.tss.len = sizeof(tss);

  gdt_desc.offset = (uint64_t) &_gdt;
  gdt_desc.limit = sizeof(_gdt) - 1;

  load_gdt((uint64_t) &gdt_desc);
  tss_update();
}