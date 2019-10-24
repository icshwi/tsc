Cross Compiling Kernel Modules (tsc.ko and pon.ko)
===

Mostly we don't need to do cc these kernel modules with cc toolchain. However, just in case if one needs it....

# How to do

## ifc14xx 

* Souce the toolchain environment
```
$ unset LD_LIBRARY_PATH
$ source /opt/ifc14xx/2.6-4.14/environment-setup-ppc64e6500-fsl-linux
```
* Generate the kernel headers according to the target arch

```
$ cd /opt/ifc14xx/2.6-4.14/sysroots/ppc64e6500-fsl-linux/lib/modules/4.14.67-ifc14xx/source
$ sudo bash -c "source /opt/ifc14xx/2.6-4.14/environment-setup-ppc64e6500-fsl-linux && modules_prepare"
  CHK     include/config/kernel.release
  CHK     include/generated/uapi/linux/version.h
  CHK     include/generated/utsrelease.h
scripts/kconfig/conf  --silentoldconfig Kconfig
  CC      scripts/mod/devicetable-offsets.s
  CHK     scripts/mod/devicetable-offsets.h
```
* Build the kernel modules

```
$ cd -
$ KERNEL_SRC=${SDKTARGETSYSROOT}/lib/modules/4.14.67-ifc14xx/source/ LDFLAGS="" make
make -C /opt/ifc14xx/2.6-4.14/sysroots/ppc64e6500-fsl-linux/lib/modules/4.14.67-ifc14xx/source/ M=/home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver modules
make[1]: Entering directory '/opt/ifc14xx/2.6-4.14/sysroots/ppc64e6500-fsl-linux/lib/modules/4.14.67-ifc14xx/build'
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/tscdrvr.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/ioctllib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/tscklib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/maplib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/irqlib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/mapmaslib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/rdwrlib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/sflashlib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/dmalib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/timerlib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/fifolib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/i2clib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/semaphorelib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/userirqlib.o
  LD [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/tsc.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/pondrvr.o
  LD [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/pon.o
  Building modules, stage 2.
  MODPOST 2 modules
  CC      /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/pon.mod.o
  LD [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/pon.ko
  CC      /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/tsc.mod.o
  LD [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsclib/driver/tsc.ko
make[1]: Leaving directory '/opt/ifc14xx/2.6-4.14/sysroots/ppc64e6500-fsl-linux/lib/modules/4.14.67-ifc14xx/build'
```

* Check the kernel modules

```
$ readelf -h *.ko  |grep Machine
  Machine:                           PowerPC64
  Machine:                           PowerPC64
$ ls *.ko
pon.ko  tsc.ko
```

## cct (not working)

* Souce the toolchain environment
```
$ unset LD_LIBRARY_PATH
$ source /opt/cct/2.6-4.14/environment-setup-corei7-64-poky-linux 
```
* Generate the kernel headers according to the target arch

```
$ cd /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/source
$ sudo bash -c "source /opt/cct/2.6-4.14/environment-setup-corei7-64-poky-linux && make modules_prepare"
 SYSTBL  arch/x86/include/generated/asm/syscalls_32.h
  SYSHDR  arch/x86/include/generated/asm/unistd_32_ia32.h
  SYSHDR  arch/x86/include/generated/asm/unistd_64_x32.h
  SYSTBL  arch/x86/include/generated/asm/syscalls_64.h
  SYSHDR  arch/x86/include/generated/uapi/asm/unistd_32.h
  SYSHDR  arch/x86/include/generated/uapi/asm/unistd_64.h
  SYSHDR  arch/x86/include/generated/uapi/asm/unistd_x32.h
  HOSTCC  scripts/basic/fixdep
  HOSTCC  scripts/basic/bin2c
  HOSTCC  arch/x86/tools/relocs_32.o
  HOSTCC  arch/x86/tools/relocs_64.o
  HOSTCC  arch/x86/tools/relocs_common.o
  HOSTLD  arch/x86/tools/relocs
  CHK     include/config/kernel.release
  WRAP    arch/x86/include/generated/asm/clkdev.h
  WRAP    arch/x86/include/generated/asm/dma-contiguous.h
  WRAP    arch/x86/include/generated/asm/early_ioremap.h
  WRAP    arch/x86/include/generated/asm/mcs_spinlock.h
  WRAP    arch/x86/include/generated/asm/mm-arch-hooks.h
  CHK     include/generated/uapi/linux/version.h
  CHK     include/generated/utsrelease.h
  DESCEND  objtool
  HOSTCC   /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/fixdep.o
  HOSTLD   /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/fixdep-in.o
  LINK     /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/fixdep
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/exec-cmd.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/help.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/pager.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/parse-options.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/run-command.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/sigchain.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/subcmd-config.o
  LD       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/libsubcmd-in.o
  AR       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/libsubcmd.a
  GEN      /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/arch/x86/lib/inat-tables.c
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/arch/x86/decode.o
  LD       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/arch/x86/objtool-in.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/builtin-check.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/builtin-orc.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/check.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/orc_gen.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/orc_dump.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/elf.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/special.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/objtool.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/libstring.o
  CC       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/str_error_r.o
  LD       /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/objtool-in.o
diff: ../../arch/x86/lib/insn.c: No such file or directory
Warning: synced file at 'tools/objtool/arch/x86/lib/insn.c' differs from latest kernel version at 'arch/x86/lib/insn.c'
diff: ../../arch/x86/lib/inat.c: No such file or directory
Warning: synced file at 'tools/objtool/arch/x86/lib/inat.c' differs from latest kernel version at 'arch/x86/lib/inat.c'
diff: ../../arch/x86/lib/x86-opcode-map.txt: No such file or directory
Warning: synced file at 'tools/objtool/arch/x86/lib/x86-opcode-map.txt' differs from latest kernel version at 'arch/x86/lib/x86-opcode-map.txt'
diff: ../../arch/x86/tools/gen-insn-attr-x86.awk: No such file or directory
Warning: synced file at 'tools/objtool/arch/x86/tools/gen-insn-attr-x86.awk' differs from latest kernel version at 'arch/x86/tools/gen-insn-attr-x86.awk'
  LINK     /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build/tools/objtool/objtool
  HOSTCC  scripts/kconfig/conf.o
  SHIPPED scripts/kconfig/zconf.tab.c
  SHIPPED scripts/kconfig/zconf.lex.c
  HOSTCC  scripts/kconfig/zconf.tab.o
  HOSTLD  scripts/kconfig/conf
scripts/kconfig/conf  --silentoldconfig Kconfig
  CC      scripts/mod/empty.o
  HOSTCC  scripts/mod/mk_elfconfig
  MKELF   scripts/mod/elfconfig.h
  HOSTCC  scripts/mod/modpost.o
  CC      scripts/mod/devicetable-offsets.s
  CHK     scripts/mod/devicetable-offsets.h
  UPD     scripts/mod/devicetable-offsets.h
  HOSTCC  scripts/mod/file2alias.o
  HOSTCC  scripts/mod/sumversion.o
  HOSTLD  scripts/mod/modpost
  HOSTCC  scripts/kallsyms
  HOSTCC  scripts/conmakehash
  HOSTCC  scripts/recordmcount
  HOSTCC  scripts/sortextable

```
* Build the kernel modules

