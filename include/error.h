// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#ifndef ERROR_H_
#define ERROR_H_

#include <stdio.h>
#include "utils_macro.h"

typedef enum ERROR_PROCGEN ERROR_PROCGEN;

// error enum
#define MACRO_ERROR(X, ...)                    \
    X(PG_SUCCESS, __VA_ARGS__, SUCCESS)        \
    X(PG_INVALID_PARAMETER, __VA_ARGS__)       \
    X(PG_ACCESS_DENIED, __VA_ARGS__)           \
    X(PG_NOT_FOUND, __VA_ARGS__)               \
    X(PG_INVALID_FILE_ATTRIBUTE, __VA_ARGS__)  \
    X(PG_INVALID_FILE_SIZE, __VA_ARGS__)       \
    X(PG_INSUFFICIENT_MEMORY, __VA_ARGS__)     \
    X(PG_UNREADABLE_FILE, __VA_ARGS__)         \
    X(PG_INVALID_SIGNATURE, __VA_ARGS__)       \
    X(PG_INVALID_DOS_SIGNATURE, __VA_ARGS__)   \
    X(PG_INVALID_NT_SIGNATURE, __VA_ARGS__)    \
    X(PG_INVALID_ARCH, __VA_ARGS__)            \
    X(PG_INVALID_HANDLE_VALUE, __VA_ARGS__)    \
    X(PG_FILE_PART_NOT_FOUND, __VA_ARGS__)     \
    X(PG_NULL_BUFFER, __VA_ARGS__)             \
    X(PG_ALLOCATION_ERROR, __VA_ARGS__)        \
    X(PG_OBJECT_ALREADY_EXISTS, __VA_ARGS__)   \
    X(PG_OBJECT_NOT_FOUND, __VA_ARGS__)        \
    X(PG_OBJECT_NOT_IN_LIST, __VA_ARGS__)      \
    X(PG_MODULE_INTERNAL_ERROR, __VA_ARGS__)   \
    X(PG_MODULE_DEPENDENCY_NOT_RESOLVED, __VA_ARGS__)   \
    X(PG_EXTERNAL_ERROR, __VA_ARGS__)          \
    X(PG_POINTER_NON_NULL, __VA_ARGS__)        \
    X(PG_OPERATION_ERROR, __VA_ARGS__)         \
    X(PG_FAIL, __VA_ARGS__)                    \
    X(PG_UNKNWON_ERROR, __VA_ARGS__)           \

#define LOG_ON_ERR(F, ...) error_print(STR(F), __FILE__, __LINE__, F(__VA_ARGS__))
#define CHECK_CALL(F, ...)                            \
    last_status = LOG_ON_ERR(F, __VA_ARGS__);   \
    if (last_status) return last_status;

#define CHECK_CALL_CUSTOM_ERROR(F, E, ...)            \
    last_status = LOG_ON_ERR(F, __VA_ARGS__);   \
    if (last_status) return E;

#define CHECK_CALL_GOTO_ERROR(F, E, ...)            \
    last_status = LOG_ON_ERR(F, __VA_ARGS__);   \
    if (last_status) goto E;

enum ERROR_PROCGEN 
{
    MACRO_ERROR(TO_ENUM)
};

char* error_str(int status);
int error_print(char* function_name, char* nfile, int nline, int status);

#endif  /* !ERROR_H_ */