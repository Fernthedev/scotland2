# Tests for scotland2 loader
# This CMakeLists.txt is included from the main project when BUILD_FOR is LINUX

if (NOT ${BUILD_FOR} STREQUAL LINUX)
  message(STATUS "Skipping scotland2 loader tests (BUILD_FOR must be LINUX)")
  return()
endif()

enable_testing()

# Add test executable sources
set(TEST_SOURCES
  test_phase_loading.cpp
  test_dependency_finding.cpp
  test_dependency_sorting.cpp
  test_missing_dependency.cpp
  test_duplicate_phases.cpp
)

# Loader source files needed for tests
set(LOADER_SOURCES
  ${SOURCE_DIR}/loader.cpp
  ${SOURCE_DIR}/elf-utils.cpp
)

# Test properties
set(TEST_TIMEOUT 30 CACHE STRING "Timeout for each test in seconds")

# Create individual test executables
foreach(test_source ${TEST_SOURCES})
  get_filename_component(test_name ${test_source} NAME_WE)
  add_executable(${test_name} ${test_source} ${LOADER_SOURCES})

  # Link against standard libraries
  target_link_libraries(${test_name} PRIVATE -ldl)

  # Add include directories
  target_include_directories(${test_name} PRIVATE ${INCLUDE_DIR})
  target_include_directories(${test_name} PRIVATE ${SOURCE_DIR})
  target_include_directories(${test_name} PUBLIC ${SHARED_DIR})
  target_include_directories(${test_name} PRIVATE ${EXTERN_DIR}/includes/fmt/fmt/include)

  # Add compile definitions and options
  target_compile_definitions(${test_name} PRIVATE LINUX_TEST FMT_HEADER_ONLY)
  target_compile_options(${test_name} PRIVATE -fno-rtti)

  # Add test to CTest
  add_test(
    NAME ${test_name}
    COMMAND ${test_name}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  )

  # Set test properties
  set_tests_properties(${test_name} PROPERTIES
    TIMEOUT ${TEST_TIMEOUT}
    LABELS "loader;core"
  )

  message(STATUS "Added test: ${test_name}")
endforeach()

# Summary
message(STATUS "Scotland2 loader tests configured")
message(STATUS "  Tests: ${TEST_SOURCES}")
message(STATUS "  Timeout: ${TEST_TIMEOUT}s")
message(STATUS "  Run tests with: ctest --verbose")
