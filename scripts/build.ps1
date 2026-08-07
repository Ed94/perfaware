$path_root      = split-path -Path $PSScriptRoot -Parent
$path_build     = join-path $path_root 'build'
$path_code      = join-path $path_root 'code'
$path_duffle    = join-path $path_code 'duffle'
$path_8086      = join-path $path_code  '8086'
$path_part_1    = join-path $path_code  'part_1'
$path_scripts   = join-path $path_root  'scripts'
$path_toolchain = join-path $path_root  'toolchain'

if ((test-path $path_build) -eq $false) {
    new-item -itemtype directory -path $path_build
}

# --- Toolchain Definition ---
# All tools default to LLVM/clang. The MIPS variables are kept for
# catalog parity with C:\projects\Pikuma\ps1\scripts\build_psyq.ps1,
# but are not used by the current Win32 build.
$compiler  = "clang"
$linker    = 'lld-link.exe'
$objcopy   = 'llvm-objcopy'
$archiver  = 'llvm-ar'

# --- Compiler Flags: General / Compilation Mode ---
$f_compile               = '-c'              # Compile to .o (no link)
$f_output                = '-o'              # Output path
$f_preprocess            = '-E'              # Preprocess to stdout
$f_syntax_only           = '-fsyntax-only'   # Parse + type-check, no codegen
$f_save_temps            = '-save-temps'     # Keep intermediate files
$f_show_includes         = '-H'              # Print include chain
$f_dependency_file       = '-MF'             # Write dep file (use with -MD)
$f_dependency_gen        = '-MD'             # Generate dep file alongside output
$f_emit_llvm             = '-emit-llvm'      # Emit LLVM IR

# --- Compiler Flags: Language Standard ---
$f_std_c11               = '-std=c11'
$f_std_c17               = '-std=c17'
$f_std_c23               = '-std=c23'
$f_std_gnu11             = '-std=gnu11'
$f_std_gnu17             = '-std=gnu17'
$f_std_gnu23             = '-std=gnu23'
$f_std_cpp17             = '-std=c++17'
$f_std_cpp20             = '-std=c++20'
$f_std_cpp23             = '-std=c++23'
$f_all_c                 = '-x','c'           # Force C language
$f_all_cpp               = '-x','c++'         # Force C++ language
$f_all_asm               = '-x','assembler'   # Force asm
$f_all_asm_with_cpp      = '-x','assembler-with-cpp'

# --- Compiler Flags: Preprocessor ---
$f_define                = '-D'              # #define NAME[=VALUE]
$f_undef                 = '-U'              # #undef NAME
$f_include               = '-I'              # Add directory to search path
$f_include_system        = '-isystem'        # System header (suppresses warnings)
$f_include_quote         = '-iquote'         # Quote-only header path
$f_include_after         = '-idirafter'      # Append dir to search path
$f_include_file          = '-include'        # Force include file
$f_no_stdinc             = '-nostdinc'       # No standard include paths
$f_charset_utf8          = '-utf-8'          # Source charset = UTF-8

# --- Compiler Flags: Warnings ---
# https://clang.llvm.org/docs/DiagnosticsReference.html
$f_wall                  = '-Wall'
$f_wextra                = '-Wextra'
$f_werror                = '-Werror'
$f_warnings_as_errors    = '-Werror'
$f_wno_attributes        = '-Wno-attributes'
$f_warnings_ignore       = '-Wno-'           # Prefix; usage: $f_warnings_ignore + 'unused'
$f_wno_unused_function   = '-Wno-unused-function'
$f_wno_unused_variable   = '-Wno-unused-variable'
$f_wno_switch            = '-Wno-switch'
$f_wno_uninitialized     = '-Wno-uninitialized'
$f_wno_implicit_function_declaration = '-Wno-implicit-function-declaration'
$f_wno_ms_include        = '-Wno-microsoft-include'
$f_wno_return_type_c_linkage = '-Wno-return-type-c-linkage'
$f_wno_main_return_type  = '-Wno-main-return-type'
$f_warning               = '-W'              # Prefix; usage: $f_warning + 'switch'
$f_pedantic              = '-Wpedantic'
$f_doc_warnings          = '-Wdocumentation'

# --- Compiler Flags: Optimization ---
$f_optimize_none         = '-O0'             # No optimization (debug default)
$f_optimize_debug        = '-Og'             # Optimize for debug
$f_optimize_size         = '-Os'             # Optimize for size
$f_optimize_fast         = '-O2'             # Optimize for speed (default release)
$f_optimize_max          = '-O3'             # Aggressive optimization
$f_optimize_intrinsics   = '-Oi'             # MSVC intrinsic hints (clang accepts; semantics ~ -O2)
$f_optimize_all          = '-Ofast'          # -O3 + non-IEEE-compliant math
$f_omit_frame_ptr        = '-fomit-frame-pointer'
$f_no_omit_frame_ptr     = '-fno-omit-frame-pointer'
$f_lto                   = '-flto'           # Link-time optimization
$f_lto_thin              = '-flto=thin'

