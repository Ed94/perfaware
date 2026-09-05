#ifdef INTELLISENSE_DIRECTIVES
#	include "duffle/dsl.h"
#	include "encoder.h"
#	include "decoder.h"
#	include "info.h"
#endif

internal void
x8616_decoder_smoke_test(void)
{
	U1 bytes[] = {
		x8616_mov_r16_i  (x8616_ax, 0x1234),
		x8616_add_r16_i8s(x8616_ax, -2),
		x8616_nop(),
	};

	X8616_DecodedInstruction instructions[3] = {0};
	U1     info_memory[1024] = {0};
	FArena info_scratch      = farena_make(slice_ut_arr(info_memory));

	X8616_DecodeInfo info = x8616_decode((X8616_DecodeRequest){
		.source               = bytes,
		.source_size          = S_(bytes),
		.instructions         = instructions,
		.instruction_capacity = Array_len(instructions),
		.info_arena           = & info_scratch,
	});

	assert(info.source_consumed   == S_(bytes));
	assert(info.instruction_count == 3);
	assert(info.msgs.error_count  == 0);

	assert(instructions[0].op == x8616_op_mov);
	assert(instructions[1].op == x8616_op_add);
	assert(instructions[2].op == x8616_op_xchg); /* 0x90 in the Part 1 table */
}
