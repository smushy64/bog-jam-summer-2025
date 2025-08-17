#if !defined(BOG_SCENE_H)
#define BOG_SCENE_H
/**
 * @file   scene.h
 * @brief  Scenes.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 13, 2025
*/
#include "bog/prelude.h" // IWYU pragma: keep
#include "bog/collections.h"

struct Node;
struct Scene;

void scene_load( const char* path, Scene* out_scene );
void scene_print( Scene* scene );

bool scene_jump_calculate( Scene* scene, int* out_scene, int* out_node );

// -1 means move on to the next scene
int scene_jump_calculate_next( Scene* scene );

struct Scene {
    int id;
    StringOffset title;

    List<Node> nodes;
    List<char> string;
    List<char> storage;

    int current_node;

    Node* get_current();

    void reset() {
        id    = -1;
        title = {};
        nodes.reset();
        string.reset();
        storage.reset();
    }
    void free() {
        nodes.free();
        string.free();
        storage.free();
    }
};

enum class NodeType {
    NONE,
    STORY,
    CONTROL,
    FORK,
    WRITE,
    FADE,

    COUNT
};
String string_from_node_type( NodeType type );
bool node_type_from_string( String string, NodeType* out );

enum class AnimationSide {
    KEEP,
    LEFT,
    CENTER,
    RIGHT,

    COUNT
};
String string_from_animation_side( AnimationSide side );
bool animation_side_from_string( String string, AnimationSide* out );

enum class ControlType {
    JUMP,
    CONDITIONAL,

    COUNT
};
String string_from_control_type( ControlType type );
bool control_type_from_string( String string, ControlType* out );

enum class ForkActionType {
    NONE,
    JUMP,
    WRITE,

    COUNT
};
String string_from_fork_action_type( ForkActionType type );
bool fork_action_type_from_string( String string, ForkActionType* out );

struct ForkOption {
    ForkActionType type;
    StringOffset   text;

    union {
        struct {
            int scene, node;
        } jump;
        struct {
            StringOffset key;
            int          value;
        } write;
    };
};

struct ConditionalJump {
    bool does_something;
    int  scene, node;
};

struct Node {
    NodeType type;
    int id;
    union {
        struct {
            StringOffset text;
            StringOffset character;
            bool         has_write;
            struct {
                StringOffset  name;
                float         speed;
                AnimationSide side;
                bool          clear;
            } animation;
            struct {
                StringOffset key;
                int          value;
            } write;
        } story;
        struct {
            ControlType  type;
            union {
                struct {
                    StringOffset key;
                    ConditionalJump if_false, if_true;
                } conditional;
                struct {
                    int scene, node;
                } jump;
            };
        } control;
        struct {
            StringOffset key;
            int          value;
        } write;
        struct {
            /* offset into string field of Scene */
            int byte_offset;
            /* number of ForkOption */
            int len;
        } fork;
        struct {
            bool reverse;
        } fade;
    };
};

// NOTE(alicia): implementation ---------------------------------------------------------

inline
Node* Scene::get_current() {
    for( int i = 0; i < nodes.len; ++i ) {
        if( nodes[i].id == current_node ) {
            return nodes + i;
        }
    }
    return nullptr;
}

inline
bool scene_jump_calculate( Scene* scene, int* out_scene, int* out_node ) {
    (void)scene;
    (void)out_scene;
    (void)out_node;
    return false;
}
inline
int scene_jump_calculate_next( Scene* scene ) {
    for( int i = 0; i < scene->nodes.len; ++i ) {
        if( scene->nodes[i].id == scene->current_node ) {
            if( (i + 1) < scene->nodes.len ) {
                return scene->nodes[i + 1].id;
            } else {
                return -1;
            }
        }
    }
    return -1;
}

inline
String string_from_node_type( NodeType type ) {
    switch( type ) {
        case NodeType::NONE    : return "none";
        case NodeType::STORY   : return "story";
        case NodeType::CONTROL : return "control";
        case NodeType::FORK    : return "fork";
        case NodeType::WRITE   : return "write";
        case NodeType::FADE    : return "fade";

        case NodeType::COUNT : break;
    }
    return "";
}
inline
String string_from_animation_side( AnimationSide side ) {
    switch( side ) {
        case AnimationSide::KEEP   : return "keep";
        case AnimationSide::LEFT   : return "left";
        case AnimationSide::CENTER : return "center";
        case AnimationSide::RIGHT  : return "right";

        case AnimationSide::COUNT: break;
    }
    return "";
}
inline
String string_from_control_type( ControlType type ) {
    switch( type ) {
        case ControlType::JUMP:        return "jump";
        case ControlType::CONDITIONAL: return "conditional";

        case ControlType::COUNT: break;
    }
    return "";
}
inline
String string_from_fork_action_type( ForkActionType type ) {
    switch( type ) {
        case ForkActionType::NONE  : return "none";
        case ForkActionType::JUMP  : return "jump";
        case ForkActionType::WRITE : return "write";

        case ForkActionType::COUNT : break;
    }
    return "";
}
inline
bool node_type_from_string( String string, NodeType* out ) {
    for( int i = 0; i < (int)NodeType::COUNT; ++i ) {
        if( string_cmp( string, string_from_node_type( (NodeType)i ) ) ) {
            *out = (NodeType)i;
            return true;
        }
    }
    return false;
}
inline
bool animation_side_from_string( String string, AnimationSide* out ) {
    for( int i = 0; i < (int)AnimationSide::COUNT; ++i ) {
        if( string_cmp( string, string_from_animation_side( (AnimationSide)i ) ) ) {
            *out = (AnimationSide)i;
            return true;
        }
    }
    return false;
}
inline
bool control_type_from_string( String string, ControlType* out ) {
    for( int i = 0; i < (int)ControlType::COUNT; ++i ) {
        if( string_cmp( string, string_from_control_type( (ControlType)i ) ) ) {
            *out = (ControlType)i;
            return true;
        }
    }
    return false;
}
inline
bool fork_action_type_from_string( String string, ForkActionType* out ) {
    for( int i = 0; i < (int)ForkActionType::COUNT; ++i ) {
        if( string_cmp( string, string_from_fork_action_type( (ForkActionType)i ) ) ) {
            *out = (ForkActionType)i;
            return true;
        }
    }
    return false;
}

#endif /* header guard */
