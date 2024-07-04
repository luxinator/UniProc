override IMAGE_NAME := UniProc

arch ?= x86_64
build = build
kernel := $(build)/kernel.bin
iso := $(build)/os-$(arch).iso

# Path to a font file
console_font := src/fonts/cp866-8x16.psf

linker_script := src/arch/$(arch)/linker.ld
grub_cfg := src/arch/$(arch)/grub.cfg

assembly_source_files := $(wildcard src/arch/$(arch)/*.asm)
assembly_object_files := $(patsubst src/arch/$(arch)/%.asm,	$(build)/arch/$(arch)/%.o, $(assembly_source_files))

arch_c_src := $(wildcard src/arch/$(arch)/*.c)
arch_c_obj := $(patsubst src/arch/$(arch)/%.c, $(build)/arch/$(arch)/%.o, $(arch_c_src))

font_c_src := $(wildcard src/fonts/**.c)
font_c_obj := $(patsubst src/fonts/%.c, $(build)/fonts/%.o, $(font_c_src))

kernel_c_src := $(wildcard src/kernel/**.c)
kernel_c_obj := $(patsubst src/kernel/%.c, $(build)/kernel/%.o, $(kernel_c_src))

libc_c_src := $(wildcard src/libc/*.c)
libc_c_obj := $(patsubst src/libc/%.c, $(build)/libc/%.o, $(libc_c_src))

usr_c_src := $(wildcard src/usr/*.c)
usr_c_obj := $(patsubst src/usr/%.c, $(build)/usr/%.o, $(libc_c_src))


objs := $(assembly_object_files) $(arch_c_obj) $(kernel_c_obj) $(libc_c_obj) $(font_c_obj)

CFLAGS := \
	-g\
	-Wall \
	-Wextra \
	-std=c11 \
	-ffreestanding \
	-fno-stack-protector \
	-fno-stack-check \
	-fno-lto \
	-fPIE \
	-m64 \
	-march=x86-64 \
	-mno-80387 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-Wno-gnu-designator \
	-D DEBUG_ \
	-D ARCH_$(arch)

K_CFLAGS := $(CFLAGS) -I src -D__is_libk -MMD -MP


override LDFLAGS +=\
   -m elf_x86_64 \
   -nostdlib \
   -static \
   -pie \
   --no-dynamic-linker \
   -z text \
   -z max-page-size=0x1000 \
   -T src/arch/$(arch)/linker.ld

.PHONY: all clean run iso

all: kernel

clean:
	rm -rf iso_root $(IMAGE_NAME).iso $(IMAGE_NAME).hdd
	rm -r $(build)

src/limine.h:
	curl -Lo $@ https://github.com/limine-bootloader/limine/raw/trunk/limine.h

kernel: test_proc src/limine.h src/fonts/font.h $(objs)
	ld.lld $(objs) $(LDFLAGS) -o $(kernel)
	objcopy --only-keep-debug $(kernel) $(build)/kernel.sym
	objdump -S $(kernel) > $(build)/kernel.asm


# compile kernel C files
$(build)/kernel/%.o: src/kernel/%.c
	@mkdir -p $(shell dirname $@)
	clang $(K_CFLAGS) -c $< -o $@

# compile assembly files
$(build)/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@mkdir -p $(shell dirname $@)
	nasm -felf64 $< -o $@

# compile arch C files
$(build)/arch/$(arch)/%.o: src/arch/$(arch)/%.c
	@mkdir -p $(shell dirname $@)
	clang $(K_CFLAGS) -c $< -o $@

# compile kernel libc C files
$(build)/libc/%.o: src/libc/%.c
	@mkdir -p $(shell dirname $@)
	clang $(K_CFLAGS) -c $< -o $@

$(build)/fonts/%.o: src/fonts/%.c
	@mkdir -p $(shell dirname $@)
	clang $(K_CFLAGS) -c $< -o $@


# compile usr C files
$(build)/usr/%.o: src/usr/%.c
	@mkdir -p $(shell dirname $@)
	clang $(K_CFLAGS) -c $< -o $@

$(build)/usr/%.o: src/usr/%.c
	@mkdir -p $(shell dirname $@)
	clang $(K_CFLAGS) -c $< -o $@

# This is temporary until we have a file system
# Store test_proc.c as char array
test_proc:
	@mkdir -p $(build)/usr/
	clang -c src/usr/test_proc.c -o $(build)/usr/test_proc
	xxd -i $(build)/usr/test_proc src/kernel/test_proc.h


### Install and build limine disk/cdrom:

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v7.x-binary --depth=1
	$(MAKE) -C limine \
		CC="$(HOST_CC)" \
		CFLAGS="$(HOST_CFLAGS)" \
		CPPFLAGS="$(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		LIBS="$(HOST_LIBS)"


.PHONY: run
run: $(IMAGE_NAME).iso
	qemu-system-x86_64 -M q35 -m 2G -cdrom $(IMAGE_NAME).iso -boot d -serial stdio

.PHONY: run-debug
run-debug: $(IMAGE_NAME).iso
	qemu-system-x86_64 -M q35 -m 2G -cdrom $(IMAGE_NAME).iso -boot d -serial stdio -s -S

.PHONY: run-hdd
run-hdd: $(IMAGE_NAME).hdd
	qemu-system-x86_64 -M q35 -m 2G -hda $(IMAGE_NAME).hdd -serial stdio

$(IMAGE_NAME).iso: limine kernel
	rm -rf iso_root
	mkdir -p iso_root
	cp -v $(build)/kernel.bin \
		limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/
	mkdir -p iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso
	./limine/limine bios-install $(IMAGE_NAME).iso
	rm -rf iso_root

$(IMAGE_NAME).hdd: limine kernel
	rm -f $(IMAGE_NAME).hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(IMAGE_NAME).hdd
	sgdisk $(IMAGE_NAME).hdd -n 1:2048 -t 1:ef00
	./limine/limine bios-install $(IMAGE_NAME).hdd
	mformat -i $(IMAGE_NAME).hdd@@1M
	mmd -i $(IMAGE_NAME).hdd@@1M ::/EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME).hdd@@1M $(build)/kernel.bin limine.cfg limine/limine-bios.sys ::/
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTIA32.EFI ::/EFI/BOOT

.PHONY: distclean
distclean: clean
	rm -rf limine ovmf
	$(MAKE) -C kernel distclean