# --- Compiler Flags: Debug / Profile ---
$f_debug                 = '-g'              # Debug info (default DWARF)
$f_debug_codeview        = '-gcodeview'      # CodeView debug format (MS tools)
$f_debug_dwarf_4         = '-gdwarf-4'       # Specific DWARF version
$f_debug_dwarf_5         = '-gdwarf-5'
$f_debug_line_tables_only = '-gline-tables-only'
$f_profiling_debug       = '-fdebug-info-for-profiling'
$f_time_trace            = '-ftime-trace'    # Emit Chrome trace JSON
$f_coverage_generate     = '-fprofile-instr-generate'
$f_coverage_use          = '-fprofile-instr-use='

# --- Compiler Flags: Code Generation ---
$f_no_color_diagnostics  = '-fno-color-diagnostics'
$f_color_diagnostics     = '-fcolor-diagnostics'
$f_diagnostics_absolute  = '-fdiagnostics-absolute-paths'
$f_exceptions_disabled   = '-fno-exceptions'
$f_exceptions_enabled    = '-fexceptions'
$f_RTTI_disabled         = '-fno-rtti'
$f_RTTI_enabled          = '-frtti'
$f_section_data          = '-fdata-sections'
$f_section_functions     = '-ffunction-sections'
$f_no_strict_alias       = '-fno-strict-aliasing'
$f_strict_aliasing       = '-fstrict-aliasing'
$f_signed_char           = '-fsigned-char'
$f_unsigned_char         = '-funsigned-char'
$f_pic                   = '-fPIC'           # Position-independent code
$f_pie                   = '-fPIE'           # Position-independent executable
$f_no_pic                = '-fno-pic'
$f_no_pie                = '-fno-pie'
$f_visible_hidden        = '-fvisibility=hidden'
$f_visible_default       = '-fvisibility=default'
$f_align_functions       = '-falign-functions='
$f_align_loops           = '-falign-loops='
$f_ms_extensions         = '-fms-extensions' # Allow MSVC language extensions
$f_ms_compatibility      = '-fms-compatibility'
$f_ms_runtime_lib_none   = '-fno-ms-runtime'
$f_preprocess_non_integrated = '-no-integrated-cpp'
$f_stack_protector       = '-fstack-protector'
$f_stack_protector_strong = '-fstack-protector-strong'
$f_stack_protector_all   = '-fstack-protector-all'
$f_vptr                  = '-fvirtual-function-elimination'
$f_vectorize             = '-vectorize-loops'
$f_vectorize_slp         = '-vectorize-slp'

# --- Compiler Flags: Sanitizers ---
$f_sanitize_address       = '-fsanitize=address'
$f_sanitize_undefined     = '-fsanitize=undefined'
$f_sanitize_thread        = '-fsanitize=thread'
$f_sanitize_memory        = '-fsanitize=memory'
$f_sanitize_hwaddress     = '-fsanitize=hwaddress'
$f_sanitize_address_safe  = '-fsanitize=address,safe-stack'
$f_sanitize_cfi           = '-fsanitize=cfi'

# --- Compiler Flags: Environment / Target ---
$f_no_stdlib             = '-nostdlib'
$f_freestanding          = '-ffreestanding'
$f_no_builtin            = '-fno-builtin'
$f_target_arch           = '-target'         # Usage: $f_target_arch + 'x86_64-pc-windows-msvc'
$f_target_abi            = '-mabi='
$f_target_march          = '-march='
$f_target_mtune          = '-mtune='
$f_target_mcpu           = '-mcpu='
$f_target_mfloat_abi     = '-mfloat-abi='
$f_target_x86            = '-m64'
$f_target_x86_32         = '-m32'
$f_target_arm            = '-marm'
$f_target_thumb          = '-mthumb'

# --- Compiler Flags: Verbosity / Misc ---
$f_verbose               = '-v'
$f_set_stack_size        = '-stack='         # Stack size (linker-side, but accepted by clang)
$f_symbol_prefix         = '-femit-all-decls'

