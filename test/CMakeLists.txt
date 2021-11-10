set(This MolSim_test)

# we use the release 1.11.0
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

FetchContent_MakeAvailable(googletest)

# collect all cpp files in the test directory
set(Test_Sources
        ParticleContainerTest.cpp
        LennardJonesTest.cpp
        main.cpp)

enable_testing()

# collect all source files
#set(SRC_COPY ${MY_SRC})
#
## exclude the _MAIN FILE_ so we don't have two main files
#list(REMOVE_ITEM SRC_COPY "${CMAKE_SOURCE_DIR}/src/MolSim.cpp")

# adds the executable with both file lists
# add_executable(${This} ${Test_Sources} ${SRC_COPY})
add_executable(${This} ${Test_Sources})

# set_target_properties(${This} PROPERTIES EXCLUDE_FROM_ALL True)

# use c++ 17
target_compile_features(${This}
        PRIVATE
        cxx_std_17
        )

# include the necessary directories
target_include_directories(${This} PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/libs/libxsd"
        )

# links the necessary libraries (especially gtest_main)
target_link_libraries(${This}
        PUBLIC
        gtest_main
        ${ThisLib}
        xerces-c)

include(GoogleTest)

## This adds the "make test" target.
#add_custom_target(testExe
#        COMMAND ${This}
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        COMMENT "What now")

gtest_discover_tests(${This})