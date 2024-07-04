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

void hcf(void);

extern ptrdiff_t elf_load_rel(Elf64_Ehdr *hdr);
extern bool elf_check_supported(Elf64_Ehdr *hdr);
extern bool elf_check_file(Elf64_Ehdr *hdr);


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


  // PARSE ELF
  Elf64_Ehdr *test_proc_elf = (Elf64_Ehdr *)&build_usr_test_proc;
  bool isValid = elf_check_file(test_proc_elf);
  if (!isValid)
	panic("elf is invalid");

  ptrdiff_t offset = elf_load_rel((Elf64_Ehdr *)test_proc_elf);
  void *first_instruction = test_proc_elf + offset;

  printf("ELF (%x) relative entry at %x, absolute: %x\n", test_proc_elf, offset, first_instruction);

  proc_slot zero = create_process((uintptr_t)first_instruction, (uintptr_t)NULL);
  name_process(zero, "TEST PROCESS");

  panic("KERNEL RETURNED");
}

[[ clang::noinline ]]
void enter_panic(char *file, int line, char *message) {
  printf("\n\n~~~ !!! PANICKING !!! ~~~\n");
  printf("[%s:%d]  %s\n", file, line, message);
  printf("~~~ !!!HALTING!!! ~~~\n\n");
  hcf();
}