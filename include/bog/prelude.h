#if !defined(BOG_PRELUDE_H)
#define BOG_PRELUDE_H
/**
 * @file   prelude.h
 * @brief  Common includes.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   July 29, 2025
*/
#include "raylib.h" // IWYU pragma: export
#include "raymath.h" // IWYU pragma: export
#include <stdint.h> // IWYU pragma: export

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef uintptr_t usize;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef intptr_t isize;

#if !defined(NULL)
    #define NULL ((void*)0)
#endif

#define Panic( ... ) do { \
    TraceLog( LOG_FATAL, "PANIC! " __VA_ARGS__ ); \
    __builtin_trap(); \
} while(0)

#define Assert( condition, ... ) do { \
    if( !(condition) ) { \
        TraceLog( LOG_FATAL, "Condition (" #condition ") failed! " __VA_ARGS__ ); \
        __builtin_trap(); \
    } \
} while(0)

#endif /* header guard */
