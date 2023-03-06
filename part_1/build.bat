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

if not exist tests\listing_0039_more_movs (
	echo Assembling: listing_0039_more_movs.asm
	call nasm ".\tests\listing_0039_more_movs.asm"
)

if not exist tests\listing_0040_challenge_movs (
	echo Assembling: listing_0040_challenge_movs.asm
	call nasm ".\tests\listing_0040_challenge_movs.asm"
)

echo:
ninja -C build