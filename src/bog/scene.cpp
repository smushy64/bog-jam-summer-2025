/**
 * @file   scene.cpp
 * @brief  scene
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 13, 2025
*/
#include "bog/scene.h"
#include "json.h"

json_value_s* search_field(
    json_object_s* obj, const char* key, json_type_e type = json_type_null
) {
    auto* at = obj->start;
    for( size_t i = 0; i < obj->length; ++i ) {
        if( strcmp( at->name->string, key ) == 0 ) {
            if( type == json_type_null ) {
                return at->value;
            } else {
                if( at->value->type == type ) {
                    return at->value;
                }
            }
        }
        at = at->next;
    }

    return nullptr;
}
String string_from_json( json_string_s* string ) {
    return { (int)string->string_size, string->string };
}

void scene_load( const char* path, Scene* sc ) {
    Slice<char> src; {
        src.buf = LoadFileText( path );
        src.len = strlen( src.buf );
    }

    auto* json = json_parse_ex( src.buf, src.len, json_parse_flags_allow_c_style_comments, 0, 0, 0 );
    Assert( json, "%s: failed to parse json!", path );

    auto* root = json_value_as_object( json );

    auto* id       = search_field( root, "id", json_type_number );
    auto* title    = search_field( root, "title", json_type_string );
    auto* tree_ptr = search_field( root, "tree", json_type_array );

    Assert( id && tree_ptr, "%s: scene requires 'id' and 'tree' fields!", path );

    auto* tree = json_value_as_array( tree_ptr );

    sc->reset();

    sc->id = atoi( json_value_as_number( id )->number );

    if( title ) {
        sc->title = string_offset_push(
            &sc->string, string_from_json( json_value_as_string( title ) ) );
    }

    sc->nodes.reserve( tree->length );

    auto* at = tree->start;
    for( size_t i = 0; i < tree->length; ++i ) {
        Node value = {};

        auto* node = json_value_as_object( at->value );
        Assert( node, "%s: tree nodes must be objects!", path );

        auto* ptr_type = search_field( node, "type", json_type_string );
        auto* ptr_id   = search_field( node, "id", json_type_number );

        String type;

        if( !(ptr_type && ptr_id ) ) {
            goto skip_node;
        }

        type = string_from_json( json_value_as_string( ptr_type ) );
        if( !node_type_from_string( type, &value.type ) ) {
            goto skip_node;
        }

        value.id = atoi( json_value_as_number( ptr_id )->number );
        if( value.id < 0 ) {
            goto skip_node;
        }

        switch( value.type ) {
            case NodeType::STORY: {
                auto* ptr_text            = search_field( node, "story.text", json_type_string );
                auto* ptr_character       = search_field( node, "story.character", json_type_string );
                auto* ptr_animation_name  = search_field( node, "story.animation.name", json_type_string );
                auto* ptr_animation_speed = search_field( node, "story.animation.speed", json_type_number );
                auto* ptr_animation_side  = search_field( node, "story.animation.side", json_type_string );
                auto* ptr_write_key       = search_field( node, "story.write.key", json_type_string );
                auto* ptr_write_value     = search_field( node, "story.write.value", json_type_number );

                if( ptr_text ) {
                    value.story.text =
                        string_offset_push(
                            &sc->string,
                            string_from_json( json_value_as_string( ptr_text ) ) );
                }

                if( ptr_character ) {
                    value.story.character =
                        string_offset_push(
                            &sc->string,
                            string_from_json( json_value_as_string( ptr_character ) ) );
                }

                if( ptr_animation_name ) {
                    value.story.animation.name =
                        string_offset_push(
                            &sc->string,
                            string_from_json( json_value_as_string( ptr_animation_name ) ) );
                }

                if( ptr_animation_speed ) {
                    float speed = atof( json_value_as_number( ptr_animation_speed )->number );
                    if( speed < 0.0f ) {
                        speed = 0.0f;
                    }

                    value.story.animation.speed = speed;
                }

                if( ptr_animation_side ) {
                    String side = string_from_json( json_value_as_string( ptr_animation_side ) );
                    animation_side_from_string( side, &value.story.animation.side );
                }

                if( ptr_write_key ) {
                    value.story.has_write = true;
                    value.story.write.key = string_offset_push(
                        &sc->string, string_from_json( json_value_as_string( ptr_write_key ) ) );
                    value.story.write.value =
                        atoi( json_value_as_number( ptr_write_value )->number );
                }

            } break;
            case NodeType::CONTROL: {
                auto* ptr_type = search_field( node, "control.type", json_type_string );

                if( !ptr_type ) {
                    goto skip_node;
                }

                if( !control_type_from_string(
                    string_from_json( json_value_as_string( ptr_type ) ),
                    &value.control.type
                ) ) {
                    goto skip_node;
                }


                switch( value.control.type ) {
                    case ControlType::JUMP: {
                        auto* ptr_scene = search_field( node, "control.jump.scene", json_type_number );
                        auto* ptr_node  = search_field( node, "control.jump.node", json_type_number );

                        if( !ptr_node ) {
                            goto skip_node;
                        }

                        if( ptr_scene ) {
                            value.control.jump.scene =
                                atoi( json_value_as_number( ptr_scene )->number );
                        } else {
                            value.control.jump.scene = -1;
                        }

                        value.control.jump.node = atoi( json_value_as_number( ptr_node )->number );
                    } break;
                    case ControlType::CONDITIONAL: {
                        auto* ptr_key = search_field( node, "control.conditional.key", json_type_string );
                        if( !ptr_key ) {
                            goto skip_node;
                        }

                        value.control.conditional.key = string_offset_push(
                            &sc->string, string_from_json( json_value_as_string( ptr_key ) ) );

                        auto* ptr_if_false =
                            search_field( node, "control.conditional.false", json_type_object );
                        auto* ptr_if_true =
                            search_field( node, "control.conditional.true", json_type_object );

                        if( ptr_if_false ) {
                            auto* target   = &value.control.conditional.if_false;
                            auto* if_false = json_value_as_object( ptr_if_false );

                            if( if_false->length ) {
                                target->does_something = true;
                                auto* ptr_scene = search_field( if_false, "scene", json_type_number );
                                auto* ptr_node  = search_field( if_false, "node", json_type_number );

                                if( ptr_scene ) {
                                    target->scene = atoi( json_value_as_number( ptr_scene )->number );
                                } else {
                                    target->scene = -1;
                                }

                                if( ptr_node ) {
                                    target->node = atoi( json_value_as_number( ptr_node )->number );
                                }
                            }
                        }

                        if( ptr_if_true ) {
                            auto* target   = &value.control.conditional.if_true;
                            auto* if_true = json_value_as_object( ptr_if_true );

                            if( if_true->length ) {
                                target->does_something = true;
                                auto* ptr_scene = search_field( if_true, "scene", json_type_number );
                                auto* ptr_node  = search_field( if_true, "node", json_type_number );

                                if( ptr_scene ) {
                                    target->scene = atoi( json_value_as_number( ptr_scene )->number );
                                } else {
                                    target->scene = -1;
                                }

                                if( ptr_node ) {
                                    target->node = atoi( json_value_as_number( ptr_node )->number );
                                }
                            }
                        }
                    } break;
                    case ControlType::COUNT:
                        break;
                }
            } break;
            case NodeType::WRITE: {
                auto* ptr_key   = search_field( node, "write.key", json_type_string );
                auto* ptr_value = search_field( node, "write.value", json_type_number );

                if( !ptr_key ) {
                    goto skip_node;
                }

                value.write.key = string_offset_push(
                    &sc->string, string_from_json( json_value_as_string( ptr_key ) ) );
                if( ptr_value ) {
                    value.write.value =
                        atoi( json_value_as_number( ptr_value )->number );
                }
            } break;
            case NodeType::FORK: {
                auto* ptr_options = search_field( node, "fork.options", json_type_array );
                if( !ptr_options ) {
                    goto skip_node;
                }
                auto* options = json_value_as_array( ptr_options );

                value.fork.byte_offset = sc->storage.len;

                auto* at_option = options->start;
                for( size_t j = 0; j < options->length; ++j ) {
                    auto* current = json_value_as_object( at_option->value );
                    ForkOption fo = {};

                    json_value_s* ptr_text, *ptr_action;
                    ptr_text = ptr_action = nullptr;

                    if( !current ) {
                        goto skip_option;
                    }

                    ptr_text   = search_field( current, "text", json_type_string );
                    ptr_action = search_field( current, "action", json_type_string );

                    if( ptr_text ) {
                        fo.text = string_offset_push(
                            &sc->string, string_from_json( json_value_as_string( ptr_text ) ) );
                    }

                    if( ptr_action ) {
                        String str_action = string_from_json( json_value_as_string( ptr_action ) );
                        if( fork_action_type_from_string( str_action, &fo.type ) ) {
                            switch( fo.type ) {
                                case ForkActionType::JUMP: {
                                    auto* ptr_scene =
                                        search_field( current, "jump.scene", json_type_number );
                                    auto* ptr_node =
                                        search_field( current, "jump.node", json_type_number );

                                    if( !ptr_node ) {
                                        fo.type = ForkActionType::NONE;
                                    }

                                    if( ptr_scene ) {
                                        fo.jump.scene =
                                            atoi( json_value_as_number( ptr_scene )->number );
                                    } else {
                                        fo.jump.scene = -1;
                                    }

                                    fo.jump.node = atoi( json_value_as_number( ptr_node )->number );
                                } break;
                                case ForkActionType::WRITE: {
                                    auto* ptr_key =
                                        search_field( current, "write.key", json_type_string );
                                    auto* ptr_value =
                                        search_field( current, "write.value", json_type_number );

                                    if( !(ptr_key && ptr_value) ) {
                                        fo.type = ForkActionType::NONE;
                                    }

                                    fo.write.key = string_offset_push(
                                        &sc->string, string_from_json(
                                            json_value_as_string( ptr_key ) ) );
                                    fo.write.value =
                                        atoi( json_value_as_number( ptr_value )->number );
                                } break;

                                case ForkActionType::NONE:
                                case ForkActionType::COUNT:
                                    break;
                            }
                        }
                    }

                    sc->storage.append( sizeof(fo), (char*)&fo );
                    value.fork.len++;

                skip_option:
                    at_option = at_option->next;
                }

            } break;

            case NodeType::NONE:
            case NodeType::COUNT: goto skip_node;
        }

        sc->nodes.push( value );

skip_node:
        at = at->next;
    }

    free( json );
    UnloadFileText( src.buf );
}

