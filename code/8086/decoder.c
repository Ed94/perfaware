#ifdef INTELLISENSE_DIRECTIVES
#	include "encoder.h"
#	include "info.h"
#	include "decoder.h"
#	include "gen/decoder_table.h"
#endif

RO_ global U1 x8616_decode_disp_bytes[32] = {
	0,0,0,0,0,0,2,0,
	1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,
	0,0,0,0,0,0,0,0,
};

RO_ global U1 x8616_decode_payload_bytes[] = {
	0, // none
	0, // imm: dynamic
	1, // imm8
	2, // imm16
	2, // mem-direct
	1, // rel8
	2, // rel16
	4, // far-ptr
};

typedef Struct_(X8616_DecodePlex) {
	U1* source;
	U4  source_size;
	U4  source_offset;

	U1 body[X8616_DECODE_BODY_CAP + X8616_DECODE_BODY_PAD];
	U1 body_available;

	U2 dispatch;
	U1 plan_idx;
	X8616_DecodePlan* plan;

	U1 header;
	U1 post_opcode;
	U1 mod_rm;

	X8616_Direction     d;
	X8616_Width         w;
	X8616_Sign          s;
	X8616_VariableShift v;
	X8616_Repeat        z;
	X8616_ALU           alu;
	X8616_Condition     cc;
	X8616_DecodedReg    reg_opcode;
	X8616_Segment       sr_opcode;

	X8616_Mod        mod;
	X8616_DecodedReg reg;
	X8616_EA         rm;
	X8616_Segment    sr_modrm;

	X8616_WidthMode width;

	U1 body_at;
	U1 displacement_at;
	U1 displacement_bytes;
	U1 payload_at;
	U1 payload_bytes;
	U1 body_required;

	S2 displacement;
	U2 direct_address;
	U2 payload_u16;
	U2 far_segment;

	B1 direct_memory;
	B1 classification_truncated;
	B1 encoding_invalid;

	X8616_DecodedPrefixes    prefixes;
	X8616_DecodedOperand     operand_source[X8616_OPERAND_SOURCE_COUNT];
	X8616_DecodedInstruction instruction;

	FArena*         info_arena;
	X8616_InfoList* msgs;
};

FI_ U2 x8616_decode_u2(U1_R bytes) { return C_(U2, bytes[0] | u2_(bytes[1] << 8)); }

internal X8616_Opcode
x8616_decode_opcode(X8616_DecodePlan const* plan, U1 header)
{
	U1 field_mask = 0;
	if (plan->flags & x8616_plan_has_d)    field_mask |= x8616_field_mask(plan->d_shift,    X8616_OPCODE_BIT_WIDTH);
	if (plan->flags & x8616_plan_has_w)    field_mask |= x8616_field_mask(plan->w_shift,    X8616_OPCODE_BIT_WIDTH);
	if (plan->flags & x8616_plan_has_s)    field_mask |= x8616_field_mask(plan->s_shift,    X8616_OPCODE_BIT_WIDTH);
	if (plan->flags & x8616_plan_has_v)    field_mask |= x8616_field_mask(plan->v_shift,    X8616_OPCODE_BIT_WIDTH);
	if (plan->flags & x8616_plan_has_z)    field_mask |= x8616_field_mask(plan->z_shift,    X8616_OPCODE_BIT_WIDTH);
	if (plan->flags & x8616_plan_has_reg)  field_mask |= x8616_field_mask(plan->reg_shift,  X8616_OPCODE_REG_REG_WIDTH);
	if (plan->flags & x8616_plan_has_sr)   field_mask |= x8616_field_mask(plan->sr_shift,   X8616_OPCODE_SR_WIDTH);
	if (plan->flags & x8616_plan_has_alu)  field_mask |= x8616_field_mask(plan->alu_shift,  X8616_OPCODE_ALU_TTT_WIDTH);
	if (plan->flags & x8616_plan_has_cc)   field_mask |= x8616_field_mask(plan->cc_shift,   X8616_OPCODE_CC_WIDTH);
	if (field_mask == 0) return C_(X8616_Opcode, header);
	U1 opcode_mask = u1_(~field_mask);
	if (opcode_mask == 0) return C_(X8616_Opcode, header);
	return C_(X8616_Opcode, (header & opcode_mask) >> count_trailing_zeros_u4(opcode_mask));
}

