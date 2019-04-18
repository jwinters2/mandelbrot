/*
 * for miscellaneous math functions that don't fit in any other file,
 * but I wanted to de-clutter main.cpp, so they're in here now
 */

#ifndef __MATH_UTILS_H_
#define __MATH_UTILS_H_

#include <iostream>

#include "cdouble.h"

cdouble add(const cdouble&, const cdouble&);
cdouble mul(const cdouble&, int);
cdouble square(const cdouble&);
double square_magnitude(const cdouble&);
std::string pad_number(int, int);

#endif
