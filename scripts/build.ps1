$path_root      = split-path -Path $PSScriptRoot -Parent
$path_build     = join-path $path_root 'build'
$path_part_1    = join-path $path_root 'part_1'
$path_scripts   = join-path $path_root 'scripts'
$path_toolchain = join-path $path_root 'toolchain'

if ((test-path $path_build) -eq $false) {
	new-item -itemtype directory -path $path_build
}

# --- Toolchain Definition ---
# Assumes 'mipsel-none-elf' toolchain is in your system's PATH.
$compiler  = "clang"
$linker    = 'lld-link.exe'

function compile-unit { param(
	[string]  $unit,
	[string]  $link_module,
	[string[]]$include_paths,
	[string[]]$user_compile_args
)
	$compile_args = @()


    write-host "Compiling '$unit' -> '$link_module'" -ForegroundColor DarkCyan
    # $compile_args | ForEach-Object { Write-Host "`t$_" -ForegroundColor Green }
		& $Compiler $compile_args
    if ($LASTEXITCODE -ne 0) { write-error "Compilation failed for $unit. Aborting."; exit 1 }
}

function link-modules { param([string[]]$link_modules, [string]  $elf, [string[]]$user_link_args)
	$link_args = @()

	write-host "Linking modules into '$elf'"  -ForegroundColor DarkCyan
	$final_link_args += ($f_link_pass_through_prefix + $f_link_end_group)
	# $final_link_args | foreach-object { write-host $_ }
		& $Compiler $final_link_args
		& mipsel-none-elf-objdump.exe -W $elf >> $dasm
	if ($LASTEXITCODE -ne 0) { write-error "Linking failed. Aborting."; exit 1 }
}

function build-part_1() {
	
}
build-part_1()

