#ifdef INTELLISENSE_DIRECTIVES
#	include "parser.h"
#	include "serializer_tables.h"
#endif

FI_ void x8616_parse_skip_ws(Str8_R cur) {
	while (cur->len && char_is_space(cur->ptr[0])) { cur->ptr += 1; cur->len -= 1; }
}

FI_ B4 x8616_parse_eat(Str8_R cur, Str8 lit) {
	B4 too_big = cur->len < lit.len;
	B4 miss    = too_big || mem_match(u8_(cur->ptr), u8_(lit.ptr), lit.len) == 0;
	if (miss) return 0;
	cur->ptr += lit.len;
	cur->len -= lit.len;
	return 1;
}

FI_ U4 x8616_parse_lookup(Str8_R cur, Str8* table, U4 table_len) {
	U4 best     = table_len;
	U8 best_len = 0;
	for (U4 i = 0; i < table_len; ++i)
	{
		Str8 lit     = table[i];
		B4   empty   = lit.len == 0;
		B4   too_big = lit.len > cur->len;
		B4   worse   = lit.len <= best_len;
		B4   skip    = empty || too_big || worse;
		B4   miss    = skip || mem_match(u8_(cur->ptr), u8_(lit.ptr), lit.len) == 0;
		if (miss) continue;
		best     = i;
		best_len = lit.len;
	}
	if (best < table_len) {
		cur->ptr += best_len;
		cur->len -= best_len;
	}
	return best;
}

FI_ B4 x8616_parse_number(Str8_R cur, U2_R value, B4_R neg_out) {
	x8616_parse_skip_ws(cur);
	B4 neg = 0;
	if (x8616_parse_eat(cur, slit8("-"))) neg = 1;
	if (cur->len == 0 || char_is_digit(cur->ptr[0], 10) == 0) return 0;
	U8 start = 0;
	while (start < cur->len && char_is_digit(cur->ptr[start], 10)) start += 1;
	U8 mag = u8_from_str8(str8(cur->ptr, start), 10);
	cur->ptr += start;
	cur->len -= start;
	value[0]   = u2_(mag);
	neg_out[0] = neg;
	return 1;
}

internal B4 x8616_parse_memory(Str8_R cur, X8616_DecodedOperand_R op)
{
	B4 ok = 1;
	if (x8616_parse_eat(cur, slit8("[")) == 0) { ok = 0; goto exit; }
	x8616_parse_skip_ws(cur);

	U4 ea_n = Array_len(x8616_serialize_ea);
	U4 ea   = x8616_parse_lookup(cur, x8616_serialize_ea, ea_n);
	if (ea < ea_n)
	{
		op->flags = x8616_decoded_operand_memory;
		op->ea    = C_(X8616_EA, ea);
		x8616_parse_skip_ws(cur);
		if (x8616_parse_eat(cur, slit8("+"))) {
			U2 mag = 0; B4 neg = 0;
			x8616_parse_skip_ws(cur);
			if (x8616_parse_number(cur, & mag, & neg) == 0) { ok = 0; goto exit; }
			op->displacement       = neg ? s2_(-s4_(mag)) : s2_(mag);
			op->displacement_bytes = (u2_(op->displacement) > 0x7F && op->displacement >= 0) || op->displacement < -128 ? 2 : (op->displacement ? 1 : 0);
		}
		else if (x8616_parse_eat(cur, slit8("-"))) {
			U2 mag = 0; B4 dummy = 0;
			x8616_parse_skip_ws(cur);
			if (x8616_parse_number(cur, & mag, & dummy) == 0) { ok = 0; goto exit; }
			op->displacement       = s2_(-s4_(mag));
			op->displacement_bytes = op->displacement < -128 ? 2 : 1;
		}
	}
	else {
		U2 mag = 0; B4 neg = 0;
		if (x8616_parse_number(cur, & mag, & neg) == 0) { ok = 0; goto exit; }
		op->flags   = x8616_decoded_operand_memory | x8616_decoded_operand_direct;
		op->address = mag;
		op->ea      = x8616_ea_direct;
	}
	x8616_parse_skip_ws(cur);
	if (x8616_parse_eat(cur, slit8("]")) == 0) { ok = 0; goto exit; }
exit:
	return ok;
}

