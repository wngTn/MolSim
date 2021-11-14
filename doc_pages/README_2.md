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

#Warning # 

> ⚠️ **If you have slow internet**: We fetch three packages with *FetchContent*,
> thus the initial CMake-build may take some time (~1min)

## Task - Googletest ##

We decided to include the googletest library with CMakes *FetchContent*. Thus,
it is required to have reasonably fast and stable internet to successfully build with CMake 
(*Deutsche Bahn*-wifi with 20 Mb/s download speed is still enough).

The CMake automatically enables building the tests. Therefore, to run the tests,
simply execute CMake as usual, head to your `build` folder, go to the `test` subfolder
and run `ctest`. 

### Enabling the tests ###

1. Create the build directory and cd into it:
```shell
mkdir build && cd build 
``` 
2. Build everything, including the tests in the build directory
```shell
cmake .. # building tests is default ON
``` 

3. Compile everything, including the tests:
```shell
make # use "make -j4" for faster compilation
``` 

5. Change directory to `test` and run `ctest`:
```shell
cd test && ctest
``` 

### Disabling the tests ###

If you do not want to compile the tests, you can turn off the build option
in the CMake (Step 2):

```shell
cmake .. -DBUILD_TEST=OFF # Makefile won't build the tests anymore
```

## Task - Logging ##


## Task - Lennard Jones Potential ##

## Benchmarking ##



## Miscellaneous ##

In this assignment we managed to run the program on the new 
**MacBook Pro with the M1 Pro** processor. 

To run the program on macOS with an M1 (Pro) processor you have to follow the following
steps:

1. Install homebrew (taken from https://brew.sh/)
```shell
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Install xerces-c:
```shell
brew install xerces-c
```

3. Install GCC 11.1.0:
```shell
brew install gcc
```

4. Create a build directory and cd into it:
```shell
mkdir build && cd build
```

5. Execute CMake and set gcc and g++ compiler you just installed:
```shell
cmake .. -DCMAKE_C_COMPILER=<path to your gcc-11> -DCMAKE_CXX_COMPILER=<path to your g++-11>
```
If you have installed gcc via homebrew on an M1 (Pro), 
then the default path to your <tt>gcc-11</tt> is: `/opt/homebrew/Cellar/gcc/11.2.0_1/bin/gcc-11`.


The default path to your <tt>g++-11</tt> is: `/opt/homebrew/Cellar/gcc/11.2.0_1/bin/g++-11`.

6. And finally you can compile everything:

```shell
make -j
```











