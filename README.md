# dehancer-common-cpp

Windows
=======
    
    # Install https://www.msys2.org/
    # https://blog.jetbrains.com/clion/2020/12/setting-up-clang-on-windows/

    pacman -S mingw-w64-x86_64-toolchain
    pacman -S mingw-w64-x86_64-clang
    pacman -S libcurl
    pacman -S zlib-devel
    pacman -S libcurl-devel

Testing 
=======
    ctest -C Debug -V