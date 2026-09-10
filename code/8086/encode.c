#ifdef INTELLISENSE_DIRECTIVES
#	include "encode.h"
#	include "encoder_table.h"
#endif

internal void
x8616_encode_push(FArena_R arena, X8616_InfoList_R msgs, X8616_EncodeStatus status, U4 id, U2 size, U4 expected, U4 actual) {
	if (status == x8616_encode_ok || arena == 0) return;
	X8616_InfoKind kind = x8616_info_error;
	X8616_InfoCode code = x8616_info_encode_invalid_record;
	     if (status == x8616_encode_output_full)  code = x8616_info_encode_output_full;
	else if (status == x8616_encode_bad_request)  code = x8616_info_encode_bad_request;
	x8616_info_push(arena, msgs, kind, code, id, size, expected, actual);
}

FI_ void x8616_encode_buf_u1(U1_R buf, U1_R n, U1 value) { buf[n[0]] = value; n[0] += 1; }
FI_ void x8616_encode_buf_u2(U1_R buf, U1_R n, U2 value) {
	x8616_encode_buf_u1(buf, n, u1_(value));
	x8616_encode_buf_u1(buf, n, u1_(value >> 8));
}
FI_ void x8616_encode_buf_disp(U1_R buf, U1_R n, S2 value, U1 bytes) {
	if (bytes == 1) x8616_encode_buf_u1(buf, n, u1_(value));
	if (bytes == 2) x8616_encode_buf_u2(buf, n, u2_(value));
}

I_ B4 x8616_encode_is_alu     (X8616_Op op) { return op >= x8616_op_add && op <= x8616_op_cmp; }
I_ B4 x8616_encode_is_shift   (X8616_Op op) { return op >= x8616_op_shl && op <= x8616_op_rcr; }
I_ B4 x8616_encode_is_jcc     (X8616_Op op) { return op >= x8616_op_je  && op <= x8616_op_jns; }
I_ U1 x8616_encode_alu_from_op(X8616_Op op) { return u1_(op - x8616_op_add); }
I_ U1 x8616_encode_cc_from_op (X8616_Op op) {
	for (U1 i = 0; i < Array_len(x8616_op_from_cc); ++i) if (x8616_op_from_cc[i] == op) return i;
	return 0;
}
I_ U1 x8616_encode_shift_from_op(X8616_Op op) {
	for (U1 i = 0; i < Array_len(x8616_op_from_shift); ++i) if (x8616_op_from_shift[i] == op) return i;
	return 0;
}
I_ void x8616_encode_or_field(U1_R header, X8616_BitField f, U1 value) {
	header[0] |= (value << f.shift) & x8616_field_mask(f.shift, f.width);
}

FI_ U1 x8616_encode_prefix_count(X8616_DecodedInstruction_R inst) {
	return u1_(inst->prefixes.lock) + u1_(inst->prefixes.has_repeat) + u1_(inst->prefixes.has_segment);
}

FI_ B4 x8616_encode_is_acc(X8616_DecodedOperand_R op) {
	if ((op->flags & x8616_decoded_operand_register) == 0) return 0;
	if  (op->width == x8616_width_byte)                    return op->reg.r8  == x8616_al;
	                                                       return op->reg.r16 == x8616_ax;
}

RO_ global U2 x8616_encode_slot_any[] = {
	[x8616_operand_none]           = 0,
	[x8616_operand_rm]             = x8616_decoded_operand_register | x8616_decoded_operand_memory,
	[x8616_operand_reg_modrm]      = x8616_decoded_operand_register,
	[x8616_operand_reg_opcode]     = x8616_decoded_operand_register,
	[x8616_operand_segment_modrm]  = x8616_decoded_operand_segment,
	[x8616_operand_segment_opcode] = x8616_decoded_operand_segment,
	[x8616_operand_acc]            = x8616_decoded_operand_register,
	[x8616_operand_imm]            = x8616_decoded_operand_immediate,
	[x8616_operand_imm8]           = x8616_decoded_operand_immediate,
	[x8616_operand_imm16]          = x8616_decoded_operand_immediate,
	[x8616_operand_mem_direct]     = x8616_decoded_operand_memory,
	[x8616_operand_rel8]           = x8616_decoded_operand_relative,
	[x8616_operand_rel16]          = x8616_decoded_operand_relative,
	[x8616_operand_far_ptr]        = x8616_decoded_operand_far_ptr,
	[x8616_operand_dx]             = x8616_decoded_operand_register,
	[x8616_operand_shift_count]    = x8616_decoded_operand_register | x8616_decoded_operand_immediate,
};

