cmake_minimum_required(VERSION 3.2)

project(
  mips_emulator
  VERSION 1.0.0
  LANGUAGES CXX
)

option(MIPS_EMULATOR_BUILD_TESTS "Build tests" FALSE)

# Targets
add_library(mips_emulator INTERFACE)

# Target configuration
target_include_directories(mips_emulator INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(mips_emulator INTERFACE cxx_std_17)

if(MIPS_EMULATOR_BUILD_TESTS)
  add_subdirectory(tests)
endif()
