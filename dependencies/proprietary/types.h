#pragma once

#define COMPILER_MSVC 1
#if COMPILER_MSVC
#define force_inline __forceinline
#endif

typedef signed char  int8;
typedef signed short int16;
typedef signed int   int32;
typedef signed long long int64;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long long uint64;

typedef uint8  u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef int8  s8;
typedef int16 s16;
typedef int32 s32;
typedef int64 s64;

typedef s32 sint;
typedef u32 uint;
typedef u32 bool32;

typedef float  f32;
typedef double f64;

typedef uint8 byte;

#include <intrin.h> //not sure about this one

typedef __m128 s128;
typedef __m128 u128;
typedef __m128 f128;
typedef __m256 s256;
typedef __m256 u256;
typedef __m256 f256;

//load/store operations
force_inline f128 f128_load(float f1, float f2, float f3, float f4) { return _mm_set_ps(f1, f2, f3, f4); }
force_inline void f128_store(float* arr, f128 val);

// float operations
force_inline f128 f128_add(f128 a, f128 b) { return _mm_add_ps(a, b); }
force_inline f128 f128_sub(f128 a, f128 b) { return _mm_sub_ps(a, b); }
force_inline f128 f128_mul(f128 a, f128 b) { return _mm_mul_ps(a, b); }
force_inline f128 f128_div(f128 a, f128 b) { return _mm_div_ps(a, b); }

// signed int operations
force_inline s128 s128_add(s128 a, s128 b) { return _mm_add_ps(a, b); }
force_inline s128 s128_sub(s128 a, s128 b) { return _mm_sub_ps(a, b); }
force_inline s128 s128_mul(s128 a, s128 b) { return _mm_mul_ps(a, b); }
force_inline s128 s128_div(s128 a, s128 b) { return _mm_div_ps(a, b); }