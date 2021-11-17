@mainpage PSE Molecular Dynamics

<!-- PROJECT LOGO -->
<div style="text-align: center">
<h1>📜: Practical Course: Molecular Dynamics</h1>
  <p>
    Project for the Practical Course: PSE Molecular Dynamics by Group C: Lukas, Severin, Tony.
  </p>
</div>

> **NOTE**: This is not the README of the assignment. It is simply a README for the whole project. The READMEs
> for the assignments can be found in the `./doc_pages` directory.


<!-- GETTING STARTED -->
## Getting Started

### Prerequisites

Basics for compiling a CMake program:

* gcc/g++ version 11.10 (used by us)
* CMake 3.20 (used by us)

Also, you need the xerces-c library for the parsers we use in the program:
* Ubuntu
  ```sh
  sudo apt install libxerces-c-dev
  ```
* Arch
    ```shell
    sudo pacman -S xerces-c
    ```
* Homebrew
    ```shell
    brew install xerces-c
    ```


### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/wngTn/MolSim.git
   ```
2. Create the build directory and cd into it:
    ```shell
    mkdir build && cd build 
    ``` 
3. Build the Makefile without our tests
    ```shell
    cmake .. -DBUILD_TEST=OFF
    ```

4. Compile everything (without our tests)
    ```shell
    make # use "make -j" for faster compilation
    ```

<!-- USAGE EXAMPLES -->
## Usage

The general program call is:

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
./MolSim -g ../input_assignment_2.json -e 5 -d 0.0002
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
./MolSim -i ../eingabe-sonne.txt -c grav -m debug
```
This will use:
- <tt>eingabe-sonne.txt</tt> as `input_file`
- <tt>1000</tt> as `end_time` (default)
- <tt>0.14</tt> as `delta_t` default)
- <tt>vtk</tt> as `writer` (default)
- <tt>gravitation</tt> as `calculator`
- No `brownian motion velocity mean`
- <tt>debug</tt> execution mode



<!-- Miscellaneous -->
## Miscellaneous

### Running the program on an M1 processor

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

