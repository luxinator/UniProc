#define VERSION "0.0.1a"

#include <stdio.h>
#include <limine.h>
#include "kernel.h"
#include "tty.h"
#include "framebuffer.h"
#include "fonts/psf_font.h"
#include "vmem.h"
#include "interrupts.h"
#include "cpu.h"
#include "proc.h"
#include "test_proc.h"
#include "elf.h"
#include "timer.h"

void hcf(void);

extern ptrdiff_t elf_load_rel(Elf64_Ehdr *hdr);
extern bool elf_check_supported(Elf64_Ehdr *hdr);
extern bool elf_check_file(Elf64_Ehdr *hdr);

extern void start_disk_drives();


// Set the base revision to 1, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.
LIMINE_BASE_REVISION(1)

void early_output() {
  printf("--- UniProc ---\n");
  printf("--- version: %s ---\n", VERSION);

  report_kernel_address();
  report_memmap();
  report_cpu();

  fb_specs fb_specs = fb_get_specs();
  printf("framebuffer: (%d;%d) px, pitch: %d; bpp: %d\n",
		 fb_specs.width,
		 fb_specs.height,
		 fb_specs.pitch,
		 fb_specs.bbp);

  glyph_info info = psf_get_info();
  printf("PSF info char size (%d,%d) \n%d bytes per glyph\n%d bytes per line\n",
		 info.width, info.height, info.bytes_per_glyph, info.bytes_per_line);
  printf("terminal width: %d (%d chars)\n", fb_specs.width, fb_specs.width/info.width);
  printf("terminal height: %d (%d chars)\n", fb_specs.height, fb_specs.height/info.height);

}

__attribute__((unused)) void _kernel_start(void) {
  // Ensure the bootloader actually understands our base revision (see spec).
  if (LIMINE_BASE_REVISION_SUPPORTED==false) {
	hcf();
  }

  fb_init();
  fb_clear();
  terminal_initialize();
  build_memmap();

  early_output();

  init_virtual_memory();

  initialize_interrupts();
  enable_interrupts();

//  start_disk_drives();

  // PARSE ELF

  panic("KERNEL RETURNED");
}

[[ clang::noinline ]]
void enter_panic(char *file, int line, char *message) {
  printf("\n\n~~~ !!! PANICKING !!! ~~~\n");
  printf("[%s:%d]  %s\n", file, line, message);
  printf("~~~ !!!HALTING!!! ~~~\n\n");
  hcf();
}