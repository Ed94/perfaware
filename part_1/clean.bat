@echo off

if exist build\nul (
	@RD /S /Q "build"
)

if exist tests\listing_0037_single_register_mov (
	DEL /Q tests\listing_0037_single_register_mov
)

if exist tests\listing_0038_many_register_mov (
	DEL /Q tests\listing_0038_many_register_mov
)

if exist tests\listing_0039_more_movs (
	DEL /Q tests\listing_0039_more_movs
)

if exist tests\listing_0040_challenge_movs (
	DEL /Q tests\listing_0040_challenge_movs
)

