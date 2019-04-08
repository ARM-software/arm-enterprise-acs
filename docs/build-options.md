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

For Example:<br/>
$ acs_sync.sh luvos<br/>
will synch only luvos for building

$ acs_sync.sh sbsa<br/>
will synch only sbsa for building

$ acs_sync.sh<br/>
will sync all modules

### luvos/scripts/build.sh

Usage:
```sh
/path/to/armacs/luvos/scripts/build.sh [sbbr|sbsa|sdei|luv-live-image] [cleanall]
```

/path/to/armacs/luvos/scripts/build.sh can accept upto 2 optional parameters

For Example:<br/>
$ ./luvos/scripts/build.sh sbbr cleanall<br/>
will clean only sbbr module

$ ./luvos/scripts/build.sh sbsa<br/>
will build only sbsa module

$ ./luvos/scripts/build.sh cleanall<br/>
will clean all modules

$ ./luvos/scripts/build.sh<br/>
will build all modules (no clean is done)


### Typical Usage Scenarios
Prerequisite : The complete build is done atleast once.

To synch and build only sbsa, the following commands may be run:<br/>
$ cd /path/to/armacs/<br/>
$ ./acs_sync.sh sbsa<br/>
$ luvos/scripts/build.sh sbsa cleanall<br/>
$ luvos/scripts/build.sh sbsa<br/>


To build the entire system, the following commands may be run:<br/>
$ cd /path/to/armacs/<br/>
$ ./acs_sync.sh<br/>
$ luvos/scripts/build.sh cleanall<br/>
$ luvos/scripts/build.sh<br/>

