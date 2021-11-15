@mainpage PSE Molecular Dynamics

<!-- PROJECT LOGO -->
<div style="text-align: center">
<h1>Practical Course: Molecular Dynamics</h1>
<a style="font-size: 4rem">📜</a>

  <p>
    Project for the Practical Course: PSE Molecular Dynamics by Group C: Lukas, Severin, Tony.
  </p>
</div>


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

