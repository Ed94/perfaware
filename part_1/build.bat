@echo off

if not exist build\nul (
	meson setup build
)

ninja -C build
