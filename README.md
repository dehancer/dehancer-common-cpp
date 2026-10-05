# dehancer-common-cpp

## Build and install

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
cmake --install build --parallel $(nproc)
```

Make sure to set proper `CMAKE_PREFIX_PATH` and `CMAKE_INSTALL_PREFIX`
to discover dependencies and install.

Use `-DBUILD_SHARED_LIBS=ON` for a shared library; the default is static.

`CMAKE_POSITION_INDEPENDENT_CODE` is set to `ON`.

## Windows build

We build in [Git Bash](https://gitforwindows.org) with `clang-cl`
and we add a magic string to CMake to select runtime.

Use Ninja as a make file generator.

You will need certain dependencies from vcpkg installed.

Set `PATH` to include `clang-cl.exe` from VS.

```sh
export PATH="$PATH:/c/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin"
```

Add something like this to cmake configuration:

```
-G Ninja \
-DCMAKE_C_COMPILER=clang-cl \
-DCMAKE_CXX_COMPILER=clang-cl \
-DCMAKE_MSVC_RUNTIME_LIBRARY='MultiThreaded$<$<CONFIG:Debug>:Debug>' \
-DCMAKE_TOOLCHAIN_FILE="$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake" \
-DVCPKG_TARGET_TRIPLET=x64-windows-static \
-DVCPKG_APPLOCAL_DEPS=OFF
```

## Usage in CMake

```cmake
find_package(dehancer_common_cpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE dehancer_common_cpp::dehancer_common_cpp)
```

The CMake package is always generated and installed.

## Usage with pkg-config

Disabled by default. Configure with `-DCREATE_PKG_CONFIG=ON` to generate and
install `dehancer-common-cpp.pc`.

```sh
export PKG_CONFIG_PATH="$HOME/local-dehancer/lib/pkgconfig"
pkg-config --cflags --libs dehancer-common-cpp
```

## Tests

Install GoogleTest, then:

```sh
cmake -B build -DBUILD_TESTING=ON
cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```
