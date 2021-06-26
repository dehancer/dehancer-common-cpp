//
// Created by денис on 26.06.2021.
//

#pragma once

#if WIN32

#include "dehancer/windows/utf8/utf8.h"

#endif

#include "dehancer/Utils.h"
#include <fstream>
#include <iostream>


#if WIN32

#include <io.h>
#include <cstdio>
#include <cstdlib>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <intrin.h>
#include <iphlpapi.h>
#include <fileapi.h>

#define F_OK 00
#define R_OK 04
#define W_OK 06
#define X_OK 06

#else

#include <unistd.h>

#endif

namespace dehancer::platform {
    
    dehancer::Error access(const std::string& path, int mode);
    dehancer::Error create_directories(const std::string& dir);
    FILE* fopen(const std::string& path, const std::string& mode);
    
    #if WIN32
    
    using ofstream = utf8::ofstream;
    using ifstream = utf8::ifstream;
    using fstream  = utf8::fstream;
    
    #else
    
    using ofstream = std::ofstream;
    using ifstream = std::ifstream;
    using fstream  = std::fstream;
    
    #endif
    
}