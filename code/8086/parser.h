#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "decoder.h"
#endif

typedef Enum_(U1, X8616_ParseStatus) {
	x8616_parse_ok               = 0x00,
	x8616_parse_output_full      = 0x01,
	x8616_parse_syntax           = 0x02,
	x8616_parse_invalid_record   = 0x03,
	x8616_parse_unsupported_form = 0x04,
	x8616_parse_unknown_mnemonic = 0x05,
};

typedef Struct_(X8616_ParseRequest) {
	Str8                      source;
	X8616_DecodedInstruction* out_instructions;
	U4                        instruction_cap;
	FArena*                   info_arena;
	X8616_InfoList_R          msgs;
};

typedef Struct_(X8616_ParseInfo) {
	X8616_InfoList msgs;
	U4             instruction_count;
	U4             source_consumed;
};

X8616_ParseInfo x8616_parse_instructions(X8616_ParseRequest request);
