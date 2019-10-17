IFC14xx Kernel Driver and Binary
====

## tsc kernel driver

```
cd driver
make modules
make modules install
```

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

## Shared User Libraries

With this buidling system, we don't support the shared user libraries. Please use e3 module instead or ESS Linux distribution. 
