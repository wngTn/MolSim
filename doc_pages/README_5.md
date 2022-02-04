# Group C - Assignment 5 #
Members:
* Lukas Baader
* Severin Schmidmeier
* Tony Wang

# Code #
* Link:     https://github.com/wngTn/MolSim
* Branch:   main
* Revision: -

# Copy Pasta Section 🍝 #

This is a new section to enable easier testing and assessment of our assignment. Simply lay back, copy the instructions and cherish the impeccable convenience:

1. Compiling the code:
    ```bash
    $ mkdir build && cd build 
    $ cmake .. -DBUILD_TEST=ON # enable the tests and OpenMP if installed
    $ make -j 
    ```
2. Running the tests:
    ```bash
    $ ctest
    ```
3. Running input files:
   3.1. Running task 1 - "Simulation of a membrane"
    ```bash
    $ ./MolSim -x ../input/files/assignment_5/task1.xml # ~1min, uses primitiveY strategy
    ```
   3.2 Running task 4 - "Nano-scale flow simulation"
    ```bash
    $ ./MolSim -x ../input/files/assignment_5/task4_experiments/task4_assignment.xml 
    ```
   3.2.1 Running task 4 with other configurations:
    ```bash
    $ ./MolSim -x ../input/files/assignment_5/task4_experiments/task4_fixed_spheres.xml 
    $ ./MolSim -x ../input/files/assignment_5/task4_experiments/task4_higher_gravity.xml 
    $ ./MolSim -x ../input/files/assignment_5/task4_experiments/task4_walls_equal_fluid.xml
    ```
   3.3 Running task 5 - "Crystallization of Argon"
   - `fluid` creates the quilibrated argon, choose either `cooling` or `supercooling` to continue from the checkpoint
    ```bash
    $ ./MolSim -x ../input/files/assignment_5/task5_fluid.xml
    $ ./MolSim -x ../input/files/assignment_5/task5_cooling.xml
    $ ./MolSim -x ../input/files/assignment_5/task5_supercooling.xml
    ```

4. Trying out parallelization strategy (`t_end` has been adjusted to a lower number):
   4.0 Adjust OMP environment variables:
    ```bash
    $ export OMP_NUM_THREADS=<Your_desired_number>
    ```
   4.1. Running task 1 *serial*:
    ```bash
    $ ./MolSim -x ../input/files/assignment_5/task1_benchmarks/task1_serial.xml -m benchmark
    ```
   4.2 Running task 1 with *primitiveX*:
    ```bash
    $ ./MolSim -x ../input/files/assignment_5/task1_benchmarks/task1_primitiveX.xml -m benchmark 
    ```
   4.3 Running task 1 with *primitiveY*:
   ```bash
      $ ./MolSim -x ../input/files/assignment_5/task1_benchmarks/task1_primitiveY.xml -m benchmark 
   ```

   4.4 Running task 1 with *primitiveZ*:
    ```bash
    $ ./MolSim -x ../input/files/assignment_5/task1_benchmarks/task1_primitiveZ.xml -m benchmark 
    ```
   4.5 Running task 1 with *subDomain*:
    ```bash
    $ ./MolSim -x ../input/files/assignment_5/task1_benchmarks/task1_subDomain.xml -m benchmark 
    ```

