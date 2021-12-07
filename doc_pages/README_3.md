# Group C - Assignment 2 #
Members:
* Lukas Baader
* Severin Schmidmeier
* Tony Wang

# Code #
* Link:     https://github.com/wngTn/MolSim
* Branch:   main
* Revision: -

# Run Instructions #

In this assignment we have used the compiler `g++ 11.1.0`.

# Warning # 

> ⚠️ **If you have slow internet**: We fetch three packages with *FetchContent*,
> thus the initial CMake-build may take some time (~1min).
>
> We decided against including the zip files into our directory, for we did not want
> to pollute the repository too much.

> ⚠️ **File structure**: Please put the `build` directory (the directory you
> execute `make`) directly in the root project path, else the automatically generated files
> will not work properly. Like such:
> ```
> MolSim
> .
> ├── CMakeLists.txt
> ├── Doxyfile
> ├── README.md
> ├── build/
> ├── cmake/
> ├── doc_pages/
> ├── input/
> ├── libs/
> ├── media/
> ├── src/
> └── test/
> ```

> ⚠️ **Doxygen**: This assignment we added the automatic generation of inheritance diagrams,
> thus you need to install the `graphviz` package to successfully generate the documentation

## Task 0 - Tests ##

This assignment we pursued the advice and added `enable_testing()` into the
root `CMakeLists.txt`. Therefore, you can run `ctest` in the `build` directory
directly.

### Running the tests ###

1. Create the build directory and cd into it:
```shell
mkdir build && cd build 
``` 
2. Build everything, including the tests in the build directory by turning on the option:
```shell
cmake .. -DBUILD_TEST=ON
``` 

3. Compile everything, including the tests:
```shell
make # use "make -j6" for faster compilation
``` 

5. Simply run `ctest`in the build directory:
```shell
ctest
``` 

### Disabling the tests ###

If you do not want to compile the tests, you can turn off the build option
in the CMake by executing cmake without any arguments (Step 2):

```shell
cmake .. # Makefile won't build the tests anymore
```

## Task - XML Parser ##


## Task - Linked Cell Method ##


## Benchmarking ##

## Miscellaneous ##

In this assignment we made our doxygen documentation design
more beautiful by adding an HTML stylesheet, since the original was gnarly ugly. We used the css templates from this
repository: https://github.com/jothepro/doxygen-awesome-css.

We used and tested the documentation with:
- `Doxygen 1.9.2`

And as browser we have used:
- Safari 15.1
- Chrome 96.0.4664.55.










