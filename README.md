# dehancer-common-cpp

Build ios
=======
    # cmake 3.21 is required
    # https://blog.tomtasche.at/2019/05/how-to-include-cmake-project-in-xcode.html

    git clone https://github.com/dehancer/ios-cmake


    #
    # Debug
    #
    export PKG_CONFIG_PATH=~/Develop/local/ios-debug/dehancer/lib/pkgconfig
    cmake -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE=~/Develop/Dehancer/Dehancer-Plugins/ios-cmake/ios.toolchain.cmake \
    -DENABLE_BITCODE=ON \
    -DPLATFORM=OS64COMBINED \
    -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=~/Develop/local/ios-debug ..
    cmake --build . --config Debug && cmake --install . --config Debug

    #
    # Release
    #
    export PKG_CONFIG_PATH=~/Develop/local/ios-release/dehancer/lib/pkgconfig
    cmake -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE=~/Develop/Dehancer/Dehancer-Plugins/ios-cmake/ios.toolchain.cmake \
    -DENABLE_BITCODE=ON \
    -DPLATFORM=OS64COMBINED \
    -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=~/Develop/local/ios-release ..
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
    # Pkgconfig
    # Vim

    cd C:
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.bat
    ./vcpkg integrate install
    ./vcpkg.exe install openssl:x64-windows-static
    ./vcpkg.exe install qt5-base:x64-windows-static --keep-going
    ./vcpkg.exe install qt5-charts:x64-windows-static --keep-going
    ./vcpkg.exe install curl:x64-windows-static --keep-going
    ./vcpkg.exe install expat:x64-windows-static --keep-going
    ./vcpkg.exe install dlfcn-win32:x64-windows-static --keep-going
    ./vcpkg.exe install libiconv:x64-windows-static --keep-going
    ./vcpkg.exe install opencv:x64-windows-static --keep-going
    ./vcpkg.exe install gtest:x64-windows-static --keep-going
    ./vcpkg.exe install openblas:x64-windows-static --keep-going
    ./vcpkg.exe install lapack:x64-windows-static --keep-going

    # Pkgconfig:
    0. create a DIRectory, add it to PATH
    1. go to http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/
    2. download the file pkg-config_0.26-1_win32.zip
    3. extract the file bin/pkg-config.exe to DIR
    4. download the file gettext-runtime_0.18.1.1-2_win32.zip
    5. extract the file bin/intl.dll to DIR
    6. go to http://ftp.gnome.org/pub/gnome/binaries/win32/glib/2.28
    7. download the file glib_2.28.8-1_win32.zip
    8. extract the file bin/libglib-2.0-0.dll to DIR

    # Vim:
    Install Vim to C:\Vim, add C:\Vim\vim82 to PATH

    # cmake integration
    -G
    "Ninja"
    -DBUILD_TESTING=ON
    -DPRINT_DEBUG=ON
    -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
    -DCMAKE_INSTALL_PREFIX=<local instalation>
    -DDEHANCER_DEBUG=ON
    -DUSE_UNICODE=ON

    # CLion toolchain
    1. Select Visual Studio toolchain
    2. Select correct Architecture
    3. Select clang compiler foc c and c++: C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clang-cl.exe 

Testing 
=======
    ctest -C Debug -V