I_ B4 x8616_encode_slot_match(X8616_Operand slot, X8616_DecodedOperand_R op, X8616_DecodedInstruction_R inst) {
	B4 ok = (op->flags & x8616_encode_slot_any[slot]) != 0;
	ok &= (slot != x8616_operand_acc)         || x8616_encode_is_acc(op);
	ok &= (slot != x8616_operand_dx)          || (op->reg.r16 == x8616_dx);
	ok &= (slot != x8616_operand_imm8)        || (op->immediate <= 0xFF);
	ok &= (slot != x8616_operand_mem_direct)  || ((op->flags & x8616_decoded_operand_direct) != 0);
	ok &= (slot != x8616_operand_shift_count) || ((op->flags & x8616_decoded_operand_register) && op->reg.r8 == x8616_cl) || (op->immediate == 1);
	if (slot == x8616_operand_rel8 && ok) {
		S2 machine = s2_(op->displacement - s2_(x8616_encode_prefix_count(inst) + 2));
		ok         = machine >= -128 && machine <= 127;
	}
	return ok;
}

FI_ U1 x8616_encode_enc_count(X8616_Encoding_R enc) { return u1_(enc->operands[0] != x8616_operand_none) + u1_(enc->operands[1] != x8616_operand_none); }

I_ B4 x8616_encode_op_match(X8616_Encoding_R enc, X8616_Op op) {
	B4 prefix = (enc->flags & x8616_encoding_prefix) != 0;
	B4 exact  = enc->op == op;
	B4 family = enc->op == x8616_op_invalid;
	B4 alu    = family && (enc->fields.alu.width || enc->digit_kind == x8616_digit_alu) && x8616_encode_is_alu(op);
	B4 cc     = family && enc->fields.cc.width && x8616_encode_is_jcc(op);
	B4 shift  = family && enc->digit_kind == x8616_digit_shift && x8616_encode_is_shift(op);
	return prefix == 0 && (exact || alu || cc || shift);
}

I_ B4 x8616_encode_row_match(X8616_Encoding_R enc, X8616_DecodedInstruction_R inst, U1 d) {
	B4 op_ok     = x8616_encode_op_match(enc, inst->op);
	B4 enc_far   = (enc->flags  & x8616_encoding_far) != 0;
	B4 inst_far  = (inst->flags & x8616_encoding_far) != 0;
	inst_far    |= (inst->operands[0].flags & x8616_decoded_operand_far_ptr) != 0;
	inst_far    |= (inst->operands[1].flags & x8616_decoded_operand_far_ptr) != 0;
	B4 far_ok    = enc_far == inst_far;
	B4 width_any = enc->width == x8616_width_dynamic || inst->width == x8616_width_dynamic;
	B4 width_ok  = width_any || enc->width == inst->width;
	B4 count_ok  = x8616_encode_enc_count(enc) == inst->operand_count;
	B4 d_ok      = enc->fields.d.width || d == 0;
	B4 slots_ok  = 1;
	for (U1 i = 0; i < inst->operand_count; ++i) {
		U1 slot_i = d ? (i ^ 1) : i;
		slots_ok &= x8616_encode_slot_match(enc->operands[slot_i], inst->operands + i, inst);
	}
	return op_ok && far_ok && width_ok && count_ok && d_ok && slots_ok;
}

FI_ U1 x8616_encode_row_score(X8616_Encoding const* enc) {
	U1 score = 0;
	for (U1 i = 0; i < 2; ++i) {
		X8616_Operand s = enc->operands[i];
		if (s == x8616_operand_reg_opcode || s == x8616_operand_acc || s == x8616_operand_mem_direct) score += 2;
		if (s == x8616_operand_rel8       || s == x8616_operand_imm8) score += 1;
	}
	return score;
}

I_ B4 x8616_encode_slot_modrm(X8616_Operand s) { return s == x8616_operand_rm || s == x8616_operand_reg_modrm || s == x8616_operand_segment_modrm; }
I_ B4 x8616_encode_needs_modrm(X8616_Encoding const* enc) {
	B4 masked = enc->mod_rm.mask != 0;
	B4 digit  = enc->digit_kind != x8616_digit_none;
	B4 slots  = x8616_encode_slot_modrm(enc->operands[0]) || x8616_encode_slot_modrm(enc->operands[1]);
	return masked || digit || slots;
}

