/*
OFX Support Library, a library that skins the OFX plug-in API with C++ classes.
Copyright (C) 2004-2005 The Open Effects Association Ltd
Author Bruno Nicoletti bruno@thefoundry.co.uk

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
* Neither the name The Open Effects Association Ltd, nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The Open Effects Association Ltd
1 Wardour St
London W1D 6PA
England


*/

/** @file This file contains the body of functions used for logging ofx problems etc...

The log file is written to using printf style functions, rather than via c++ iostreams.

*/

#include <cstdio>
#include <cstdarg>
#include <string>
#include "dehancer/Log.h"
#include "dehancer/Utils.h"

#import <Foundation/Foundation.h>

namespace dehancer {
    namespace log {
        
        /** @brief log file */
        static FILE *gLogFP = stdout;
        
        /** @brief global indent level, not MP sane */
        static int gIndent = 0;
        
        /** @brief Sets the name of the log file. */
        void setFileName(const std::string &value)
        {
          //gLogFileName = value;
        }
        
        /** @brief Opens the log file, returns whether this was sucessful or not. */
        bool open()
        {
#ifdef PRINT_DEBUG
          return true;
#else
          return false;
#endif
        }
        
        /** @brief Closes the log file. */
        void close()
        {
        }
        
        /** @brief Indent it, not MP sane at the moment */
        void indent()
        {
          ++gIndent;
        }
        
        /** @brief Outdent it, not MP sane at the moment */
        void outdent()
        {
          --gIndent;
        }
        
        void LogIt (NSString *format, ...)
        {
          va_list args;
          
          va_start (args, format);
          
          NSString *string;
          
          string = [[NSString alloc] initWithFormat: format  arguments: args];
          
          va_end (args);
          
          fprintf (gLogFP, "%s\n", [string UTF8String]);
          
        } // LogIt
        
        
        /** @brief do the indenting */
        static inline void doIndent()
        {
          if(open()) {
            for(int i = 0; i < gIndent; i++) {
              LogIt(@"    ");
            }
          }
        }
        
        /** @brief Prints to the log file. */
        void print(const char *format, ...)
        {
#if PRINT_DEBUG
          if(open()) {
            doIndent();
            fputs("[", gLogFP);
            fputs(time::utc_to_time_string(time::now_utc()).c_str(), gLogFP);
            fputs("] ", gLogFP);
            fputs("INFO: ", gLogFP);
            
            va_list args;
            va_start(args, format);
            vfprintf(gLogFP, format, args);
            fputc('\n', gLogFP);
            fflush(gLogFP);
            va_end(args);
          }
#endif
        }
        
        /** @brief Prints to the log file. */
        void printl(const char *format, ...)
        {
#if PRINT_DEBUG
          if(open()) {
            doIndent();
            fputs("[", gLogFP);
            fputs(time::utc_to_time_string(time::now_utc()).c_str(), gLogFP);
            fputs("] ", gLogFP);
            fputs("INFO: ", gLogFP);
            
            va_list args;
            va_start(args, format);
            vfprintf(gLogFP, format, args);
            fflush(gLogFP);
            va_end(args);
          }
#endif
        }
        
        /** @brief Prints to the log file only if the condition is true and prepends a warning notice. */
        void warning(bool condition, const char *format, ...)
        {
#if PRINT_DEBUG
          if(condition && open()) {
            doIndent();
            fputs("[", gLogFP);
            fputs(time::utc_to_time_string(time::now_utc()).c_str(), gLogFP);
            fputs("] ", gLogFP);
            fputs("WARNING: ", gLogFP);
            
            va_list args;
            va_start(args, format);
            vfprintf(gLogFP, format, args);
            fputc('\n', gLogFP);
            va_end(args);
            
            fflush(gLogFP);
          }
#endif
        }
        
        /** @brief Prints to the log file only if the condition is true and prepends an error notice. */
        void error(bool condition, const char *format, ...)
        {
#      ifdef PRINT_DEBUG
          if(condition && open()) {
            doIndent();
            fputs("[", gLogFP);
            fputs(time::utc_to_time_string(time::now_utc()).c_str(), gLogFP);
            fputs("] ", gLogFP);
            fputs("ERROR : ", gLogFP);
            
            va_list args;
            va_start(args, format);
            vfprintf(gLogFP, format, args);
            fputc('\n', gLogFP);
            va_end(args);
            
            fflush(gLogFP);
          }
#endif
        }
    };
};