# --- Compiler Flags: MIPS Architecture (catalog parity with ps1/build_psyq.ps1) ---
# Used by the PS1 cross-compile path (C:\projects\Pikuma\ps1). Not used by
# the current Win32 build. Kept here so the catalog is complete.
$f_arch_mips1            = '-march=mips1'
$f_arch_mips2            = '-march=mips2'
$f_arch_mips3            = '-march=mips3'
$f_arch_abi32            = '-mabi=32'
$f_arch_abi64            = '-mabi=64'
$f_arch_little_endian    = '-EL'
$f_arch_big_endian       = '-EB'
$f_arch_fp32             = '-mfp32'
$f_arch_no_pic           = '-fno-pic'
$f_arch_no_shared        = '-mno-shared'
$f_arch_no_abicalls      = '-mno-abicalls'
$f_arch_no_llsc          = '-mno-llsc'
$f_arch_no_gpopt         = '-mno-gpopt'
$f_arch_no_stack_prot    = '-fno-stack-protector'

# --- Linker Flags (passed via -Wl, to clang; or directly to gcc-style linkers) ---
# These are GNU ld-style flags. When invoking lld-link.exe directly, prefer
# the Win32-flavored flags below.
$f_linker                = '-Wl,'           # Prefix; usage: $f_linker + '--gc-sections'
$f_link_mapfile          = '--Map='         # Output map file (GNU ld)
$f_link_gc_sections      = '--gc-sections'  # Dead-strip unused sections
$f_link_format           = '--oformat='     # Output format (e.g. elf32-littlemips)
$f_link_start_group      = '--start-group'  # Circular library resolution
$f_link_end_group        = '--end-group'
$f_link_static           = '-static'        # Static linking
$f_link_script           = '-T'             # Linker script
$f_link_lib_path         = '-L'             # Library search path
$f_link_lib              = '-l'             # Library shorthand (foo.lib -> -lfoo)
$f_link_relaxable        = '--relax'        # Enable linker relaxation
$f_link_strip_all        = '-s'             # Strip all symbols
$f_link_strip_debug      = '-S'             # Strip debug symbols
$f_link_no_undefined     = '--no-undefined' # Error on unresolved symbols
$f_link_undefined        = '-u'             # Force a symbol to be defined
$f_link_version_script   = '--version-script='
$f_link_entry            = '-e'             # Entry point (GNU ld)
$f_link_hash_style       = '--hash-style='
$f_link_as_needed        = '--as-needed'
$f_link_no_as_needed     = '--no-as-needed'

# --- Linker Flags (Win32 lld-link, used directly) ---
# https://lld.llvm.org/windows_support.html
$f_nologo                = '/NOLOGO'
$f_link_win_no_incremental = '/INCREMENTAL:NO'
$f_link_win_no_default_lib = '/NODEFAULTLIB'
$f_link_win_subsystem_console = '/SUBSYSTEM:CONSOLE'
$f_link_win_subsystem_windows = '/SUBSYSTEM:WINDOWS'
$f_link_win_subsystem_efi_app = '/SUBSYSTEM:EFI_APPLICATION'
$f_link_win_subsystem_native   = '/SUBSYSTEM:NATIVE'
$f_link_win_machine_32   = '/MACHINE:X86'
$f_link_win_machine_64   = '/MACHINE:X64'
$f_link_win_machine_arm  = '/MACHINE:ARM'
$f_link_win_machine_arm64 = '/MACHINE:ARM64'
$f_link_win_debug        = '/DEBUG'
$f_link_win_pdb          = '/PDB:'
$f_link_win_path_output  = '/OUT:'
$f_link_win_map          = '/MAP:'
$f_link_win_entry        = '/ENTRY:'
$f_link_win_lib_path     = '/LIBPATH:'
$f_link_win_default_lib  = '/DEFAULTLIB:'
$f_link_win_dll          = '/DLL'
$f_link_win_optimize_ref = '/OPT:REF'        # Eliminate unreferenced data/functions
$f_link_win_optimize_no_icf = '/OPT:NOICF'   # Disable identical COMDAT folding
$f_link_win_optimize_no_ref = '/OPT:NOREF'   # Disable dead-strip
$f_link_win_stack        = '/STACK:'
$f_link_win_heap         = '/HEAP:'
$f_link_win_merge        = '/MERGE:'
$f_link_win_align        = '/ALIGN:'
$f_link_win_base         = '/BASE:'
$f_link_win_version      = '/VERSION:'
$f_link_win_dynamic_base = '/DYNAMICBASE'
$f_link_win_release      = '/RELEASE'
$f_link_win_force        = '/FORCE'
$f_link_win_verbose      = '/VERBOSE'
$f_link_win_wholearchive = '/WHOLEARCHIVE:'
$f_link_win_export       = '/EXPORT:'
$f_link_win_delay_load   = '/DELAYLOAD:'
$f_link_win_import       = '/IMPORT:'
$f_link_win_def          = '/DEF:'
$f_link_win_manifest     = '/MANIFEST'
$f_link_win_manifest_no  = '/MANIFEST:NO'
$f_link_win_manifest_uac = '/MANIFESTUAC:'
$f_link_win_manifest_input = '/MANIFESTINPUT:'
$f_link_win_section      = '/SECTION:'
$f_link_win_swaprun      = '/SWAPRUN'
$f_link_win_driver       = '/DRIVER'
$f_link_win_large_address_aware = '/LARGEADDRESSAWARE'
$f_link_win_fail_if_mismatch = '/FAILIFMISMATCH'
$f_link_win_through      = '/THREADOPT'
$f_link_win_error_report = '/ERRORREPORT'
$f_link_win_repro        = '/BREPRO'
$f_link_win_safeseh      = '/SAFESEH'
$f_link_win_guard_cf     = '/GUARD:CF'
$f_link_win_guard_no_cf  = '/GUARD:NOCF'
$f_link_win_pgo_run      = '/USEPROFILE'
$f_link_win_pgo_gen      = '/PROFILE'
$f_link_win_idl          = '/IDLOUT:'
$f_link_win_delay_unload = '/DELAY:UNLOAD'
$f_link_win_compat_property = '/CETCOMPAT'
$f_link_win_debug_fastlink = '/DEBUG:FASTLINK'
$f_link_win_lib_list     = '/LIB'

