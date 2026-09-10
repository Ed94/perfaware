#ifdef INTELLISENSE_DIRECTIVES
#	include "encode.h"
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

FI_ void x8616_encode_buf_u1(U1* buf, U1_R n, U1 value) { buf[n[0]] = value; n[0] += 1; }
FI_ void x8616_encode_buf_u2(U1* buf, U1_R n, U2 value) {
	x8616_encode_buf_u1(buf, n, u1_(value));
	x8616_encode_buf_u1(buf, n, u1_(value >> 8));
}
FI_ void x8616_encode_buf_disp(U1* buf, U1_R n, S2 value, U1 bytes) {
	if (bytes == 1) x8616_encode_buf_u1(buf, n, u1_(value));
	if (bytes == 2) x8616_encode_buf_u2(buf, n, u2_(value));
}

internal X8616_EncodeStatus
x8616_encode_instruction(Str8Gen_R gen, X8616_DecodedInstruction_R inst)
{
	B4 bad = inst->decode_flags & (x8616_decode_invalid | x8616_decode_truncated);
	bad |= inst->size == 0;
	if (bad) return x8616_encode_invalid_record;
	U1 buf[16];
	U1 n   = 0;
	U1 rep = inst->prefixes.repeat == x8616_rep ? x8616_rep_prefix() : x8616_repne_prefix();
	if (inst->prefixes.lock)        { x8616_encode_buf_u1(buf, & n, x8616_lock_prefix()); }
	if (inst->prefixes.has_repeat)  { x8616_encode_buf_u1(buf, & n, rep); }
	if (inst->prefixes.has_segment) { x8616_encode_buf_u1(buf, & n, x8616_segment_prefix(inst->prefixes.segment)); }
	                                  x8616_encode_buf_u1(buf, & n, inst->header);
	if (inst->post_opcode)          { x8616_encode_buf_u1(buf, & n, inst->post_opcode); }
	if (inst->has_mod_rm)           { x8616_encode_buf_u1(buf, & n, inst->mod_rm); }

	for (U1 id = 0; id < inst->operand_count; ++id) {
		X8616_DecodedOperand_R op = inst->operands + id;
		if ((op->flags & x8616_decoded_operand_memory) == 0) continue;
		if ( op->flags & x8616_decoded_operand_direct) x8616_encode_buf_u2(buf, & n, op->address);
		else x8616_encode_buf_disp(buf, & n, op->displacement, op->displacement_bytes);
	}
	for (U1 id = 0; id < inst->operand_count; ++id) {
		X8616_DecodedOperand_R op = inst->operands + id;
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
