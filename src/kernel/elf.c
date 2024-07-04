#include "../include/elf.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "vmem.h"

#ifdef ARCH_x86_64
#define ELF_MACHINE_SUPPORTED    ((Elf64_Half)ELFCLASS64)
#define ELF_CLASS                ELFCLASS64
#define ELF_BIT_ORDER            ELFDATA2LSB
#elif
#define ELF_MACHINE_SUPPORTED   (Elf64_Half)EM_NONE)
#define ELF_CLASS               ELFCLASSNONE
#define ELF_BIT_ORDER            ELFDATANONE
#endif

#define ELF_RELOC_ERR (-1)

bool elf_check_file(Elf64_Ehdr *hdr) {
  if (hdr==NULL)
	return false;

  if (hdr->e_ident[EI_MAG0]!=ELFMAG0) {
	printf("ELF header EI_MAG0 incorrect\n");
	return false;
  }
  if (hdr->e_ident[EI_MAG1]!=ELFMAG1) {
	printf("ELF header EI_MAG1 incorrect\n");
	return false;
  }
  if (hdr->e_ident[EI_MAG2]!=ELFMAG2) {
	printf("ELF header EI_MAG2 incorrect\n");
	return false;
  }
  if (hdr->e_ident[EI_MAG3]!=ELFMAG3) {
	printf("ELF header EI_MAG3 incorrect\n");
	return false;
  }

  return true;
}

bool elf_check_supported(Elf64_Ehdr *hdr) {
  if (!elf_check_file(hdr)) {
	printf("Invalid ELF File.\n");
	return false;
  }

  if (hdr->e_ident[EI_CLASS]!=ELF_CLASS) {
	printf("Unsupported ELF File Class\n");
	return false;
  }

  if (hdr->e_ident[EI_DATA]!=ELF_BIT_ORDER) {
	printf("Unsupported ELF File byte order.\n");
	return false;
  }

  // ToDo: make this compilation specific...
  if (hdr->e_machine!=ELF_MACHINE_SUPPORTED) {
	printf("Unsupported ELF File target. Only x86_64 is supported\n");
	return false;
  }

  if (hdr->e_ident[EI_VERSION]!=EV_CURRENT) {
	printf("Unsupported ELF File version.\n");
	return false;
  }

  if (hdr->e_type!=ET_REL && hdr->e_type!=ET_EXEC) {
	printf("Unsupported ELF File type.\n");
	return false;
  }
  return true;
}

Elf64_Shdr *elf_sheader(Elf64_Ehdr *hdr) {
  return (Elf64_Shdr *)(hdr + hdr->e_shoff);
}

Elf64_Shdr *elf_section(Elf64_Ehdr *hdr, size_t idx) {
  return &elf_sheader(hdr)[idx];
}

// Todo: implement this to lookup symbols by name
extern void *elf_lookup_symbol(const char *name) {
  return NULL;
}

size_t elf_get_symval(Elf64_Ehdr *hdr, int table, size_t idx) {
  if (table==SHN_UNDEF || idx==SHN_UNDEF) return 0;
  Elf64_Shdr *symbol_table = elf_section(hdr, idx);

  uint64_t table_size = symbol_table->sh_size/symbol_table->sh_entsize;
  if (idx >= table_size) {
	printf("Symbol Index out of Range (%d:%u).\n", table, idx);
	return ELF_RELOC_ERR;
  }

  Elf64_Sym *symbol_addr = (Elf64_Sym *)hdr + symbol_table->sh_offset;
  Elf64_Sym *symbol = &symbol_addr[idx];

  // Symbol is external, need to look it up in another table
  if (symbol->st_shndx==SHN_UNDEF) {
	Elf64_Shdr *string_table = elf_section(hdr, symbol_table->sh_link);
	const char *name = (const char *)hdr + string_table->sh_offset + symbol->st_name;

	void *target = elf_lookup_symbol(name);
	if (target==NULL) {
	  // Symbol not found and weak
	  if (ELF64_ST_BIND(symbol->st_info) & STB_WEAK)
		return 0;
	} else {
	  printf("Undefined External Symbol : %s.\n", name);
	  return ELF_RELOC_ERR;
	}
	return (size_t)target;
  } else if (symbol->st_shndx==SHN_ABS) {
	return symbol->st_value;
  } else {
	Elf64_Shdr *target = elf_section(hdr, symbol->st_shndx);
	return (size_t)hdr + symbol->st_value + target->sh_offset;
  }
}

