
## Cross Compiling

Mostly we don't need to do cc these kernel modules with cc toolchain. However, just in case if one needs it....

(WIP)

```
unset LD_LIBRARY_PATH
source /opt/ifc14xx/2.6-4.14/environment-setup-ppc64e6500-fsl-linux
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} KERNELDIR=${SDKTARGETSYSROOT}/lib/modules/4.14.67-ifc14xx/source/  modules
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


```
unset LD_LIBRARY_PATH

source /opt/cct/2.6-4.14/environment-setup-corei7-64-poky-linux 
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} KERNELDIR=${SDKTARGETSYSROOT}/lib/modules/4.14.92-cct/source/ modules
jhlee@qweak: driver (generic_scope)$ make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} KERNELDIR=${SDKTARGETSYSROOT}/lib/modules/4.14.92-cct/source/ modules
make -C /opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/source/ M=/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver modules
make[1]: Entering directory '/opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build'
Makefile:960: "Cannot use CONFIG_STACK_VALIDATION=y, please install libelf-dev, libelf-devel or elfutils-libelf-devel"
  CC [M]  /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscdrvr.o
In file included from ./arch/x86/include/asm/realmode.h:15,
                 from ./arch/x86/include/asm/acpi.h:33,
                 from ./arch/x86/include/asm/fixmap.h:29,
                 from ./arch/x86/include/asm/apic.h:10,
                 from ./arch/x86/include/asm/smp.h:13,
                 from ./include/linux/smp.h:64,
                 from ./include/linux/percpu.h:7,
                 from ./include/linux/hrtimer.h:22,
                 from ./include/linux/sched.h:20,
                 from /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscos.h:47,
                 from /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscdrvr.c:48:
./arch/x86/include/asm/io.h:44:10: fatal error: asm/early_ioremap.h: No such file or directory
 #include <asm/early_ioremap.h>
          ^~~~~~~~~~~~~~~~~~~~~
compilation terminated.
make[2]: *** [scripts/Makefile.build:327: /home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver/tscdrvr.o] Error 1
make[1]: *** [Makefile:1532: _module_/home/jhlee/e3-7.0.3/e3-tsclib/tsc-dev/driver] Error 2
make[1]: Leaving directory '/opt/cct/2.6-4.14/sysroots/corei7-64-poky-linux/lib/modules/4.14.92-cct/build'
make: *** [Makefile:66: modules] Error 2

```
