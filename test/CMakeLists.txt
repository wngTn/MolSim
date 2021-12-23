if(BUILD_TEST)

    set(This MolSim_test)
    # we use release 1.11.0
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
    )

    FetchContent_MakeAvailable(googletest)

    # collect all cpp files in the test directory
    set(Test_Sources
            ParticleContainerTest.cpp
            LennardJonesTest.cpp
            MainTest.cpp LinkedCellContainerTest.cpp LinkedCellTest.cpp XMLReaderTest.cpp ParticleTest.cpp ThermostatTest.cpp)

    # add the files to our executable
    add_executable(${This} ${Test_Sources})

    # use c++ 17
    target_compile_features(${This}
            PRIVATE
            cxx_std_17
            )

    # links the necessary libraries (we have our source files as libraries)
    target_link_libraries(${This}
            PUBLIC
            gtest_main
            ${ThisLib})

    include(GoogleTest)

    gtest_discover_tests(${This})
endif()