internal B4
x8616_parse_operand(Str8_R cur, X8616_DecodedInstruction_R inst, X8616_DecodedOperand_R op)
{
	B4 ok = 1;
	x8616_parse_skip_ws(cur);
	X8616_WidthMode size = x8616_width_dynamic;
	if      (x8616_parse_eat(cur, slit8("byte "))) size         = x8616_width_byte;
	else if (x8616_parse_eat(cur, slit8("word "))) size         = x8616_width_word;
	if      (x8616_parse_eat(cur, slit8("far ")))  inst->flags |= x8616_encoding_far;
	x8616_parse_skip_ws(cur);

	U4   seg_n = Array_len(x8616_serialize_seg);
	Str8 save  = cur[0];
	U4   seg   = x8616_parse_lookup(cur, x8616_serialize_seg, seg_n);
	if (seg < seg_n)
	{
		x8616_parse_skip_ws(cur);
		if (x8616_parse_eat(cur, slit8(":"))) {
			inst->prefixes.has_segment = 1;
			inst->prefixes.segment     = C_(X8616_Segment, seg);
			x8616_parse_skip_ws(cur);
			if (x8616_parse_memory(cur, op) == 0) { ok = 0; goto exit; }
		}
		else {
			op->flags   = x8616_decoded_operand_segment;
			op->segment = C_(X8616_Segment, seg);
			op->width   = x8616_width_word;
		}
	}
	else
	{
		cur[0] = save;
		U4 r16_n = Array_len(x8616_serialize_reg16);
		U4 r8_n  = Array_len(x8616_serialize_reg8);
		U4 r16   = x8616_parse_lookup(cur, x8616_serialize_reg16, r16_n);
		if (r16 < r16_n) {
			op->flags    = x8616_decoded_operand_register;
			op->width    = x8616_width_word;
			op->reg.r16  = C_(X8616_Reg16, r16);
		}
		else
		{
			cur[0] = save;
			U4 r8 = x8616_parse_lookup(cur, x8616_serialize_reg8, r8_n);
			if (r8 < r8_n) {
				op->flags   = x8616_decoded_operand_register;
				op->width   = x8616_width_byte;
				op->reg.r8  = C_(X8616_Reg8, r8);
			}
			else if (x8616_parse_eat(cur, slit8("$"))) {
				B4 neg = 0;
				if (x8616_parse_eat(cur, slit8("+"))) {}
				else if (x8616_parse_eat(cur, slit8("-"))) neg = 1;
				else { ok = 0; goto exit; }
				U2 mag = 0; B4 extra = 0;
				if (x8616_parse_number(cur, & mag, & extra) == 0) { ok = 0; goto exit; }
				if (extra) neg = 1;
				op->flags        = x8616_decoded_operand_relative;
				op->displacement = neg ? s2_(-s4_(mag)) : s2_(mag);
				op->width        = x8616_width_byte;
			}
			else if (cur->len && cur->ptr[0] == '[') {
				if (x8616_parse_memory(cur, op) == 0) { ok = 0; goto exit; }
			}
			else
			{
				U2 mag = 0; B4 neg = 0;
				Str8 before = *cur;
				if (x8616_parse_number(cur, & mag, & neg) == 0) { ok = 0; goto exit; }
				x8616_parse_skip_ws(cur);
				if (x8616_parse_eat(cur, slit8(":"))) {
					U2 off = 0; B4 off_neg = 0;
					if (x8616_parse_number(cur, & off, & off_neg) == 0) { ok = 0; goto exit; }
					op->flags       = x8616_decoded_operand_far_ptr;
					op->far_segment = mag;
					op->far_offset  = off;
				}
				else {
					*cur = before;
					if (x8616_parse_number(cur, & mag, & neg) == 0) { ok = 0; goto exit; }
					op->flags     = x8616_decoded_operand_immediate;
					op->immediate = mag;
					if (neg) {
						op->flags    |= x8616_decoded_operand_sign_extended;
						op->immediate = u2_(s2_(-s4_(mag)));
					}
					op->immediate_bytes = mag > 0xFF ? 2 : 1;
					op->width           = mag > 0xFF ? x8616_width_word : x8616_width_byte;
				}
			}
		}
	}

	if (size != x8616_width_dynamic) {
		op->width = size;
		if (inst->width == x8616_width_dynamic) inst->width = size;
	}
exit:
	return ok;
}