# --- Objcopy Flags ---
$f_objcopy_format        = '-O'             # Output format
$f_objcopy_add_section   = '--add-section'
$f_objcopy_update_section = '--update-section'
$f_objcopy_set_section_flags = '--set-section-flags'

# --- Baseline Libraries (lld-link, Win32) ---
$libraries_win32 = @(
    'kernel32.lib'
    'user32.lib'
    'gdi32.lib'
	'vcruntime.lib'
)

# --- Functions ---

function compile-unit {
    param(
        [string]   $unit,
        [string]   $link_module,
        [string[]] $include_paths,
        [string[]] $user_compile_args
    )
    $compile_args = @()
    $compile_args += $f_std_c11
	$compile_args += $f_all_c
	$compile_args += $f_ms_ex
    $compile_args += $f_wall
    $compile_args += $f_wno_attributes
    $compile_args += $f_exceptions_disabled
    $compile_args += $f_diagnostics_absolute
    $compile_args += $f_debug
    foreach ($p in $include_paths) {
        $compile_args += ($f_include + $p)
    }
    $compile_args += $user_compile_args
    $compile_args += $f_compile
    $compile_args += $unit
    $compile_args += ($f_output + $link_module)

    write-host "Compiling '$unit' -> '$link_module'" -ForegroundColor DarkCyan
	$time_to_compile = Measure-Command { & $compiler $compile_args }
	write-host "Compilation took $($time_to_compile.TotalMilliseconds)ms"
    if ($LASTEXITCODE -ne 0) { write-error "Compilation failed for $unit. Aborting."; exit 1 }
}

function link-modules {
    param(
        [string[]] $link_modules,
        [string]   $module,
        [string]   $pdb,
        [string[]] $user_link_args
    )
    $base_name = [System.IO.Path]::GetFileNameWithoutExtension($module)
    $map       = join-path $path_build "$base_name.map"

    $link_args = @()
    $link_args += $f_nologo
    $link_args += $f_link_win_no_default_lib
    $link_args += $f_link_win_machine_64
    $link_args += $f_link_win_no_incremental
    $link_args += ($f_link_win_path_output + $module)
    $link_args += $f_link_win_subsystem_console
    $link_args += $f_link_win_debug
    $link_args += ($f_link_win_entry + 'main')
    $link_args += ($f_link_win_pdb + $pdb)
    $link_args += ($f_link_win_map + $map)

    foreach ($lib in $libraries_win32) {
        $link_args += $lib
    }

    $link_args += $link_modules
    $link_args += $user_link_args

    write-host "Linking modules into '$module'" -ForegroundColor DarkCyan
	$time_to_link = Measure-Command { & $linker $link_args }
	write-host "Linking took $($time_to_link.TotalMilliseconds)ms"
    if ($LASTEXITCODE -ne 0) { write-error "Linking failed. Aborting."; exit 1 }
}

function build-part_1 {
    # The base lib uses subdir-prefixed includes (e.g. "duffle/dsl.h"),
    # so the include root is <code>, not <code>/duffle.
    $includes = @(
        $path_code
    )

    $source_c = join-path $path_part_1 'sim_8086.c'
    $module_c = join-path $path_build  'sim_8086.o'

    $compile_args = @()
	$compile_args += $f_debug
	$compile_args += $f_optimize_none
	# $compile_args += $f_optimize_size
    $compile_args += ($f_define + 'BUILD_DEBUG=1')

    compile-unit $source_c $module_c $includes $compile_args

    $pdb = join-path $path_build 'sim_8086.pdb'
    $exe = join-path $path_build 'sim_8086.exe'

    link-modules $module_c $exe $pdb @()
}

build-part_1