internal void
x8616_decode_apply_prefix(X8616_DecodePlex_R plex, X8616_DecodePlan_R plan, U1 header) {
	plex->prefixes.count += 1;
	plex->prefixes.lock  |= plan->prefix_kind == x8616_prefix_lock;
	if (plan->prefix_kind == x8616_prefix_repeat) {
		plex->prefixes.has_repeat = 1;
		plex->prefixes.repeat     = C_(X8616_Repeat, x8616_bit_field_extract(header, (X8616_BitField){ plan->z_shift, X8616_OPCODE_BIT_WIDTH }));
	}
	if (plan->prefix_kind == x8616_prefix_segment) {
		plex->prefixes.has_segment = 1;
		plex->prefixes.segment     = C_(X8616_Segment, x8616_bit_field_extract(header, (X8616_BitField){ plan->sr_shift, X8616_OPCODE_SR_WIDTH }));
	}
}

internal U4
x8616_decode_one_plex(X8616_DecodePlex* plex)
{
	U4 prefix_at = 0;

	// Prefix pass. LOCK / REP / segment are prior bytes. Encodings that need a
	// second byte (aux dispatch) are never prefixes.
	while (prefix_at < plex->source_size)
	{
		U1 prefix_header   = plex->source[prefix_at];
		U2 prefix_dispatch = x8616_decode_dispatch[prefix_header];
		if (prefix_dispatch == false || (prefix_dispatch & X8616_DECODE_AUX_BIT)) break;

		X8616_DecodePlan_R prefix_plan = x8616_decode_plans + prefix_dispatch;
		if ((prefix_plan->flags & x8616_plan_is_prefix) == false) break;

		x8616_decode_apply_prefix(plex, prefix_plan, prefix_header);
		++ prefix_at;
	}

	U4 body_source_size  = plex->source_size - prefix_at;
	plex->body_available = u1_(body_source_size < X8616_DECODE_BODY_CAP ? body_source_size : X8616_DECODE_BODY_CAP);

	for (U1 idx = 0; idx < plex->body_available; ++ idx) plex->body[idx] = plex->source[prefix_at + idx];

	plex->header   = plex->body[0];
	plex->dispatch = x8616_decode_dispatch[plex->header];

	if (body_source_size == 0) {
		plex->classification_truncated = 1;
		x8616_info_push(plex->info_arena, plex->msgs, x8616_info_error
			, x8616_info_truncated_instruction
			, plex->source_offset + prefix_at
			, 0
			, 1
			, 0
		);
	}
	else if (plex->dispatch & X8616_DECODE_AUX_BIT)
	{
		if (body_source_size < 2) {
			plex->classification_truncated = 1;
			x8616_info_push(plex->info_arena, plex->msgs, x8616_info_error
				, x8616_info_truncated_instruction
				, plex->source_offset + prefix_at
				, u2_(body_source_size)
				, 2
				, body_source_size
			);
		}
		else
		{
			U2 aux_base = plex->dispatch & X8616_DECODE_AUX_MASK;
			plex->plan_idx = x8616_decode_aux[aux_base + plex->body[1]];
			if (plex->plan_idx == false) {
				plex->encoding_invalid = 1;
				x8616_info_push(plex->info_arena, plex->msgs, x8616_info_error
					, x8616_info_invalid_opcode_extension
					, plex->source_offset + prefix_at
					, 2
					, 0
					, plex->body[1]
				);
			}
		}
	}
	else
	{
		plex->plan_idx = u1_(plex->dispatch);
		if (plex->plan_idx == 0) {
			plex->encoding_invalid = 1;
			x8616_info_push(plex->info_arena, plex->msgs, x8616_info_error
				, x8616_info_invalid_opcode
				, plex->source_offset + prefix_at
				, 1
				, 0
				, plex->header
			);
		}
	}

	plex->plan = x8616_decode_plans + plex->plan_idx;

	// A direct-dispatch plan may still have a constrained second byte.
	// Headers with multiple catalog rows were already resolved through aux.
	if (plex->plan_idx && (plex->plan->flags & (x8616_plan_has_modrm | x8616_plan_has_post_opcode)))
	{
		if (body_source_size < 2)
		{
			if (plex->classification_truncated == 0) {
				plex->classification_truncated = 1;
				x8616_info_push(plex->info_arena, plex->msgs, x8616_info_error
					, x8616_info_truncated_instruction
					, plex->source_offset + prefix_at
					, u2_(body_source_size)
					, 2
					, body_source_size
				);
			}
		}
		else
		{
			U1 second = plex->body[1];
			if (plex->plan->mod_rm.mask && (second & plex->plan->mod_rm.mask) != plex->plan->mod_rm.bits) {
				plex->encoding_invalid = 1;
				x8616_info_push(plex->info_arena, plex->msgs, x8616_info_error
					, x8616_info_invalid_opcode_extension
					, plex->source_offset + prefix_at + 1
					, 1
					, plex->plan->mod_rm.bits
					, second & plex->plan->mod_rm.mask
				);
			}

			if (plex->plan->post_opcode.mask && (second & plex->plan->post_opcode.mask) != plex->plan->post_opcode.bits) {
				plex->encoding_invalid = 1;
				x8616_info_push(plex->info_arena, plex->msgs, x8616_info_error
					, x8616_info_invalid_post_opcode
					, plex->source_offset + prefix_at + 1
					, 1
					, plex->plan->post_opcode.bits
					, second & plex->plan->post_opcode.mask
				);
			}
		}
	}

	// Nil plan: still return a normal zero-valued instruction record and make
	// progress through the stream. Aux classification inspected two bytes.
	if (plex->plan_idx == 0) {
		U1 invalid_body_size = (plex->dispatch & X8616_DECODE_AUX_BIT) && body_source_size >= 2 ? 2 : u1_(body_source_size != 0);
		plex->instruction.prefixes      = plex->prefixes;
		plex->instruction.op            = x8616_op_invalid;
		plex->instruction.decode_flags  = (plex->encoding_invalid ? x8616_decode_invalid : 0) | (plex->classification_truncated ? x8616_decode_truncated : 0);
		plex->instruction.size          = u1_(prefix_at + invalid_body_size);
		plex->instruction.size_required = plex->instruction.size;
		return plex->instruction.size;
	}

	X8616_DecodePlan_R plan = plex->plan;
	plex->d              = C_(X8616_Direction,     x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->d_shift,   X8616_OPCODE_BIT_WIDTH }));
	plex->w              = C_(X8616_Width,         x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->w_shift,   X8616_OPCODE_BIT_WIDTH }));
	plex->s              = C_(X8616_Sign,          x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->s_shift,   X8616_OPCODE_BIT_WIDTH }));
	plex->v              = C_(X8616_VariableShift, x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->v_shift,   X8616_OPCODE_BIT_WIDTH }));
	plex->z              = C_(X8616_Repeat,        x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->z_shift,   X8616_OPCODE_BIT_WIDTH }));
	plex->reg_opcode.r16 = C_(X8616_Reg16,         x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->reg_shift, X8616_OPCODE_REG_REG_WIDTH }));
	plex->sr_opcode      = C_(X8616_Segment,       x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->sr_shift,  X8616_OPCODE_SR_WIDTH }));

	plex->width = plan->width;
	if (plex->width == x8616_width_dynamic && (plan->flags & x8616_plan_has_w))
		plex->width = plex->w ? x8616_width_word : x8616_width_byte;

	plex->body_at = 1;

	if (plan->flags & x8616_plan_has_post_opcode) {
		plex->post_opcode = plex->body[plex->body_at];
		plex->body_at    += 1;
	}

	if (plan->flags & x8616_plan_has_modrm) {
		plex->mod_rm   = plex->body[plex->body_at];
		plex->mod      = C_(X8616_Mod,     x8616_modrm_mod(plex->mod_rm));
		plex->reg.r16  = C_(X8616_Reg16,   x8616_modrm_reg(plex->mod_rm));
		plex->rm       = C_(X8616_EA,      x8616_modrm_rm(plex->mod_rm));
		plex->sr_modrm = C_(X8616_Segment, x8616_modrm_sr(plex->mod_rm));
		plex->body_at += 1;
	}

	if (plan->flags & x8616_plan_has_alu) {
		plex->alu = C_(X8616_ALU, x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->alu_shift, X8616_OPCODE_ALU_TTT_WIDTH }));
	}
	if (plan->flags & x8616_plan_has_cc) {
		plex->cc = C_(X8616_Condition, x8616_bit_field_extract(plex->header, (X8616_BitField){ plan->cc_shift, X8616_OPCODE_CC_WIDTH }));
	}

	plex->displacement_at = plex->body_at;

	if (plan->flags & x8616_plan_uses_rm) plex->displacement_bytes = x8616_decode_disp_bytes[(u1_(plex->mod) << 3) | u1_(plex->rm)];

	plex->direct_memory = (plan->flags & x8616_plan_uses_rm) && plex->mod == x8616_mod_mem && plex->rm  == x8616_ea_direct;

	if      (plex->displacement_bytes == 1) plex->displacement = s2_(s1_(plex->body[plex->displacement_at]));
	else if (plex->displacement_bytes == 2) plex->displacement = s2_(x8616_decode_u2(plex->body + plex->displacement_at));

	if (plex->direct_memory) plex->direct_address = x8616_decode_u2(plex->body + plex->displacement_at);

	plex->payload_at    = plex->displacement_at + plex->displacement_bytes;
	plex->payload_bytes = x8616_decode_payload_bytes[plan->payload];

	if (plan->payload == x8616_payload_imm) {
		plex->payload_bytes = (plex->width == x8616_width_word && ((plan->flags & x8616_plan_has_s) && plex->s) == false) ? 2 : 1;
	}

	plex->body_required = plex->payload_at + plex->payload_bytes;
	plex->payload_u16   = x8616_decode_u2(plex->body + plex->payload_at);
	plex->far_segment   = x8616_decode_u2(plex->body + plex->payload_at + 2);

	if (body_source_size < plex->body_required && plex->classification_truncated == 0) {
		plex->classification_truncated = 1;
		x8616_info_push(plex->info_arena, plex->msgs, x8616_info_error
			, x8616_info_truncated_instruction
			, plex->source_offset + prefix_at
			, u2_(body_source_size)
			, plex->body_required, body_source_size);
	}

	// ------------------------------------------------------------------------
	// Operand-source plex
	//
	// Every possible operand source is populated once from the shared decoded
	// fields. The plan projects two slots out of this array; there is no
	// operand-kind switch on the hot path.

	X8616_DecodedOperand* source = plex->operand_source;

	source[x8616_operand_reg_modrm].flags = x8616_decoded_operand_register;
	source[x8616_operand_reg_modrm].width = plex->width;
	source[x8616_operand_reg_modrm].reg   = plex->reg;

	source[x8616_operand_reg_opcode].flags = x8616_decoded_operand_register;
	source[x8616_operand_reg_opcode].width = plex->width;
	source[x8616_operand_reg_opcode].reg   = plex->reg_opcode;

	source[x8616_operand_segment_modrm].flags   = x8616_decoded_operand_segment;
	source[x8616_operand_segment_modrm].width   = x8616_width_word;
	source[x8616_operand_segment_modrm].segment = plex->sr_modrm;

	source[x8616_operand_segment_opcode].flags   = x8616_decoded_operand_segment;
	source[x8616_operand_segment_opcode].width   = x8616_width_word;
	source[x8616_operand_segment_opcode].segment = plex->sr_opcode;

	source[x8616_operand_acc].flags   = x8616_decoded_operand_register | x8616_decoded_operand_implicit;
	source[x8616_operand_acc].width   = plex->width;
	source[x8616_operand_acc].reg.r16 = x8616_ax;

	source[x8616_operand_dx].flags   = x8616_decoded_operand_register | x8616_decoded_operand_implicit;
	source[x8616_operand_dx].width   = x8616_width_word;
	source[x8616_operand_dx].reg.r16 = x8616_dx;

	source[x8616_operand_rm].width = plex->width;
	if (plex->mod == x8616_mod_reg) {
		source[x8616_operand_rm].flags   = x8616_decoded_operand_register;
		source[x8616_operand_rm].reg.r16 = C_(X8616_Reg16, plex->rm);
	}
	else {
		source[x8616_operand_rm].flags              = x8616_decoded_operand_memory;
		source[x8616_operand_rm].mod                = plex->mod;
		source[x8616_operand_rm].ea                 = plex->rm;
		source[x8616_operand_rm].displacement       = plex->displacement;
		source[x8616_operand_rm].displacement_bytes = plex->displacement_bytes;
		if (plex->direct_memory) {
			source[x8616_operand_rm].flags  |= x8616_decoded_operand_direct;
			source[x8616_operand_rm].address = plex->direct_address;
		}
	}

	source[x8616_operand_imm].flags = x8616_decoded_operand_immediate 
		| (((plan->flags & x8616_plan_has_s) && plex->s) ? x8616_decoded_operand_sign_extended : 0);
	source[x8616_operand_imm].width           = plex->width;
	source[x8616_operand_imm].immediate       = plex->payload_u16;
	source[x8616_operand_imm].immediate_bytes = plex->payload_bytes;

	source[x8616_operand_imm8].flags           = x8616_decoded_operand_immediate;
	source[x8616_operand_imm8].width           = x8616_width_byte;
	source[x8616_operand_imm8].immediate       = u1_(plex->payload_u16);
	source[x8616_operand_imm8].immediate_bytes = 1;

	source[x8616_operand_imm16].flags           = x8616_decoded_operand_immediate;
	source[x8616_operand_imm16].width           = x8616_width_word;
	source[x8616_operand_imm16].immediate       = plex->payload_u16;
	source[x8616_operand_imm16].immediate_bytes = 2;

	source[x8616_operand_mem_direct].flags   = x8616_decoded_operand_memory | x8616_decoded_operand_direct;
	source[x8616_operand_mem_direct].width   = plex->width;
	source[x8616_operand_mem_direct].address = plex->payload_u16;

	source[x8616_operand_rel8].flags              = x8616_decoded_operand_relative;
	source[x8616_operand_rel8].width              = x8616_width_byte;
	source[x8616_operand_rel8].displacement       = C_(S2, s1_(plex->payload_u16));
	source[x8616_operand_rel8].displacement_bytes = 1;

	source[x8616_operand_rel16].flags              = x8616_decoded_operand_relative;
	source[x8616_operand_rel16].width              = x8616_width_word;
	source[x8616_operand_rel16].displacement       = s2_(plex->payload_u16);
	source[x8616_operand_rel16].displacement_bytes = 2;

	source[x8616_operand_far_ptr].flags       = x8616_decoded_operand_far_ptr;
	source[x8616_operand_far_ptr].far_offset  = plex->payload_u16;
	source[x8616_operand_far_ptr].far_segment = plex->far_segment;

	source[x8616_operand_shift_count].flags = x8616_decoded_operand_implicit;
	source[x8616_operand_shift_count].width = x8616_width_byte;
	if (plex->v == x8616_v_cl) {
		source[x8616_operand_shift_count].flags |= x8616_decoded_operand_register;
		source[x8616_operand_shift_count].reg.r8 = x8616_cl;
	}
	else {
		source[x8616_operand_shift_count].flags          |= x8616_decoded_operand_immediate;
		source[x8616_operand_shift_count].immediate       = 1;
		source[x8616_operand_shift_count].immediate_bytes = 0;
	}

	// Final projection. Authored operand order is d=0; d=1 exchanges the two source slots without introducing a separate decoder path.
	U1 swap = C_(U1, ((plan->flags & x8616_plan_has_d) != 0) & u1_(plex->d));
	plex->instruction.operands[swap]     = source[plan->operands[0]];
	plex->instruction.operands[swap ^ 1] = source[plan->operands[1]];

	plex->instruction.op            = plan->op;
	if (plan->flags & x8616_plan_has_alu) {
		plex->instruction.alu = plex->alu;
		plex->instruction.op  = x8616_op_from_alu[plex->alu];
	}
	if (plan->flags & x8616_plan_has_cc) {
		plex->instruction.cc = plex->cc;
		plex->instruction.op = x8616_op_from_cc[plex->cc];
	}
	if (plan->digit_kind != x8616_digit_none) {
		U1 digit = u1_(plex->reg.r16);
		if (plan->digit_kind == x8616_digit_alu) {
			plex->alu             = C_(X8616_ALU, digit);
			plex->instruction.alu = plex->alu;
			plex->instruction.op  = x8616_op_from_alu[digit];
		}
		else if (plan->digit_kind == x8616_digit_shift) plex->instruction.op = x8616_op_from_shift[digit];
	}
	plex->instruction.flags         = plan->encoding_flags;
	plex->instruction.decode_flags  = (plex->encoding_invalid ? x8616_decode_invalid : 0) | (plex->classification_truncated ? x8616_decode_truncated : 0);
	plex->instruction.width         = plex->width;
	plex->instruction.prefixes      = plex->prefixes;
	plex->instruction.operand_count = plan->operand_count;
	plex->instruction.opcode        = x8616_decode_opcode(plan, plex->header);
	plex->instruction.d             = (plan->flags & x8616_plan_has_d) ? plex->d : x8616_d_rm_dst;
	plex->instruction.w             = (plan->flags & x8616_plan_has_w) ? plex->w : x8616_w_byte;
	plex->instruction.has_mod_rm    = (plan->flags & x8616_plan_has_modrm) != 0;
	plex->instruction.header        = plex->header;
	plex->instruction.mod_rm        = plex->mod_rm;
	plex->instruction.post_opcode   = plex->post_opcode;

	U4 total_required  = prefix_at + plex->body_required;
	U4 total_available = plex->source_size;
	U4 total_consumed  = total_required < total_available ? total_required : total_available;

	plex->instruction.size          = u1_(total_consumed);
	plex->instruction.size_required = u1_(total_required);
	return total_consumed;
}

