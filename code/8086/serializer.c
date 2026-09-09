#ifdef INTELLISENSE_DIRECTIVES
#	include "serializer.h"
#endif

enum {
	X8616_SERIALIZE_HEADER_LEN = 9,
};

RO_ global Str8 x8616_serialize_header = slit8("bits 16\n\n");

RO_ global Str8 x8616_serialize_mnemonic[] = {
	[x8616_op_invalid] = slit8(""),
	[x8616_op_nop]     = slit8("nop"),
	[x8616_op_mov]     = slit8("mov"),
	[x8616_op_add]     = slit8("add"),
	[x8616_op_or]      = slit8("or"),
	[x8616_op_adc]     = slit8("adc"),
	[x8616_op_sbb]     = slit8("sbb"),
	[x8616_op_and]     = slit8("and"),
	[x8616_op_sub]     = slit8("sub"),
	[x8616_op_xor]     = slit8("xor"),
	[x8616_op_cmp]     = slit8("cmp"),
	[x8616_op_push]    = slit8("push"),
	[x8616_op_pop]     = slit8("pop"),
	[x8616_op_xchg]    = slit8("xchg"),
	[x8616_op_in]      = slit8("in"),
	[x8616_op_out]     = slit8("out"),
	[x8616_op_xlat]    = slit8("xlat"),
	[x8616_op_lea]     = slit8("lea"),
	[x8616_op_lds]     = slit8("lds"),
	[x8616_op_les]     = slit8("les"),
	[x8616_op_lahf]    = slit8("lahf"),
	[x8616_op_sahf]    = slit8("sahf"),
	[x8616_op_pushf]   = slit8("pushf"),
	[x8616_op_popf]    = slit8("popf"),
	[x8616_op_inc]     = slit8("inc"),
	[x8616_op_aaa]     = slit8("aaa"),
	[x8616_op_daa]     = slit8("daa"),
	[x8616_op_dec]     = slit8("dec"),
	[x8616_op_neg]     = slit8("neg"),
	[x8616_op_aas]     = slit8("aas"),
	[x8616_op_das]     = slit8("das"),
	[x8616_op_mul]     = slit8("mul"),
	[x8616_op_imul]    = slit8("imul"),
	[x8616_op_aam]     = slit8("aam"),
	[x8616_op_div]     = slit8("div"),
	[x8616_op_idiv]    = slit8("idiv"),
	[x8616_op_aad]     = slit8("aad"),
	[x8616_op_cbw]     = slit8("cbw"),
	[x8616_op_cwd]     = slit8("cwd"),
	[x8616_op_not]     = slit8("not"),
	[x8616_op_shl]     = slit8("shl"),
	[x8616_op_shr]     = slit8("shr"),
	[x8616_op_sar]     = slit8("sar"),
	[x8616_op_rol]     = slit8("rol"),
	[x8616_op_ror]     = slit8("ror"),
	[x8616_op_rcl]     = slit8("rcl"),
	[x8616_op_rcr]     = slit8("rcr"),
	[x8616_op_test]    = slit8("test"),
	[x8616_op_rep]     = slit8(""),
	[x8616_op_movs]    = slit8("movs"),
	[x8616_op_cmps]    = slit8("cmps"),
	[x8616_op_scas]    = slit8("scas"),
	[x8616_op_lods]    = slit8("lods"),
	[x8616_op_stos]    = slit8("stos"),
	[x8616_op_call]    = slit8("call"),
	[x8616_op_jmp]     = slit8("jmp"),
	[x8616_op_ret]     = slit8("ret"),
	[x8616_op_retf]    = slit8("retf"),
	[x8616_op_je]      = slit8("je"),
	[x8616_op_jl]      = slit8("jl"),
	[x8616_op_jle]     = slit8("jle"),
	[x8616_op_jb]      = slit8("jb"),
	[x8616_op_jbe]     = slit8("jbe"),
	[x8616_op_jp]      = slit8("jp"),
	[x8616_op_jo]      = slit8("jo"),
	[x8616_op_js]      = slit8("js"),
	[x8616_op_jne]     = slit8("jne"),
	[x8616_op_jnl]     = slit8("jnl"),
	[x8616_op_jg]      = slit8("jg"),
	[x8616_op_jnb]     = slit8("jnb"),
	[x8616_op_ja]      = slit8("ja"),
	[x8616_op_jnp]     = slit8("jnp"),
	[x8616_op_jno]     = slit8("jno"),
	[x8616_op_jns]     = slit8("jns"),
	[x8616_op_loop]    = slit8("loop"),
	[x8616_op_loopz]   = slit8("loopz"),
	[x8616_op_loopnz]  = slit8("loopnz"),
	[x8616_op_jcxz]    = slit8("jcxz"),
	[x8616_op_int]     = slit8("int"),
	[x8616_op_int3]    = slit8("int3"),
	[x8616_op_into]    = slit8("into"),
	[x8616_op_iret]    = slit8("iret"),
	[x8616_op_clc]     = slit8("clc"),
	[x8616_op_cmc]     = slit8("cmc"),
	[x8616_op_stc]     = slit8("stc"),
	[x8616_op_cld]     = slit8("cld"),
	[x8616_op_std]     = slit8("std"),
	[x8616_op_cli]     = slit8("cli"),
	[x8616_op_sti]     = slit8("sti"),
	[x8616_op_hlt]     = slit8("hlt"),
	[x8616_op_wait]    = slit8("wait"),
	[x8616_op_lock]    = slit8(""),
	[x8616_op_segment] = slit8(""),
};

