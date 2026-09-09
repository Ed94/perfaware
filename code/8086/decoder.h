#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	include "encoder.h"
#	include "info.h"
#endif

enum {
	X8616_OPERAND_SOURCE_COUNT = 0x10,
	X8616_DECODE_BODY_CAP      = 0x06,
	X8616_DECODE_BODY_PAD      = 0x02,
};

typedef Enum_(U2, X8616_DecodedOperandFlags) {
	Bit_(x8616_decoded_operand_none,          0),
	Bit_(x8616_decoded_operand_register,      1),
	Bit_(x8616_decoded_operand_segment,       2),
	Bit_(x8616_decoded_operand_memory,        3),
	Bit_(x8616_decoded_operand_immediate,     4),
	Bit_(x8616_decoded_operand_relative,      5),
	Bit_(x8616_decoded_operand_far_ptr,       6),
	Bit_(x8616_decoded_operand_direct,        7),
	Bit_(x8616_decoded_operand_sign_extended, 8),
	Bit_(x8616_decoded_operand_implicit,      9),
};

typedef Union_(X8616_DecodedReg) {
	X8616_Reg8  r8;
	X8616_Reg16 r16;
};

typedef Struct_(X8616_DecodedOperand) {
	X8616_DecodedOperandFlags flags;
	X8616_WidthMode           width;
	X8616_DecodedReg          reg;
	X8616_Segment             segment;
	X8616_Mod                 mod;
	X8616_EA                  ea;
	S2 displacement;
	U2 address;
	U2 immediate;
	U2 far_offset;
	U2 far_segment;
	U1 displacement_bytes;
	U1 immediate_bytes;
};

typedef Struct_(X8616_DecodedPrefixes) {
	U1 count;
	B1 lock;
	B1            has_repeat;
	X8616_Repeat  repeat;
	B1            has_segment;
	X8616_Segment segment;
};


typedef Enum_(U1, X8616_DecodeFlags) {
	x8616_decode_none      = 0b00000000,
	x8616_decode_invalid   = 0b00000001,
	x8616_decode_truncated = 0b00000010,
};

typedef Struct_(X8616_DecodedInstruction) {
	X8616_Op              op;
	X8616_EncodingFlags   flags;
	X8616_DecodeFlags     decode_flags;
	X8616_WidthMode       width;
	X8616_DecodedPrefixes prefixes;
	X8616_DecodedOperand  operands[2];
	U1 operand_count;
	X8616_OpcodePrefix opcode;
	X8616_Direction    d;
	X8616_Width        w;
	B1 has_mod_rm;
	U1 size;
	U1 size_required;
};

typedef Enum_(U2, X8616_DecodePlanFlags) {
	Bit_(x8616_plan_none,            0),
	Bit_(x8616_plan_has_modrm,       1),
	Bit_(x8616_plan_has_post_opcode, 2),
	Bit_(x8616_plan_has_d,           3),
	Bit_(x8616_plan_has_w,           4),
	Bit_(x8616_plan_has_s,           5),
	Bit_(x8616_plan_has_v,           6),
	Bit_(x8616_plan_has_z,           7),
	Bit_(x8616_plan_has_reg,         8),
	Bit_(x8616_plan_has_sr,          9),
	Bit_(x8616_plan_uses_rm,        10),
	Bit_(x8616_plan_is_prefix,      11),
};

typedef Enum_(U1, X8616_DecodePayload) {
	x8616_payload_none       = 0x0,
	x8616_payload_imm        = 0x1,
	x8616_payload_imm8       = 0x2,
	x8616_payload_imm16      = 0x3,
	x8616_payload_mem_direct = 0x4,
	x8616_payload_rel8       = 0x5,
	x8616_payload_rel16      = 0x6,
	x8616_payload_far_ptr    = 0x7,
};

typedef Enum_(U1, X8616_DecodePrefixKind) {
	x8616_prefix_none    = 0x0,
	x8616_prefix_lock    = 0x1,
	x8616_prefix_repeat  = 0x2,
	x8616_prefix_segment = 0x3,
};

typedef Struct_(X8616_DecodePlan) {
	X8616_DecodePlanFlags flags;

	X8616_Op            op;
	X8616_EncodingFlags encoding_flags;
	X8616_WidthMode     width;

	X8616_Operand operands[2];
	U1            operand_count;

	X8616_DecodePayload    payload;
	X8616_DecodePrefixKind prefix_kind;

	U1 d_shift;
	U1 w_shift;
	U1 s_shift;
	U1 v_shift;
	U1 z_shift;
	U1 reg_shift;
	U1 sr_shift;

	X8616_BytePattern mod_rm;
	X8616_BytePattern post_opcode;
};

enum {
	X8616_DECODE_PLAN_INVALID = 0x00,
	X8616_DECODE_AUX_BIT      = 0x8000,
	X8616_DECODE_AUX_MASK     = 0x7FFF,
};

typedef Struct_(X8616_DecodeRequest) {
	U1 const* source;
	U4        source_size;

	X8616_DecodedInstruction* instructions;
	U4                        instruction_capacity;

	FArena* info_arena;
};

typedef Struct_(X8616_DecodeInfo) {
	U4 source_consumed;
	U4 instruction_count;
	U4 instruction_capacity;
	X8616_InfoList msgs;
};

X8616_DecodeInfo x8616_decode(X8616_DecodeRequest request);

typedef Struct_(X8616_DecodeOneInfo) {
	X8616_DecodedInstruction instruction;
	X8616_InfoList           msgs;
	U4                       source_consumed;
};

X8616_DecodeOneInfo x8616_decode_one(U1_R source, U4 source_size, FArena_R info_arena);