internal B4
x8616_encode_assemble(X8616_DecodedInstruction_R inst)
{
	X8616_Encoding const* best = 0;
	U1 best_d     = 0;
	U1 best_score = 0;
	B4 found      = 0;
	for (U4 id = 0; id < Array_len(x8616_encodings); ++id)
	{
		X8616_Encoding_R enc = x8616_encodings + id;
		U1 d_hi = enc->fields.d.width ? 1 : 0;
		for (U1 d = 0; d <= d_hi; ++d) {
			if        (x8616_encode_row_match(enc, inst, d) == 0) continue;
			U1 score = x8616_encode_row_score(enc);
			B4  not_bested = found && (score < best_score || (score == best_score && d < best_d));
			if (not_bested) continue;
			best = enc; best_d = d; best_score = score; found = 1;
		}
	}
	if (found == 0) return 0;

	X8616_DecodedOperand_R slot[X8616_OPERAND_SOURCE_COUNT] = {0};
	for (U1 op_id = 0; op_id < inst->operand_count; ++op_id) {
		U1 slot_id = best_d ? (op_id ^ 1) : op_id;
		slot[best->operands[slot_id]] = inst->operands + op_id;
	}

	X8616_DecodedOperand_R operand_acc = slot[x8616_operand_acc];
	X8616_Width      w = inst->width        == x8616_width_word ? x8616_w_word : x8616_w_byte;
	if (operand_acc) w = operand_acc->width == x8616_width_word ? x8616_w_word : x8616_w_byte;

	U1 header = best->header.bits;
	X8616_DecodedOperand_R sc  = slot[x8616_operand_shift_count];
	X8616_DecodedOperand_R imm = slot[x8616_operand_imm];
	if (imm == 0) imm = slot[x8616_operand_imm8];
	if (imm == 0) imm = slot[x8616_operand_imm16];
	U1 s = 0;
	if (best->fields.s.width && imm) {
		s = (inst->width == x8616_width_word && imm->immediate_bytes <= 1) || (imm->flags & x8616_decoded_operand_sign_extended);
	}
	U1 v = sc && (sc->flags & x8616_decoded_operand_register);
	U1 reg_op = slot[x8616_operand_reg_opcode]     ? u1_(slot[x8616_operand_reg_opcode    ]->reg.r16) : 0;
	U1 sr_op  = slot[x8616_operand_segment_opcode] ? u1_(slot[x8616_operand_segment_opcode]->segment) : 0;
	x8616_encode_or_field(& header, best->fields.w,   u1_(w));
	x8616_encode_or_field(& header, best->fields.d,   best_d);
	x8616_encode_or_field(& header, best->fields.alu, x8616_encode_alu_from_op(inst->op));
	x8616_encode_or_field(& header, best->fields.cc,  x8616_encode_cc_from_op(inst->op));
	x8616_encode_or_field(& header, best->fields.reg, reg_op);
	x8616_encode_or_field(& header, best->fields.sr,  sr_op);
	x8616_encode_or_field(& header, best->fields.v,   v);
	x8616_encode_or_field(& header, best->fields.s,   s);
	header = (header & ~best->header.mask) | (best->header.bits & best->header.mask);
	if (sc && (sc->flags & x8616_decoded_operand_immediate) && sc->immediate == 1) sc->immediate_bytes = 0;

	U1 modrm     = 0;
	B4 has_modrm = x8616_encode_needs_modrm(best);
	X8616_DecodedOperand_R rm_op = slot[x8616_operand_rm];
	U1 mod = x8616_mod_reg;
	U1 rm  = 0;
	if (rm_op) {
		B4 mem = (rm_op->flags & x8616_decoded_operand_memory) != 0;
		B4 dir = (rm_op->flags & x8616_decoded_operand_direct) != 0;
		B4 bp0 = mem && dir == 0 && rm_op->displacement_bytes == 0 && rm_op->ea == x8616_ea_bp;
		if (bp0) { rm_op->displacement_bytes = 1; rm_op->displacement = 0; }
		U1 db = rm_op->displacement_bytes;
		mod = mem ? (dir ? x8616_mod_mem : (db > 2 ? 2 : db))    : x8616_mod_reg;
		rm  = dir ? u1_(x8616_ea_direct) : (mem ? u1_(rm_op->ea) : u1_(rm_op->reg.r16));
	}
	U1 kind_digit[3] = { 0, x8616_encode_alu_from_op(inst->op), x8616_encode_shift_from_op(inst->op) };
	U1 reg           = slot[x8616_operand_reg_modrm] ? u1_(slot[x8616_operand_reg_modrm]->reg.r16) : 0;
	if (best->digit_kind) reg = kind_digit[best->digit_kind];
	X8616_DecodedOperand_R sr_m = slot[x8616_operand_segment_modrm];
	modrm = sr_m ? x8616_enc_modrm_seg(mod, sr_m->segment, rm) : x8616_enc_modrm(mod, reg, rm);
	modrm = (modrm & ~best->mod_rm.mask) | (best->mod_rm.bits & best->mod_rm.mask);

	X8616_DecodedOperand_R rel = slot[x8616_operand_rel8];
	U1 rel_bytes = 1;
	if (rel == 0) { rel = slot[x8616_operand_rel16]; rel_bytes = 2; }
	if (imm) {
		U1 imm16 = slot[x8616_operand_imm16] != 0;
		U1 imm8  = slot[x8616_operand_imm8]  != 0;
		imm->immediate_bytes = imm16 ? 2 : (imm8 || w == x8616_w_byte || s ? 1 : 2);
	}
	if (rel) {
		U1 size = x8616_encode_prefix_count(inst) + 1 + rel_bytes;
		rel->displacement       = s2_(rel->displacement - s2_(size));
		rel->displacement_bytes = rel_bytes;
	}

	inst->header      = header;
	inst->mod_rm      = modrm;
	inst->has_mod_rm  = has_modrm;
	inst->post_opcode = best->post_opcode.mask ? best->post_opcode.bits : 0;
	return 1;
}

