#pragma once

#include "libc/stdint.h"
#include "libc/stdbool.h" // for bool


int putchar(int ic);
int printf(const char* __restrict__ format, ...);
bool print(const char* data, size_t length);