cmake_minimum_required(VERSION 3.16)

# define project name, version
project(PSEMolDyn_GroupC)

# indicate the documentation build as an option and set it to ON by default
# to disable write cmake -D BUILD_DOC=OFF
option(BUILD_DOC "Build documentation" ON)

# indicate the test build as an option and set it to OFF by default
# to enable write cmake -D BUILD_TEST=ON
option(BUILD_TEST "Build tests" OFF)

set(This MolSim)

set(ThisLib MolSim_Lib)


# let ccmake and cmake-gui offer the default build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

# set Release as the default build type if it is not yet set.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(FetchContent)

include(CTest)

FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.9.2.zip
)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG fec56a1a16c6e1c1b1f4e116a20e79398282626c
)

FetchContent_MakeAvailable(spdlog json)

# collect all cpp files
file(GLOB_RECURSE MY_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        # header don't need to be included but this might be necessary for some IDEs
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        )

# create a library with all our files
add_library(${ThisLib} STATIC ${MY_SRC})
# set an alias for distinct library identification
add_library(lib::${ThisLib} ALIAS ${ThisLib})

# include our necessary directories
target_include_directories(${ThisLib} SYSTEM
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/libxsd
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/test
        )

# link other libraries to our library
target_link_libraries(${ThisLib}
        # stuff that is used in headers and source files
        PUBLIC
        xerces-c
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        )

# link targets with openmp
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${ThisLib}
            PUBLIC
            OpenMP::OpenMP_CXX)
endif()

# set c++ to 17
target_compile_features(${ThisLib}
        PUBLIC
        cxx_std_20
        )

# create our executable main target
add_executable(${This} ${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp)
# link our libraries to that target
target_link_libraries(${This} lib::${ThisLib})




# activate all compiler warnings. Clean up your code :P
# depending on the compiler different flags are used
target_compile_options(${This}
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        # disable some spam warnings for icpc...
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
        )

target_compile_options(${ThisLib}
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        # disable some spam warnings for icpc...
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
        )

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include(doxygen)

# add the needed subdirectories
add_subdirectory(test)

enable_testing()