RO_ global Str8 x8616_serialize_reg8[] = {
	[x8616_al] = slit8("al"),
	[x8616_cl] = slit8("cl"),
	[x8616_dl] = slit8("dl"),
	[x8616_bl] = slit8("bl"),
	[x8616_ah] = slit8("ah"),
	[x8616_ch] = slit8("ch"),
	[x8616_dh] = slit8("dh"),
	[x8616_bh] = slit8("bh"),
};

RO_ global Str8 x8616_serialize_reg16[] = {
	[x8616_ax] = slit8("ax"),
	[x8616_cx] = slit8("cx"),
	[x8616_dx] = slit8("dx"),
	[x8616_bx] = slit8("bx"),
	[x8616_sp] = slit8("sp"),
	[x8616_bp] = slit8("bp"),
	[x8616_si] = slit8("si"),
	[x8616_di] = slit8("di"),
};

RO_ global Str8 x8616_serialize_seg[] = {
	[x8616_es] = slit8("es"),
	[x8616_cs] = slit8("cs"),
	[x8616_ss] = slit8("ss"),
	[x8616_ds] = slit8("ds"),
};

RO_ global Str8 x8616_serialize_ea[] = {
	[x8616_ea_bx_si] = slit8("bx + si"),
	[x8616_ea_bx_di] = slit8("bx + di"),
	[x8616_ea_bp_si] = slit8("bp + si"),
	[x8616_ea_bp_di] = slit8("bp + di"),
	[x8616_ea_si]    = slit8("si"),
	[x8616_ea_di]    = slit8("di"),
	[x8616_ea_bp]    = slit8("bp"),
	[x8616_ea_bx]    = slit8("bx"),
};

typedef Enum_(U1, X8616_SerializeSizeWhere) {
	x8616_serialize_size_none       = 0x00,
	x8616_serialize_size_before_mem = 0x01,
	x8616_serialize_size_before_imm = 0x02,
};

FI_ B4 x8616_serialize_put(Str8Gen_R gen, Str8 piece) {
	if (piece.len > gen->cap - gen->len) return false;
	str8gen_append_str8(gen, piece);     return true;
}

FI_ X8616_SerializeStatus x8616_serialize_put_or_full(Str8Gen_R gen, Str8 piece) {
	X8616_SerializeStatus status = x8616_serialize_ok;
	if (x8616_serialize_put(gen, piece) == 0) status = x8616_serialize_output_full;
	return status;
}

internal B4 x8616_serialize_put_u4(Str8Gen_R gen, U4 value) {
	Info_str8_from_u4 info = str8_from_u4_info(value, 10, 0, 0);
	UTF8 digits[16];
	B4 ok = info.size_required != 0 && info.size_required <= Array_len(digits) && info.size_required <= gen->cap - gen->len;
	if (ok) {
		Str8 text = str8_from_u4_buf(slice_ut(digits, info.size_required), value, 10, 0, 0, info);
		ok = x8616_serialize_put(gen, text);
	}
	return ok;
}

