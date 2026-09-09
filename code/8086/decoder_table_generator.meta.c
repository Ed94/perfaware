#include "duffle/dsl.h"
#include "duffle/asm.h"
#include "duffle/analysis.h"
#include "duffle/math.h"
#include "duffle/encoding.h"
#include "duffle/memory.h"
#include "duffle/hashing.h"
#include "duffle/tables.h"
#include "duffle/text.h"
#include "duffle/files.h"
#include "duffle/win32.h"

#include "info.h"
#include "encoder.h"
#include "encoder_table.h"
#include "decoder.h"

enum {
	X8616_DECODE_GEN_MAX_AUX = X8616_DECODE_AUX_MASK + 1,
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

FI_ B4 x8616_decode_gen_operand_uses_modrm(X8616_Operand operand) { 
	return operand == x8616_operand_rm || operand == x8616_operand_reg_modrm || operand == x8616_operand_segment_modrm; 
}

FI_ B4 x8616_decode_gen_operand_uses_rm(X8616_Operand operand) { return operand == x8616_operand_rm; }

internal X8616_DecodePayload
x8616_decode_gen_payload_from_operand(X8616_Operand operand) { switch (operand) {
	case x8616_operand_imm:        return x8616_payload_imm;
	case x8616_operand_imm8:       return x8616_payload_imm8;
	case x8616_operand_imm16:      return x8616_payload_imm16;
	case x8616_operand_mem_direct: return x8616_payload_mem_direct;
	case x8616_operand_rel8:       return x8616_payload_rel8;
	case x8616_operand_rel16:      return x8616_payload_rel16;
	case x8616_operand_far_ptr:    return x8616_payload_far_ptr;
	default:                       return x8616_payload_none;
}}

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
x8616_decode_gen_plan(X8616_Encoding_R encoding, U4 encoding_idx, X8616_InfoList_R msgs, FArena_R info_scratch)
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

FI_ B4 x8616_decode_gen_encoding_matches_opcode(X8616_Encoding_R encoding, U1 opcode) {
	return (opcode & encoding->opcode.mask) == encoding->opcode.bits;
}

FI_ B4 x8616_decode_gen_plan_matches_second(X8616_DecodePlan_R plan, U1 byte) {
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
x8616_decode_gen_pass_validate(X8616_DecodeGen_R gen, FArena_R info_scratch)
{
	for (U4 opcode = 0; opcode < 256; ++ opcode)
	for (U4 second = 0; second < 256; ++ second)
	{
		U1 expected = 0; 
		for (U4 encoding_idx = 0; encoding_idx < X8616_ENCODING_COUNT; ++ encoding_idx)
		{
			X8616_Encoding_R encoding = x8616_encodings + encoding_idx;
			if (x8616_decode_gen_encoding_matches_opcode(encoding, C_(U1, opcode)) == false) continue;

			X8616_DecodePlan_R plan = gen->plans + encoding_idx + 1;
			if (x8616_decode_gen_plan_matches_second(plan, C_(U1, second)) == false) continue;

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
			actual  = gen->aux[base + second];
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
	mem_zero_struct(gen[0]);
	// gen[0] = (X8616_DecodeGen){0};
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


#define X8616_DECODE_TABLE_OUTPUT "./code/8086/gen/decoder_table.h"

enum {
	INFO_MEMORY_SIZE = kilo(64),
	TEXT_MEMORY_SIZE = kilo(128),
	FILE_MEMORY_SIZE = kilo(4),
};

typedef FStack_(FStack_64k, U1, kilo(64));
typedef Struct_(SMemory) {
	U1 info[INFO_MEMORY_SIZE];
	U1 text[TEXT_MEMORY_SIZE];
	U1 file[FILE_MEMORY_SIZE];

	FStack_64k scratch;

	X8616_DecodeGen gen;
};
global SMemory smem;

I_ void str8gen_append_u4(Str8Gen_R out, U4 value, U4 radix, U4 min_digits) {
	UTF8 buffer[64]; Info_str8_from_u4 info = str8_from_u4_info(value, radix, min_digits, 0);
	Str8 text = str8_from_u4_buf(slice_ut_arr(buffer), value, radix, min_digits, 0, info);
	str8gen_append_str8(out, text);
}

I_ void str8gen_append_hex_u1(Str8Gen_R out, U1 value) { str8gen_append_u4(out, value, 16, 2); }
I_ void str8gen_append_hex_u2(Str8Gen_R out, U2 value) { str8gen_append_u4(out, value, 16, 4); }
I_ void str8gen_append_dec   (Str8Gen_R out, U4 value) { str8gen_append_u4(out, value, 10, 1); }

FI_ Slice scratch_push(U8 len) { return fstack_push_(smem.scratch, len); }

typedef Opt_(str8_from_u4) { U4 radix, min_digits, digit_group_separator; }; 
I_ Str8 str8_from_u4_opt(U4 num, Opt_str8_from_u4 o) { if (o.radix == 0) {o.radix = 10;} 
/*gather info*/Info_str8_from_u4 info = str8_from_u4_info(num, o.radix, o.min_digits, o.digit_group_separator);
/*write buf  */return str8_from_u4_buf(scratch_push(128), num, o.radix, o.min_digits, o.digit_group_separator, info);
}
#define str8_from_u4(num, ...) str8_from_u4_opt(num, opt_(str8_from_u4, __VA_ARGS__))

#define code_str8(...) slit8(stringify(__VA_ARGS__))

#define dec(v)        str8_from_u4(v, .radix = 10, .min_digits = 1)
#define hex_u1(v)     str8_from_u4(v, .radix = 16, .min_digits = 2)
#define hex_u2(v)     str8_from_u4(v, .radix = 16, .min_digits = 4)
#define entry(k,v)    { ktl_str8_key(k), v }

internal void
x8616_decode_gen_emit_plan(Str8Gen_R out, X8616_DecodePlan_R plan) { defer_rewind(smem.scratch.top) {
	Str8 template = code_str8(
		\t{ 
			<flags>, <op>, <encoding_flags>, <width>,
			{<operands[0]>, <operands[1]>}, <operand_count>, 
			<payload>, <prefix_kind>,
			<d_shift>, <w_shift>, <s_shift>, <v_shift>, <z_shift>, <reg_shift>, <sr_shift>,
			{<mod_rm.bits>, <mod_rm.mask>},
			{<post_opcode.bits>, <post_opcode.mask>}, 
		},\n
	);
	KTL_Slot_Str8 tbl[] = {
		entry("flags",            hex_u2(plan->flags)),
		entry("op",               hex_u1(plan->op)),
		entry("encoding_flags",   hex_u1(plan->encoding_flags)),
		entry("width",            hex_u1(plan->width)),
		entry("operands[0]",      hex_u1(plan->operands[0])),
		entry("operands[1]",      hex_u1(plan->operands[1])),
		entry("operand_count",    dec(plan->operand_count)),
		entry("payload",          dec(plan->payload)),
		entry("prefix_kind",      dec(plan->prefix_kind)),
		entry("d_shift",          dec(plan->d_shift)),
		entry("w_shift",          dec(plan->w_shift)),
		entry("s_shift",          dec(plan->s_shift)),
		entry("v_shift",          dec(plan->v_shift)),
		entry("z_shift",          dec(plan->z_shift)),
		entry("reg_shift",        dec(plan->reg_shift)),
		entry("sr_shift",         dec(plan->sr_shift)),
		entry("mod_rm.bits",      hex_u1(plan->mod_rm.bits)),
		entry("mod_rm.mask",      hex_u1(plan->mod_rm.mask)),
		entry("post_opcode.bits", hex_u1(plan->post_opcode.bits)),
		entry("post_opcode.mask", hex_u1(plan->post_opcode.mask)),
	}; 
	str8gen_append_fmt(out, template, ktl_str8_from_arr(tbl));
}}

#define gen_fmt(out, tmpl, ...) str8gen_append_fmt((out), (tmpl), ktl_str8_from_arr(((KTL_Slot_Str8[]){ __VA_ARGS__ })))

internal Str8
x8616_decode_gen_emit(Str8Gen_R out, X8616_DecodeGen_R gen)
{
#pragma push_macro("RO_")
#pragma push_macro("global")
#undef RO_
#undef global
	defer_rewind(smem.scratch.top)
	{
		str8gen_append_str8(out, slit8(
			"// Generated from encoder_table.h. Do not hand-edit.\n"
			"// Plan 0 is a nil/invalid entry.\n"
			"#ifdef INTELLISENSE_DIRECTIVES\n"
			"#\tinclude \"dsl.h\"\n"
			"#endif\n\n"
		));
		gen_fmt(out, code_str8(RO_ global X8616_DecodePlan x8616_decode_plans[<plan_count>] =\n{\n)
			, entry("plan_count", dec(X8616_ENCODING_COUNT + 1))
		);

		for (U4 idx = 0; idx < X8616_ENCODING_COUNT + 1; ++ idx) { x8616_decode_gen_emit_plan(out, gen->plans + idx); }

		str8gen_append_str8(out, code_str8(
			};\n\n
		));

		str8gen_append_str8(out, code_str8(RO_ global U2 x8616_decode_dispatch[256] =\n{\n));
		{
			Str8 line = code_str8(\t<e0>, <e1>, <e2>, <e3>, <e4>, <e5>, <e6>, <e7>, <e8>, <e9>, <e10>, <e11>, <e12>, <e13>, <e14>, <e15>,\n);
			for (U4 idx = 0; idx < 256; idx += 16) defer_rewind(smem.scratch.top) {
				U2_R d = gen->dispatch + idx;
				gen_fmt(out, line,
					entry("e0",  hex_u2(d[0])),
					entry("e1",  hex_u2(d[1])),
					entry("e2",  hex_u2(d[2])),
					entry("e3",  hex_u2(d[3])),
					entry("e4",  hex_u2(d[4])),
					entry("e5",  hex_u2(d[5])),
					entry("e6",  hex_u2(d[6])),
					entry("e7",  hex_u2(d[7])),
					entry("e8",  hex_u2(d[8])),
					entry("e9",  hex_u2(d[9])),
					entry("e10", hex_u2(d[10])),
					entry("e11", hex_u2(d[11])),
					entry("e12", hex_u2(d[12])),
					entry("e13", hex_u2(d[13])),
					entry("e14", hex_u2(d[14])),
					entry("e15", hex_u2(d[15]))
				);
			}
		}
		str8gen_append_str8(out, code_str8(};\n\n));

		gen_fmt(out, code_str8(RO_ global U1 x8616_decode_aux[<aux_count>] =\n{\n)
			, entry("aux_count", dec(gen->aux_count))
		);
		{
			Str8 line = code_str8(\t<a0>, <a1>, <a2>, <a3>, <a4>, <a5>, <a6>, <a7>, <a8>, <a9>, <a10>, <a11>, <a12>, <a13>, <a14>, <a15>,\n);
			U4 idx = 0;
			for (; idx + 16 <= gen->aux_count; idx += 16) defer_rewind(smem.scratch.top) {
				U1_R a = gen->aux + idx;
				gen_fmt(out, line,
					entry("a0",  hex_u1(a[0])),
					entry("a1",  hex_u1(a[1])),
					entry("a2",  hex_u1(a[2])),
					entry("a3",  hex_u1(a[3])),
					entry("a4",  hex_u1(a[4])),
					entry("a5",  hex_u1(a[5])),
					entry("a6",  hex_u1(a[6])),
					entry("a7",  hex_u1(a[7])),
					entry("a8",  hex_u1(a[8])),
					entry("a9",  hex_u1(a[9])),
					entry("a10", hex_u1(a[10])),
					entry("a11", hex_u1(a[11])),
					entry("a12", hex_u1(a[12])),
					entry("a13", hex_u1(a[13])),
					entry("a14", hex_u1(a[14])),
					entry("a15", hex_u1(a[15]))
				);
			}
			if (idx < gen->aux_count) {
				str8gen_append_str8(out, slit8("\t"));
				for (; idx < gen->aux_count; ++ idx) {
					str8gen_append_hex_u1(out, gen->aux[idx]); str8gen_append_str8(out, slit8(", "));
				}
				str8gen_append_str8(out, slit8("\n"));
			}
		}
		str8gen_append_str8(out, code_str8(};\n\n));

		gen_fmt(out, code_str8(enum {\n
			\tX8616_DECODE_PLAN_COUNT = <plan_count>,\n
			\tX8616_DECODE_AUX_COUNT  = <aux_count>,
			\n};\n
		),
			entry("plan_count", dec(X8616_ENCODING_COUNT + 1)),
			entry("aux_count",  dec(gen->aux_count))
		);
	}
	return str8(out->ptr, out->len);
#pragma pop_macro("RO_")
#pragma pop_macro("global")
}

#undef gen_fmt
#undef entry
#undef hex_u2
#undef hex_u1
#undef dec

int main(void) {
	FArena info_scratch = farena_make(slice_ut_arr(smem.info));
	X8616_DecodeGenInfo gen_info = x8616_decode_table_generate(& smem.gen, & info_scratch);
	if (gen_info.msgs.error_count) { ms_exit_process(1); return 1; }

	Str8Gen output = str8gen_make(slice_ut_arr(smem.text));
	Str8 generated = x8616_decode_gen_emit(& output, & smem.gen);

	FArena file_scratch = farena_make(slice_ut_arr(smem.file));
	B4 wrote = write_data_to_file_path(slit8(X8616_DECODE_TABLE_OUTPUT), generated, & file_scratch);
	if (wrote == false) { ms_exit_process(2); return 2; }
	if (gen_info.verified_count != 256 * 256) { ms_exit_process(3); return 3; }

	ms_exit_process(0);
	return 0;
}
