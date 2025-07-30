/**
 * @file   cbuild.c
 * @brief  Bog Jam Summer 2025 build system.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   July 27, 2025
*/
#define CB_STRIP_PREFIXES
#include "cbuild.h"

typedef StringSlice String;
#define S(...) CB_STRING(__VA_ARGS__)

// NOTE(alicia): these can be redefined to point to
// the actual name of these executables
// On void linux, these names are correct but they
// may not be on other machines
#define MINGW32_GCC "x86_64-w64-mingw32-gcc"
#define MINGW32_GPP "x86_64-w64-mingw32-g++"
#define MINGW32_AR  "x86_64-w64-mingw32-ar"

// NOTE(alicia): these can be redefined to point
// to the actual name of these executables
#define EMCC "emcc"
#define EMPP "em++"
#define EMAR "emar"

#define EXECUTABLE_NAME "bog-jam-summer-2025"

typedef enum {
    E_NONE,
    /* %s mode string */
    E_UNRECOGNIZED_MODE,
    /* Mode mode */
    /* %s   argument string */
    E_UNRECOGNIZED_ARGUMENT,
    /* Mode mode */
    E_MISSING_ARGUMENT,
    /* Mode mode */
    /* %s   target string */
    E_UNRECOGNIZED_TARGET,

    /* %s program name */
    E_MISSING_PROGRAM,

    E_MAKE_DIRECTORIES,
    E_MOVE_FILE,

    /* %s process name */
    /* %d return code */
    E_SUBPROCESS,

    E_COUNT
} Error;

typedef enum {
    M_HELP,
    M_BUILD,
    M_RUN,
    M_PACKAGE,

    M_COUNT
} Mode;

typedef enum {
    T_GNU_LINUX,
    T_WINDOWS,
    T_WASM,
    T_COUNT,

#if CB_PLATFORM_CURRENT == CB_PLATFORM_WINDOWS
    T_NATIVE = T_WINDOWS,
#else
    T_NATIVE = T_GNU_LINUX,
#endif
} Target;

typedef struct {
    Mode mode;
    union {
        struct OptHelp {
            Mode mode;
        } help;
        struct OptBuild {
            Target target;
            bool   is_release;
            bool   always_rebuild;
        } build;
        struct OptRun {
            struct OptBuild build;
            CommandLine     cl;
        } run;
        struct OptPackage {
            struct OptBuild build;
        } package;
    };
} Opt;

int mode_help( Opt* opt );
int mode_build( Opt* opt );
int mode_run( Opt* opt );
int mode_package( Opt* opt );

String string_from_mode( Mode mode );
bool mode_from_string( String string, Mode* out_mode );

String string_from_target( Target target );
bool target_from_string( String string, Target* out_target );

const char* target_compiler_c( Target target );
const char* target_compiler_cpp( Target target );
const char* target_archiver( Target target );
const char* target_ext( Target target );

const char* mode_description( Mode mode );

int check_prog( const char* prog );

int error( Error err, ... );

String sb_path( StringBuilder* sb, const char* fmt, ... );

CommandBuilder cb = {};
StringBuilder  sb = {};

String executable = {};