internal B4 x8616_serialize_put_s4(Str8Gen_R gen, S4 value) {
	B4 ok = true;
	if (value < 0) {
		ok = x8616_serialize_put(gen, slit8("-"));
		if (ok) ok = x8616_serialize_put_u4(gen, C_(U4, -value));
	}
	else {
		ok = x8616_serialize_put_u4(gen, C_(U4, value));
	}
	return ok;
}

internal S4 x8616_serialize_s4_from_bits(U2 bits, U1 byte_count) {
	if (byte_count <= 1) {
		U4  value = bits & 0xFF;
		if (value & 0x80) return C_(S4, value) - 256;
		return C_(S4, value);
	}
	U4  value = bits;
	if (value & 0x8000) return C_(S4, value) - 65536;
	return C_(S4, value);
}

FI_ B4 x8616_serialize_imm_is_signed(X8616_Op op) { switch (op) {
	case x8616_op_mov:
	case x8616_op_add:
	case x8616_op_adc:
	case x8616_op_sub:
	case x8616_op_sbb:
	case x8616_op_cmp:
	case x8616_op_ret:
	case x8616_op_retf:
		return 1;
	default:
		return 0;
}}

FI_ U1 x8616_serialize_imm_bytes(X8616_DecodedOperandFlags flags, U1 immediate_bytes, X8616_WidthMode width) {
	if (immediate_bytes)                             return immediate_bytes;
	if (flags & x8616_decoded_operand_sign_extended) return 1;
	if (width == x8616_width_word)                   return 2;
	return 1;
}

internal B4
x8616_serialize_put_imm(Str8Gen_R gen, X8616_DecodedInstruction_R inst, X8616_DecodedOperand_R operand) {
	U1 bytes = x8616_serialize_imm_bytes(operand->flags, operand->immediate_bytes, operand->width);
	if (x8616_serialize_imm_is_signed(inst->op)) {
		return x8616_serialize_put_s4(gen, x8616_serialize_s4_from_bits(operand->immediate, bytes));
	}
	U4 value = operand->immediate;
	if (bytes <= 1) value &= 0xFF;
	return x8616_serialize_put_u4(gen, value);
}

internal X8616_WidthMode
x8616_serialize_mem_width(X8616_DecodedInstruction_R inst) {
	for (U1 id = 0; id < inst->operand_count; ++id) {
		if (inst->operands[id].flags & x8616_decoded_operand_memory) return inst->operands[id].width;
	}
	return inst->width;
}

internal X8616_SerializeSizeWhere
x8616_serialize_size_where(X8616_Op op, U1 operand_count, X8616_DecodedOperand operands[static 2])
{
	B4 has_mem = 0;
	B4 has_imm = 0;
	for (U1 id = 0; id < operand_count; ++id) {
		X8616_DecodedOperandFlags flags = operands[id].flags;
		if (flags & x8616_decoded_operand_memory) has_mem = 1;
		if ((flags & x8616_decoded_operand_immediate) && ((flags & x8616_decoded_operand_register) == 0)) has_imm = 1;
	}
	switch (op) {
	case x8616_op_mov:
		if (has_mem && has_imm) return x8616_serialize_size_before_imm;
		return x8616_serialize_size_none;
	case x8616_op_add:
	case x8616_op_adc:
	case x8616_op_sub:
	case x8616_op_sbb:
	case x8616_op_cmp:
	case x8616_op_and:
	case x8616_op_or:
	case x8616_op_xor:
	case x8616_op_test:
		if (has_mem && has_imm) return x8616_serialize_size_before_mem;
		return x8616_serialize_size_none;
	case x8616_op_not:
	case x8616_op_neg:
	case x8616_op_inc:
	case x8616_op_dec:
	case x8616_op_mul:
	case x8616_op_imul:
	case x8616_op_div:
	case x8616_op_idiv:
	case x8616_op_push:
	case x8616_op_pop:
	case x8616_op_shl:
	case x8616_op_shr:
	case x8616_op_sar:
	case x8616_op_rol:
	case x8616_op_ror:
	case x8616_op_rcl:
	case x8616_op_rcr:
		if (has_mem) return x8616_serialize_size_before_mem;
		return x8616_serialize_size_none;
	default:
		return x8616_serialize_size_none;
	}
}

