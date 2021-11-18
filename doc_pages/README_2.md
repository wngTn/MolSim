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
> thus the initial CMake-build may take some time (~1min)

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

## Task - Googletest ##

We decided to include the googletest library with CMakes *FetchContent*. Thus,
it is required to have reasonably fast and stable internet to successfully build with CMake 
(*Deutsche Bahn*-wifi with 20 Mb/s download speed is still enough).

The CMake does not automatically enable building the tests. Therefore, to run the tests,
you have to explicitly turn on the option. Thereafter, you can head to your `build` folder, go to the `test` subfolder
and run `ctest`. 

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

5. Change directory to `test` and run `ctest`:
```shell
cd test && ctest
``` 

### Disabling the tests ###

If you do not want to compile the tests, you can turn off the build option
in the CMake by executing cmake without any arguments (Step 2):

```shell
cmake .. # Makefile won't build the tests anymore
```

## Task - Logging ##


## Task - Lennard Jones Potential ##

The input format is [JSON](https://www.json.org/json-en.html). 
**You can declare several shapes in an array.** 
For every shape of those you have to specify the following:

| Property   | Possible Values  
|----------|:-------------:|
| type |  "cuboid", "sphere" |
| pos |  [\<double>, \<double>, \<double>]   | 
| vel |  [\<double>, \<double>, \<double>] |
| radius <br> <div style="font-size: 8pt">*Only for "spheres"*</div> | \<double> |
| N | [\<int>, \<int>, \<int>] |
| distance | \<double> |
| mass | \<double> |
| brownian factor | \<double> |
| brownianDIM | \<int> |

> 🚧 **There are two implementations for generating spheres, one is suited for LJP, one not. Currently the one working for LJP is used**

Here is an example for an input file, which creates two "cuboid" shapes: \
This is the `./input/files/input_assignment_2.json` file.
```json
{
  "shapes":
  [
    {
      "type": "cuboid",
      "pos": [0.0,0.0,0.0],
      "vel": [0.0,0.0,0.0],
      "N": [40,8,1],
      "distance": 1.1225,
      "mass": 1.0,
      "brownianFactor": 0.1,
      "brownianDIM": 3
    },
    {
      "type": "cuboid",
      "pos": [15.0,15.0, 0.0],
      "vel": [0.0,-10.0,0.0],
      "N": [8,8,1],
      "distance": 1.1225,
      "mass": 1.0,
      "brownianFactor": 0.1,
      "brownianDIM": 3
    }
  ]
}
```

If you use the "sphere" type, you have to additionally provide a radius! \
For example:

```json
{
  "shapes":
  [
    {
      "type": "sphere",
      "pos": [0.0,0.0,0.0],
      "vel": [0.0, 0.0,0.0],
      "radius": 1.0,
      "distance": 1.1225,
      "mass": 1.0,
      "brownianFactor": 0.0,
      "brownianDIM": 3
    },
    {
      "type": "cuboid",
      "pos": [15.0,1.0,1.0],
      "vel": [-175.0,-0.5,-0.5],
      "N": [12,12,12],
      "distance": 1.1225,
      "mass": 50.0,
      "brownianFactor": 0.0,
      "brownianDIM": 3
    }
  ]
}
```

> ⚠️ **If you do not comply to these specifications, it may lead to unexpected behavior**

### How to run the program ### 

In this assignment, our input format has changed a little, to give you - 
our sacred and precious user - even more functionality!

First and foremost, build and Compile our program as usual:

```shell
mkdir build && cd build
cmake ..
make
```

The general program call is now:

```shell
./MolSim [-i <input_file>] [-g <generator input>] [-e <end_time>] [-d <delta_t>] [-w <writer>] [-c <calculator>] [-b <brownian_motion_velocity_mean>] [-r] [-m <execution_mode>]
```

| Flag   | Possible Values | Explanation | Default |
|----------|:-------------:|-------------| ------- |
| `i` |  `path/to/file` | This is the relative or absolute path to your `input_file`. | *None*
| `g` |  `path/to/file`| Specify an input file used for the ParticleGenerator. | *None*
| `e` |  \<double> | The end_time value | 1000
| `d` | \<double> | The delta_t value | 0.14
| `w` | v, vtk, x, xyz | Specifies the output writer, consequently also the output file format | v (vtk)
| `c` | g, grav, gravitation, lj, lennardjones | Declares what forces between the particles should be calculated | lj (lennardjones)
| `b` | \<double> | The <tt>brownian motion velocity mean</tt>. **Will be discarded for the JSON file generated particles** | *None*
| `r` | *None* | Specifies whether random particles should be generated. | *None*
| `m` | normal, debug, benchmark | Specifies which execution mode should be used. **Debug** to enable logging, **normal** to disable logging or **benchmark** to disable all output files | normal

> ⚠️ **Random generated files** only support the input file format of assignment 1. Also, you will need Python 3 to use it.

Example:

```shell
./MolSim -g ../input/files/input_assignment_2.json -e 5 -d 0.0002
```

This will use:
- <tt>input_assignment_2.json</tt> as `input_file` for the ParticleGenerator
- <tt>5</tt> as `end_time`,
- <tt>0.0002</tt> as `delta_t`
- <tt>vtk</tt> as `writer` (default)
- <tt>lennardjones</tt> as `calculator` (default)
- <tt>0.1</tt> as `brownian motion velocity mean` (set in the input_file)
- <tt>normal</tt> execution mode (default)

Another example:

```shell
./MolSim -i ../input/files/eingabe-sonne.txt -c grav -m debug
```
This will use:
- <tt>eingabe-sonne.txt</tt> as `input_file`
- *because file format of assignment 1 is used, the t flag is omitted*
- <tt>1000</tt> as `end_time` (default)
- <tt>0.14</tt> as `delta_t` default)
- <tt>vtk</tt> as `writer` (default)
- <tt>gravitation</tt> as `calculator`
- No `brownian motion velocity mean`
- <tt>debug</tt> execution mode



## Benchmarking ##

### Recreating our benchmarks ###

1. Build and compile the program. Your current work directory should be the build directory:
```shell
pwd
/path/to/build
```
2. Benchmarking with the small dataset:
   1. Generate the small dataset: `python ../input/generate_json.py --size small`
   2. **We have used 1 as `end_time` and 0.0002 as our `delta_t`**: `./MolSim -g ../input/files/automatic_generated_input_s.json -e 1 -d 0.0002 -m benchmark`



3. Benchmarking the medium dataset:
    1. Generate the medium dataset: `python ../input/generate_json.py --size medium`
    3. **Specify the correct `end_time` and `delta_t` value**: `./MolSim -g ../input/files/automatic_generated_input_m.json -e 1 -d 0.0002 -m benchmark`

4. Benchmarking the large dataset:
    1. Generate the small dataset: `python ../input/generate_json.py --size small`
    2. **Specify the correct `end_time` and `delta_t` value**: `./MolSim -g ../input/files/automatic_generated_input_l.json -e 1 -d 0.0002 -m benchmark`

The results can be observed in our slides!


> 📝 **Careful:** Make sure to enable the benchmark mode (-m benchmark) and specify the correct `end_time` 
> and `delta_t` values.
> 
> Also, the large dataset might take some minutes (~4min) to finish 



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

---
**📝 NOTE**

We use ANSI escape sequences for our console output. If you do not see any color and
see seemingly random sequences, switch to a Unix operating system :).

---











