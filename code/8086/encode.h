#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "decoder.h"
#endif

typedef Enum_(U1, X8616_EncodeStatus) {
	x8616_encode_ok             = 0x00,
	x8616_encode_output_full    = 0x01,
	x8616_encode_invalid_record = 0x02,
	x8616_encode_bad_request    = 0x03,
};

typedef Struct_(X8616_EncodeRequest) {
	X8616_DecodedInstruction* instructions;
	U4    instruction_count; byte_pad(4);
	Slice output;
	FArena*          info_arena;
	X8616_InfoList_R msgs;
};

typedef Struct_(X8616_EncodeInfo) {
	X8616_InfoList msgs;
	Slice          bytes;
	U4             instructions_written;
	byte_pad(4);
};

X8616_EncodeInfo x8616_encode_instructions(X8616_EncodeRequest request);
