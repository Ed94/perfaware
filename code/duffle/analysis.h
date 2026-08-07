#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	if _WIN32
#		include "win32.h"
#	endif
#endif

#pragma region Debug
#define debug_trap() __builtin_debugtrap()

#if BUILD_DEBUG
	FI_ void assert(U8 cond);
#else
	#define assert(cond)
#endif

#define trap() __builtin_trap()

#define assert_always(x) do { if((x) == false) {trap();} } while(0)
#pragma endregion Debug
