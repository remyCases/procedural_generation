// Copyright (C) 2025 Rémy Cases
// See LICENSE file for extended copyright information.
// This file is part of procedural_generation project from https://github.com/remyCases/procedural_generation.

#include <stdint.h>
#include <stdlib.h>
#include "utils_macro.h"

#define INITIAL_SIZE 2

#define VEC(T) S_CAT_UND(vec, T, t)
#define TO_VEC(T) typedef struct S_CAT_UND(vec, T, s) { \
        T* buf;    \
        size_t len;\
        size_t cap;\
    } VEC(T);
#define INIT(T) S_CAT_UND(init, vec, T)
#define INIT_DEF(T) static void INIT(T)(VEC(T)* S_CAT_UND(p, vec, T))
#define INIT_DEC(T) INIT_DEF(T) {                                                   \
	S_CAT_UND(p, vec, T)->buf = (T*)calloc(INITIAL_SIZE, sizeof(T));                \
	S_CAT_UND(p, vec, T)->len = 0;                                                  \
	S_CAT_UND(p, vec, T)->cap = INITIAL_SIZE;                                       \
}
#define PUSH(T) S_CAT_UND(push, vec, T)
#define PUSH_DEF(T) static void PUSH(T)(VEC(T)* S_CAT_UND(p, vec, T), T* p) 
#define PUSH_DEC(T) PUSH_DEF(T) {                                                   \
	S_CAT_UND(p, vec, T)->buf[S_CAT_UND(p, vec, T)->len] = *p;                      \
	S_CAT_UND(p, vec, T)->len++;                                                    \
	if (S_CAT_UND(p, vec, T)->len == S_CAT_UND(p, vec, T)->cap) {                   \
		S_CAT_UND(p, vec, T)->cap *= 2;                                             \
		S_CAT_UND(p, vec, T)->buf = (T*)realloc(S_CAT_UND(p, vec, T)->buf, S_CAT_UND(p, vec, T)->cap * sizeof(T)); \
	}                                                                               \
}
#define FREE(T) S_CAT_UND(free, vec, T)
#define FREE_DEF(T) static void FREE(T)(VEC(T)* S_CAT_UND(p, vec, T)) 
#define FREE_DEC(T) FREE_DEF(T) {                                                   \
	free(S_CAT_UND(p, vec, T)->buf);                                                \
}

#define VEC_DEF(T) TO_VEC(T)    \
    INIT_DEF(T);                \
    PUSH_DEF(T);                \
    FREE_DEF(T);
#define VEC_DEC(T)              \
    INIT_DEC(T)                 \
    PUSH_DEC(T)                 \
    FREE_DEC(T)
