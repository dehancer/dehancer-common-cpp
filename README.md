# dehancer-common-cpp

## Build and install

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)
cmake --install build --parallel $(nproc)
```

Make sure to set proper `CMAKE_PREFIX_PATH` and `CMAKE_INSTALL_PREFIX` to discover dependencies and install.

Use `-DBUILD_SHARED_LIBS=ON` for a shared library; the default is static.

`CMAKE_POSITION_INDEPENDENT_CODE` is set to `ON`.

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