5. Enjoy our videos 😊:
   [Click me! 😇](https://nextcloud.in.tum.de/index.php/s/4A7Srgx3iNF7QT6)
   * `task1.mp4`: Task 1 Aufgabe
   * `task3_rayleigh_taylor.mp4`: Task 3 Aufgabe
   * `task3_rayleigh_taylor_orbit.mp4`: Task 3 Aufgabe mit einer sehr *fancy*  Kamerabewegung
   * `task4_fixed_spheres_orbit.mp4`: Task 4 Aufgabe mit zwei fixierten Kugeln in der Mitte + *fancy* Kamerarotationen
   * `task4_wall_eq_fluid.mp4`: ε und σ von wand unf fluid sind gleich
   * `task4.mp4`: Task 4 Aufgabe
   * `task5_cooling_complete.mp4`: Task 5 alle Zeitschritte (cooling Argon) mit `velocity` als Einfärbung
   * `task5_supercooling_complete_velocity.mp4`: Task 5 alle Zeitschritte (supercooling Argon) mit `velocity` als Einfärbung
   * `task5_cooling_only_60_fps.mp4`: Zeitschritte ab dem das Argon gecooled wird in *60 fps*
   * `task5_supercooling_60_fps.mp4`: Zeitschritte ab dem das Argon supergecooled wird in *60 fps*
   * `task5_supercooling_cooling_only_veloctiy.mp4`: Zeitschritte ab dem das Argon supergecooled wird mit `velocity` als Einfärbung

6. Go to the Miscellaneous section and test out our cool R script! (Optional)

# Run Instructions #

In this last assignment of ours, the holy `g++ 11.1.0` continued to be our great compiler companion.

The run instructions and flags did not change since last assigment.

```shell
./MolSim [-x <XML_file>] [-i <input_file>] [-g <generator input>] [-e <end_time>] [-d <delta_t>] [-w <writer>] [-c <calculator>] [-b <brownian_motion_velocity_mean>] [-r] [-m <execution_mode>]
```

| Flag |            Possible Values             | Explanation                                                                                                                                            | Default           |
|------|:--------------------------------------:|--------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------|
| `i`  |             `path/to/file`             | This is the relative or absolute path to your `input_file`.                                                                                            | *None*            |
| `x`  |             `path/to/file`             | This is the relative or absolute path to your `XML_file`.                                                                                              | *None*            |
| `g`  |             `path/to/file`             | Specify an input file used for the ParticleGenerator.                                                                                                  | *None*            |
| `e`  |               \<double>                | The end_time value                                                                                                                                     | 1000              |
| `d`  |               \<double>                | The delta_t value                                                                                                                                      | 0.14              |
| `w`  |             v, vtk, x, xyz             | Specifies the output writer, consequently also the output file format                                                                                  | v (vtk)           |
| `c`  | g, grav, gravitation, lj, lennardjones | Declares what forces between the particles should be calculated                                                                                        | lj (lennardjones) |
| `b`  |               \<double>                | The <tt>brownian motion velocity mean</tt>. **Will be discarded for the JSON file generated particles**                                                | *None*            |
| `r`  |                 *None*                 | Specifies whether random particles should be generated.                                                                                                | *None*            |
| `m`  |        normal, debug, benchmark        | Specifies which execution mode should be used. **Debug** to enable logging, **normal** to disable logging or **benchmark** to disable all output files | normal            |




# Warning # 

> ✅ No **new** warnings in this assignment

# Task 0 - Tests #

Tests are run as usual.

> ⚠️ **The test `LinkedCellParallelTest.parallelStrategyTest`:** may need a few more seconds, since it compares all parallelization techniques with different number of threads over multiple iterations with 1000 particles (MacBook needs ~10s for this test).


## Running the tests ##

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

## Disabling the tests ##

If you do not want to compile the tests, you can turn off the build option
in the CMake by executing cmake without any arguments (Step 2):

```shell
cmake .. # Makefile won't build the tests anymore
```

# Task 1 - Simulation of a membrane #

For the membrane, we introduced a new `membrane` attribute in the `Particle` class, which can be set with a new behaviour attribute in the XML file.

We also store the index of the particle in the grid of the generated cuboid, to determine with which other particles the harmonic potential should be used.

If during force calculation, both particles are of membrane type, the Lennard-Jones force is truncated and if they additionally are neighbors in the membrane grid, the harmonic potential is applied.

To have the special force that acts on some particles, we introduced the possibility to give 'special particles' in the XML file different attributes based on their index in the cuboid/sphere grid.

E.g. the base velocity and the mass can differ from the other particles, one can set a base force vector like needed in Task 1 or one can change whether some particles are immovable/part of a membrane.

The base force is applied to all particles for the time specified in the calculator part of the XML (or forever).
Example of new XML:
```xml
 <generatorInfo type="cuboid" behaviour="membrane">
   [ ... ]
   <special_particle immovable="true">
      <position x="17" y="24" z="0" />
      <mass>1.7</mass>
      <force x="0" y="0" z="0.8" />
   </special_particle>
   <special_particle>
      <position x="17" y="25" z="0" />
      <force x="0" y="0" z="0.8" />
   </special_particle>
   [ ... ]
```

It is also possible the set the base force for the whole cuboid at once using the `baseForce` element in the XML.

Sneak Peak:

![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbab.16.03.png)
Simulating a membrane ([video](https://nextcloud.in.tum.de/index.php/s/4A7Srgx3iNF7QT6?))


# Task 2 - Parallelization #

## Prelude ##

Our CMake automatically compiles with OpenMP if it is found on the machine. If you want to disable it you can use the `OPEN_MP` option:
* `-DOPEN_MP=ON`, if you want to compile with OpenMP (default)
* `-OPEN_MP=OFF`, if you want to compile without OpenMP

You can also use the different parallelization techniques without compiling with OpenMP, therefore you will get some warnings

We use two parallelization techniques: `primitive` and `subDomain`.

## Primitive ##

This technique splits the force calculation in two parts. The domain will be split at one of the two or three axis into <tt>n</tt> lines of cells (<tt>n</tt> is length of the specific axis). The first part computes the first <tt>n/2</tt> lines, the second parts the remaining <tt>n/2</tt> lines. The neat things here is that no two lines, in each part, has neighboring cells. Therefore, each thread can compute a line without inhibiting any of the other processes.

### PrimitiveX ##

An example for a 10 * 28 domain:

![](https://codimd.s3.shivering-isles.com/demo/uploads/51a99d051f3b1f1fe4246508f.45.04.png)
*Figure 1: First part, split at the x-axis*


![](https://codimd.s3.shivering-isles.com/demo/uploads/51a99d051f3b1f1fe42465090.45.27.png)
*Figure 2: Second part, also split at the x-axis*


Each process takes one vertical line (or more, if there are more lines than processes) and can calculate its respective cells without considering other processes -> this allows no need of synchronization or locking within each part.

However, before the second part may commence, the first part has to finish, else there will be race conditions. That is, the red columns have to finish, before the green columns may commence.


### PrimitiveY ###

We can also split the domain along the y-axis. This, therefore, is called *primitiveY*.

We, however, need to change the corresponding neighbors, too, so that each row will not interfere with any of the neighbors of another row. To achieve this, we need to change the neighbor so that they only appear at one side of the axis we are splitting at. Therefore, if we want to split at the y-axis, our neighbors will only be the one beneath the y-axis, or above the y-axis. We chose beneath the y-axis.

Example for a 28 * 10 domain:

![](https://codimd.s3.shivering-isles.com/demo/uploads/b701362d73f35ebdd3fa8d3c1.13.25.png)
*Figure 3: First part, split at the y-axis*


![](https://codimd.s3.shivering-isles.com/demo/uploads/b701362d73f35ebdd3fa8d3c0.13.12.png)
*Figure 4: Second part, split at the x-axis*


### PrimitiveZ ###

At last, we can also do the same on the z-axis - however, only in 3D.

### Notes ###

If you try to split the domain along the z-axis in a 2D domain, the program will automatically switch to the y-axis.


## SubDomain ##

The second technique we have thought of is dividing the domain in
subdomains. These domains are split across the longest axis and the number of subdomains is `min{omp_get_max_threads(), longest_dimension/2}`.

Example on a 28 * 10 domain:

![](https://codimd.s3.shivering-isles.com/demo/uploads/b701362d73f35ebdd3fa8d3ca.21.24.png)
*Figure 5: Domain split into two subdomains, darker colors depict the border cells*



The remaining problem in this approach is that that cells at the border still interfere with cells of other subdomains. However, since we always split from top to bottom, the upper and bottom border always count toward the same sub-domain. Therefore we can update our "actual" borders:

![](https://codimd.s3.shivering-isles.com/demo/uploads/b701362d73f35ebdd3fa8d3cd.31.41.png)
*Figure 6: Domain split into two subdomains, darker colors depict the "actual" borders*

Now, we still have to deal with the other 4 borders (two borders per subdomain).
Our approach **for the border cells** can be written as follows:
```
For every subDomain:
    Loop through every border cell:
        Loop through every neighbor (turqouise arrows):
            If neighbor is in subDomain:
                Do force calculation with newton
            Else:
                Do force calculation without newton (we do not write on other subDomains)
        Loop through every other neighbor (yellow arrows):
            If neighbor is not in subDomain:
                Do force calculation without newton (we do not write on other subDomains)
```
*Colors used in this code snippet refer to the color of the arrows in figure 7*

![](https://codimd.s3.shivering-isles.com/demo/uploads/b701362d73f35ebdd3fa8d3ce.34.20.png)
*Figure 7: Sub-domains with their cell-specific neighbors*


The forces in the inner cells (lighter color) are calculated in the usual fashion.

This way we can calculate subdomains independently of each other because they do not interfere with each other in any way. We do not have to synchronize anything, however, we do have some calculation overhead ad the borders, as we calculate the forces at the borders twice, since we do not use newton's third law.

### Benchmarks ###

For benchmarking we ran simulations with different parallelization strategies on a single node from the [CoolMuc2](https://doku.lrz.de/display/PUBLIC/CoolMUC-2) cluster with 28 threads, thus utilizing all 28 cores.
![](https://codimd.s3.shivering-isles.com/demo/uploads/3d6ce97c22bd22991c8989209.png)

## XML - File ##

The parallelization technique is indicated in the XML file. Simply add a parallelization technique to the `simulation` tag with the `parallelization` attribute as follows:

```xml=
<simulation t_end="1.5" delta_t="0.0005" writeFrequency="100" outputWriter="vtk" parallelization="primitiveX">
```
This uses the *primitiveX* strategy.
Other possible values are:
* *primitiveY*
* *primitiveZ*
* *subdomain*
* *none*

If you do not include the `parallelization` attribute, you will not use any strategy and fall back to the serial strategy.

## CalcX - LinkedCell ##

We also parallelized our calcX method in the linked cell method, since we also check in this method whether any particles need to change its cell. If you do not want this method to perform with multiple threads, simply compile the program without OpenMP.

## Making it fast(er) ##

Comparing the runtime of different thread amounts with the sequential runtime we get the following results, peaking at 14 threads:
![](https://codimd.s3.shivering-isles.com/demo/uploads/3d6ce97c22bd22991c898920a.png)

## Profiling ##

We tried the MAQAO analysis framework.
It generates nice html overviews of the results, but didn't really give us some surprising insights.
One interesting thing was the potential speedup if fully vectorized for some loops.
E.g. in the ljforce method one loop can apparently be sped up 3.64-fold if fully vectorized.

# Task 3 - Rayleigh-Taylor instability in 3D #



## Contest 2 ##

As in the last contest we compiled our project with g++ like this:
```shell=
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/dss/dsshome1/lrz/sys/spack/release/21.1.1/opt/x86_64/gcc/10.2.0-gcc-ll77x2s/bin/g++ -DCMAKE_C_COMPILER=//dss/dsshome1/lrz/sys/spack/release/21.1.1/opt/x86_64/gcc/10.2.0-gcc-ll77x2s/bin/gcc ..
```

Although we got good results with icpc, we can't use it anymore, as we now use C++20 features, which Intel doesn't support.

Sneak Peak:
![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbed.07.04.png)
*Rayleigh-Taylor Instability in 3D* ([video](https://nextcloud.in.tum.de/index.php/s/4A7Srgx3iNF7QT6?))

### Contest Results ###

We got these results with the Rayleigh-Taylor 2D/3D input files, primitiveX parallelization and 1000 iterations.
![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbf0.png)


| Simulation Type | Time     | MMUPS   |
| --------------- | -------- | --------|
| 2D              | 2937ms   | 3,40483 |
| 3D              | 167202ms | 0,59808 |



# Task 4 - Nano-Scale flow simulation (Option A) #

To fix the positions of some particles, we introduced an `immovable` attribute in the `Particle` class.
If this attribute is set, the `setF()` method does not set the force.

Additionally, we also check whether both particles are immovable before we calculate some things to perform less unnecessary calculations.

We tried both a branchless and a branched approach, but didn't see any major performance differences using QuickBench.
For the CLang compiler the branched version is slightly faster, while the GCC compiler hasn't any significant difference since it produces almost identical assembly for both versions.
```cpp
// branchless
void Particle::setF(std::array<double, 3> force) {
f = !immovable*force;
}

//branched
void Particle::setF(std::array<double, 3> force) {
if(!immovable){  f = force; }
}
```

We extended the Thermostat to be able to exclude the Y direction like specified in the assignment and also adapted it to not factor in immovable particles.

For the statistics we added the abstract superclass `StatisticsLogger` which removes any need to check which kind of statistics we use in the main loop of our program.

For this task we added the subclass `DensityVelocityProfile`, which partitions the domain into `x` bins along the x-axis, computes statistics every `n`-th iteration and writes them to a csv-file. `x`, `n`, the filename and the type of statistics can be specified in the XML.

The result of the Density Velocity Profile:
![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbc0.jpg)
It's clearly visible that the edges are slower as they are slowed down by friction with the walls.
After around 210.000 iterations the edges start to speed up again until they're the same speed as the center at the end.
See graphs in media folder.

We also did the experiment with the following changes:
- Walls and fluid have same σ and ε values `media/week_5/task4/walls_eq_fluid/`
- Higher gravity `media/week_5/task4/highgrav/`
- 2 fixed spheres in fluid `media/week_5/task4/spheres/`

See `input/files/assignment_5/task4_experiments/` for the corresponding input files.

Sneak Peak:
![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbef.47.48.png)
*Nano-Scale flow simulation* ([video](https://nextcloud.in.tum.de/index.php/s/4A7Srgx3iNF7QT6?))


# Task 5 - Crystallization of Argon (Option B) #

We also implemented task 5.

For the smoothed Lennard-Jones potential, we simply added an attribute to the calculator class to choose between the normal and smoothed LJ.

For the statistics of task 5, we extended the `StatisticsLogger` superclass with a `Themodynamical` subclass.

While the radial distribution function was more straightforward, for the diffusion we needed a way to access the old position of the particles.

After considering some different ideas, we settled on storing it in the particle class like the `oldF` attribute.
To accommodate particles crossing periodic boundaries, we also added `passedPeriodic` booleans for each direction to the particle, which are flipped everytime a particle crosses the specific boundary.

Using these we can correct for higher distances due to the periodic boundaries during diffusion calculation.

Below the comparison of the RDFs and the density between cooling and supercooling.
Notice the different scales on the diffusion graphs.
- cooling RDF
  ![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbc3.jpg)
- supercooling RDF
  ![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbc2.jpg)
- cooling diffusion
  ![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbc4.jpg)
- supercooling diffusion
  ![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbc1.jpg)

Like the graph already tells, it is clearly visible in the simulation that the supercooled argon is moving way less (is colder) even after merely 100.000 iterations.

It is also visible that the supercooled argon freezes into a grid structure, albeit with big holes, whereas the cooled argon is not as structured.

Sneak peak:

![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbe7.11.02.png)
*Cooling Argon* ([video](https://nextcloud.in.tum.de/index.php/s/4A7Srgx3iNF7QT6?))


![](https://codimd.s3.shivering-isles.com/demo/uploads/e76375821f940e93084addbe6.10.12.png)
*Supercooling Argon* ([video](https://nextcloud.in.tum.de/index.php/s/4A7Srgx3iNF7QT6?))

# Miscellaneous #

## R Script ##

### Instructions ### 

We added an R script at `./input/assignment_5_script.R` that can automatically generate plots for either task 4 and task 5.

The general run instruction:

```bash=
$ Rscript assignment_5_script.R [<iteration_num>] [<task>] [<path_to_csv>]
```

| Args | Possible Values | Explanation | Default |
| -------- | -------- | -------- | --- |
| `iteration_num`     | \<integer>    | Specifies how many iterations should be plotted. In case of the density plotting, it specifies every nth iteration it should plot | 30 |
| `task` | task4, task5 | Specifies what task the input file is from | task4
| `path_to_csv` | `path/to/file` | Relative or absolute path to your `csv_file` | `files/assignment_5/csv/statistics_task4.csv`


To reproduce:

If you (the reader) do not have R, please install R.

0. Install some R libraries:
```bash=
$ R # enter R shell
> install.packages(c("ggplot2", "dplyr", "tidyverse")) # installs necessary packages
> q() # quit shell
```

1. Change directory:
```bash=
$ cd input
```

2. Generate plot for task 4 (first 25 iterations):
```bash=
$ Rscript assignment_5_script.R 25 task4 files/assignment_5/csv/statistics_task4.csv
```

3. Generate plot for task 5:
   3.1 Generate Particle Density Graph of the cooling configuration (**every 25th iteration!**):
    ```bash=
    $ Rscript assignment_5_script.R 25 task5 files/assignment_5/csv/RDF_task5_cooling_statistics.csv
    ```
   3.2 Generate Particle Density Graph of the supercooling configuration (**every 25th iteration!**):
    ```bash=
    $ Rscript assignment_5_script.R 25 task5 files/assignment_5/csv/RDF_task5_supercooling_statistics.csv
    ```
   3.3. Generate Diffusion Graph of the cooling configuration (first 250 iterations!):
    ```bash=
    $ Rscript assignment_5_script.R 250 task5 files/assignment_5/csv/diffusion_task5_cooling_statistics.csv
    ```
   3.4. Generate Diffusion Graph of the cooling configuration (first 250 iterations!):
    ```bash=
    $ Rscript assignment_5_script.R 250 task5 files/assignment_5/csv/diffusion_task5_cooling_statistics.csv
    ```

The resulting plot will be saved in the `./input/` directory.
The file name is: `<input_csv_file_name>_<iteration_num>_iterations.pdf`.

### Task 4 ###

The script plots the (absolute) falling velocities (i.e. the velocity in the y-axis direction of the particles) on the ordinate and the bins on the abscissa. Every color depicts one iteration and every point on the graph depicts the specific average falling velocity of the particles in the bin, which are connected by the dotted lines.

The solid lines represent the respective regression lines of the velocities in every iteration. We used the local regression with the locally estimated scatterplot smoothing (loess) method while using the velocity as dependent and bins as independent variables.

### Task 5 ###

#### Particle Density ####

The dots in the particle density scatter plots are simply connected with a linear line for better visualization. The different colors also depict each iteration.

#### Diffusion ####

The dots in this scatter plot are connected with a black line. The blue line represents the regression line, the grey zone the confidence interval.
The regression lines was also used with the loess method and a confidence level interval of 95%.

## Input Validation ##

We added a component that tries to validate our input and catch some common mistakes. Besides basics like `end_time <= start_time`, we especially check whether any particles are outside of the LinkedCell domain, since this would lead to segmentation faults.
We also check, whether any particles are closer than 0.5\*σ, since this probably means two shapes intersect which again would lead to segmentation faults. The 0.5 value is choosen without further physical backing and could be changed in the future if other values turn out to be better fitting.

## Automated Running ##

Since simulations tend to be quite big by now and running a multithreaded simulation on our private PCs really takes a toll on their usability, we want to run different simulations remote and extract only the statistics and maybe the output. To make this process easier, we added a python script, that runs a list of input files and zips their output.
Using this we can let a private server run many simulations and retrieve the statistics later without rendering our PCs useless for an hour for each simulation.

The `run_multiple_inputs.py` script needs `zip` to be installed on the system.

## [[unlikely]] ##

In the main simulation loop, there are multiple `if` conditions that are true only very seldom (e.g. for writing statistics, output, applying the thermostat etc).

In C++20 there is the `[[unlikely]]` attribute to help the branch predictor skip those seldom used branches.
Unfortunately we didn't notice any real speedup, but we're not 100% sure there isn't any since things like this are hard to benchmark.
Maybe the branch predictor is smart enough to skip those `if`s even without the attribute or maybe the compilers don't really use this new feature.

## goodbye ##
We really enjoyed the course! Thank you :)