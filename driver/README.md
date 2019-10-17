
## Cross Compiling

Mostly we don't need to do cc these kernel modules with cc toolchain. However, just in case if one needs it....

(WIP)

```
unset LD_LIBRARY_PATH
source /opt/ifc14xx/2.6-4.14/environment-setup-ppc64e6500-fsl-linux
make
```
```
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} KERNELDIR=....
```
