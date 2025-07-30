#if !defined(ENTRY_H)
#define ENTRY_H
/**
 * @file   entry.h
 * @brief  Entry points.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   July 29, 2025
*/
#include "common.h"

usize query_memory_requirement(void);

bool on_init( void* memory );
void on_update( void* memory );
void on_close( void* memory );

#endif /* header guard */
