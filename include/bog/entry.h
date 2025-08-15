#if !defined(BOG_ENTRY_H)
#define BOG_ENTRY_H
/**
 * @file   entry.h
 * @brief  Entry points.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   July 29, 2025
*/
#include "bog/prelude.h"

usize query_memory_requirement(void);

bool on_init( void* memory );
bool on_update( void* memory );
void on_close( void* memory );

#endif /* header guard */
