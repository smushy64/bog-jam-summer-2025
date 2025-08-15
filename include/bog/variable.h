#if !defined(BOG_VARIABLE_H)
#define BOG_VARIABLE_H
/**
 * @file   variable.h
 * @brief  Key/value pairs.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 14, 2025
*/
#include "bog/prelude.h"
#include "bog/collections.h"

struct KV {
    StringOffset key;
    int          value;
};

struct StorageKV {
    List<KV>   pairs;
    List<char> string;

    int read( String key ) {
        for( int i = 0; i < pairs.len; ++i ) {
            auto* pair = pairs + i;
            String this_key = pair->key.to_string( string );
            if( string_cmp( this_key, key ) ) {
                return pair->value;
            }
        }

        bool no_null = true;
        KV pair;
        pair.key   = string_offset_push( &string, key, no_null );
        pair.value = 0;

        pairs.push( pair );

        return 0;
    }
    int write( String key, int value ) {
        int found = -1;
        for( int i = 0; i < pairs.len; ++i ) {
            auto* pair = pairs + i;
            String this_key = pair->key.to_string( string );

            if( string_cmp( this_key, key ) ) {
                found = i;
                break;
            }
        }

        bool no_null = true;
        if( found >= 0 ) {
            return pairs[found].value = value;
        }

        KV pair = {};
        pair.key   = string_offset_push( &string, key, no_null );
        pair.value = value;

        pairs.push( pair );

        return value;
    }

    void reset() {
        pairs.reset();
        string.reset();
    }
    void free() {
        pairs.free();
        string.free();
    }
};

#endif /* header guard */
