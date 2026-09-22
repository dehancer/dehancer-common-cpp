# dehancer-common-cpp

## Build

Requires CMake 4.2+, C++17, and installed dependencies: `base64cpp`, `ed25519cpp`, SQLite3 and threads.

One of: Ninja Multi-Config, Xcode, or Visual Studio build generator is recommended for builds.
Dependencies supplied by a parent target are reused.

```sh
cmake -S . -B build -DCMAKE_INSTALL_PREFIX="$HOME/local-dehancer"
cmake --build build --config Release --parallel $(nproc)
cmake --install build --config Release
```

The default build produces a static library. Use `-DBUILD_SHARED_LIBS=ON` for a shared library.

Override install directories with `CMAKE_INSTALL_LIBDIR`,
`CMAKE_INSTALL_INCLUDEDIR`, or `CMAKE_INSTALL_BINDIR`. Relative directories
support `cmake --install build --prefix /another/prefix` and relocating the
installed tree. Absolute directory overrides remain fixed.

## Usage

```cmake
find_package(dehancer_common_cpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE dehancer_common_cpp::dehancer_common_cpp)
```

The same target is available with `add_subdirectory(path/to/dehancer-common-cpp)`
or FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(dehancer_common_cpp
    GIT_REPOSITORY https://github.com/dehancer/dehancer-common-cpp.git
)
FetchContent_MakeAvailable(dehancer_common_cpp)
```

No interproject pkg-config files or `DEHANCER_ROOT` are required. Tests are
opt-in and are not enabled by a parent's `BUILD_TESTING` setting.
