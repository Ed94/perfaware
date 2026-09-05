#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "duffle/dsl.h"
#	include "duffle/memory.h"
#	include "duffle/text.h"
#endif

typedef Enum_(U1, X8616_InfoKind) {
	x8616_info_note    = 0x0,
	x8616_info_warning = 0x1,
	x8616_info_error   = 0x2,
};

typedef Enum_(U1, X8616_InfoCode) {
	x8616_info_none                     = 0x00,

	x8616_info_invalid_opcode           = 0x01,
	x8616_info_invalid_opcode_extension = 0x02,
	x8616_info_invalid_post_opcode      = 0x03,
	x8616_info_truncated_instruction    = 0x04,
	x8616_info_output_full              = 0x05,

	x8616_info_gen_multiple_payloads    = 0x06,
	x8616_info_gen_body_cap_exceeded    = 0x07,
	x8616_info_gen_aux_cap_exceeded     = 0x08,
	x8616_info_gen_ambiguous_decode     = 0x09,
	x8616_info_gen_dispatch_mismatch    = 0x0A,

	x8616_info_count,
};

/* The hot path stores structured diagnostic facts only.
   Human-readable text is a projection of those facts. */
typedef Struct_(X8616_InfoMsg) {
	X8616_InfoMsg* next;
	X8616_InfoKind kind;
	X8616_InfoCode code;
	U4   source_offset;
	U2   source_size;
	U4   expected;
	U4   actual;
	Str8 text;
};

/* Static templates use the same <identifier> vocabulary as Duffle's str8_fmt_ktl_buf path.
   The decoder itself never expands these. */
RO_ global Str8 x8616_info_templates[x8616_info_count] = {
	[x8616_info_none]                     = str8(""),
	[x8616_info_invalid_opcode]           = str8("Opcode <actual> is not in the Part 1 8086 decode table at <offset>."),
	[x8616_info_invalid_opcode_extension] = str8("Opcode extension <actual> does not match the selected encoding at <offset>."),
	[x8616_info_invalid_post_opcode]      = str8("Post-opcode byte <actual> does not match expected <expected> at <offset>."),
	[x8616_info_truncated_instruction]    = str8("Instruction at <offset> needs <expected> bytes; <actual> are available."),
	[x8616_info_output_full]              = str8("Decoded-instruction output is full: capacity <expected>, produced <actual>."),
	[x8616_info_gen_multiple_payloads]    = str8("Encoding <offset> describes more than one stream payload."),
	[x8616_info_gen_body_cap_exceeded]    = str8("Encoding <offset> requires <actual> body bytes; decoder body capacity is <expected>."),
	[x8616_info_gen_aux_cap_exceeded]     = str8("Generated auxiliary decode table exceeds capacity <expected>."),
	[x8616_info_gen_ambiguous_decode]     = str8("Decode is ambiguous for opcode/second-byte key <offset>: plans <expected> and <actual>."),
	[x8616_info_gen_dispatch_mismatch]    = str8("Generated dispatch mismatch for opcode/second-byte key <offset>: expected <expected>, actual <actual>."),
};

typedef Struct_(X8616_InfoList) {
	X8616_InfoMsg* first;
	X8616_InfoMsg* last;
	U4 count;
	U4 error_count;
	U4 warning_count;
	U4 dropped_count;
};

FI_ Str8 x8616_info_template(X8616_InfoCode code) { return code < x8616_info_count ? x8616_info_templates[code] : x8616_info_templates[x8616_info_none]; }

FI_ void x8616_info_push(FArena_R scratch
	, X8616_InfoList_R list
	, X8616_InfoKind   kind
	, X8616_InfoCode   code
	, U4               source_offset
	, U2               source_size
	, U4               expected
	, U4               actual
){
	assert(scratch != nullptr);
	list->count         += 1;
	list->error_count   += kind == x8616_info_error;
	list->warning_count += kind == x8616_info_warning;

	U4 allocation_size = align_pow2(S_(X8616_InfoMsg), MEM_ALIGNMENT_DEFAULT);
	U4 left            = scratch->capacity - scratch->used;
	if (allocation_size > left) { ++ list->dropped_count; return; }

	X8616_InfoMsg_R msg = farena_push_type(scratch, X8616_InfoMsg);
	// msg[0] = (X8616_InfoMsg){0};
	msg->kind          = kind;
	msg->code          = code;
	msg->source_offset = source_offset;
	msg->source_size   = source_size;
	msg->expected      = expected;
	msg->actual        = actual;
	msg->text          = x8616_info_template(code);
	sll_queue_push_n(list->first, list->last, msg, next);
}
