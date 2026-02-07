cmake_minimum_required(VERSION 3.15)

# ========================================================================
# BULLsEYE TEST CONFIGURATION
# ========================================================================

project(BULLsEYETests VERSION 1.0.0)

# Enable C++17 for tests
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GoogleTest
find_package(GTest REQUIRED)

# Find Threads (required by GTest)
find_package(Threads REQUIRED)

# ========================================================================
# TEST SOURCES
# ========================================================================

set(TEST_SOURCES
    DSP/TestBULLsEYEProcessor.cpp
    Integration/TestBULLsEYEIntegration.cpp
)

# ========================================================================
# EXECUTABLE
# ========================================================================

add_executable(BULLsEYETests ${TEST_SOURCES})

# ========================================================================
# INCLUDE DIRECTORIES
# ========================================================================

target_include_directories(BULLsEYETests PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
)

# ========================================================================
# LINK LIBRARIES
# ========================================================================

target_link_libraries(BULLsEYETests PRIVATE
    GTest::GTest
    GTest::Main
    Threads::Threads
)

# ========================================================================
# COMPILE OPTIONS
# ========================================================================

target_compile_options(BULLsEYETests PRIVATE
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Debug>:-g>
)

# ========================================================================
# ADD TEST TO CTEST
# ========================================================================

include(GoogleTest)
gtest_discover_tests(BULLsEYETests)

# ========================================================================
# CUSTOM TARGETS
# ========================================================================

# Target to run tests directly
add_custom_target(run_tests
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BULLsEYETests
    DEPENDS BULLsEYETests
    COMMENT "Running BULLsEYE tests..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Target to generate test report
add_custom_target(test_report
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BULLsEYETests --gtest_output=xml:test_results/
    DEPENDS BULLsEYETests
    COMMENT "Running tests and generating report..."
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
