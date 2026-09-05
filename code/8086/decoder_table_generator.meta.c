#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "duffle/dsl.h"
#	include "duffle/text.h"
#	include "duffle/win32.h"
#	include "encoder.h"
#	include "encoder_table.h"
#	include "decoder.h"
#	include "info.h"
#endif

enum {
	X8616_DECODE_GEN_MAX_AUX = 256 * 256,
};

typedef Struct_(X8616_DecodeGen) {
	X8616_DecodePlan plans[X8616_ENCODING_COUNT + 1];

	U2 dispatch[256];
	U1 aux[X8616_DECODE_GEN_MAX_AUX];

	U4 aux_count;
	U4 ambiguous_opcode_count;
	U4 verified_count;

	X8616_InfoList msgs;
};

typedef Struct_(X8616_DecodeGenInfo) {
	U4 plan_count;
	U4 aux_count;
	U4 ambiguous_opcode_count;
	U4 verified_count;

	X8616_InfoList msgs;
};

FI_ B1 x8616_decode_gen_operand_uses_modrm(X8616_Operand operand) { 
	return operand == x8616_operand_rm || operand == x8616_operand_reg_modrm || operand == x8616_operand_segment_modrm; 
}

FI_ B1 x8616_decode_gen_operand_uses_rm(X8616_Operand operand) { return operand == x8616_operand_rm; }

internal X8616_DecodePayload
x8616_decode_gen_payload_from_operand(X8616_Operand operand) {
	switch (operand) {
		case x8616_operand_imm:        return x8616_payload_imm;
		case x8616_operand_imm8:       return x8616_payload_imm8;
		case x8616_operand_imm16:      return x8616_payload_imm16;
		case x8616_operand_mem_direct: return x8616_payload_mem_direct;
		case x8616_operand_rel8:       return x8616_payload_rel8;
		case x8616_operand_rel16:      return x8616_payload_rel16;
		case x8616_operand_far_ptr:    return x8616_payload_far_ptr;
		default:                       return x8616_payload_none;
	}
}

internal X8616_DecodePrefixKind
x8616_decode_gen_prefix_kind(X8616_Encoding const* encoding) {
	if ((encoding->flags & x8616_encoding_prefix) == 0) return x8616_prefix_none;
	switch (encoding->op) {
		case x8616_op_lock:    return x8616_prefix_lock;
		case x8616_op_rep:     return x8616_prefix_repeat;
		case x8616_op_segment: return x8616_prefix_segment;
		default:               return x8616_prefix_none;
	}
}

