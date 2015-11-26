# Fire Framework

## Prerequisites
You will need git and cmake to build Fire.

## Checkout and build

From a shell, execute the following commands:


```bash
git clone https://github.com/jayjaybillings/fire
mkdir fire-build
cd fire-build
cmake ../fire -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.5
make
```

Build flags, such as -Wall, can be set by prepending the CXX_FLAGS variable to 
the cmake command as such

```bash
CXX_FLAGS='-Wall' cmake ../fire -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.5
```

Optimization flags should be handled by setting -DCMAKE_BUILD_TYPE=Release 
instead of Debug. Likewise, an optimized build with debug information can be 
acheived by setting -DCMAKE_BUILD_TYPE=RelWithDebugInfo.

## Questions
Questions can be directed to me at billingsjj <at> ornl <dot> gov.
