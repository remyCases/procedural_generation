// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#include "../include/error.h"

char* error_str(int status) 
{
    switch(status) 
    {
        MACRO_ERROR(TO_CASE_STR)
        default: return "UNKNOWN ERROR";
    }
}

int error_print(char* function_name, char* nfile, int nline, int status)
{
    switch(status) 
    {
        MACRO_ERROR(TO_CASE_PRINT, function_name, nfile, nline)
        default: printf("[!] File %s line %d, in function %s: UNKNOWN ERROR (code error %d)\n", nfile, nline, function_name, status);
    }

    return status;
}