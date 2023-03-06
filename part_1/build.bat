@echo off

if not exist build\nul (
	meson setup build
)

echo:
if not exist tests\listing_0037_single_register_mov (
	echo Assembling: listing_0037_single_register_mov.asm
	call nasm ".\tests\listing_0037_single_register_mov.asm"
)

if not exist tests\listing_0038_many_register_mov (
	echo Assembling: listing_0038_many_register_mov.asm
	call nasm ".\tests\listing_0038_many_register_mov.asm"
)

echo:
ninja -C build