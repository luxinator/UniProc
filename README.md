# UniProc

A small *UniProc*ess OS written in C (C20). Think a hybrid between SingleProcess Unix and DOS.
So everything is a file but only one process may run at a time.

# Why

I have been experimenting with writing OS kernels from scratch in C for a while now, but I always get stuck on
multiprocessing, so why not skip it? CP/M and DOS did alright without.

# PLAN

- [x] Boot with the help of a bootloader ([limine](https://github.com/limine-bootloader/limine))
- [x] PIC Interrupts
- [ ] First Process
- [ ] SystemCalls ReadKeyboard input
- [ ] Simple CommandPrompt
    - [ ] Build in command (echo) to test basic IO
- [ ] DiskDriver
- [ ] FS [FAT32](https://wiki.osdev.org/FAT#Implementation_Details)
- [ ] VFS
- [ ] Process Loader
- [ ] uLibC
- [ ] Basic file manipulation
- [ ] Load Programs from disk
- [ ] Port a "real" application
- [ ] Are we Doom yet?
- [ ] Bonus: Own Boatloader
    - [ ] EFI
    - [ ] BIOS (MBR boot)
    - [ ] Multiboot 1
    - [ ] Multiboot 2

# Will it be Multiprocess?

Maybe, if I can get it to work.