internal X8616_EncodeStatus
x8616_encode_instruction(Str8Gen_R gen, X8616_DecodedInstruction_R inst)
{
	if (inst->decode_flags & (x8616_decode_invalid | x8616_decode_truncated)) return x8616_encode_invalid_record;
	X8616_DecodedInstruction local = *inst;
	if (local.size == 0) {
		if (x8616_encode_assemble(& local) == 0) return x8616_encode_invalid_record;
	}

	U1 buf[16];
	U1 n   = 0;
	U1 rep = local.prefixes.repeat == x8616_rep ? x8616_rep_prefix() : x8616_repne_prefix();
	if (local.prefixes.lock)        { x8616_encode_buf_u1(buf, & n, x8616_lock_prefix()); }
	if (local.prefixes.has_repeat)  { x8616_encode_buf_u1(buf, & n, rep); }
	if (local.prefixes.has_segment) { x8616_encode_buf_u1(buf, & n, x8616_segment_prefix(local.prefixes.segment)); }
	                                  x8616_encode_buf_u1(buf, & n, local.header);
	if (local.post_opcode)          { x8616_encode_buf_u1(buf, & n, local.post_opcode); }
	if (local.has_mod_rm)           { x8616_encode_buf_u1(buf, & n, local.mod_rm); }

	for (U1 id = 0; id < local.operand_count; ++id) {
		X8616_DecodedOperand_R op = local.operands + id;
		if ((op->flags & x8616_decoded_operand_memory) == 0) continue;
		if ( op->flags & x8616_decoded_operand_direct) x8616_encode_buf_u2(buf, & n, op->address);
		else x8616_encode_buf_disp(buf, & n, op->displacement, op->displacement_bytes);
	}
	for (U1 id = 0; id < local.operand_count; ++id) {
		X8616_DecodedOperand_R op = local.operands + id;
		B4 op_rel =  op->flags & x8616_decoded_operand_relative;
		B4 op_imm = (op->flags & x8616_decoded_operand_immediate) && op->immediate_bytes;
		B4 op_far =  op->flags & x8616_decoded_operand_far_ptr;
		     if (op_rel) { x8616_encode_buf_disp(buf, & n, op->displacement,   op->displacement_bytes); }
		else if (op_imm) { x8616_encode_buf_disp(buf, & n, s2_(op->immediate), op->immediate_bytes); }
		else if (op_far) {
			x8616_encode_buf_u2(buf, & n, op->far_offset);
			x8616_encode_buf_u2(buf, & n, op->far_segment);
		}
	}

	if (n > gen->cap - gen->len) return x8616_encode_output_full;
	mem_copy(u8_(gen->ptr + gen->len), u8_(buf), n);
	gen->len += n;
	return x8616_encode_ok;
}

X8616_EncodeInfo x8616_encode_instructions(X8616_EncodeRequest request)
{
	X8616_EncodeInfo result = {0};
	X8616_InfoList   local  = {0};
	X8616_InfoList_R msgs   = request.msgs ? request.msgs : & local;
	Str8Gen gen = {0};

	B4 bad =
	   (request.instruction_count && request.instructions == 0)
	|| (request.output.len        && request.output.ptr   == 0)
	|| (request.info_arena        == 0);
	if (bad) {
		if (request.info_arena) {
			x8616_encode_push(request.info_arena, msgs, x8616_encode_bad_request, 0, 0, 0, 0);
		}
		goto exit;
	}

	gen              = str8gen_make(request.output);
	result.bytes.ptr = u8_(gen.ptr);

	for (U4 id = 0; id < request.instruction_count; ++id)
	{
		X8616_DecodedInstruction_R inst = & request.instructions[id];
		U8 at = gen.len;
		X8616_EncodeStatus st = x8616_encode_instruction(& gen, inst);
		if (st == x8616_encode_invalid_record) {
			gen.len = at;
			x8616_encode_push(request.info_arena, msgs, st, id, inst->size, 0, u4_(inst->op));
			continue;
		}
		if (st == x8616_encode_output_full) {
			gen.len = at;
			x8616_encode_push(request.info_arena, msgs, st
				, id, inst->size, u4_(request.output.len), u4_(at));
			goto exit;
		}
		result.instructions_written += 1;
	}

exit:
	result.bytes.len = gen.len;
	result.msgs      = *msgs;
	return result;
}
