#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "decoder.h"
#endif

typedef Enum_(U1, X8616_SerializeStatus) {
	x8616_serialize_ok               = 0x00,
	x8616_serialize_output_full      = 0x01,
	x8616_serialize_invalid_record   = 0x02,
	x8616_serialize_unsupported_form = 0x03,
};

typedef Struct_(X8616_SerializeRequest) {
	X8616_DecodedInstruction* instructions;
	U4    instruction_count;
	Slice output;
	Slice scratch;
};

typedef Struct_(X8616_SerializeInfo) {
	Str8                   text;
	U4                     instructions_written;
	X8616_SerializeStatus  status;
};

X8616_SerializeInfo x8616_serialize_instructions(X8616_SerializeRequest request);
