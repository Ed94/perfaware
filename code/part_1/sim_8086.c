#include "duffle/dsl.h"
#include "duffle/analysis.h"
#include "duffle/math.h"
#include "duffle/memory.h"
#include "duffle/hashing.h"
#include "duffle/encoding.h"
#include "duffle/tables.h"
#include "duffle/text.h"
#include "duffle/files.h"

#include "duffle/win32.h"

enum {
	Scratchpad_Len = kilo(1),
	FileRam_Len    = kilo(16),
};
typedef Struct_(SMemory) {
	U1 Scratchpad [Scratchpad_Len];
	U1 FileRam    [FileRam_Len];
};
global SMemory smem;

#define path_course_content "./course_content/perfaware/"
#define path_part1          path_course_content "part1/"

CLANG_OPTIMIZE_DISABLE
int main()
{
	FArena scratch    = farena_make(slice_ut_arr(smem.Scratchpad));
	FArena file_arena = farena_make(slice_ut_arr(smem.FileRam));

	Str8 path_listing_0037_single_register_mov     = slit8(path_part1 "listing_0037_single_register_mov");
	Str8 path_listing_0037_single_register_mov_asm = slit8(path_part1 "listing_0037_single_register_mov.asm");
	Slice_U1 data = data_from_file_path(& file_arena, path_listing_0037_single_register_mov, & scratch);

	ms_exit_process(0);
	return 0;
}
CLANG_OPTIMIZE_ENABLE