internal X8616_DecodePlan
x8616_decode_gen_plan(X8616_Encoding const* encoding, U4 encoding_idx, X8616_InfoList* msgs, FArena_R info_scratch)
{
	X8616_DecodePlan plan = {0};
	plan.op             = encoding->op;
	plan.encoding_flags = encoding->flags;
	plan.width          = encoding->width;
	plan.operands[0]    = encoding->operands[0];
	plan.operands[1]    = encoding->operands[1];
	plan.operand_count  = (encoding->operands[0] != x8616_operand_none) + (encoding->operands[1] != x8616_operand_none);
	plan.prefix_kind    = x8616_decode_gen_prefix_kind(encoding);
	plan.mod_rm         = encoding->mod_rm;
	plan.post_opcode    = encoding->post_opcode;

	if (encoding->mod_rm.mask || x8616_decode_gen_operand_uses_modrm(encoding->operands[0]) || x8616_decode_gen_operand_uses_modrm(encoding->operands[1]))
		plan.flags |= x8616_plan_has_modrm;

	if (encoding->post_opcode.mask)
		plan.flags |= x8616_plan_has_post_opcode;

	if (encoding->fields.d.width)   { plan.flags |= x8616_plan_has_d;   plan.d_shift   = encoding->fields.d.shift; }
	if (encoding->fields.w.width)   { plan.flags |= x8616_plan_has_w;   plan.w_shift   = encoding->fields.w.shift; }
	if (encoding->fields.s.width)   { plan.flags |= x8616_plan_has_s;   plan.s_shift   = encoding->fields.s.shift; }
	if (encoding->fields.v.width)   { plan.flags |= x8616_plan_has_v;   plan.v_shift   = encoding->fields.v.shift; }
	if (encoding->fields.z.width)   { plan.flags |= x8616_plan_has_z;   plan.z_shift   = encoding->fields.z.shift; }
	if (encoding->fields.reg.width) { plan.flags |= x8616_plan_has_reg; plan.reg_shift = encoding->fields.reg.shift; }
	if (encoding->fields.sr.width)  { plan.flags |= x8616_plan_has_sr;  plan.sr_shift  = encoding->fields.sr.shift; }

	if (x8616_decode_gen_operand_uses_rm(encoding->operands[0]) || x8616_decode_gen_operand_uses_rm(encoding->operands[1]))
		plan.flags |= x8616_plan_uses_rm;

	if (encoding->flags & x8616_encoding_prefix)
		plan.flags |= x8616_plan_is_prefix;

	X8616_DecodePayload payload_0 = x8616_decode_gen_payload_from_operand(encoding->operands[0]);
	X8616_DecodePayload payload_1 = x8616_decode_gen_payload_from_operand(encoding->operands[1]);

	if (payload_0 && payload_1 && payload_0 != payload_1)
		x8616_info_push(info_scratch, msgs, x8616_info_error, x8616_info_gen_multiple_payloads, encoding_idx, 0, payload_0, payload_1);

	plan.payload = payload_0 ? payload_0 : payload_1;

	U4 payload_max = 0;
	switch (plan.payload) {
		case x8616_payload_imm:        payload_max = 2; break;
		case x8616_payload_imm8:       payload_max = 1; break;
		case x8616_payload_imm16:      payload_max = 2; break;
		case x8616_payload_mem_direct: payload_max = 2; break;
		case x8616_payload_rel8:       payload_max = 1; break;
		case x8616_payload_rel16:      payload_max = 2; break;
		case x8616_payload_far_ptr:    payload_max = 4; break;
		default: break;
	}

	U4 body_max = 1 
	+ ((plan.flags & x8616_plan_has_post_opcode) != 0) 
	+ ((plan.flags & x8616_plan_has_modrm) != 0) 
	+ ((plan.flags & x8616_plan_uses_rm) ? 2 : 0) 
	+ payload_max;

	if (body_max > X8616_DECODE_BODY_CAP) x8616_info_push(info_scratch, msgs, x8616_info_error
		, x8616_info_gen_body_cap_exceeded
		, encoding_idx
		, 0
		, X8616_DECODE_BODY_CAP
		, body_max
	);
	return plan;
}

FI_ B1 x8616_decode_gen_encoding_matches_opcode(X8616_Encoding const* encoding, U1 opcode) {
	return (opcode & encoding->opcode.mask) == encoding->opcode.bits;
}

FI_ B1 x8616_decode_gen_plan_matches_second(X8616_DecodePlan const* plan, U1 byte) {
	if (plan->mod_rm.mask      && ((byte & plan->mod_rm.mask)      != plan->mod_rm.bits))      return false;
	if (plan->post_opcode.mask && ((byte & plan->post_opcode.mask) != plan->post_opcode.bits)) return false;
	return true;
}

internal void x8616_decode_gen_pass_plans(X8616_DecodeGen* gen, FArena_R info_scratch) {
	for (U4 idx = 0; idx < X8616_ENCODING_COUNT; ++ idx)
		gen->plans[idx + 1] = x8616_decode_gen_plan(x8616_encodings + idx, idx, & gen->msgs, info_scratch);
}

