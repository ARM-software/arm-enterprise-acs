# Build Options in Arm Enterprise ACS

There are options to selectively update and build the modules of ARM Enterprise ACS. These options are provided through the scripts
- acs_sync.sh
- luvos/scripts/build.sh

##### Prerequisite:
Before using a any options as described below, it is a must that the entire system must be compiled atleast once completely. i.e. when building for the first time, acs_sync.sh and luvos/scripts/build.sh must be run without any options.
If this is not done, then the build may fail.

### acs_sync.sh
Usage:
```sh
/path/to/armacs/acs_sync.sh [luvos|sbbr|sbsa|sdei]
```

acs_sync.sh can accept one optional parameter.

For example:
$ acs_sync.sh luvos
will synch only luvos for building

$ acs_sync.sh sbsa
will synch only sbsa for building

$ acs_sync.sh
will sync all modules

### luvos/scripts/build.sh

Usage:
```sh
/path/to/armacs/luvos/scripts/build.sh [sbbr|sbsa|sdei|luv-live-image] [cleanall]
```

/path/to/armacs/luvos/scripts/build.sh can accept upto 2 optional parameters

For Example.
$ ./luvos/scripts/build.sh sbbr cleanall
will clean only sbbr module

$ ./luvos/scripts/build.sh sbsa
will build only sbsa module

$ ./luvos/scripts/build.sh cleanall
will clean all modules

$ ./luvos/scripts/build.sh
will build all modules (no clean is done)


### Typical Usage Scenarios
Prerequisite : The complete build is done atleast once.

To synch and build only sbsa, the following commands may be run :
$ cd /path/to/armacs/
$ ./acs_sync.sh sbsa
$ luvos/scripts/build.sh sbsa cleanall
$ luvos/scripts/build.sh sbsa


To build the entire system, the following commands may be run :
$ cd /path/to/armacs/
$ ./acs_sync.sh
$ luvos/scripts/build.sh cleanall
$ luvos/scripts/build.sh