internal B4 x8616_serialize_put_size(Str8Gen_R gen, X8616_WidthMode width) {
	Str8 name = slit8("");
	if (width == x8616_width_byte) name = slit8("byte");
	if (width == x8616_width_word) name = slit8("word");
	B4 ok = name.len != 0;
	if (ok) ok = x8616_serialize_put(gen, name);
	if (ok) ok = x8616_serialize_put(gen, slit8(" "));
	return ok;
}

internal B4
x8616_serialize_put_memory(Str8Gen_R gen, X8616_DecodedInstruction_R inst, X8616_DecodedOperand_R operand)
{
	B4 ok = true;
	if (inst->prefixes.has_segment) {
		ok = x8616_serialize_put(gen, x8616_serialize_seg[inst->prefixes.segment]);
		if (ok) ok = x8616_serialize_put(gen, slit8(":"));
	}
	if (ok) ok = x8616_serialize_put(gen, slit8("["));
	if (ok && (operand->flags & x8616_decoded_operand_direct)) {
		ok = x8616_serialize_put_u4(gen, operand->address);
	}
	else if (ok) {
		ok = x8616_serialize_put(gen, x8616_serialize_ea[operand->ea]);
		S4 disp = operand->displacement;
		if (ok && disp > 0) {
			ok = x8616_serialize_put(gen, slit8(" + "));
			if (ok) ok = x8616_serialize_put_u4(gen, C_(U4, disp));
		}
		else if (ok && disp < 0) {
			ok = x8616_serialize_put(gen, slit8(" - "));
			if (ok) ok = x8616_serialize_put_u4(gen, C_(U4, -disp));
		}
	}
	if (ok) ok = x8616_serialize_put(gen, slit8("]"));
	return ok;
}

FI_ B4 x8616_serialize_is_string(X8616_Op op) { switch (op) {
	case x8616_op_movs:
	case x8616_op_cmps:
	case x8616_op_scas:
	case x8616_op_lods:
	case x8616_op_stos:
		return 1;
	default:
		return 0;
}}

FI_ B4 x8616_serialize_has_memory(X8616_DecodedInstruction* inst) {
	for (U1 id = 0; id < inst->operand_count; ++id) { if (inst->operands[id].flags & x8616_decoded_operand_memory) return 1; }
	return 0;
}

internal X8616_SerializeStatus
x8616_serialize_put_operand(Str8Gen_R gen, X8616_DecodedInstruction* inst, X8616_DecodedOperand* operand)
{
	X8616_SerializeStatus    status = x8616_serialize_ok;
	X8616_SerializeSizeWhere where  = x8616_serialize_size_where(inst->op, inst->operand_count, inst->operands);
	X8616_WidthMode          width  = x8616_serialize_mem_width(inst);
	B4 is_mem =  (operand->flags & x8616_decoded_operand_memory)    != 0;
	B4 is_imm = ((operand->flags & x8616_decoded_operand_immediate) != 0) && ((operand->flags & x8616_decoded_operand_register) == 0);
	B4 is_reg =  (operand->flags & x8616_decoded_operand_register)  != 0;
	B4 is_seg =  (operand->flags & x8616_decoded_operand_segment)   != 0;
	B4 is_rel =  (operand->flags & x8616_decoded_operand_relative)  != 0;
	B4 is_far =  (operand->flags & x8616_decoded_operand_far_ptr)   != 0;

	if (status == x8616_serialize_ok && where == x8616_serialize_size_before_mem && is_mem) {
		if (x8616_serialize_put_size(gen, width) == 0) status = x8616_serialize_output_full;
	}
	if (status == x8616_serialize_ok && where == x8616_serialize_size_before_imm && is_imm) {
		if (x8616_serialize_put_size(gen, width) == 0) status = x8616_serialize_output_full;
	}
	if (status == x8616_serialize_ok && (inst->flags & x8616_encoding_far) && is_mem) {
		status = x8616_serialize_put_or_full(gen, slit8("far "));
	}

	if (status == x8616_serialize_ok) {
		if (is_reg) {
			U1 index = C_(U1, operand->reg.r16);
			if      (operand->width == x8616_width_byte) status = x8616_serialize_put_or_full(gen, x8616_serialize_reg8 [index]);
			else if (operand->width == x8616_width_word) status = x8616_serialize_put_or_full(gen, x8616_serialize_reg16[index]);
			else                                         status = x8616_serialize_unsupported_form;
		}
		else if (is_seg) {
			status = x8616_serialize_put_or_full(gen, x8616_serialize_seg[operand->segment]);
		}
		else if (is_mem) {
			if (x8616_serialize_put_memory(gen, inst, operand) == 0) status = x8616_serialize_output_full;
		}
		else if (is_imm) {
			if (x8616_serialize_put_imm(gen, inst, operand) == 0) status = x8616_serialize_output_full;
		}
		else if (is_rel) {
			S4 rel = C_(S4, inst->size) + operand->displacement;
			status = x8616_serialize_put_or_full(gen, slit8("$"));
			if (status == x8616_serialize_ok && rel >= 0) status = x8616_serialize_put_or_full(gen, slit8("+"));
			if (status == x8616_serialize_ok && x8616_serialize_put_s4(gen, rel) == 0) status = x8616_serialize_output_full;
		}
		else if (is_far) {
			if (x8616_serialize_put_u4(gen, operand->far_segment) == 0) status = x8616_serialize_output_full;
			if (status == x8616_serialize_ok) status = x8616_serialize_put_or_full(gen, slit8(":"));
			if (status == x8616_serialize_ok && x8616_serialize_put_u4(gen, operand->far_offset) == 0) status = x8616_serialize_output_full;
		}
		else {
			status = x8616_serialize_unsupported_form;
		}
	}
	return status;
}

