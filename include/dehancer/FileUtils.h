//
// Created by денис on 26.06.2021.
//

#pragma once

#include "dehancer/Utils.h"
#include <fstream>
#include <iostream>


#if WIN32

#include <io.h>
#include <cstdio>
#include <cstdlib>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#define F_OK 00
#define R_OK 04
#define W_OK 06
#define X_OK 06

#else

#include <unistd.h>

#endif

namespace dehancer::platform {
    
    Error access(const std::string& path, int mode);
    Error create_directories(const std::string& dir);
    FILE* fopen(const std::string& path, const std::string& mode);

}