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
# Assumes 'mipsel-none-elf' toolchain is in your system's PATH.
$compiler  = "clang"
$linker    = 'lld-link.exe'

# General Compiler Flags
$f_compile          = "-c"
$f_debug            = "-g"
$f_define           = "-D"
$f_include          = "-I"
$f_output           = "-o"
$f_std_c11          = "-std=c11"
$f_std_c23          = "-std=c23"

$f_exceptions_disabled		    = '-fno-exceptions'
$f_diagnostics_absolute_paths   = '-fdiagnostics-absolute-paths'

# Warning Flags
$f_wall             = "-Wall"
$f_wno_attributes   = "-Wno-attributes"

# Optimization Flags
$f_optimize_none       = "-O0"
$f_optimize_size       = "-Os"
$f_optimize_intrinsics = "-Oi"
$f_optimize_debug      = "-Og"
$f_omit_frame_ptr      = "-fomit-frame-pointer"

$f_path_output      = '-o'
$f_wall 			= '-Wall'
$f_nologo 			= '/nologo'

# Environment & Standard Library Flags
$f_no_stdlib        = "-nostdlib"
$f_freestanding     = "-ffreestanding"
$f_no_builtin       = "-fno-builtin"

# Linker-related Flags (for Compiler)
$f_code_sections    = "-ffunction-sections"
$f_data_sections    = "-fdata-sections"
$f_no_strict_alias  = "-fno-strict-aliasing"


# Linker Flags (passed via -Wl,)

$f_link_pass_through_prefix = "-Wl,"
$f_link_mapfile             = "-Map=" # Usage: $flag_link_pass_through_prefix + $flag_link_mapfile + path
$f_link_gc_sections         = "--gc-sections"
$f_link_format              = "--oformat="
$f_link_start_group         = "--start-group"
$f_link_end_group           = "--end-group"
$f_link_static              = "-static"
$f_link_script              = "-T"
$f_link_lib_path            = "-L"
$f_link_lib                 = "-l"

$f_link_no_incremental          = '/INCREMENTAL:NO'
$f_link_win_subsystem_console   = '/SUBSYSTEM:CONSOLE'
$f_link_win_machine_64          = '/MACHINE:X64'
$f_link_win_debug               = '/DEBUG'
$f_link_win_pdb 			    = '/PDB:'
$f_link_win_path_output         = '/OUT:'

function compile-unit { param(
	[string]  $unit,
	[string]  $link_module,
	[string[]]$include_paths,
	[string[]]$user_compile_args
)
	$compile_args = @()
	$compile_args += $f_wall
	$compile_args += $f_c11
	$compile_args += $f_no_optimization
	$compile_args += $f_diagnostics_absolute_paths
	$compile_args += $f_exceptions_disabled
	# $compiler_args += "-nostdlib"
	# $compiler_args += "-ffreestanding"

	$compile_args += $user_compile_args

	$compile_args += $f_compile
	$compile_args += $unit, ($f_output + $link_module)

    write-host "Compiling '$unit' -> '$link_module'" -ForegroundColor DarkCyan
    # $compile_args | ForEach-Object { Write-Host "`t$_" -ForegroundColor Green }
		& $compiler $compile_args
    if ($LASTEXITCODE -ne 0) { write-error "Compilation failed for $unit. Aborting."; exit 1 }
}

function link-modules { param([string[]]$link_modules, [string]  $module, [string] $pdb, [string[]]$user_link_args)
	$link_args = @()
	$link_args = @()
	$link_args += $f_nologo
	$link_args += $f_link_win_machine_64
	$link_args += $f_link_no_incremental
	$link_args += ($f_link_win_path_output + $binary)
	$link_args += $f_link_win_debug
	$link_args += $f_link_win_pdb + $pdb
	$link_args += $f_link_mapfile + $map
	$link_args += $f_link_win_subsystem_console
	# $link_args += "/nodefaultlib"

	$base_name  = [System.IO.Path]::GetFileNameWithoutExtension($module)
	$map        = join-path $path_build "$base_name.map"
	$link_args += ($f_link_pass_through_prefix + $f_link_mapfile + $map)

	$link_args += ($f_link_pass_through_prefix + $f_link_start_group)
	$libraries = @(
		'kernel32.lib',
		'user32.lib',
		'gdi32.lib'
	)
	foreach ($lib in $libraries) {
		$link_args += ($f_link_lib + $lib)
	}

	$link_args += $link_modules

	$final_link_args = @($link_args) + ($f_output + $elf)

	write-host "Linking modules into '$elf'"  -ForegroundColor DarkCyan
	$final_link_args += ($f_link_pass_through_prefix + $f_link_end_group)
	# $final_link_args | foreach-object { write-host $_ }
		& $linker  $final_link_args
	if ($LASTEXITCODE -ne 0) { write-error "Linking failed. Aborting."; exit 1 }
}

function build-part_1 {
	$includes = @(
		$path_duffle,
		$path_8086,
		$path_part_1
	)

	$source_c = join-path $path_part_1 'sim_8086.c'
	$module_c = join-path $path_build  'sim_8086.o'

	$compile_args = @()
	$compile_args  += ($flag_define + 'BUILD_DEBUG=1')
	$compile_args  += $f_debug
	$compiler_args += $f_optimize_none
	compile-unit $source_c $module_c $includes $compile_args

	$pdb = join-path $path_build 'sim_8086.pdb'
	$exe = join-path $path_build 'sim_8086.exe'

	$link_args = @()
	$link_args += $f_debug
	$link_modules = @(
		$module_c
	)
	link-modules $link_modules $exe $pdb $link_args
}
build-part_1