internal B4 x8616_parse_line(Str8 line, X8616_DecodedInstruction_R inst)
{
	B4 ok = 1;
	x8616_parse_skip_ws(& line);
	if (line.len == 0) { ok = 0; goto exit; }

	if (x8616_parse_eat(& line, slit8("lock "))) inst->prefixes.lock = 1;
	if (x8616_parse_eat(& line, slit8("repne "))) {
		inst->prefixes.has_repeat = 1;
		inst->prefixes.repeat     = x8616_repne;
	}
	else if (x8616_parse_eat(& line, slit8("rep "))) {
		inst->prefixes.has_repeat = 1;
		inst->prefixes.repeat     = x8616_rep;
	}

	U4 mnem_n = Array_len(x8616_serialize_mnemonic);
	U4 mnem   = x8616_parse_lookup(& line, x8616_serialize_mnemonic, mnem_n);
	if (mnem >= mnem_n) { ok = 0; goto exit; }
	inst->op = C_(X8616_Op, mnem);

	if (inst->op == x8616_op_movs || inst->op == x8616_op_cmps || inst->op == x8616_op_scas
	 || inst->op == x8616_op_lods || inst->op == x8616_op_stos) {
		if (x8616_parse_eat(& line, slit8("b"))) inst->width = x8616_width_byte;
		else if (x8616_parse_eat(& line, slit8("w"))) inst->width = x8616_width_word;
	}

	x8616_parse_skip_ws(& line);
	if (line.len == 0) goto exit;

	if (x8616_parse_operand(& line, inst, inst->operands + 0) == 0) { ok = 0; goto exit; }
	inst->operand_count = 1;
	x8616_parse_skip_ws(& line);
	if (x8616_parse_eat(& line, slit8(","))) {
		if (x8616_parse_operand(& line, inst, inst->operands + 1) == 0) { ok = 0; goto exit; }
		inst->operand_count = 2;
	}
	x8616_parse_skip_ws(& line);
	if (inst->width == x8616_width_dynamic) {
		if (inst->operand_count && inst->operands[0].width != x8616_width_dynamic)
			inst->width = inst->operands[0].width;
	}
	ok = line.len == 0;
exit:
	return ok;
}

X8616_ParseInfo x8616_parse_instructions(X8616_ParseRequest request)
{
	X8616_ParseInfo result = {0};
	X8616_InfoList   local = {0};
	X8616_InfoList_R msgs  = request.msgs ? request.msgs : & local;
	U4 offset = 0;

	B4 bad = request.source.ptr == 0 && request.source.len != 0;
	bad |= request.instruction_cap && request.out_instructions == 0;
	bad |= request.info_arena == 0;
	if (bad) {
		if (request.info_arena) {
			x8616_info_push(request.info_arena, msgs, x8616_info_error
				, x8616_info_parse_bad_request, 0, 0, 0, 0);
		}
		goto exit;
	}

	Str8 cur = request.source;
	if (x8616_parse_eat(& cur, x8616_serialize_header)) {}

	offset = u4_(request.source.len - cur.len);
	while (cur.len)
	{
		U8 n = 0;
		while (n < cur.len && cur.ptr[n] != '\n') n += 1;
		Str8 line  = str8(cur.ptr, n);
		U8 step    = n + (n < cur.len);
		U4 line_at = offset;
		cur.ptr += step;
		cur.len -= step;
		offset  += u4_(step);

		while (line.len && char_is_space(line.ptr[line.len - 1])) line.len -= 1;
		x8616_parse_skip_ws(& line);
		if (line.len == 0) continue;

		if (result.instruction_count == request.instruction_cap) {
			x8616_info_push(request.info_arena, msgs, x8616_info_error
				, x8616_info_parse_output_full, line_at, 0, request.instruction_cap, result.instruction_count);
			goto exit;
		}

		X8616_DecodedInstruction inst = {0};
		if (x8616_parse_line(line, & inst) == 0) {
			x8616_info_push(request.info_arena, msgs, x8616_info_error
				, x8616_info_parse_syntax, line_at, u2_(line.len), 0, 0);
			continue;
		}
		request.out_instructions[result.instruction_count] = inst;
		result.instruction_count += 1;
	}

exit:
	result.source_consumed = offset;
	result.msgs            = *msgs;
	return result;
}
