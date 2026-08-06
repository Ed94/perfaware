<#
.SYNOPSIS
    Build script for the perfaware project (8086 simulator + tooling).

.DESCRIPTION
    Compiles the canonical source tree into a Win32 console executable
    using clang and lld-link. No CRT (/ENTRY:main, /NODEFAULTLIB).

    Prerequisites:
      - clang on PATH (LLVM install)
      - lld-link.exe on PATH (LLVM install)
      - Windows SDK (for kernel32.lib, user32.lib, gdi32.lib)

    Build artifacts land in <root>/build/. The script is not
    incremental -- every invocation recompiles every source file.
#>

$path_root      = split-path -Path $PSScriptRoot -Parent
$path_build     = join-path $path_root 'build'
$path_code      = join-path $path_root 'code'
$path_duffle    = join-path $path_code 'duffle'
$path_8086      = join-path $path_code '8086'
$path_part_1    = join-path $path_code 'part_1'
$path_scripts   = join-path $path_root 'scripts'
$path_toolchain = join-path $path_root 'toolchain'

if ((test-path $path_build) -eq $false) {
    new-item -itemtype directory -path $path_build
}

# --- Toolchain Definition ---
$compiler  = "clang"
$linker    = 'lld-link.exe'

# --- Compiler Flags ---
$f_compile               = "-c"
$f_debug                 = "-g"
$f_define                = "-D"
$f_include               = "-I"
$f_output                = "-o"
$f_std_c11               = "-std=c11"
$f_std_c23               = "-std=c23"
$f_wall                  = "-Wall"
$f_wno_attributes        = "-Wno-attributes"
$f_exceptions_disabled   = "-fno-exceptions"
$f_diagnostics_absolute  = "-fdiagnostics-absolute-paths"
$f_optimize_none         = "-O0"
$f_optimize_size         = "-Os"
$f_optimize_intrinsics   = "-Oi"
$f_optimize_debug        = "-Og"
$f_omit_frame_ptr        = "-fomit-frame-pointer"
$f_code_sections         = "-ffunction-sections"
$f_data_sections         = "-fdata-sections"
$f_no_strict_alias       = "-fno-strict-aliasing"

# --- Linker Flags (Win32 lld-link) ---
# lld-link takes Win32-style flags directly (no `-Wl,` prefix).
$f_nologo                = '/NOLOGO'
$f_no_default_lib        = '/NODEFAULTLIB'
$f_link_win_machine_64   = '/MACHINE:X64'
$f_link_no_incremental   = '/INCREMENTAL:NO'
$f_link_win_subsystem    = '/SUBSYSTEM:CONSOLE'
$f_link_win_debug        = '/DEBUG'
$f_link_win_entry        = '/ENTRY:'
$f_link_win_pdb          = '/PDB:'
$f_link_win_path_output  = '/OUT:'
$f_link_win_map          = '/MAP:'

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
    $compile_args += $f_wall
    $compile_args += $f_wno_attributes
    $compile_args += $f_exceptions_disabled
    $compile_args += $f_diagnostics_absolute
    $compile_args += $f_optimize_none
    $compile_args += $f_debug
    foreach ($p in $include_paths) {
        $compile_args += ($f_include + $p)
    }
    $compile_args += $user_compile_args
    $compile_args += $f_compile
    $compile_args += $unit
    $compile_args += ($f_output + $link_module)

    write-host "Compiling '$unit' -> '$link_module'" -ForegroundColor DarkCyan
    & $compiler $compile_args
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
    $link_args += $f_no_default_lib
    $link_args += $f_link_win_machine_64
    $link_args += $f_link_no_incremental
    $link_args += ($f_link_win_path_output + $module)
    $link_args += $f_link_win_subsystem
    $link_args += $f_link_win_debug
    $link_args += ($f_link_win_entry + 'main')
    $link_args += ($f_link_win_pdb + $pdb)
    $link_args += ($f_link_win_map + $map)

    $libraries = @(
        'kernel32.lib',
        'user32.lib',
        'gdi32.lib'
    )
    foreach ($lib in $libraries) {
        $link_args += $lib
    }

    $link_args += $link_modules
    $link_args += $user_link_args

    write-host "Linking modules into '$module'" -ForegroundColor DarkCyan
    & $linker $link_args
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
    $compile_args += ($f_define + 'BUILD_DEBUG=1')

    compile-unit $source_c $module_c $includes $compile_args

    $pdb = join-path $path_build 'sim_8086.pdb'
    $exe = join-path $path_build 'sim_8086.exe'

    link-modules $module_c $exe $pdb @()
}

build-part_1