/// Loads the BSS (and other similar sections) to zero
/// \param hdr
/// \return
int elf_load_stage1(Elf64_Ehdr *hdr) {
  Elf64_Shdr *shdr = elf_sheader(hdr);

  for (size_t i = 0; i < hdr->e_shnum; i++) {
	Elf64_Shdr *section = &shdr[i];

	// Found a section that should be zeroed
	if (section->sh_type==SHT_NOBITS && section->sh_size!=0) {
	  // Section should be present before code is run
	  if (section->sh_flags & SHF_ALLOC) {
		void *mem = malloc(section->sh_size);
		memset(mem, 0, section->sh_size);

		// Assign the memory offset to the section offset
		section->sh_offset = (size_t)mem - (size_t)hdr;
		printf("Allocated memory for section %d\n", section->sh_size);
	  }
	}
  }

  return 0;
}

size_t elf_do_reloc(Elf64_Ehdr *hdr, Elf64_Rel *rel, Elf64_Shdr *reltab) {
  Elf64_Shdr *target = elf_section(hdr, reltab->sh_info);

  size_t addr = (size_t)hdr + target->sh_offset;
  size_t *ref = (size_t *)(addr + rel->r_offset);

  size_t symval = 0;
  if (ELF64_R_SYM(rel->r_info)!=SHN_UNDEF) {
	symval = elf_get_symval(hdr, reltab->sh_link, ELF64_R_SYM(rel->r_info));
	if (symval==ELF_RELOC_ERR)
	  return ELF_RELOC_ERR;
  }
  switch (ELF64_R_TYPE(rel->r_info)) {
	case R_X86_64_NONE: break;
	case R_X86_64_32: *ref = symval + *ref;
	  break;
	case R_X86_64_PC32:
	case R_X86_64_PC64: *ref = symval + *ref - (size_t)ref;
	  break;
	default: printf("Unsupported Relocation Type (%d).\n", ELF64_R_TYPE(rel->r_info));
	  return ELF_RELOC_ERR;
  }

  return symval;
}

int elf_load_stage2(Elf64_Ehdr *hdr) {
  Elf64_Shdr *shdr = elf_sheader(hdr);

  for (size_t i = 0; i < hdr->e_shnum; i++) {
	Elf64_Shdr *section = &shdr[i];
	// if this section is a relocation section
	if (section->sh_type==SHT_REL) {
	  for (size_t idx = 0; idx < section->sh_size/section->sh_entsize; idx++) {
		Elf64_Rel *reltab = &((Elf64_Rel *)(hdr + section->sh_offset))[idx];
		int result = elf_do_reloc(hdr, reltab, section);
		if (result==ELF_RELOC_ERR) {
		  printf("Failed to relocate symbol\n");
		  return ELF_RELOC_ERR;
		}
	  }
	}
  }
  return 0;
}

ptrdiff_t elf_load_rel(Elf64_Ehdr *hdr) {
//  int result = elf_load_stage1(hdr);
//  if (result==ELF_RELOC_ERR) {
//	printf("Unable to load ELF file.\n");
//	return NULL;
//  }
//
//  result = elf_load_stage2(hdr);
//  if (result==ELF_RELOC_ERR) {
//	printf("Unable to load ELF file.\n");
//	return NULL;
//  }
//

  // Parse program headers
  Elf64_Phdr *prog_header;
  for (size_t i = 0, off = hdr->e_phoff; i < hdr->e_phnum; i++, off += sizeof(Elf64_Phdr)) {
	prog_header = (Elf64_Phdr *)(hdr + off);
	if ((prog_header->p_type!=PT_LOAD))
	  continue;
	if (prog_header->p_memsz < prog_header->p_filesz) {
	  printf("Elf tried to overflow, denying to load program\n");
	  return 0;
	}
	printf("proc vaddr: %x, size: %x (%d)\n", prog_header->p_vaddr, prog_header->p_memsz, prog_header->p_memsz);
  }

  return (ptrdiff_t)hdr->e_entry;
}