/**
 * @file   collections.cpp
 * @brief  Collections.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 09, 2025
*/
#include "bog/collections.h"
#include <string.h>
#include <ctype.h>

StringOffset string_offset_push(
    List<char>* list, String string,
    bool no_null, StringConvert convert
) {
    list->reserve( string.len + (no_null ? 0 : 1) ); // null byte

    StringOffset result = {};
    result.len = string.len;

    result.offset = list->len;

    switch( convert ) {
        case StringConvert::NONE: {
            memcpy( list->buf + list->len, string.buf, string.len );
        } break;
        case StringConvert::UPPER: {
            for( int i = 0; i < string.len; ++i ) {
                *(list->buf + list->len + i) = toupper( string[i] );
            }
        } break;
        case StringConvert::LOWER: {
            for( int i = 0; i < string.len; ++i ) {
                *(list->buf + list->len + i) = tolower( string[i] );
            }
        } break;
    }

    list->len += string.len;
    if( !no_null ) {
        list->buf[list->len++] = 0; // null byte
    }

    return result;
}

bool string_cmp( String a, String b ) {
    if( a.len != b.len ) {
        return false;
    }
    return memcmp( a.buf, b.buf, a.len ) == 0;
}

bool find_char( String string, char c, int* opt_out_index ) {
    const char* result = (const char*)memchr( string.buf, c, string.len );
    if( !result ) {
        return false;
    }

    if( opt_out_index ) {
        *opt_out_index = result - string.buf;
    }
    return true;
}
bool find_set( String string, String set, int* opt_out_index ) {
    for( int i = 0; i < string.len; ++i ) {
        for( int j = 0; j < set.len; ++j ) {
            if( string[i] == set[j] ) {
                if( opt_out_index ) {
                    *opt_out_index = i;
                }
                return true;
            }
        }
    }
    return false;
}
bool find_string( String string, String substring, int* opt_out_index ) {
    for( int i = 0; i < string.len; ++i ) {
        int remaining = string.len - i;

        if( remaining < substring.len ) {
            break;
        }

        if( memcmp( string + i, substring, substring.len ) == 0 ) {
            if( opt_out_index ) {
                *opt_out_index = i;
            }
            return true;
        }
    }

    return false;
}

Color parse_color( String string ) {
    int i         = 0;
    u8  result[4] = {};

    while( string.len && i < 4 ) {
        String chunk = string;
        find_char( chunk, ',', &chunk.len );

        string = advance( string, chunk.len + 1 );

        if( chunk.len > 2 || chunk.len <= 0 ) {
            continue;
        } else if( chunk.len == 2 ) {
            u8 value = 0;

            if( *chunk >= '0' && *chunk <= '9' ) {
                value = (*chunk - '0') * 16;
            } else if( *chunk >= 'A' && *chunk <= 'F' ) {
                value = ((*chunk - 'A') + 10) * 16;
            } else if( *chunk >= 'a' && *chunk <= 'f' ) {
                value = ((*chunk - 'a') + 10) * 16;
            }

            chunk = advance( chunk );

            if( *chunk >= '0' && *chunk <= '9' ) {
                value += (*chunk - '0');
            } else if( *chunk >= 'A' && *chunk <= 'F' ) {
                value += ((*chunk - 'A') + 10);
            } else if( *chunk >= 'a' && *chunk <= 'f' ) {
                value += ((*chunk - 'a') + 10);
            }

            result[i++] = value;
        } else if( chunk.len == 1 ) {
            u8 value = 0;

            if( *chunk >= '0' && *chunk <= '9' ) {
                value |= (*chunk - '0');
            } else if( *chunk >= 'A' && *chunk <= 'F' ) {
                value |= (*chunk - 'A');
            } else if( *chunk >= 'a' && *chunk <= 'f' ) {
                value |= (*chunk - 'a');
            }

            result[i++] = value;
        }
    }

    Color out = *(Color*)result;
    return out;
}

