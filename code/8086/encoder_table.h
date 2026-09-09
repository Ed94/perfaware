#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "encoder.h"
#endif

// =========================================================================================
// Encoding Table
//
// Part 1 target: the instruction cross-section exercised by Computer Enhance listing 0042.
// Later Part 1 simulation listings use a subset of this table.
//
// Table 4-12: .header is byte 1 (opcode bits aligned, fields masked).
// X8616_Opcode members are the identifying bits.
// ModR/M /digit selectors use their encoded enumeration values.
// =========================================================================================

RO_ global X8616_Encoding x8616_encodings[] =
{
	// MOV---------------------------------------------------------------------

	/* MOV: 100010 d w | mod reg r/m */ {
		.header = x8616_opc(x8616_opcode_mov_rm_r, X8616_OPCODE_DW_HOLE),
		.fields = {
			.d = { X8616_OPCODE_DW_D_SHIFT, 1 },
			.w = { X8616_OPCODE_DW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_reg_modrm },
		.op = x8616_op_mov,
	},

	/* MOV imm -> r/m: 1100011 w | mod 000 r/m */ {
		.header = x8616_opc(x8616_opcode_mov_rm_i, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_digit_0 << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_imm },
		.op = x8616_op_mov,
	},

	/* MOV imm -> reg: 1011 w reg */ {
		.header = x8616_opc(x8616_opcode_mov_r_i, X8616_OPCODE_WREG_HOLE),
		.fields = {
			.w = { X8616_OPCODE_WREG_W_SHIFT, 1 },
			.reg = { X8616_OPCODE_WREG_REG_SHIFT, X8616_OPCODE_WREG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode, x8616_operand_imm },
		.op = x8616_op_mov,
	},

	/* MOV accumulator <-> direct memory: 101000 d w */ {
		.header = x8616_opc(x8616_opcode_mov_acc_mem, X8616_OPCODE_DW_HOLE),
		.fields = {
			.d = { X8616_OPCODE_DW_D_SHIFT, 1 },
			.w = { X8616_OPCODE_DW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_mem_direct },
		.op = x8616_op_mov,
	},

	/* MOV segment <-> r/m: 100011 d 0 | mod 0 sr r/m */ {
		.header = x8616_header_d0(x8616_opcode_mov_seg_rm),
		.mod_rm = {
			.bits = 0,
			.mask = X8616_MODRM_SEG_FIXED_MASK,
		},
		.fields = {
			.d = { X8616_OPCODE_D0_D_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_segment_modrm },
		.width = x8616_width_word,
		.op = x8616_op_mov,
	},

	// Stack / exchange--------------------------------------------------------

	/* PUSH r/m16: 1111111 w=1 | mod 110 r/m */ {
		.header = x8616_header_w(x8616_opcode_incdec_rm, x8616_w_word),
		.mod_rm = {
			.bits = x8616_ff_push << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_push,
	},

	/* PUSH reg16: 01010 reg */ {
		.header = x8616_opc(x8616_opcode_push_reg, X8616_OPCODE_REG_HOLE),
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode },
		.width    = x8616_width_word,
		.op       = x8616_op_push,
	},

	/* POP reg16: 01011 reg */ {
		.header = x8616_opc(x8616_opcode_pop_reg, X8616_OPCODE_REG_HOLE),
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode },
		.width    = x8616_width_word,
		.op       = x8616_op_pop,
	},

	/* PUSH segment: 000 sr 110 */ {
		.header = x8616_header_sr(x8616_opcode_sr_stack, x8616_sr_low_push),
		.fields = {
			.sr = { X8616_OPCODE_SR_SHIFT, X8616_OPCODE_SR_WIDTH },
		},
		.operands = { x8616_operand_segment_opcode },
		.width = x8616_width_word,
		.op = x8616_op_push,
	},

	/* POP r/m16: 10001111 | mod 000 r/m */ {
		.header = x8616_opc_byte(x8616_opcode_pop_rm),
		.mod_rm = {
			.bits = x8616_digit_0 << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_pop,
	},



	/* POP segment: 000 sr 111 */ {
		.header = x8616_header_sr(x8616_opcode_sr_stack, x8616_sr_low_pop),
		.fields = {
			.sr = { X8616_OPCODE_SR_SHIFT, X8616_OPCODE_SR_WIDTH },
		},
		.operands = { x8616_operand_segment_opcode },
		.width = x8616_width_word,
		.op = x8616_op_pop,
	},

	/* XCHG r/m,reg: 1000011 w | mod reg r/m */ {
		.header = x8616_opc(x8616_opcode_xchg_rm_r, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_reg_modrm, x8616_operand_rm },
		.op = x8616_op_xchg,
	},

	/* XCHG AX,reg16: 10010 reg (also encodes NOP at reg=AX) */ {
		.header = x8616_opc(x8616_opcode_xchg_ax_reg, X8616_OPCODE_REG_HOLE),
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_acc, x8616_operand_reg_opcode },
		.width = x8616_width_word,
		.op = x8616_op_xchg,
	},

	// I/O / address / flags---------------------------------------------------

	/* IN acc,imm8: 1110 0 10 w */ {
		.header = x8616_opc(x8616_opcode_in_i, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_imm8 },
		.op = x8616_op_in,
	},

	/* IN acc,DX: 1110 1 10 w */ {
		.header = x8616_opc(x8616_opcode_in_dx, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_dx },
		.op = x8616_op_in,
	},

	/* OUT imm8,acc: 1110 0 11 w */ {
		.header = x8616_opc(x8616_opcode_out_i, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_imm8, x8616_operand_acc },
		.op = x8616_op_out,
	},

	/* OUT DX,acc: 1110 1 11 w */ {
		.header = x8616_opc(x8616_opcode_out_dx, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_dx, x8616_operand_acc },
		.op = x8616_op_out,
	},

	/* XLAT */ {
		.header = x8616_opc_byte(x8616_opcode_xlat),
		.op = x8616_op_xlat,
	},

	/* LEA: mod reg r/m */ {
		.header = x8616_opc_byte(x8616_opcode_lea),
		.operands = { x8616_operand_reg_modrm, x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_lea,
	},

	/* LDS: mod reg r/m */ {
		.header = x8616_opc_byte(x8616_opcode_lds),
		.operands = { x8616_operand_reg_modrm, x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_lds,
	},

	/* LES: mod reg r/m */ {
		.header = x8616_opc_byte(x8616_opcode_les),
		.operands = { x8616_operand_reg_modrm, x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_les,
	},

	/* LAHF */ {
		.header = x8616_opc_byte(x8616_opcode_lahf),
		.op = x8616_op_lahf,
	},

	/* SAHF */ {
		.header = x8616_opc_byte(x8616_opcode_sahf),
		.op = x8616_op_sahf,
	},

	/* PUSHF */ {
		.header = x8616_opc_byte(x8616_opcode_pushf),
		.op = x8616_op_pushf,
	},

	/* POPF */ {
		.header = x8616_opc_byte(x8616_opcode_popf),
		.op = x8616_op_popf,
	},

	// Arithmetic / logical----------------------------------------------------
	// 00 ttt 0 d w  |  00 ttt 10 w  |  100000 s w /ttt

	/* ALU r/m,reg: 00 ttt 0 d w */ {
		.header = {
			.bits = x8616_enc_alu_class(),
			.mask = x8616_field_mask(X8616_OPCODE_ALU_CLASS_SHIFT, X8616_OPCODE_ALU_CLASS_WIDTH)
			      | x8616_field_mask(X8616_OPCODE_ALU_BIT2_SHIFT, 1),
		},
		.fields = {
			.alu = { X8616_OPCODE_ALU_TTT_SHIFT, X8616_OPCODE_ALU_TTT_WIDTH },
			.d   = { X8616_OPCODE_DW_D_SHIFT, 1 },
			.w   = { X8616_OPCODE_DW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_reg_modrm },
		.op = x8616_op_invalid,
	},


	/* ALU acc,imm: 00 ttt 10 w */ {
		.header = {
			.bits = x8616_enc_alu_class() | (X8616_OPCODE_ALU_ACC_FORM << X8616_OPCODE_ALU_ACC_FORM_SHIFT),
			.mask = x8616_field_mask(X8616_OPCODE_ALU_CLASS_SHIFT, X8616_OPCODE_ALU_CLASS_WIDTH)
			      | x8616_field_mask(X8616_OPCODE_ALU_ACC_FORM_SHIFT, 2),
		},
		.fields = {
			.alu = { X8616_OPCODE_ALU_TTT_SHIFT, X8616_OPCODE_ALU_TTT_WIDTH },
			.w   = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_imm },
		.op = x8616_op_invalid,
	},

	/* ALU imm,r/m: 100000 s w /ttt */ {
		.header = x8616_opc(x8616_opcode_alu_rm_i, X8616_OPCODE_SW_HOLE),
		.fields = {
			.s = { X8616_OPCODE_SW_S_SHIFT, 1 },
			.w = { X8616_OPCODE_SW_W_SHIFT, 1 },
		},
		.operands   = { x8616_operand_rm, x8616_operand_imm },
		.digit_kind = x8616_digit_alu,
		.op         = x8616_op_invalid,
	},


	// INC / DEC / unary / adjust---------------------------------------------

	/* INC r/m: 1111111 w /0 */ {
		.header = x8616_opc(x8616_opcode_incdec_rm, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_inc << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.op = x8616_op_inc,
	},

	/* INC reg16: 01000 reg */ {
		.header = x8616_opc(x8616_opcode_inc_reg, X8616_OPCODE_REG_HOLE),
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode },
		.width    = x8616_width_word,
		.op       = x8616_op_inc,
	},

	/* DEC reg16: 01001 reg */ {
		.header = x8616_opc(x8616_opcode_dec_reg, X8616_OPCODE_REG_HOLE),
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode },
		.width    = x8616_width_word,
		.op       = x8616_op_dec,
	},

	/* AAA */ {
		.header = x8616_opc_byte(x8616_opcode_aaa),
		.op = x8616_op_aaa,
	},

	/* DAA */ {
		.header = x8616_opc_byte(x8616_opcode_daa),
		.op = x8616_op_daa,
	},

	/* DEC r/m: 1111111 w /1 */ {
		.header = x8616_opc(x8616_opcode_incdec_rm, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_dec << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.op = x8616_op_dec,
	},



	/* NEG r/m: 1111011 w /neg */ {
		.header = x8616_opc(x8616_opcode_group3, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_g3_neg << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.op = x8616_op_neg,
	},

	/* MUL r/m: 1111011 w /mul */ {
		.header = x8616_opc(x8616_opcode_group3, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_g3_mul << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.op = x8616_op_mul,
	},

	/* IMUL r/m: 1111011 w /imul */ {
		.header = x8616_opc(x8616_opcode_group3, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_g3_imul << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.op = x8616_op_imul,
	},

	/* DIV r/m: 1111011 w /div */ {
		.header = x8616_opc(x8616_opcode_group3, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_g3_div << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.op = x8616_op_div,
	},

	/* IDIV r/m: 1111011 w /idiv */ {
		.header = x8616_opc(x8616_opcode_group3, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_g3_idiv << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.op = x8616_op_idiv,
	},

	/* NOT r/m: 1111011 w /not */ {
		.header = x8616_opc(x8616_opcode_group3, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_g3_not << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.op = x8616_op_not,
	},

	/* AAS */ {
		.header = x8616_opc_byte(x8616_opcode_aas),
		.op = x8616_op_aas,
	},

	/* DAS */ {
		.header = x8616_opc_byte(x8616_opcode_das),
		.op = x8616_op_das,
	},

	/* AAM: 11010100 00001010 */ {
		.header = x8616_opc_byte(x8616_opcode_aam),
		.post_opcode = {
			.bits = X8616_POST_OPCODE_AAM_AAD,
			.mask = X8616_BYTE_MASK,
		},
		.op = x8616_op_aam,
	},

	/* AAD: 11010101 00001010 */ {
		.header = x8616_opc_byte(x8616_opcode_aad),
		.post_opcode = {
			.bits = X8616_POST_OPCODE_AAM_AAD,
			.mask = X8616_BYTE_MASK,
		},
		.op = x8616_op_aad,
	},

	/* CBW */ {
		.header = x8616_opc_byte(x8616_opcode_cbw),
		.op = x8616_op_cbw,
	},

	/* CWD */ {
		.header = x8616_opc_byte(x8616_opcode_cwd),
		.op = x8616_op_cwd,
	},

	// Shift / rotate / TEST---------------------------------------------------

	/* SHIFT/ROTATE r/m,1|CL: 110100 v w /ttt */ {
		.header = x8616_opc(x8616_opcode_shift_rm, X8616_OPCODE_VW_HOLE),
		.fields = {
			.v = { X8616_OPCODE_VW_V_SHIFT, 1 },
			.w = { X8616_OPCODE_VW_W_SHIFT, 1 },
		},
		.operands   = { x8616_operand_rm, x8616_operand_shift_count },
		.digit_kind = x8616_digit_shift,
		.op         = x8616_op_invalid,
	},


	/* TEST r/m,reg: 1000010 w */ {
		.header = x8616_opc(x8616_opcode_test_rm_r, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_reg_modrm },
		.op = x8616_op_test,
	},

	/* TEST r/m,imm: 1111011 w /0 */ {
		.header = x8616_opc(x8616_opcode_group3, X8616_OPCODE_W_HOLE),
		.mod_rm = {
			.bits = x8616_g3_test << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_imm },
		.op = x8616_op_test,
	},

	/* TEST accumulator,imm: 1010100 w */ {
		.header = x8616_opc(x8616_opcode_test_acc_i, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_imm },
		.op = x8616_op_test,
	},

	// String / prefixes-------------------------------------------------------

	/* REP/REPNE prefix: 1111001 z */ {
		.header = x8616_opc(x8616_opcode_rep, X8616_OPCODE_Z_HOLE),
		.fields = {
			.z = { X8616_OPCODE_Z_Z_SHIFT, 1 },
		},
		.flags = x8616_encoding_prefix,
		.op = x8616_op_rep,
	},

	/* MOVS: ... w */ {
		.header = x8616_opc(x8616_opcode_movs, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_movs,
	},

	/* CMPS: ... w */ {
		.header = x8616_opc(x8616_opcode_cmps, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_cmps,
	},

	/* SCAS: ... w */ {
		.header = x8616_opc(x8616_opcode_scas, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_scas,
	},

	/* LODS: ... w */ {
		.header = x8616_opc(x8616_opcode_lods, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_lods,
	},

	/* STOS: ... w */ {
		.header = x8616_opc(x8616_opcode_stos, X8616_OPCODE_W_HOLE),
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_stos,
	},

	/* LOCK prefix */ {
		.header = x8616_opc_byte(x8616_opcode_lock),
		.flags = x8616_encoding_prefix,
		.op = x8616_op_lock,
	},

	/* Segment override: 001 sr 110 */ {
		.header = x8616_header_sr(x8616_opcode_sr_override, x8616_sr_low_push),
		.fields = {
			.sr = { X8616_OPCODE_SR_SHIFT, X8616_OPCODE_SR_WIDTH },
		},
		.flags = x8616_encoding_prefix,
		.op = x8616_op_segment,
	},

	// Control transfer--------------------------------------------------------

	/* CALL rel16 */ {
		.header = x8616_opc_byte(x8616_opcode_call_rel16),
		.operands = { x8616_operand_rel16 },
		.op = x8616_op_call,
	},

	/* CALL r/m16: FF /2 */ {
		.header = x8616_header_w(x8616_opcode_incdec_rm, x8616_w_word),
		.mod_rm = {
			.bits = x8616_ff_call_near << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_call,
	},

	/* CALL far ptr16:16 */ {
		.header = x8616_opc_byte(x8616_opcode_call_far),
		.operands = { x8616_operand_far_ptr },
		.flags = x8616_encoding_far,
		.op = x8616_op_call,
	},

	/* CALL far r/m16: FF /3 */ {
		.header = x8616_header_w(x8616_opcode_incdec_rm, x8616_w_word),
		.mod_rm = {
			.bits = x8616_ff_call_far << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.flags = x8616_encoding_far,
		.op = x8616_op_call,
	},

	/* JMP rel16 */ {
		.header = x8616_opc_byte(x8616_opcode_jmp_rel16),
		.operands = { x8616_operand_rel16 },
		.op = x8616_op_jmp,
	},

	/* JMP rel8 */ {
		.header = x8616_opc_byte(x8616_opcode_jmp_rel8),
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_jmp,
	},

	/* JMP r/m16: FF /4 */ {
		.header = x8616_header_w(x8616_opcode_incdec_rm, x8616_w_word),
		.mod_rm = {
			.bits = x8616_ff_jmp_near << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_jmp,
	},

	/* JMP far ptr16:16 */ {
		.header = x8616_opc_byte(x8616_opcode_jmp_far),
		.operands = { x8616_operand_far_ptr },
		.flags = x8616_encoding_far,
		.op = x8616_op_jmp,
	},

	/* JMP far r/m16: FF /5 */ {
		.header = x8616_header_w(x8616_opcode_incdec_rm, x8616_w_word),
		.mod_rm = {
			.bits = x8616_ff_jmp_far << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.flags = x8616_encoding_far,
		.op = x8616_op_jmp,
	},

	/* RET */ {
		.header = x8616_opc_byte(x8616_opcode_ret),
		.op = x8616_op_ret,
	},

	/* RET imm16 */ {
		.header = x8616_opc_byte(x8616_opcode_ret_i),
		.operands = { x8616_operand_imm16 },
		.op = x8616_op_ret,
	},

	/* RETF */ {
		.header = x8616_opc_byte(x8616_opcode_retf),
		.op = x8616_op_retf,
	},

	/* RETF imm16 */ {
		.header = x8616_opc_byte(x8616_opcode_retf_i),
		.operands = { x8616_operand_imm16 },
		.op = x8616_op_retf,
	},

	/* Jcc rel8: 0111 cccc */ {
		.header = x8616_opc(x8616_opcode_jcc, X8616_OPCODE_CC_HOLE),
		.fields = {
			.cc = { X8616_OPCODE_CC_SHIFT, X8616_OPCODE_CC_WIDTH },
		},
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_invalid,
	},


	/* LOOPNZ rel8 */ {
		.header = x8616_opc_byte(x8616_opcode_loopnz),
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_loopnz,
	},

	/* LOOPZ rel8 */ {
		.header = x8616_opc_byte(x8616_opcode_loopz),
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_loopz,
	},

	/* LOOP rel8 */ {
		.header = x8616_opc_byte(x8616_opcode_loop),
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_loop,
	},

	/* JCXZ rel8 */ {
		.header = x8616_opc_byte(x8616_opcode_jcxz),
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_jcxz,
	},

	// Interrupt / machine control--------------------------------------------

	/* INT imm8 */ {
		.header = x8616_opc_byte(x8616_opcode_int),
		.operands = { x8616_operand_imm8 },
		.op = x8616_op_int,
	},

	/* INT3 */ {
		.header = x8616_opc_byte(x8616_opcode_int3),
		.op = x8616_op_int3,
	},

	/* INTO */ {
		.header = x8616_opc_byte(x8616_opcode_into),
		.op = x8616_op_into,
	},

	/* IRET */ {
		.header = x8616_opc_byte(x8616_opcode_iret),
		.op = x8616_op_iret,
	},

	/* CLC */ {
		.header = x8616_opc_byte(x8616_opcode_clc),
		.op = x8616_op_clc,
	},

	/* CMC */ {
		.header = x8616_opc_byte(x8616_opcode_cmc),
		.op = x8616_op_cmc,
	},

	/* STC */ {
		.header = x8616_opc_byte(x8616_opcode_stc),
		.op = x8616_op_stc,
	},

	/* CLD */ {
		.header = x8616_opc_byte(x8616_opcode_cld),
		.op = x8616_op_cld,
	},

	/* STD */ {
		.header = x8616_opc_byte(x8616_opcode_std),
		.op = x8616_op_std,
	},

	/* CLI */ {
		.header = x8616_opc_byte(x8616_opcode_cli),
		.op = x8616_op_cli,
	},

	/* STI */ {
		.header = x8616_opc_byte(x8616_opcode_sti),
		.op = x8616_op_sti,
	},

	/* HLT */ {
		.header = x8616_opc_byte(x8616_opcode_hlt),
		.op = x8616_op_hlt,
	},

	/* WAIT */ {
		.header = x8616_opc_byte(x8616_opcode_wait),
		.op = x8616_op_wait,
	},

};

enum {
	X8616_ENCODING_COUNT = S_(x8616_encodings) / S_(x8616_encodings[0]),
};
