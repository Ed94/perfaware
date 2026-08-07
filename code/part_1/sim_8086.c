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

typedef Struct_(U2_HL) { U1 Low; U1 High; };

#define bitmask_(pos)              (1 << pos)
#define bit_isolate(bitfield, pos) (bitmask_(pos) & value)

#define serialize_bit(bitfield, pos) (bit_isolate(bitfield, pos) == bitmask_(pos) ? '1' : '0')

typedef Enum_(U4, Endieaness) {
	Little,
	Big,
};

FI_ void a8utf8_from_u1_le(UTF8 out[8], U1 value) {
	out[0] = serialize_bit(value, 7);
	out[1] = serialize_bit(value, 6);
	out[2] = serialize_bit(value, 5);
	out[3] = serialize_bit(value, 4);
	out[4] = serialize_bit(value, 3);
	out[5] = serialize_bit(value, 2);
	out[6] = serialize_bit(value, 1);
	out[7] = serialize_bit(value, 0);
}
FI_ void a8utf8_from_u1_be(UTF8 out[8], U1 value) {
	out[0] = serialize_bit(value, 0);
	out[1] = serialize_bit(value, 1);
	out[2] = serialize_bit(value, 2);
	out[3] = serialize_bit(value, 3);
	out[4] = serialize_bit(value, 4);
	out[5] = serialize_bit(value, 5);
	out[6] = serialize_bit(value, 6);
	out[7] = serialize_bit(value, 7);
}

I_ U4 binary_as_str8_eval_len(Slice_U1 data) { return data.len * 8; }

#define jump_lt(a,b,label) if (a < b) goto label

I_ Str8 binary_as_str8(Slice_U1 data, FArena* str8_mem) { Str8 result = {0};
	U4 req_len = data.len * 8; jump_lt(str8_mem->capacity,req_len, jret);
	   result  = farena_push_array(str8_mem, UTF8, req_len);
	for index_iter(U4,cursor, 0,<,req_len) { a8utf8_from_u1_le(result.ptr + cursor, data.ptr[cursor]); }
	jret: return result;
}

typedef Enum_(U4, x86_Op) {
	x86_mov
};


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