void scene_print( Scene* scene ) {
    TraceLog( LOG_INFO, "title: '%s'", scene->title.to_string( scene->string ).buf );
    TraceLog( LOG_INFO, "id:    %i", scene->id );
    for( int i = 0; i < scene->nodes.len; ++i ) {
        auto* node = scene->nodes + i;
        TraceLog( LOG_INFO, "{" );
        TraceLog( LOG_INFO, "  id:              %i", node->id );
        TraceLog( LOG_INFO, "  type:            %s", string_from_node_type( node->type ).buf );
        switch( node->type ) {
            case NodeType::STORY: {
                TraceLog( LOG_INFO, "  text:            '%s'", node->story.text.to_string( scene->string ).buf );
                TraceLog( LOG_INFO, "  character:       '%s'", node->story.character.to_string( scene->string ).buf );
                TraceLog( LOG_INFO, "  animation.name:  '%s'", node->story.animation.name.to_string( scene->string ).buf );
                TraceLog( LOG_INFO, "  animation.speed: %f", node->story.animation.speed );
                TraceLog( LOG_INFO, "  animation.side:  %s", string_from_animation_side( node->story.animation.side ).buf );
                TraceLog( LOG_INFO, "  animation.clear: %s", node->story.animation.clear ? "true" : "false" );
                if( node->story.has_write ) {
                    TraceLog( LOG_INFO, "  write.key:       '%s'", node->story.write.key.to_string( scene->string ).buf );
                    TraceLog( LOG_INFO, "  write.value:     %i'", node->story.write.value );
                }
            } break;
            case NodeType::CONTROL: {
                TraceLog( LOG_INFO, "  type:            %s", string_from_control_type( node->control.type ).buf );
                switch( node->control.type ) {
                    case ControlType::JUMP: {
                        TraceLog( LOG_INFO, "  scene:           %i", node->control.jump.scene );
                        TraceLog( LOG_INFO, "  node:            %i", node->control.jump.node );
                    } break;
                    case ControlType::CONDITIONAL: {
                        TraceLog( LOG_INFO, "  key:             '%s'", node->control.conditional.key.to_string( scene->string ).buf );
                        TraceLog( LOG_INFO, "  false:           %s", node->control.conditional.if_false.does_something ? TextFormat( "scene %i - node %i", node->control.conditional.if_false.scene, node->control.conditional.if_false.node ) : "none" );
                        TraceLog( LOG_INFO, "  true:            %s", node->control.conditional.if_true.does_something ? TextFormat( "scene %i - node %i", node->control.conditional.if_true.scene, node->control.conditional.if_true.node ) : "none" );
                    } break;
                    case ControlType::COUNT:
                        break;
                }
            } break;
            case NodeType::WRITE: {
                TraceLog( LOG_INFO, "  key:             '%s'", node->write.key.to_string( scene->string ).buf );
                TraceLog( LOG_INFO, "  value:           %i", node->write.value );
            } break;
            case NodeType::FORK: {
                TraceLog( LOG_INFO, "  options: [%i] {", node->fork.len );

                Slice<ForkOption> options = {
                    node->fork.len,
                    (ForkOption*)( scene->storage + node->fork.byte_offset ) };

                for( int i = 0; i < options.len; ++i ) {
                    auto* opt = options + i;
                    TraceLog( LOG_INFO, "    {" );
                    TraceLog( LOG_INFO, "      text:   '%s'", opt->text.to_string( scene->string ).buf );
                    TraceLog( LOG_INFO, "      action: %s", string_from_fork_action_type( opt->type ).buf );
                    switch( opt->type ) {
                        case ForkActionType::JUMP: {
                            TraceLog( LOG_INFO, "      scene:  %i", opt->jump.scene );
                            TraceLog( LOG_INFO, "      node:   %i", opt->jump.node );
                        } break;
                        case ForkActionType::WRITE: {
                            String key = opt->write.key.to_string( scene->string );
                            TraceLog( LOG_INFO, "      key:   '%s'", key.buf );
                            TraceLog( LOG_INFO, "      value: %i", opt->write.value );
                        } break;

                        case ForkActionType::NONE:
                        case ForkActionType::COUNT:
                            break;
                    }
                    TraceLog( LOG_INFO, "    }" );
                }

                TraceLog( LOG_INFO, "  }", node->fork.len );
            } break;

            case NodeType::NONE:
            case NodeType::COUNT:
                break;
        }
        TraceLog( LOG_INFO, "}" );
    }
}

