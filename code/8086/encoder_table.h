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
// Fixed opcode bits are kept in binary/signature form; 
// ModR/M /digit selectors use their encoded enumeration values.
// =========================================================================================

RO_ global X8616_Encoding x8616_encodings[] =
{
	// MOV---------------------------------------------------------------------

	/* MOV: 100010 d w | mod reg r/m */ {
		.opcode = {
			.bits = x8616_opcode_mov_rm_r << X8616_OPCODE_DW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_DW_PREFIX_MASK,
		},
		.fields = {
			.d = { X8616_OPCODE_DW_D_SHIFT, 1 },
			.w = { X8616_OPCODE_DW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_reg_modrm },
		.op = x8616_op_mov,
	},

	/* MOV imm -> r/m: 1100011 w | mod 000 r/m */ {
		.opcode = {
			.bits = x8616_opcode_mov_rm_i << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_mov_r_i << X8616_OPCODE_WREG_PREFIX_SHIFT,
			.mask = X8616_OPCODE_WREG_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_WREG_W_SHIFT, 1 },
			.reg = { X8616_OPCODE_WREG_REG_SHIFT, X8616_OPCODE_WREG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode, x8616_operand_imm },
		.op = x8616_op_mov,
	},

	/* MOV accumulator <-> direct memory: 101000 d w */ {
		.opcode = {
			.bits = x8616_opcode_mov_acc_mem << X8616_OPCODE_DW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_DW_PREFIX_MASK,
		},
		.fields = {
			.d = { X8616_OPCODE_DW_D_SHIFT, 1 },
			.w = { X8616_OPCODE_DW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_mem_direct },
		.op = x8616_op_mov,
	},

	/* MOV segment <-> r/m: 100011 d 0 | mod 0 sr r/m */ {
		.opcode = {
			.bits = x8616_opcode_mov_seg_rm << X8616_OPCODE_D0_PREFIX_SHIFT,
			.mask = X8616_OPCODE_D0_PREFIX_MASK,
		},
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

	/* PUSH r/m16: 11111111 | mod 110 r/m */ {
		.opcode = {
			.bits = x8616_opcode_group_ff,
			.mask = X8616_OPCODE_MASK,
		},
		.mod_rm = {
			.bits = x8616_ff_push << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_push,
	},

	/* PUSH reg16: 01010 reg */ {
		.opcode = {
			.bits = x8616_opcode_push_reg << X8616_OPCODE_REG_PREFIX_SHIFT,
			.mask = X8616_OPCODE_REG_PREFIX_MASK,
		},
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode },
		.width = x8616_width_word,
		.op = x8616_op_push,
	},

	/* PUSH segment: 000 sr 110 */ {
		.opcode = {
			.bits = X8616_OPCODE_PUSH_SR_FIXED,
			.mask = X8616_OPCODE_SR_PATTERN_MASK,
		},
		.fields = {
			.sr = { X8616_OPCODE_SR_SHIFT, X8616_OPCODE_SR_WIDTH },
		},
		.operands = { x8616_operand_segment_opcode },
		.width = x8616_width_word,
		.op = x8616_op_push,
	},

	/* POP r/m16: 10001111 | mod 000 r/m */ {
		.opcode = {
			.bits = x8616_opcode_pop_rm,
			.mask = X8616_OPCODE_MASK,
		},
		.mod_rm = {
			.bits = x8616_digit_0 << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_pop,
	},

	/* POP reg16: 01011 reg */ {
		.opcode = {
			.bits = x8616_opcode_pop_reg << X8616_OPCODE_REG_PREFIX_SHIFT,
			.mask = X8616_OPCODE_REG_PREFIX_MASK,
		},
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode },
		.width = x8616_width_word,
		.op = x8616_op_pop,
	},

	/* POP segment: 000 sr 111 */ {
		.opcode = {
			.bits = X8616_OPCODE_POP_SR_FIXED,
			.mask = X8616_OPCODE_SR_PATTERN_MASK,
		},
		.fields = {
			.sr = { X8616_OPCODE_SR_SHIFT, X8616_OPCODE_SR_WIDTH },
		},
		.operands = { x8616_operand_segment_opcode },
		.width = x8616_width_word,
		.op = x8616_op_pop,
	},

	/* XCHG r/m,reg: 1000011 w | mod reg r/m */ {
		.opcode = {
			.bits = x8616_opcode_xchg_rm_r << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_reg_modrm, x8616_operand_rm },
		.op = x8616_op_xchg,
	},

	/* XCHG AX,reg16: 10010 reg (also encodes NOP at reg=AX) */ {
		.opcode = {
			.bits = x8616_opcode_xchg_ax_reg << X8616_OPCODE_REG_PREFIX_SHIFT,
			.mask = X8616_OPCODE_REG_PREFIX_MASK,
		},
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_acc, x8616_operand_reg_opcode },
		.width = x8616_width_word,
		.op = x8616_op_xchg,
	},

	// I/O / address / flags---------------------------------------------------

	/* IN acc,imm8: 1110010 w */ {
		.opcode = {
			.bits = x8616_opcode_in_i << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_imm8 },
		.op = x8616_op_in,
	},

	/* IN acc,DX: 1110110 w */ {
		.opcode = {
			.bits = x8616_opcode_in_dx << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_dx },
		.op = x8616_op_in,
	},

	/* OUT imm8,acc: 1110011 w */ {
		.opcode = {
			.bits = x8616_opcode_out_i << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_imm8, x8616_operand_acc },
		.op = x8616_op_out,
	},

	/* OUT DX,acc: 1110111 w */ {
		.opcode = {
			.bits = x8616_opcode_out_dx << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_dx, x8616_operand_acc },
		.op = x8616_op_out,
	},

	/* XLAT */ {
		.opcode = {
			.bits = x8616_opcode_xlat,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_xlat,
	},

	/* LEA: mod reg r/m */ {
		.opcode = {
			.bits = x8616_opcode_lea,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_reg_modrm, x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_lea,
	},

	/* LDS: mod reg r/m */ {
		.opcode = {
			.bits = x8616_opcode_lds,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_reg_modrm, x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_lds,
	},

	/* LES: mod reg r/m */ {
		.opcode = {
			.bits = x8616_opcode_les,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_reg_modrm, x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_les,
	},

	/* LAHF */ {
		.opcode = {
			.bits = x8616_opcode_lahf,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_lahf,
	},

	/* SAHF */ {
		.opcode = {
			.bits = x8616_opcode_sahf,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_sahf,
	},

	/* PUSHF */ {
		.opcode = {
			.bits = x8616_opcode_pushf,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_pushf,
	},

	/* POPF */ {
		.opcode = {
			.bits = x8616_opcode_popf,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_popf,
	},

	// Arithmetic / logical----------------------------------------------------
	// 00 ttt 0 d w  |  00 ttt 10 w  |  100000 s w /ttt

	/* ALU r/m,reg: 00 ttt 0 d w */ {
		.opcode = {
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
		.opcode = {
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
		.opcode = {
			.bits = x8616_opcode_alu_rm_i << X8616_OPCODE_SW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_SW_PREFIX_MASK,
		},
		.fields = {
			.alu = { X8616_MODRM_REG_SHIFT, X8616_MODRM_REG_WIDTH },
			.s   = { X8616_OPCODE_SW_S_SHIFT, 1 },
			.w   = { X8616_OPCODE_SW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_imm },
		.op = x8616_op_invalid,
	},


	// INC / DEC / unary / adjust---------------------------------------------

	/* INC r/m: 1111111 w /0 */ {
		.opcode = {
			.bits = x8616_opcode_incdec_rm << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_inc_reg << X8616_OPCODE_REG_PREFIX_SHIFT,
			.mask = X8616_OPCODE_REG_PREFIX_MASK,
		},
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode },
		.width = x8616_width_word,
		.op = x8616_op_inc,
	},

	/* AAA */ {
		.opcode = {
			.bits = x8616_opcode_aaa,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_aaa,
	},

	/* DAA */ {
		.opcode = {
			.bits = x8616_opcode_daa,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_daa,
	},

	/* DEC r/m: 1111111 w /1 */ {
		.opcode = {
			.bits = x8616_opcode_incdec_rm << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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

	/* DEC reg16: 01001 reg */ {
		.opcode = {
			.bits = x8616_opcode_dec_reg << X8616_OPCODE_REG_PREFIX_SHIFT,
			.mask = X8616_OPCODE_REG_PREFIX_MASK,
		},
		.fields = {
			.reg = { X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH },
		},
		.operands = { x8616_operand_reg_opcode },
		.width = x8616_width_word,
		.op = x8616_op_dec,
	},

	/* NEG r/m: 1111011 w /neg */ {
		.opcode = {
			.bits = x8616_opcode_group3 << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_group3 << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_group3 << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_group3 << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_group3 << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_group3 << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_aas,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_aas,
	},

	/* DAS */ {
		.opcode = {
			.bits = x8616_opcode_das,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_das,
	},

	/* AAM: 11010100 00001010 */ {
		.opcode = {
			.bits = x8616_opcode_aam,
			.mask = X8616_OPCODE_MASK,
		},
		.post_opcode = {
			.bits = X8616_POST_OPCODE_AAM_AAD,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_aam,
	},

	/* AAD: 11010101 00001010 */ {
		.opcode = {
			.bits = x8616_opcode_aad,
			.mask = X8616_OPCODE_MASK,
		},
		.post_opcode = {
			.bits = X8616_POST_OPCODE_AAM_AAD,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_aad,
	},

	/* CBW */ {
		.opcode = {
			.bits = x8616_opcode_cbw,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_cbw,
	},

	/* CWD */ {
		.opcode = {
			.bits = x8616_opcode_cwd,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_cwd,
	},

	// Shift / rotate / TEST---------------------------------------------------

	/* ROL r/m,1|CL: 110100 v w */ {
		.opcode = {
			.bits = x8616_opcode_shift_rm << X8616_OPCODE_VW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_VW_PREFIX_MASK,
		},
		.mod_rm = {
			.bits = x8616_rol << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.v = { X8616_OPCODE_VW_V_SHIFT, 1 },
			.w = { X8616_OPCODE_VW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_shift_count },
		.op = x8616_op_rol,
	},

	/* ROR r/m,1|CL: 110100 v w */ {
		.opcode = {
			.bits = x8616_opcode_shift_rm << X8616_OPCODE_VW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_VW_PREFIX_MASK,
		},
		.mod_rm = {
			.bits = x8616_ror << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.v = { X8616_OPCODE_VW_V_SHIFT, 1 },
			.w = { X8616_OPCODE_VW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_shift_count },
		.op = x8616_op_ror,
	},

	/* RCL r/m,1|CL: 110100 v w */ {
		.opcode = {
			.bits = x8616_opcode_shift_rm << X8616_OPCODE_VW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_VW_PREFIX_MASK,
		},
		.mod_rm = {
			.bits = x8616_rcl << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.v = { X8616_OPCODE_VW_V_SHIFT, 1 },
			.w = { X8616_OPCODE_VW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_shift_count },
		.op = x8616_op_rcl,
	},

	/* RCR r/m,1|CL: 110100 v w */ {
		.opcode = {
			.bits = x8616_opcode_shift_rm << X8616_OPCODE_VW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_VW_PREFIX_MASK,
		},
		.mod_rm = {
			.bits = x8616_rcr << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.v = { X8616_OPCODE_VW_V_SHIFT, 1 },
			.w = { X8616_OPCODE_VW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_shift_count },
		.op = x8616_op_rcr,
	},

	/* SHL r/m,1|CL: 110100 v w */ {
		.opcode = {
			.bits = x8616_opcode_shift_rm << X8616_OPCODE_VW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_VW_PREFIX_MASK,
		},
		.mod_rm = {
			.bits = x8616_shl << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.v = { X8616_OPCODE_VW_V_SHIFT, 1 },
			.w = { X8616_OPCODE_VW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_shift_count },
		.op = x8616_op_shl,
	},

	/* SHR r/m,1|CL: 110100 v w */ {
		.opcode = {
			.bits = x8616_opcode_shift_rm << X8616_OPCODE_VW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_VW_PREFIX_MASK,
		},
		.mod_rm = {
			.bits = x8616_shr << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.v = { X8616_OPCODE_VW_V_SHIFT, 1 },
			.w = { X8616_OPCODE_VW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_shift_count },
		.op = x8616_op_shr,
	},

	/* SAR r/m,1|CL: 110100 v w */ {
		.opcode = {
			.bits = x8616_opcode_shift_rm << X8616_OPCODE_VW_PREFIX_SHIFT,
			.mask = X8616_OPCODE_VW_PREFIX_MASK,
		},
		.mod_rm = {
			.bits = x8616_sar << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.fields = {
			.v = { X8616_OPCODE_VW_V_SHIFT, 1 },
			.w = { X8616_OPCODE_VW_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_shift_count },
		.op = x8616_op_sar,
	},

	/* TEST r/m,reg: 1000010 w */ {
		.opcode = {
			.bits = x8616_opcode_test_rm_r << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_rm, x8616_operand_reg_modrm },
		.op = x8616_op_test,
	},

	/* TEST r/m,imm: 1111011 w /0 */ {
		.opcode = {
			.bits = x8616_opcode_group3 << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
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
		.opcode = {
			.bits = x8616_opcode_test_acc_i << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.operands = { x8616_operand_acc, x8616_operand_imm },
		.op = x8616_op_test,
	},

	// String / prefixes-------------------------------------------------------

	/* REP/REPNE prefix: 1111001 z */ {
		.opcode = {
			.bits = x8616_opcode_rep << X8616_OPCODE_Z_PREFIX_SHIFT,
			.mask = X8616_OPCODE_Z_PREFIX_MASK,
		},
		.fields = {
			.z = { X8616_OPCODE_Z_Z_SHIFT, 1 },
		},
		.flags = x8616_encoding_prefix,
		.op = x8616_op_rep,
	},

	/* MOVS: ... w */ {
		.opcode = {
			.bits = x8616_opcode_movs << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_movs,
	},

	/* CMPS: ... w */ {
		.opcode = {
			.bits = x8616_opcode_cmps << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_cmps,
	},

	/* SCAS: ... w */ {
		.opcode = {
			.bits = x8616_opcode_scas << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_scas,
	},

	/* LODS: ... w */ {
		.opcode = {
			.bits = x8616_opcode_lods << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_lods,
	},

	/* STOS: ... w */ {
		.opcode = {
			.bits = x8616_opcode_stos << X8616_OPCODE_W_PREFIX_SHIFT,
			.mask = X8616_OPCODE_W_PREFIX_MASK,
		},
		.fields = {
			.w = { X8616_OPCODE_W_W_SHIFT, 1 },
		},
		.op = x8616_op_stos,
	},

	/* LOCK prefix */ {
		.opcode = {
			.bits = x8616_opcode_lock,
			.mask = X8616_OPCODE_MASK,
		},
		.flags = x8616_encoding_prefix,
		.op = x8616_op_lock,
	},

	/* Segment override: 001 sr 110 */ {
		.opcode = {
			.bits = X8616_OPCODE_SEG_FIXED,
			.mask = X8616_OPCODE_SR_PATTERN_MASK,
		},
		.fields = {
			.sr = { X8616_OPCODE_SR_SHIFT, X8616_OPCODE_SR_WIDTH },
		},
		.flags = x8616_encoding_prefix,
		.op = x8616_op_segment,
	},

	// Control transfer--------------------------------------------------------

	/* CALL rel16 */ {
		.opcode = {
			.bits = x8616_opcode_call_rel16,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_rel16 },
		.op = x8616_op_call,
	},

	/* CALL r/m16: FF /2 */ {
		.opcode = {
			.bits = x8616_opcode_group_ff,
			.mask = X8616_OPCODE_MASK,
		},
		.mod_rm = {
			.bits = x8616_ff_call_near << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_call,
	},

	/* CALL far ptr16:16 */ {
		.opcode = {
			.bits = x8616_opcode_call_far,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_far_ptr },
		.flags = x8616_encoding_far,
		.op = x8616_op_call,
	},

	/* CALL far r/m16: FF /3 */ {
		.opcode = {
			.bits = x8616_opcode_group_ff,
			.mask = X8616_OPCODE_MASK,
		},
		.mod_rm = {
			.bits = x8616_ff_call_far << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.flags = x8616_encoding_far,
		.op = x8616_op_call,
	},

	/* JMP rel16 */ {
		.opcode = {
			.bits = x8616_opcode_jmp_rel16,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_rel16 },
		.op = x8616_op_jmp,
	},

	/* JMP rel8 */ {
		.opcode = {
			.bits = x8616_opcode_jmp_rel8,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_jmp,
	},

	/* JMP r/m16: FF /4 */ {
		.opcode = {
			.bits = x8616_opcode_group_ff,
			.mask = X8616_OPCODE_MASK,
		},
		.mod_rm = {
			.bits = x8616_ff_jmp_near << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.op = x8616_op_jmp,
	},

	/* JMP far ptr16:16 */ {
		.opcode = {
			.bits = x8616_opcode_jmp_far,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_far_ptr },
		.flags = x8616_encoding_far,
		.op = x8616_op_jmp,
	},

	/* JMP far r/m16: FF /5 */ {
		.opcode = {
			.bits = x8616_opcode_group_ff,
			.mask = X8616_OPCODE_MASK,
		},
		.mod_rm = {
			.bits = x8616_ff_jmp_far << X8616_MODRM_REG_SHIFT,
			.mask = X8616_MODRM_REG_MASK,
		},
		.operands = { x8616_operand_rm },
		.width = x8616_width_word,
		.flags = x8616_encoding_far,
		.op = x8616_op_jmp,
	},

	/* RET */ {
		.opcode = {
			.bits = x8616_opcode_ret,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_ret,
	},

	/* RET imm16 */ {
		.opcode = {
			.bits = x8616_opcode_ret_i,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_imm16 },
		.op = x8616_op_ret,
	},

	/* RETF */ {
		.opcode = {
			.bits = x8616_opcode_retf,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_retf,
	},

	/* RETF imm16 */ {
		.opcode = {
			.bits = x8616_opcode_retf_i,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_imm16 },
		.op = x8616_op_retf,
	},

	/* Jcc rel8: 0111 cccc */ {
		.opcode = {
			.bits = x8616_opcode_jcc << X8616_OPCODE_CC_PREFIX_SHIFT,
			.mask = x8616_stem_mask(X8616_OPCODE_CC_PREFIX_SHIFT),
		},
		.fields = {
			.cc = { X8616_OPCODE_CC_SHIFT, X8616_OPCODE_CC_WIDTH },
		},
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_invalid,
	},


	/* LOOPNZ rel8 */ {
		.opcode = {
			.bits = x8616_opcode_loopnz,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_loopnz,
	},

	/* LOOPZ rel8 */ {
		.opcode = {
			.bits = x8616_opcode_loopz,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_loopz,
	},

	/* LOOP rel8 */ {
		.opcode = {
			.bits = x8616_opcode_loop,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_loop,
	},

	/* JCXZ rel8 */ {
		.opcode = {
			.bits = x8616_opcode_jcxz,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_rel8 },
		.op = x8616_op_jcxz,
	},

	// Interrupt / machine control--------------------------------------------

	/* INT imm8 */ {
		.opcode = {
			.bits = x8616_opcode_int,
			.mask = X8616_OPCODE_MASK,
		},
		.operands = { x8616_operand_imm8 },
		.op = x8616_op_int,
	},

	/* INT3 */ {
		.opcode = {
			.bits = x8616_opcode_int3,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_int3,
	},

	/* INTO */ {
		.opcode = {
			.bits = x8616_opcode_into,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_into,
	},

	/* IRET */ {
		.opcode = {
			.bits = x8616_opcode_iret,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_iret,
	},

	/* CLC */ {
		.opcode = {
			.bits = x8616_opcode_clc,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_clc,
	},

	/* CMC */ {
		.opcode = {
			.bits = x8616_opcode_cmc,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_cmc,
	},

	/* STC */ {
		.opcode = {
			.bits = x8616_opcode_stc,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_stc,
	},

	/* CLD */ {
		.opcode = {
			.bits = x8616_opcode_cld,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_cld,
	},

	/* STD */ {
		.opcode = {
			.bits = x8616_opcode_std,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_std,
	},

	/* CLI */ {
		.opcode = {
			.bits = x8616_opcode_cli,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_cli,
	},

	/* STI */ {
		.opcode = {
			.bits = x8616_opcode_sti,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_sti,
	},

	/* HLT */ {
		.opcode = {
			.bits = x8616_opcode_hlt,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_hlt,
	},

	/* WAIT */ {
		.opcode = {
			.bits = x8616_opcode_wait,
			.mask = X8616_OPCODE_MASK,
		},
		.op = x8616_op_wait,
	},

};

enum {
	X8616_ENCODING_COUNT = S_(x8616_encodings) / S_(x8616_encodings[0]),
};
