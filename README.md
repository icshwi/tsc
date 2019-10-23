IFC14xx Kernel Driver and Binary
====

## tsc kernel driver

```
cd driver
make modules
make modules install
```

### Cross Compiling kernel drivers
Please look at README.md in the driver path

## TscMon and SmemCalibration

Stand-alone TscMon and SmemCalibration executables (not shared library)


```
make
```
* install TscMon and SmemCalibration into $(TOP)/bin
```
make install
```

* install TscMon and SmemCalibration into $(DESTDIR)

```
make install DESTDIR=~/bin
```

### Cross Compling TscMon and SmemCalibration

* Set the cross compling tools and build
```
unset LD_LIBRARY_PATH
source /opt/ifc14xx/2.6-4.14/environment-setup-ppc64e6500-fsl-linux
make
```

* Check the binaries are for PowerPC64
```
readelf -h {TscMon,SmemCalibration} |grep Machine
  Machine:                           PowerPC64
  Machine:                           PowerPC64
```



## Shared User Libraries

With this buidling system, we don't support the shared user libraries. Please use e3 module instead or ESS Linux distribution. 

