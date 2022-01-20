# Mips Emulator (Work in progress)

## Building and running tests
### Requirements
- [CMake](https://cmake.org/)
- [Catch2](https://github.com/catchorg/Catch2) CMake automatically downloads this dependency

### Instructions
```
mkdir build
cd build
cmake .. -DMIPS_EMULATOR_BUILD_TESTS=TRUE
make
make test
```