FI_ X8616_DecodedOperandFlags x8616_serialize_base_flags(X8616_DecodedOperandFlags flags) {
	return flags & (
		x8616_decoded_operand_register
	|	x8616_decoded_operand_segment
	|	x8616_decoded_operand_memory
	|	x8616_decoded_operand_immediate
	|	x8616_decoded_operand_relative
	|	x8616_decoded_operand_far_ptr
	);
}

FI_ B4 x8616_serialize_one_flag(X8616_DecodedOperandFlags flags) { U2 bits = C_(U2, flags); return (bits != 0) && ((bits & (bits - 1)) == 0); }

internal X8616_SerializeStatus
x8616_serialize_validate(X8616_DecodedInstruction_R inst)
{
	X8616_SerializeStatus status = x8616_serialize_ok;

	B4 bad_decode              = inst->decode_flags & (x8616_decode_invalid | x8616_decode_truncated);
	B4 invalid_sig             = inst->size == 0 || inst->operand_count > 2;
	B4 invalid_mnemonic        = u4_(inst->op) >= Array_len(x8616_serialize_mnemonic) || inst->op == x8616_op_invalid;
	B4 invalid_prefix_with_seg = inst->prefixes.has_segment && u1_(inst->prefixes.segment) >= Array_len(x8616_serialize_seg);

	if (bad_decode || invalid_sig || invalid_mnemonic || invalid_prefix_with_seg)  {
		status = x8616_serialize_invalid_record; goto status_failed;
	}
	B4 bad_prefix_segement = inst->prefixes.has_segment && (x8616_serialize_has_memory(inst)    == 0);
	B4 bad_prefix_repeat   = inst->prefixes.has_repeat  && (x8616_serialize_is_string(inst->op) == 0);
	if (bad_prefix_segement || bad_prefix_repeat) { 
		status = x8616_serialize_unsupported_form; goto status_failed; 
	}

	for (U1 id = 0; id < inst->operand_count; ++id)
	{
		X8616_DecodedOperand_R operand = & inst->operands[id];
		X8616_DecodedOperandFlags base = x8616_serialize_base_flags(operand->flags);
		B4 invalid_flag         = x8616_serialize_one_flag(base) == 0;
		B4 has_operand_register = base & x8616_decoded_operand_register;
		B4 has_operand_segment  = base & x8616_decoded_operand_segment;
		B4 has_operand_memory   = base & x8616_decoded_operand_memory;
		B4 invalid_reg_r16      = u1_(operand->reg.r16) >= Array_len(x8616_serialize_reg16);
		B4 invalid_width        = operand->width != x8616_width_byte && operand->width != x8616_width_word;
		B4 invalid_operand_seg  = u1_(operand->segment) >= Array_len(x8616_serialize_seg);
		B4 invalid_operand_ea   = u1_(operand->ea)      >= Array_len(x8616_serialize_ea);
		B4 invalid_direct_ea    = has_operand_memory && ((operand->flags & x8616_decoded_operand_direct) == 0) && invalid_operand_ea;
		B4 invalid_operand      = invalid_flag
			|| (has_operand_register && (invalid_reg_r16 || invalid_width))
			|| (has_operand_segment  && invalid_operand_seg)
			|| invalid_direct_ea;
		if (invalid_operand) {
			status = x8616_serialize_invalid_record; goto status_failed;
		}
	}
status_failed:
	return status;
}

