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
$ sudo bash -c "source /opt/ifc14xx/2.6-4.14/environment-setup-ppc64e6500-fsl-linux && make silentoldconfig scripts"
```
* Build the kernel modules

```
$ cd -
$ KERNELDIR=${SDKTARGETSYSROOT}/lib/modules/4.14.67-ifc14xx/source/ LDFLAGS="" make
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
$ sudo bash -c "source /opt/cct/2.6-4.14/environment-setup-corei7-64-poky-linux && make silentoldconfig scripts"
  HOSTCC  scripts/basic/fixdep
  HOSTCC  scripts/basic/bin2c
  HOSTCC  scripts/kconfig/conf.o
  SHIPPED scripts/kconfig/zconf.tab.c
  SHIPPED scripts/kconfig/zconf.lex.c
  HOSTCC  scripts/kconfig/zconf.tab.o
  HOSTLD  scripts/kconfig/conf
scripts/kconfig/conf  --silentoldconfig Kconfig
  WRAP    arch/x86/include/generated/asm/clkdev.h
  WRAP    arch/x86/include/generated/asm/dma-contiguous.h
  WRAP    arch/x86/include/generated/asm/early_ioremap.h
  WRAP    arch/x86/include/generated/asm/mcs_spinlock.h
  WRAP    arch/x86/include/generated/asm/mm-arch-hooks.h
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
$ KERNELDIR=${SDKTARGETSYSROOT}/lib/modules/4.14.92-cct/source/  LDFLAGS="" make
make -C /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/source/ M=/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver modules
make[1]: Entering directory '/opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build'
make[2]: *** No rule to make target '/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscdrvr.o', needed by '/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tsc.o'.  Stop.
make[1]: *** [Makefile:1532: _module_/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver] Error 2
make[1]: Leaving directory '/opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build'
make: *** [Makefile:66: modules] Error 2

```

* Check the kernel modules

```
$ readelf -h *.ko  |grep Machine
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