internal void
x8616_decode_gen_pass_dispatch(X8616_DecodeGen* gen, FArena_R info_scratch)
{
	for (U4 opcode = 0; opcode < 256; ++ opcode)
	{
		U1 candidates[X8616_ENCODING_COUNT];
		U4 candidate_count = 0;

		for (U4 encoding_idx = 0; encoding_idx < X8616_ENCODING_COUNT; ++ encoding_idx)
			if (x8616_decode_gen_encoding_matches_opcode(x8616_encodings + encoding_idx, C_(U1, opcode)))
				candidates[candidate_count ++] = C_(U1, encoding_idx + 1);

		if (candidate_count == 0) continue;
		if (candidate_count == 1) { gen->dispatch[opcode] = candidates[0]; continue; }

		if (gen->aux_count + 256 > X8616_DECODE_GEN_MAX_AUX) {
			x8616_info_push(info_scratch, & gen->msgs, x8616_info_error
				, x8616_info_gen_aux_cap_exceeded
				, opcode
				, 0
				, X8616_DECODE_GEN_MAX_AUX
				, gen->aux_count + 256
			);
			return;
		}

		U4 base = gen->aux_count;
		gen->dispatch[opcode]        = C_(U2, X8616_DECODE_AUX_BIT | base);
		gen->aux_count              += 256;
		gen->ambiguous_opcode_count += 1;

		for (U4 second = 0; second < 256; ++ second)
		{
			U1 selected = 0;
			for (U4 candidate_idx = 0; candidate_idx < candidate_count; ++ candidate_idx)
			{
				U1 plan_idx = candidates[candidate_idx];
				if (! x8616_decode_gen_plan_matches_second(gen->plans + plan_idx, C_(U1, second)))
					continue;

				if (selected) x8616_info_push(info_scratch, & gen->msgs, x8616_info_error
					, x8616_info_gen_ambiguous_decode
					, (opcode << 8) | second
					, 0
					, selected
					, plan_idx
				);
				selected = plan_idx;
			}

			gen->aux[base + second] = selected;
		}
	}
}

internal void
x8616_decode_gen_pass_validate(X8616_DecodeGen* gen, FArena_R info_scratch)
{
	for (U4 opcode = 0; opcode < 256; ++ opcode)
	for (U4 second = 0; second < 256; ++ second)
	{
		U1 expected = 0; 
		for (U4 encoding_idx = 0; encoding_idx < X8616_ENCODING_COUNT; ++ encoding_idx)
		{
			X8616_Encoding const* encoding = x8616_encodings + encoding_idx;
			if (! x8616_decode_gen_encoding_matches_opcode(encoding, C_(U1, opcode))) continue;

			X8616_DecodePlan const* plan = gen->plans + encoding_idx + 1;
			if (! x8616_decode_gen_plan_matches_second(plan, C_(U1, second))) continue;

			if (expected) x8616_info_push(info_scratch, & gen->msgs, x8616_info_error
				, x8616_info_gen_ambiguous_decode
				, (opcode << 8) | second, 0
				, expected, encoding_idx + 1
			);
			expected = C_(U1, encoding_idx + 1);
		}

		U2 dispatch = gen->dispatch[opcode];
		U1 actual   = 0;

		if (dispatch & X8616_DECODE_AUX_BIT) {
			U2 base = dispatch & X8616_DECODE_AUX_MASK;
			actual = gen->aux[base + second];
		}
		else if (dispatch) {
			U1 candidate = C_(U1, dispatch);
			if (x8616_decode_gen_plan_matches_second(gen->plans + candidate, C_(U1, second)))
				actual = candidate;
		}

		if (actual != expected) x8616_info_push(info_scratch, & gen->msgs, x8616_info_error
			, x8616_info_gen_dispatch_mismatch
			, (opcode << 8) | second, 0
			, expected, actual
		);
		++ gen->verified_count;
	}
}

X8616_DecodeGenInfo
x8616_decode_table_generate(X8616_DecodeGen* gen, FArena_R info_scratch) {
	gen[0] = (X8616_DecodeGen){0};
	x8616_decode_gen_pass_plans   (gen, info_scratch);
	x8616_decode_gen_pass_dispatch(gen, info_scratch);
	x8616_decode_gen_pass_validate(gen, info_scratch);
	X8616_DecodeGenInfo result = {
		.plan_count             = X8616_ENCODING_COUNT + 1,
		.aux_count              = gen->aux_count,
		.ambiguous_opcode_count = gen->ambiguous_opcode_count,
		.verified_count         = gen->verified_count,
		.msgs                   = gen->msgs,
	};
	return result;
}
