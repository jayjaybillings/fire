---
layout: post
title: Running Fire's Tests
permalink: /about/tests
category: about
---

Fire combines CMake's testing system, CTest, and Boost's Unit Testing 
Framework for testing. 

You can run the tests in Fire after the build by simply executing

```bash
make test
```

Since Fire's build system is built on cmake, you can also execute the tests
using ctest. For example, you can execute

```bash
ctest
```

###Executing By Label

You can run subsets of Fire's tests by passing the labels of the tests that you
want to run to ctest. The following command will execute all of the
"astrophysics" tests in Fire.

```bash
ctest -L FireAstro
```

The valid labels are the CMake module names defined for each packaged in its 
CMakeLists.txt file.

###On NVidia Optimus Cards with Linux

NVidia's Optimus Technology requires special care and feeding on Linux. If you
have this setup and your system is configure correctly, you need only modify
the above command by adding the "optirun" prefix

```bash
optirun make test
```

If you do not use this command, you may see segmentation faults or other
errors. The alternative to optirun, called "primusrun," may or may not work.
(Sometimes it works for the author and sometimes it doesn't.)