# What is Avo?

Avo is a light, modern C++20 library containing various modular components useful for writing applications.

The library was initially written in C++17, but now it is being rewritten from scratch in C++20 and with a better design that is less object oriented.

It can currently be built by GCC and MSVC, but not Clang because it does not yet support all the features used.

Avo consists of:
* Various modern utilities
* Mathematical types and functions
* A windowing library
* A graphics drawing library

# Aims and features

* The design of the library is modular and different parts can be used (and tested) separately.
* The library utilizes different programming paradigms and avoids inheritance hierarchies to be more flexible.
* Interfaces are hard to misuse and as many errors as possible are caught at compile time.
* Library code follows C++ Core Guidelines.
* Written with modules when build systems and compilers have good support for modules.
* Support for Windows, Linux, and MacOS.
* UTF-8 by default.
* Free from warnings with all useful warning flags turned on.
* Modern CMake integration.

# CMake usage

## Building and installing
```
git clone https://github.com/avocadoboi/avo.git
mkdir avo/build; cd avo/build
cmake ..
cmake --build . --target avo --config Release
cmake --install .
```

## Using as installation
```cmake
find_package(Avo REQUIRED)
target_link_libraries(target PRIVATE Avo::avo)
```

## Using as subproject
```cmake
add_subdirectory(external/avo)
target_link_libraries(target PRIVATE Avo::avo)
```

## Using FetchContent to download
```cmake
include(FetchContent)

FetchContent_Declare(
  Avo
  GIT_REPOSITORY https://github.com/avocadoboi/avo.git
)
FetchContent_MakeAvailable(Avo)

target_link_libraries(target PRIVATE Avo::avo)
```
