set(This ${CMAKE_PROJECT_NAME}_test)

# collect all cpp files in the test directory
set(Test_Sources
        ParticleContainerTest.cpp
        LennardJonesTest.cpp
        main.cpp)

enable_testing()

# collect all source files
set(SRC_COPY ${MY_SRC})

# exclude the _MAIN FILE_ so we don't have two main files
list(REMOVE_ITEM SRC_COPY "${CMAKE_SOURCE_DIR}/src/MolSim.cpp")

# adds the executable with both file lists
add_executable(${This} ${Test_Sources} ${SRC_COPY})

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
        xerces-c
        log4cxx)

include(GoogleTest)

gtest_discover_tests(${This})