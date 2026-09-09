#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "decoder.h"
#endif

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
