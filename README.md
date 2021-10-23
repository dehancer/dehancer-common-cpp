# dehancer-common-cpp

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
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.bat
    ./vcpkg integrate install
    ./vcpkg.exe install openssl:x64-windows-static
    ./vcpkg.exe install qt5-base:x64-windows-static --keep-going
    ./vcpkg.exe install qt5-charts:x64-windows-static --keep-going
    ./vcpkg.exe install qt5-charts:x64-windows-static --keep-going
    ./vcpkg.exe install curl:x64-windows-static --keep-going
    ./vcpkg.exe install expat:x64-windows-static --keep-going
    ./vcpkg.exe install iconv:x64-windows-static --keep-going
    ./vcpkg.exe install dlfcn-win32:x64-windows-static --keep-going
    ./vcpkg.exe install libiconv:x64-windows-static --keep-going
    ./vcpkg.exe install opencv:x64-windows-static --keep-going
    ./vcpkg.exe install gtest:x64-windows-static --keep-going
    ./vcpkg.exe install openblas:x64-windows-static --keep-going
    ./vcpkg.exe install lapack:x64-windows-static --keep-going


    # cmake integration
    -G
    "Ninja"
    -DBUILD_TESTING=ON
    -DPRINT_DEBUG=ON
    -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
    -DCMAKE_INSTALL_PREFIX=<local instalation>
    -DDEHANCER_DEBUG=ON
    -DUSE_UNICODE=ON

Testing 
=======
    ctest -C Debug -V