```
$ cd -
$ KERNEL_SRC=${SDKTARGETSYSROOT}/lib/modules/4.14.92-cct/source/  LDFLAGS="" make
make -C /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/source/ M=/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver modules
make[1]: Entering directory '/opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build'
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscdrvr.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/ioctllib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscklib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/maplib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/irqlib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/mapmaslib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/rdwrlib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/sflashlib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/dmalib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/timerlib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/fifolib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/i2clib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/semaphorelib.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/userirqlib.o
  LD [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tsc.o
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/pondrvr.o
  LD [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/pon.o
  Building modules, stage 2.
  MODPOST 2 modules
  CC      /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/pon.mod.o
  LD [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/pon.ko
  CC      /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tsc.mod.o
  LD [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tsc.ko
make[1]: Leaving directory '/opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build'
```

* Check the kernel modules

```
$ readelf -h *.ko  |grep Machine
  Machine:                           Advanced Micro Devices X86-64
  Machine:                           Advanced Micro Devices X86-64
$ ls *.ko
pon.ko  tsc.ko
```



# How NOT to do...


```
$ unset LD_LIBRARY_PATH
$ source /opt/ifc14xx/2.6-4.14/environment-setup-ppc64e6500-fsl-linux
$ make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} KERNELDIR=${SDKTARGETSYSROOT}/lib/modules/4.14.67-ifc14xx/source/  modules
make -C /opt/ifc14xx/2.6-4.14/sysroots/ppc64e6500-fsl-linux/lib/modules/4.14.67-ifc14xx/source/ M=/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver modules
make[1]: Entering directory '/opt/ifc14xx/2.6-4.14/sysroots/ppc64e6500-fsl-linux/lib/modules/4.14.67-ifc14xx/build'
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscdrvr.o
In file included from ./include/linux/rcupdate.h:40,
                 from ./include/linux/rculist.h:11,
                 from ./include/linux/pid.h:5,
                 from ./include/linux/sched.h:14,
                 from /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscos.h:47,
                 from /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscdrvr.c:48:
./include/linux/preempt.h:81:10: fatal error: asm/preempt.h: No such file or directory
 #include <asm/preempt.h>
          ^~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [scripts/Makefile.build:329: /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscdrvr.o] Error 1
make[1]: *** [Makefile:1525: _module_/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver] Error 2
make[1]: Leaving directory '/opt/ifc14xx/2.6-4.14/sysroots/ppc64e6500-fsl-linux/lib/modules/4.14.67-ifc14xx/build'
make: *** [Makefile:66: modules] Error 2


```
# Troubleshooting

The host needs the following package if the host is Debian based one `libelf-dev`. So please install `libelf-dev`, `libelf-devel` or `elfutils-libelf-devel` on the host OS. If not, one can see the following error while making kernel modules:
```
Makefile:960: "Cannot use CONFIG_STACK_VALIDATION=y, please install libelf-dev, libelf-devel or elfutils-libelf-devel"
``
