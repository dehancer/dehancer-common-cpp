//
// Created by денис on 26.06.2021.
//

#if WIN32

#include "dehancer/windows/utf8/utf8.h"
#include <windows.h>
#include <intrin.h>
#include <iphlpapi.h>
#include <fileapi.h>

#endif

#include "dehancer/FileUtils.h"

#if WIN32
#include <filesystem>
#endif

namespace dehancer::platform {
// #if WIN32

  // using ofstream = utf8::ofstream;
  // using ifstream = utf8::ifstream;
  // using fstream  = utf8::fstream;

// #else

  // using ofstream = std::ofstream;
  // using ifstream = std::ifstream;
  // using fstream  = std::fstream;

// #endif


    #if WIN32
    
    Error access(const std::string& path, int mode) {
      if (!utf8::access(path, mode)) {
        return Error(CommonError::PERMISSIONS_ERROR, error_string("File %s could not be found or has no certain permissions", path.c_str()));
      }
      else {
        return Error(CommonError::OK);
      }
    }
    
    dehancer::Error create_directories(const std::string& dir) {
      std::error_code ec;
      std::filesystem::create_directories(utf8::widen(dir).c_str(), ec);
      if (ec) {
        return Error(CommonError::EXCEPTION, ec.message());
      }
      return Error(CommonError::OK);
    }
    
    FILE* fopen(const std::string& path, const std::string& mode){
      return utf8::fopen(path, mode);
    }
    
    #else
    
    Error access(const std::string& path, int mode) {
      if (::access(path.c_str(), mode) !=0 ) {
        return Error(CommonError::PERMISSIONS_ERROR, error_string("File %s could not be found or has no certain permissions", path.c_str()));
      }
      else {
        return Error(CommonError::OK);
      }
    }
    
    dehancer::Error create_directories(const std::string& dir) {
      if (file::mkdir_p(dir.c_str(), 0755) != 0) {
        return Error(CommonError::PERMISSIONS_ERROR, error_string("Directory %s could not be found or user has no certain permissions", dir.c_str()));
      }
      return Error(CommonError::OK);
    }
    
    FILE* fopen(const std::string& path, const std::string& mode) {
      return ::fopen(path.c_str(), mode.c_str());
    }
    
    #endif
    
}