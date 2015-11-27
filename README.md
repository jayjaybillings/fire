# Fire Framework

Fire is a framework for scientific computing. It includes various reusable tools 
and utilities. This includes
* parsers

## Documentation

Documentation in Fire is generated via Doxygen by running "make doc" during the 
build. This documentation is viewable online at 
[Read The Docs](http://fire-framework.readthedocs.org/en/latest/) with detailed
[API information](http://fire-framework.readthedocs.org/en/latest/doxygen-html/index.html)  

Doxygen handles most of the required documentation without developer 
intervention. This means that in some cases there may be classes that seem to 
have minimal documentation in the source, like classes that implement interfaces 
and provide no additional functionality, but are in fact quite well documented 
by Doxygen. Most IDEs will also auto-generate descriptions for developers too, 
so the author(s) see no need to cover every piece of code with comments.

## Prerequisites
You will need git and cmake to build Fire.

## Checkout and build

From a shell, execute the following commands to compile the code:


```bash
git clone https://github.com/jayjaybillings/fire
mkdir fire-build
cd fire-build
cmake ../fire -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.5
make
```


The above will get the code running, but it will not run the tests or generate the 
documentation. Issue the following commands to do that:
```bash
make test
make doc
```


Build flags, such as -Wall, can be set by prepending the CXX_FLAGS variable to 
the cmake command as such

```bash
CXX_FLAGS='-Wall' cmake ../fire -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_VERSION=4.5
```

Optimization flags should be handled by setting -DCMAKE_BUILD_TYPE=Release 
instead of Debug. Likewise, an optimized build with debug information can be 
acheived by setting -DCMAKE_BUILD_TYPE=RelWithDebugInfo.

## License

See the LICENSE file licensing and copyright information. In short, 3-clause BSD.

## Questions
Questions can be directed to me at jayjaybillings <at> gmail <dot> com.