internal X8616_SerializeStatus
x8616_serialize_instruction_line(Str8Gen_R line, X8616_DecodedInstruction* inst)
{
	X8616_SerializeStatus st = x8616_serialize_ok;
	Str8 mnemonic = x8616_serialize_mnemonic[inst->op];
	if (mnemonic.len == 0) st = x8616_serialize_unsupported_form;

	if (st == x8616_serialize_ok && inst->prefixes.lock) {
		st = x8616_serialize_put_or_full(line, slit8("lock "));
	}
	if (st == x8616_serialize_ok && inst->prefixes.has_repeat) {
		if (inst->prefixes.repeat == x8616_rep) st = x8616_serialize_put_or_full(line, slit8("rep "));
		else                                    st = x8616_serialize_put_or_full(line, slit8("repne "));
	}
	if (st == x8616_serialize_ok) st = x8616_serialize_put_or_full(line, mnemonic);

	if (st == x8616_serialize_ok && x8616_serialize_is_string(inst->op)) {
		if      (inst->width == x8616_width_byte) st = x8616_serialize_put_or_full(line, slit8("b"));
		else if (inst->width == x8616_width_word) st = x8616_serialize_put_or_full(line, slit8("w"));
		else                                      st = x8616_serialize_unsupported_form;
	}

	if (st == x8616_serialize_ok && inst->operand_count) {
		st = x8616_serialize_put_or_full(line, slit8(" "));
		for (U1 id = 0; st == x8616_serialize_ok && id < inst->operand_count; ++id) {
			if (id) st = x8616_serialize_put_or_full(line, slit8(", "));
			if (st == x8616_serialize_ok) st = x8616_serialize_put_operand(line, inst, & inst->operands[id]);
		}
	}
	if (st == x8616_serialize_ok) st = x8616_serialize_put_or_full(line, slit8("\n"));
	return st;
}

X8616_SerializeInfo
x8616_serialize_instructions(X8616_SerializeRequest request)
{
	X8616_SerializeInfo result = {0}; result.text.ptr = C_(UTF8*, request.output.ptr);
	B4 invalid_record =
	   (request.instruction_count && request.instructions == 0)
	|| (request.output.len        && request.output.ptr   == 0)
	|| (request.scratch.len       && request.scratch.ptr  == 0);
	if (invalid_record)                                  { result.status = x8616_serialize_invalid_record; goto exit; }
	if (request.output.len < X8616_SERIALIZE_HEADER_LEN) { result.status = x8616_serialize_output_full;    goto exit; }

	Str8Gen gen = str8gen_make(request.output);
	if (x8616_serialize_header.len > gen.cap - gen.len)  { result.status = x8616_serialize_output_full; goto exit; }
	str8gen_append_str8(& gen, x8616_serialize_header);
	result.text.len = gen.len;

	for (U4 id = 0; id < request.instruction_count; ++id) {
		X8616_SerializeStatus line_status = x8616_serialize_validate(& request.instructions[id]);
		if (line_status != x8616_serialize_ok) { result.status = line_status; goto exit; }

		Str8Gen line = str8gen_make(request.scratch);
		line_status  = x8616_serialize_instruction_line(& line, & request.instructions[id]);
		if (line_status != x8616_serialize_ok) { result.status = line_status; goto exit; }

		Str8 text = str8(line.ptr, line.len);
		if (text.len > gen.cap - gen.len) { result.status = x8616_serialize_output_full; goto exit; }
		str8gen_append_str8(& gen, text);
		result.text.len              = gen.len;
		result.instructions_written += 1;
	}

	result.status = x8616_serialize_ok;
exit:
	return result;
}
