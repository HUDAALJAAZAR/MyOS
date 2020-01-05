The first part of writing an operating system is to write a bootloader in 16 bit assembly (real mode).
Bootloader is a piece of program that runs before any operating system is running.
it is used to boot other operating systems, usually each operating system has a set of bootloaders specific for it.
Go to following link to create your own bootloader in 16 bit assembly

               https://createyourownos.blogspot.in/

Bootloaders generally select a specififc operating system and starts it's process and then operating system loads itself into memory.
If you are writing your own bootloader for loading a kernel you need to know the overall addressing/interrupts of memory as well as BIOS.
Mostly each operating system has specific bootloader for it.
