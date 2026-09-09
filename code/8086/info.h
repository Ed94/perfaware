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

	x8616_info_serialize_bad_request      = 0x0B,
	x8616_info_serialize_invalid_record   = 0x0C,
	x8616_info_serialize_unsupported_form = 0x0D,
	x8616_info_serialize_output_full      = 0x0E,

	x8616_info_count,
};

/* The hot path stores structured diagnostic facts only.
   Human-readable text is a projection of those facts. */
typedef Struct_(X8616_InfoMsg) {
	X8616_InfoMsg* next;
	X8616_InfoKind kind;
	X8616_InfoCode code;
	U2   source_size;
	U4   source_offset;
	U4   expected;
	U4   actual;
	Str8 text;
};

/* Static templates use the same <identifier> vocabulary as Duffle's str8_fmt_ktl_buf path.
   The decoder itself never expands these. */
RO_ global Str8 x8616_info_templates[x8616_info_count] = {
	[x8616_info_none]                       = slit8(""),
	[x8616_info_invalid_opcode]             = slit8("Header <actual> is not in the Part 1 8086 decode table at <offset>."),
	[x8616_info_invalid_opcode_extension]   = slit8("Opcode extension <actual> does not match the selected encoding at <offset>."),
	[x8616_info_invalid_post_opcode]        = slit8("Post-opcode byte <actual> does not match expected <expected> at <offset>."),
	[x8616_info_truncated_instruction]      = slit8("Instruction at <offset> needs <expected> bytes; <actual> are available."),
	[x8616_info_output_full]                = slit8("Decoded-instruction output is full: capacity <expected>, produced <actual>."),
	[x8616_info_gen_multiple_payloads]      = slit8("Encoding <offset> describes more than one stream payload."),
	[x8616_info_gen_body_cap_exceeded]      = slit8("Encoding <offset> requires <actual> body bytes; decoder body capacity is <expected>."),
	[x8616_info_gen_aux_cap_exceeded]       = slit8("Generated auxiliary decode table exceeds capacity <expected>."),
	[x8616_info_gen_ambiguous_decode]       = slit8("Decode is ambiguous for header/second-byte key <offset>: plans <expected> and <actual>."),
	[x8616_info_gen_dispatch_mismatch]      = slit8("Generated dispatch mismatch for header/second-byte key <offset>: expected <expected>, actual <actual>."),
	[x8616_info_serialize_bad_request]      = slit8("Serialize request is missing instructions, output, scratch, or info arena."),
	[x8616_info_serialize_invalid_record]   = slit8("Serialize record <offset> is not printable (op <actual>)."),
	[x8616_info_serialize_unsupported_form] = slit8("Serialize record <offset> has an unsupported display form (op <actual>)."),
	[x8616_info_serialize_output_full]      = slit8("Serialize output is full at record <offset>: capacity <expected>, produced <actual>."),
};

typedef Struct_(X8616_InfoList) {
	X8616_InfoMsg* first;
	X8616_InfoMsg* last;
	U4 count;
	U4 error_count;
	U4 warning_count;
	U4 dropped_count;
};

I_ Str8 x8616_info_template(X8616_InfoCode code) { return code < x8616_info_count ? x8616_info_templates[code] : x8616_info_templates[x8616_info_none]; }

FI_ void x8616_info_push(FArena_R scratch
	, X8616_InfoList_R msgs
	, X8616_InfoKind   kind
	, X8616_InfoCode   code
	, U4               source_offset
	, U2               source_size
	, U4               expected
	, U4               actual
){
	assert(scratch != nullptr);
	msgs->count         += 1;
	msgs->error_count   += kind == x8616_info_error;
	msgs->warning_count += kind == x8616_info_warning;

	U4 allocation_size = align_pow2(S_(X8616_InfoMsg), MEM_ALIGNMENT_DEFAULT);
	U4 left            = scratch->capacity - scratch->used;
	if (allocation_size > left) { ++ msgs->dropped_count; return; }

	X8616_InfoMsg_R msg = farena_push_type(scratch, X8616_InfoMsg);
	// msg[0] = (X8616_InfoMsg){0};
	msg->kind          = kind;
	msg->code          = code;
	msg->source_offset = source_offset;
	msg->source_size   = source_size;
	msg->expected      = expected;
	msg->actual        = actual;
	msg->text          = x8616_info_template(code);
	sll_queue_push_n(msgs->first, msgs->last, msg, next);
}