int main( int argc, char** argv ) {
    CB_INITIALIZE( CB_LOG_ALL );

    CommandLine cl = CB_CL( argc, argv );
    cl = CB_CL_NEXT( &cl );

    Opt opt = {};
    
    if( !cl.len ) {
        return mode_help( NULL );
    }
    String arg = CB_STRING_FROM_CSTR( cl.buf[0] );

    if( !mode_from_string( arg, &opt.mode ) ) {
        return error( E_UNRECOGNIZED_MODE, arg.buf );
    }

    switch( opt.mode ) {
        case M_BUILD:
        case M_RUN:
        case M_PACKAGE: {
            opt.build.target = T_NATIVE;
        } break;

        case M_HELP:
        case M_COUNT:
            break;
    }

    bool break_loop = false;
    for(;;) {
        if( break_loop ) {
            break;
        }

        cl = CB_CL_NEXT( &cl );
        if( !cl.len ) {
            break;
        }
        arg = CB_STRING_FROM_CSTR( cl.buf[0] );

        switch( opt.mode ) {
            case M_HELP: {
                if( !mode_from_string( arg, &opt.help.mode ) ) {
                    return error( E_UNRECOGNIZED_MODE, cl.buf[0] );
                }

                break_loop = true;
                continue;
            } break;
            case M_BUILD: {
                if( strcmp( cl.buf[0], "-t" ) == 0 ) {
                    cl  = CB_CL_NEXT( &cl );
                    if( !cl.len ) {
                        return error( E_MISSING_ARGUMENT, opt.mode );
                    }
                    arg = CB_STRING_FROM_CSTR( cl.buf[0] );

                    if( !target_from_string( arg, &opt.build.target ) ) {
                        return error( E_UNRECOGNIZED_TARGET, opt.mode, arg.buf );
                    }

                    continue;
                } else if( strcmp( cl.buf[0], "-release" ) == 0 ) {
                    opt.build.is_release = true;
                    continue;
                } else if( strcmp( cl.buf[0], "-rebuild" ) == 0 ) {
                    opt.build.always_rebuild = true;
                    continue;
                }
            } break;
            case M_RUN: {
                if( strcmp( cl.buf[0], "-t" ) == 0 ) {
                    cl  = CB_CL_NEXT( &cl );
                    if( !cl.len ) {
                        return error( E_MISSING_ARGUMENT, opt.mode );
                    }
                    arg = CB_STRING_FROM_CSTR( cl.buf[0] );

                    if( !target_from_string( arg, &opt.build.target ) ) {
                        return error( E_UNRECOGNIZED_TARGET, opt.mode, arg.buf );
                    }

                    continue;
                } else if( strcmp( cl.buf[0], "-release" ) == 0 ) {
                    opt.build.is_release = true;
                    continue;
                } else if( strcmp( cl.buf[0], "-rebuild" ) == 0 ) {
                    opt.build.always_rebuild = true;
                    continue;
                } else if( strcmp( cl.buf[0], "--" ) == 0 ) {
                    opt.run.cl = CB_CL_NEXT( &cl );
                    break_loop = true;
                    continue;
                }
            } break;
            case M_PACKAGE: {
                if( strcmp( cl.buf[0], "-t" ) == 0 ) {
                    cl  = CB_CL_NEXT( &cl );
                    if( !cl.len ) {
                        return error( E_MISSING_ARGUMENT, opt.mode );
                    }
                    arg = CB_STRING_FROM_CSTR( cl.buf[0] );

                    if( !target_from_string( arg, &opt.build.target ) ) {
                        return error( E_UNRECOGNIZED_TARGET, opt.mode, arg.buf );
                    }

                    continue;
                }
            } break;

            case M_COUNT:
                break;
        }

        return error( E_UNRECOGNIZED_ARGUMENT, opt.mode, cl.buf[0] );
    }

    switch( opt.mode ) {
        case M_HELP    : return mode_help( &opt );
        case M_BUILD   : return mode_build( &opt );
        case M_RUN     : return mode_run( &opt );
        case M_PACKAGE : return mode_package( &opt );

        case M_COUNT:
            break;
    }

    return E_NONE;
}