#define x8616_decode_(...) x8616_decode((X8616_DecodeRequest){__VA_ARGS__})
X8616_DecodeInfo x8616_decode(X8616_DecodeRequest request)
{
	X8616_DecodeInfo result = {0};
	result.instruction_cap = request.instruction_cap;

	while (result.source_consumed < request.source_len && result.instruction_count < request.instruction_cap) {
		X8616_DecodePlex plex = {0};
		plex.source        = request.source + result.source_consumed;
		plex.source_size   = request.source_len - result.source_consumed;
		plex.source_offset = result.source_consumed;
		plex.info_arena    = request.info_arena;
		plex.msgs          = & result.msgs;
		U4 consumed = x8616_decode_one_plex(& plex);
		request.out_instructions[result.instruction_count] = plex.instruction; 
		result.instruction_count += 1;
		if (consumed == false) consumed = 1;
		result.source_consumed += consumed;
	}

	if (result.source_consumed < request.source_len && result.instruction_count == request.instruction_cap) {
		x8616_info_push(request.info_arena, & result.msgs, x8616_info_warning
			, x8616_info_output_full
			, result.source_consumed
			, 0
			, request.instruction_cap
			, result.instruction_count
		);
	}
	return result;
}

X8616_DecodeOneInfo x8616_decode_one(U1_R source, U4 source_size, FArena_R info_arena) {
	X8616_DecodeOneInfo result = {0};
	X8616_DecodePlex    plex   = {0};
	plex.source        = source;
	plex.source_size   = source_size;
	plex.source_offset = 0;
	plex.info_arena    = info_arena;
	plex.msgs          = & result.msgs;
	result.source_consumed = x8616_decode_one_plex(& plex);
	result.instruction     = plex.instruction;
	return result;
}
