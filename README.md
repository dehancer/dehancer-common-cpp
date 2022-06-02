# dehancer-common-cpp

Build ios
=======
    # cmake 3.21 is required
    # https://blog.tomtasche.at/2019/05/how-to-include-cmake-project-in-xcode.html

    git clone https://github.com/dehancer/ios-cmake
    cmake -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE=~/Develop/Dehancer/Dehancer-Plugins/ios-cmake/ios.toolchain.cmake \
    -DENABLE_BITCODE=ON \
    -DPLATFORM=OS64COMBINED \
    -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=~/Develop/local/ios ..
    cmake --build . --config Release && cmake --install . --config Release

Windows GCC
=======
    
    # mingw
    # Install https://www.msys2.org/
    # https://blog.jetbrains.com/clion/2020/12/setting-up-clang-on-windows/

    pacman -S mingw-w64-x86_64-toolchain
    pacman -S mingw-w64-x86_64-clang
    pacman -S mingw-w64-x86_64-cmake
    pacman -S libcurl
    pacman -S zlib-devel
    pacman -S libcurl-devel


Windows MVSC
=======
    # Requrements: 
    # Visual Studio, English Language Pack!
    # https://vcpkg.info/
    # GitBash

    cd C:
    git clone https://github.com/microsoft/vcpkg
    cd /c/vcpkg/
    ./bootstrap-vcpkg.sh
    /c/vcpkg/vcpkg integrate install
    /c/vcpkg/vcpkg install gtest

    # cmake integration
    -G
    "Ninja"
    -DBUILD_TESTING=ON
    -DPRINT_DEBUG=ON
    -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
    -DCMAKE_INSTALL_PREFIX=C:/Users/dennn/AppData/Temp/dehancer
    -DDEHANCER_DEBUG=ON
    -DUSE_UNICODE=ON

Testing 
=======
    ctest -C Debug -V