int mode_build_raylib(
    String      path,
    String      obj_path,
    Target      target,
    const char* cc,
    bool        should_rebuild,
    bool        is_release
) {
    Error err = E_NONE;

    // NOTE(alicia): only if archiving is necessary
    const char* ar  = target_archiver( target );

    if( (err = check_prog( ar )) ) {
        return err;
    }

    CB_INFO( "building raylib for %s . . .", string_from_target( target ).buf );

    #define CONCAT( a, b ) a "" b

    #define COMP( part ) do { \
        String part = paths[pathi++] = sb_path( &sb, CONCAT( "%", CONCAT( CB_STRING_FMT, CONCAT( "/", CONCAT( #part, ".o" ) ) ) ), obj_path ); \
        if( should_rebuild || !file_exists( part.buf ) ) { \
            command_builder_reset( &cb ); \
            command_builder_append( &cb, cc, CONCAT( "raylib/src/", CONCAT( #part, ".c" ) ) ); \
            command_builder_append( &cb, "-c", "-o", part.buf, FLAGS ); \
            if( is_release ) { \
                command_builder_append( &cb, RELEASE_FLAGS ); \
            } else { \
                command_builder_append( &cb, DEBUG_FLAGS ); \
            } \
            if( (res = process_exec( cb.cmd ) ) ) { \
                return error( E_SUBPROCESS, "raylib compile " #part , res ); \
            } \
        } \
    } while(0)

    int reset_point = sb.len;
    command_builder_reset( &cb );
    switch( target ) {
        case T_GNU_LINUX : {
            int    pathi    = 0;
            String paths[8] = {};

            #define FLAGS \
                "-D_GNU_SOURCE", "-DPLATFORM_DESKTOP_GLFW", \
                "-DGRAPHICS_API_OPENGL_33", "-Wno-missing-braces", \
                "-Werror=pointer-arith", "-fno-strict-aliasing", \
                "-std=c99", "-fPIC", "-Werror=implicit-function-declaration", \
                "-D_GLFW_X11", "-Iraylib/src", "-Iraylib/src/external/glfw/include", \
                "-Wno-macro-redefined"

            #define RELEASE_FLAGS "-O1"
            #define DEBUG_FLAGS   "-O0", "-ggdb"

            int res = 0;

            COMP(rcore);
            COMP(rshapes);
            COMP(rtextures);
            COMP(rtext);
            COMP(utils);
            COMP(rglfw);
            COMP(rmodels);
            COMP(raudio);

            command_builder_reset( &cb );
            command_builder_append( &cb, ar, "rcs", path.buf );
            for( int i = 0; i < pathi; ++i ) {
                command_builder_append( &cb, paths[i].buf );
            }

            if( (res = process_exec( cb.cmd )) ) {
                return error( E_SUBPROCESS, "raylib link", res );
            }

            #undef FLAGS
            #undef DEBUG_FLAGS
            #undef RELEASE_FLAGS
        } break;
        case T_WINDOWS   : {
            int    pathi    = 0;
            String paths[8] = {};

            #define FLAGS \
                "-DPLATFORM_DESKTOP_GLFW", \
                "-DGRAPHICS_API_OPENGL_33", "-Wno-missing-braces", \
                "-Werror=pointer-arith", "-fno-strict-aliasing", \
                "-std=c99", "-O1", "-Werror=implicit-function-declaration", \
                "-Iraylib/src", "-Iraylib/src/external/glfw/include", \
                "-static-libgcc"

            #define DEBUG_FLAGS   "-O0", "-g"
            #define RELEASE_FLAGS "-O1"

            int res = 0;

            COMP(rcore);
            COMP(rshapes);
            COMP(rtextures);
            COMP(rtext);
            COMP(utils);
            COMP(rglfw);
            COMP(rmodels);
            COMP(raudio);

            command_builder_reset( &cb );
            command_builder_append( &cb, ar, "rcs", path.buf );
            for( int i = 0; i < pathi; ++i ) {
                command_builder_append( &cb, paths[i].buf );
            }

            if( (res = process_exec( cb.cmd )) ) {
                return error( E_SUBPROCESS, "raylib link", res );
            }

            #undef FLAGS
            #undef DEBUG_FLAGS
            #undef RELEASE_FLAGS
        } break;
        case T_WASM      : {
            int    pathi    = 0;
            String paths[8] = {};

            #define FLAGS \
                "-Wall", "-DPLATFORM_WEB", "-DGRAPHICS_API_OPENGL_ES2" \

            #define DEBUG_FLAGS   "-Os"
            #define RELEASE_FLAGS "-Os"

            int res = 0;

            COMP(rcore);
            COMP(rshapes);
            COMP(rtextures);
            COMP(rtext);
            COMP(utils);
            COMP(rmodels);
            COMP(raudio);

            command_builder_reset( &cb );
            command_builder_append( &cb, ar, "rcs", path.buf );
            for( int i = 0; i < pathi; ++i ) {
                command_builder_append( &cb, paths[i].buf );
            }

            if( (res = process_exec( cb.cmd )) ) {
                return error( E_SUBPROCESS, "raylib link", res );
            }

            #undef FLAGS
            #undef DEBUG_FLAGS
            #undef RELEASE_FLAGS
        } break;

        case T_COUNT:
            break;
    }
    #undef CONCAT
    #undef COMP

    sb.len = reset_point;
    return err;
}

int mode_build( Opt* opt ) {
    double start_time = time_msec();

    Error err = E_NONE;

    CB_RESERVE( &sb, CB_KIBIBYTES(4) );

    const char* cc  = target_compiler_c( opt->build.target );
    const char* cpp = target_compiler_cpp( opt->build.target );

    String target_name = string_from_target( opt->build.target );

    string_builder_fmt( &sb, "build/%s", target_name.buf );

    String target_dir = sb_path( &sb, "build/%s", string_from_target( opt->build.target ).buf );
    String obj_dir    = sb_path( &sb, "%" CB_STRING_FMT "/obj", CB_STRING_FMT_ARG(&target_dir) );
    String raylib     = sb_path(
        &sb, "%" CB_STRING_FMT "/libraylib.a", CB_STRING_FMT_ARG(&obj_dir) );

    executable = sb_path( &sb,
        "%" CB_STRING_FMT "/" EXECUTABLE_NAME "%s",
        CB_STRING_FMT_ARG(&target_dir), target_ext(opt->build.target) );

    if( (err = check_prog( cc )) ) {
        return err;
    }
    if( (err = check_prog( cpp )) ) {
        return err;
    }
    if( opt->build.target == T_WINDOWS ) {
        if( (err = check_prog( "lld" )) ) {
            return err;
        }
    }

    CB_INFO( "building project for %s . . .", target_name.buf );

    if( !make_directories( "build", target_dir.buf, obj_dir.buf ) ) {
        return E_MAKE_DIRECTORIES;
    }

    if( opt->build.always_rebuild || !file_exists( raylib.buf ) ) {
        if( (err = mode_build_raylib(
            raylib, obj_dir, opt->build.target,
            cc, opt->build.always_rebuild, opt->build.is_release ))
        ) {
            return err;
        }
    }

    command_builder_reset(&cb);
    command_builder_append( &cb, cpp, "src/sources.cpp", "-Iinclude" );
    command_builder_append( &cb, raylib.buf, "-Iraylib/src" );
    command_builder_append( &cb, "-o", executable.buf );
    command_builder_append( &cb, "-Wall", "-Wextra", "-Werror=vla" );

    switch( opt->build.target ) {
        case T_GNU_LINUX : {
            if( opt->build.is_release ) {
                command_builder_append( &cb, "-O2" );
            } else {
                command_builder_append( &cb, "-O0", "-ggdb" );
            }

            command_builder_append( &cb, "-lGL", "-lm", "-pthread", "-ldl", "-lrt", "-lX11" );
        } break;
        case T_WINDOWS   : {
            if( opt->build.is_release ) {
                command_builder_append( &cb, "-O2" );
            } else {
                command_builder_append( &cb, "-O0", "-g", "-fuse-ld=lld", "-Wl,/debug" );
            }

            command_builder_append(
                &cb, "-lkernel32", "-lgdi32", "-lwinmm", "-lopengl32", "-lshell32" );
        } break;
        case T_WASM      : {
            String link_path = sb_path( &sb, "-L%" CB_STRING_FMT, CB_STRING_FMT_ARG( &raylib ) );

            command_builder_append(
                &cb, "-Os", "-Wall", "-DPLATFORM_WEB",
                link_path.buf,
                "-s", "USE_GLFW=3", "--shell-file", "raylib/src/minshell.html" );

            command_builder_append( &cb, "--preload-file", "resources" );
        } break;
        case T_COUNT:
            break;
    }

    int res = 0;
    if( (res = process_exec( cb.cmd )) ) {
        return error( E_SUBPROCESS, "compile project", res );
    }

    if( opt->build.target == T_WASM ) {
        if( !file_move( "build/wasm/index.html", "build/wasm/" EXECUTABLE_NAME ".html", false ) ) {
            return error( E_MOVE_FILE );
        }
    }

    CB_INFO( "compilation finished in %fms", time_msec() - start_time );
    return err;
}
int mode_run( Opt* opt ) {
    Error err = E_NONE;

    if( (err = mode_build(opt)) ) {
        return err;
    }

    command_builder_reset( &cb );
    switch( opt->build.target ) {
        case T_GNU_LINUX: {
            command_builder_append( &cb, executable.buf );

            while( opt->run.cl.len ) {
                command_builder_append( &cb, opt->run.cl.buf[0] );
                opt->run.cl = CB_CL_NEXT( &opt->run.cl );
            }

            CB_INFO( "running project . . ." );
            int res = process_exec( cb.cmd );

            CB_INFO( "project exited with code %i", res );
        } break;
        case T_WINDOWS: {
            bool is_using_wine = false;
#if CB_PLATFORM_CURRENT == CB_PLATFORM_GNU_LINUX
            if( (err = check_prog( "wine" )) ) {
                return err;
            }

            is_using_wine = true;
            command_builder_append( &cb, "wine" );
#endif

            command_builder_append( &cb, executable.buf );
            while( opt->run.cl.len ) {
                command_builder_append( &cb, opt->run.cl.buf[0] );
                opt->run.cl = CB_CL_NEXT( &opt->run.cl );
            }

            CB_INFO( "running project%s. . .", is_using_wine ? " under wine " : " " );
            int res = process_exec( cb.cmd );

            CB_INFO( "project exited with code %i", res );
        } break;
        case T_WASM: {
            if( (err = check_prog("python")) ) {
                return err;
            }

            CB_INFO( "running web version using python. control+c in terminal to kill process." );
            ProcessID pid;
            if( process_exec_async(
                CB_COMMAND("python", "-m", "http.server", "8080"),
                &pid, "build/wasm"
            ) ) {
                thread_sleep(8);
                process_exec( CB_COMMAND("xdg-open", "localhost:8080/index.html") );
                process_wait( &pid );
            }
        } break;
        case T_COUNT:
            break;
    }

    return err;
}
int mode_package( Opt* opt ) {
    Error err = E_NONE;
    opt->build.always_rebuild = opt->build.is_release = true;

    if( (err = mode_build(opt)) ) {
        return err;
    }

    if( !make_directories( "bin" ) ) {
        return error( E_MAKE_DIRECTORIES );
    }

    switch( opt->build.target ) {
        case T_GNU_LINUX: {
            if( (err = check_prog("tar")) ) {
                return err;
            }

            Command cmd = CB_COMMAND(
                "tar",
                "-cJf",
                "bin/" EXECUTABLE_NAME "-linux-x86_64.tar.xz",
                "resources",
                "-C", "build/linux",
                EXECUTABLE_NAME,
                "--transform=s,^," EXECUTABLE_NAME "-linux-x86_64/,"
            );

            CB_INFO( "compressing and archiving . . ." );

            int res = 0;
            if( (res = process_exec( cmd )) ) {
                return error( E_SUBPROCESS, "tar " EXECUTABLE_NAME, res );
            }

            CB_INFO( "archived project at bin/" EXECUTABLE_NAME "-linux-x86_64.tar.xz" );
        } break;
        case T_WINDOWS: {
            if( (err = check_prog("zip")) ) {
                return err;
            }

            CB_INFO( "compressing and archiving . . ." );

            int res = 0;
            if( (res = process_exec( CB_COMMAND("zip", "-r", "resources.zip", "resources") )) ) {
                return error( E_SUBPROCESS, "zip resources", res );
            }

            if( !file_move( "build/windows/resources.zip", "resources.zip", false ) ) {
                return error( E_MOVE_FILE );
            }

            if( (res = process_exec( CB_COMMAND("zip", "resources.zip", EXECUTABLE_NAME ".exe"), "build/windows" )) ) {
                if( file_exists( "build/windows/resources.zip" ) ) {
                    file_remove( "build/windows/resources.zip" );
                }
                return error( E_SUBPROCESS, "zip executable", res );
            }

            if( !file_move( "bin/" EXECUTABLE_NAME "-windows-x86_64.zip", "build/windows/resources.zip", false ) ) {
                if( file_exists( "build/windows/resources.zip" ) ) {
                    file_remove( "build/windows/resources.zip" );
                }
                return error( E_MOVE_FILE );
            }

            CB_INFO( "archived project at bin/" EXECUTABLE_NAME "-windows-x86_64.zip" );
        } break;
        case T_WASM: {
            if( (err = check_prog("zip")) ) {
                return err;
            }

            CB_INFO( "compressing and archiving . . ." );

            Command cmd = CB_COMMAND(
                "zip",
                "files.zip",
                EXECUTABLE_NAME ".data",
                "index.html",
                EXECUTABLE_NAME ".js",
                EXECUTABLE_NAME ".wasm"
            );

            int res = 0;
            if( (res = process_exec( cmd, "build/wasm" )) ) {
                return error( E_SUBPROCESS, "zip wasm resources", res );
            }

            if( !file_move( "bin/" EXECUTABLE_NAME "-wasm.zip", "build/wasm/files.zip", false ) ) {
                if( file_exists( "build/wasm/files.zip" ) ) {
                    file_remove( "build/wasm/files.zip" );
                }
                return error( E_MOVE_FILE );
            }

            CB_INFO( "archived project at bin/" EXECUTABLE_NAME "-wasm.zip" );
        } break;
        case T_COUNT:
            break;
    }

    return err;
}

int mode_help( Opt* opt ) {
    Mode mode = M_HELP;
    if( opt ) {
        if( opt->mode == M_HELP ) {
            mode = opt->help.mode;
        } else {
            mode = opt->mode;
        }
    }

    printf( "OVERVIEW:    Build system for Bog Jam Summer 2025.\n" );
    printf( "USAGE:       ./cbuild %s [args...]\n", mode == M_HELP ? "<mode>" : string_from_mode(mode).buf );
    printf( "DESCRIPTION:\n" );
    printf( "  %s\n", mode_description( mode ) );

    switch( mode ) {
        case M_HELP: {
            printf( "ARGUMENTS:\n" );
            printf( "  <mode>  (optional) Mode to print help for.\n" );
            printf( "             valid: " );
            for( int m = 0; m < M_COUNT; ++m ) {
                printf( "%s", string_from_mode( m ).buf );
                if( (m + 1) < M_COUNT ) {
                    printf( ", " );
                } else {
                    printf( "\n" );
                }
            }
        } break;
        case M_BUILD: {
            printf( "NOTE:\n" );
            printf( "  If target is wasm, requires emcc and em++ in PATH.\n" );
            printf( "  If target is windows and native platform is linux,\n"
                   "    requires x86_64-w64-mingw32-gcc, x86_64-w64-mingw32-g++ and x86_64-w64-mingw32-ar in PATH.\n" );
            printf( "  If target is windows, requires lld in PATH.\n" );
            printf( "ARGUMENTS:\n" );
            printf( "  -t <target>  Set target platform.\n" );
            printf( "                 default: native (%s)\n", string_from_target( T_NATIVE ).buf );
            printf( "                 valid:   native, " );
            for( int t = 0; t < T_COUNT; ++t ) {
                printf( "%s", string_from_target( t ).buf );
                if( (t + 1) < T_COUNT ) {
                    printf( ", " );
                } else {
                    printf( "\n" );
                }
            }
            printf( "  -release     Build in release mode.\n" );
            printf( "                 Strips debug symbols and enables optimizations.\n" );
            printf( "  -rebuild     Always rebuild dependencies.\n" );
        } break;
        case M_RUN: {
            printf( "NOTE:\n" );
            printf( "  If target is wasm, requires emcc and em++ in PATH.\n" );
            printf( "  Only builds project if target is wasm.\n" );
            printf( "  If target is windows and native platform is linux,\n"
                   "    requires x86_64-w64-mingw32-gcc, x86_64-w64-mingw32-g++, x86_64-w64-mingw32-ar and wine in PATH.\n" );
            printf( "  If target is windows, requires lld in PATH.\n" );
            printf( "ARGUMENTS:\n" );
            printf( "  -t <target>  Set target platform.\n" );
            printf( "                 default: native (%s)\n", string_from_target( T_NATIVE ).buf );
            printf( "                 valid:   native, " );
            for( int t = 0; t < T_COUNT; ++t ) {
                printf( "%s", string_from_target( t ).buf );
                if( (t + 1) < T_COUNT ) {
                    printf( ", " );
                } else {
                    printf( "\n" );
                }
            }
            printf( "  -release     Build in release mode.\n" );
            printf( "                 Strips debug symbols and enables optimizations.\n" );
            printf( "  -rebuild     Always rebuild dependencies.\n" );
            printf( "  --           Stop parsing arguments and pass remaining arguments to project.\n" );
        } break;
        case M_PACKAGE: {
            printf( "NOTE:\n" );
            printf( "  If target is linux, requires tar in PATH.\n" );
            printf( "  If target is windows or wasm, requires zip in PATH.\n" );
            printf( "  If target is windows and native platform is linux,\n"
                   "    requires x86_64-w64-mingw32-gcc, x86_64-w64-mingw32-g++ and x86_64-w64-mingw32-ar in PATH.\n" );
            printf( "ARGUMENTS:\n" );
            printf( "  -t <target>  Set target platform.\n" );
            printf( "                 default: native (%s)\n", string_from_target( T_NATIVE ).buf );
            printf( "                 valid:   native, " );
            for( int t = 0; t < T_COUNT; ++t ) {
                printf( "%s", string_from_target( t ).buf );
                if( (t + 1) < T_COUNT ) {
                    printf( ", " );
                } else {
                    printf( "\n" );
                }
            }
        } break;
        case M_COUNT: break;
    }

    return 0;
}

const char* mode_description( Mode mode ) {
    switch( mode ) {
        case M_HELP    : return "Print help for current mode and exit.";
        case M_BUILD   : return "Build project.";
        case M_RUN     : return "Build project and run.";
        case M_PACKAGE : return "Build in release mode and create archive for release.";
        case M_COUNT: break;
    }
    return "";
}

String string_from_mode( Mode mode ) {
    switch( mode ) {
        case M_HELP    : return S("help");
        case M_BUILD   : return S("build");
        case M_RUN     : return S("run");
        case M_PACKAGE : return S("package");
        case M_COUNT   : break;
    }
    return S( "" );
}
bool mode_from_string( String string, Mode* out_mode ) {
    for( int m = 0; m < M_COUNT; ++m ) {
        if( string_cmp( string_from_mode(m), string ) == 0 ) {
            *out_mode = m;
            return true;
        }
    }
    return false;
}
String string_from_target( Target target ) {
    switch( target ) {
        case T_GNU_LINUX : return S("linux");
        case T_WINDOWS   : return S("windows");
        case T_WASM      : return S("wasm");

        case T_COUNT: break;
    }
    return S("");
}
bool target_from_string( String string, Target* out_target ) {
    if( string_cmp( string, S("native") ) == 0 ) {
        *out_target = T_NATIVE;
        return true;
    }
    for( int t = 0; t < T_COUNT; ++t ) {
        if( string_cmp( string, string_from_target(t) ) == 0 ) {
            *out_target = t;
            return true;
        }
    }
    return false;
}

const char* target_compiler_c( Target target ) {
    switch( target ) {
        case T_GNU_LINUX : return "clang";
        case T_WINDOWS   :
#if CB_PLATFORM_CURRENT == CB_PLATFORM_WINDOWS
            return "clang";
#else
            return MINGW32_GCC;
#endif
        case T_WASM      : return EMCC;

        case T_COUNT: break;
    }
    return "";
}
const char* target_compiler_cpp( Target target ) {
    switch( target ) {
        case T_GNU_LINUX : return "clang++";
        case T_WINDOWS   :
#if CB_PLATFORM_CURRENT == CB_PLATFORM_WINDOWS
            return "clang++";
#else
            return MINGW32_GPP;
#endif
        case T_WASM      : return EMPP;

        case T_COUNT: break;
    }
    return "";
}
const char* target_archiver( Target target ) {
    switch( target ) {
        case T_GNU_LINUX : return "ar";
        case T_WINDOWS   :
#if CB_PLATFORM_CURRENT == CB_PLATFORM_WINDOWS
            return "ar";
#else
            return MINGW32_AR;
#endif
        case T_WASM : return EMAR;

        case T_COUNT: break;
    }
    return "";
}
const char* target_ext( Target target ) {
    switch( target ) {
        case T_WINDOWS : return ".exe";
        case T_WASM    : return ".html";

        case T_GNU_LINUX:
        case T_COUNT:
            break;
    }
    return "";
}

int check_prog( const char* prog ) {
    CB_INFO( "checking for %s . . .", prog );
    if( process_is_in_path( prog ) ) {
        CB_INFO( "    %s found.", prog );
        return E_NONE;
    } else {
        return error( E_MISSING_PROGRAM, prog );
    }
}

int error( Error err, ... ) {
    va_list va;
    va_start( va, err );

    switch( err ) {
        case E_UNRECOGNIZED_MODE: {
            const char* mode = va_arg( va, const char* );
            CB_ERROR( "unrecognized mode: %s", mode );
            mode_help( NULL );
        } break;
        case E_UNRECOGNIZED_ARGUMENT: {
            Opt o = {};
            o.mode = va_arg( va, Mode );
            const char* str = va_arg( va, const char* );
            CB_ERROR( "unrecognized argument: %s", str );
            mode_help( &o );
        } break;
        case E_MISSING_ARGUMENT: {
            Opt o = {};
            o.mode = va_arg( va, Mode );

            CB_ERROR( "missing required argument!" );
            mode_help( &o );
        } break;
        case E_UNRECOGNIZED_TARGET: {
            Opt o  = {};
            o.mode = va_arg( va, Mode );

            const char* target = va_arg( va, const char* );
            CB_ERROR( "unrecognized target: %s", target );
            mode_help( &o );
        } break;
        case E_MISSING_PROGRAM: {
            const char* prog = va_arg( va, const char* );
            CB_ERROR( "required program '%s' is not in PATH!", prog );
        } break;

        case E_SUBPROCESS: {
            const char* proc = va_arg( va, const char* );
            int         res  = va_arg( va, int );

            CB_ERROR( "sub-process '%s' exited with code %d!", proc, res );
        } break;

        case E_MAKE_DIRECTORIES: {
            CB_ERROR( "failed to make directories!" );
        } break;
        case E_MOVE_FILE: {
            CB_ERROR( "failed to move file!" );
        } break;
        case E_NONE:
        case E_COUNT:
            break;
    }

    va_end( va );
    return err;
}

String sb_path( StringBuilder* sb, const char* fmt, ... ) {
    int start_len = sb->len;

    String result = {};
    result.buf = sb->buf + sb->len;

    va_list va;
    va_start( va, fmt );

    string_builder_fmt_va( sb, fmt, va );

    va_end( va );
    
    result.len = sb->len - start_len;

    CB_PUSH( sb, 0 );

    return result;
}

#define CB_IMPLEMENTATION
#include "cbuild.h"

