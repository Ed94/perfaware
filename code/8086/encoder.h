#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "duffle/dsl.h"
#endif

/* Intel 8086 Family User's Manual, Oct 1979 (9800722-03) ch.4 p.4-18,
   Figure 4-20, Table 4-11, Table 4-12.

   Opcode  = the identifying bits in Table 4-12 (width varies: 4, 5, 6, 7, or 8).
             Not the first instruction byte.
   Fields  = d, w, s, v, z, reg, sr (Table 4-11). Packed into that same byte.
   Header  = byte 1 as stored: opcode bits sitting in position, field bits OR'd in.

   BytePattern matches a header byte: .bits = opcode << field_width (fields zero),
   .mask = bits of the header that are opcode. */
typedef Struct_(X8616_BytePattern) { U1 bits;  U1 mask; };
typedef Struct_(X8616_BitField)    { U1 shift; U1 width; };

typedef Struct_(X8616_OpcodeFields) {
	X8616_BitField d;
	X8616_BitField w;
	X8616_BitField s;
	X8616_BitField v;
	X8616_BitField z;
	X8616_BitField reg;
	X8616_BitField sr;
	X8616_BitField alu;
	X8616_BitField cc;
	X8616_BitField pair;
};

#define x8616_field_mask(shift, width)        u1_(((1u << (width)) - 1u) << (shift))
#define x8616_header_opcode_mask(field_width) u1_(~((1u << (field_width)) - 1u))
#define x8616_opc(opcode, field_width)        ((X8616_BytePattern){ u1_((opcode) << (field_width)), x8616_header_opcode_mask(field_width) })
#define x8616_opc_byte(opcode)                x8616_opc((opcode), 0)

typedef Enum_(U1, X8616_Op) {
	x8616_op_invalid = 0x00,
	x8616_op_nop     = 0x01,
	x8616_op_mov     = 0x02,
	x8616_op_add     = 0x03,
	x8616_op_or      = 0x04,
	x8616_op_adc     = 0x05,
	x8616_op_sbb     = 0x06,
	x8616_op_and     = 0x07,
	x8616_op_sub     = 0x08,
	x8616_op_xor     = 0x09,
	x8616_op_cmp     = 0x0A,
	x8616_op_push    = 0x0B,
	x8616_op_pop     = 0x0C,
	x8616_op_xchg    = 0x0D,
	x8616_op_in      = 0x0E,
	x8616_op_out     = 0x0F,
	x8616_op_xlat    = 0x10,
	x8616_op_lea     = 0x11,
	x8616_op_lds     = 0x12,
	x8616_op_les     = 0x13,
	x8616_op_lahf    = 0x14,
	x8616_op_sahf    = 0x15,
	x8616_op_pushf   = 0x16,
	x8616_op_popf    = 0x17,
	x8616_op_inc     = 0x18,
	x8616_op_aaa     = 0x19,
	x8616_op_daa     = 0x1A,
	x8616_op_dec     = 0x1B,
	x8616_op_neg     = 0x1C,
	x8616_op_aas     = 0x1D,
	x8616_op_das     = 0x1E,
	x8616_op_mul     = 0x1F,
	x8616_op_imul    = 0x20,
	x8616_op_aam     = 0x21,
	x8616_op_div     = 0x22,
	x8616_op_idiv    = 0x23,
	x8616_op_aad     = 0x24,
	x8616_op_cbw     = 0x25,
	x8616_op_cwd     = 0x26,
	x8616_op_not     = 0x27,
	x8616_op_shl     = 0x28,
	x8616_op_shr     = 0x29,
	x8616_op_sar     = 0x2A,
	x8616_op_rol     = 0x2B,
	x8616_op_ror     = 0x2C,
	x8616_op_rcl     = 0x2D,
	x8616_op_rcr     = 0x2E,
	x8616_op_test    = 0x2F,
	x8616_op_rep     = 0x30,
	x8616_op_movs    = 0x31,
	x8616_op_cmps    = 0x32,
	x8616_op_scas    = 0x33,
	x8616_op_lods    = 0x34,
	x8616_op_stos    = 0x35,
	x8616_op_call    = 0x36,
	x8616_op_jmp     = 0x37,
	x8616_op_ret     = 0x38,
	x8616_op_retf    = 0x39,
	x8616_op_je      = 0x3A,
	x8616_op_jl      = 0x3B,
	x8616_op_jle     = 0x3C,
	x8616_op_jb      = 0x3D,
	x8616_op_jbe     = 0x3E,
	x8616_op_jp      = 0x3F,
	x8616_op_jo      = 0x40,
	x8616_op_js      = 0x41,
	x8616_op_jne     = 0x42,
	x8616_op_jnl     = 0x43,
	x8616_op_jg      = 0x44,
	x8616_op_jnb     = 0x45,
	x8616_op_ja      = 0x46,
	x8616_op_jnp     = 0x47,
	x8616_op_jno     = 0x48,
	x8616_op_jns     = 0x49,
	x8616_op_loop    = 0x4A,
	x8616_op_loopz   = 0x4B,
	x8616_op_loopnz  = 0x4C,
	x8616_op_jcxz    = 0x4D,
	x8616_op_int     = 0x4E,
	x8616_op_int3    = 0x4F,
	x8616_op_into    = 0x50,
	x8616_op_iret    = 0x51,
	x8616_op_clc     = 0x52,
	x8616_op_cmc     = 0x53,
	x8616_op_stc     = 0x54,
	x8616_op_cld     = 0x55,
	x8616_op_std     = 0x56,
	x8616_op_cli     = 0x57,
	x8616_op_sti     = 0x58,
	x8616_op_hlt     = 0x59,
	x8616_op_wait    = 0x5A,
	x8616_op_lock    = 0x5B,
	x8616_op_segment = 0x5C,
};

typedef Enum_(U1, X8616_Operand) {
	x8616_operand_none           = 0x00,
	x8616_operand_rm             = 0x01,
	x8616_operand_reg_modrm      = 0x02,
	x8616_operand_reg_opcode     = 0x03,
	x8616_operand_segment_modrm  = 0x04,
	x8616_operand_segment_opcode = 0x05,
	x8616_operand_acc            = 0x06,
	x8616_operand_imm            = 0x07,
	x8616_operand_imm8           = 0x08,
	x8616_operand_imm16          = 0x09,
	x8616_operand_mem_direct     = 0x0A,
	x8616_operand_rel8           = 0x0B,
	x8616_operand_rel16          = 0x0C,
	x8616_operand_far_ptr        = 0x0D,
	x8616_operand_dx             = 0x0E,
	x8616_operand_shift_count    = 0x0F,
};

typedef Enum_(U1, X8616_WidthMode) {
	x8616_width_dynamic = 0x0,
	x8616_width_byte    = 0x1,
	x8616_width_word    = 0x2,
};

typedef Enum_(U1, X8616_EncodingFlags) {
	x8616_encoding_none   = 0b00000000,
	x8616_encoding_far    = 0b00000001,
	x8616_encoding_prefix = 0b00000010,
};

typedef Enum_(U1, X8616_DigitKind) {
	x8616_digit_none   = 0x0,
	x8616_digit_alu    = 0x1,
	x8616_digit_shift  = 0x2,
	x8616_digit_g3     = 0x3,
	x8616_digit_ff     = 0x4,
	x8616_digit_incdec = 0x5,
};

typedef Enum_(U1, X8616_PairKind) {
	x8616_pair_none    = 0x0,
	x8616_pair_incdec  = 0x1,
	x8616_pair_pushpop = 0x2,
};

/* header: Byte 1 match (opcode bits aligned, field bits masked). Not the opcode.
    mod_rm: Optional constraint on the ModR/M byte.
    post_opcode: Optional fixed byte immediately after byte 1 (AAM/AAD imm).
    fields: Table 4-11 fields packed into byte 1. width == 0: field is absent.
    operands: Operand order describes d == 0.
      If d exists and decodes to 1, operands[0] and operands[1] are swapped.
    width: Explicit width only when the encoding has no w field.
    flags: Encoding metadata not represented by an explicit operand. */
typedef Struct_(X8616_Encoding) {
	X8616_BytePattern   header;
	X8616_BytePattern   mod_rm;
	X8616_BytePattern   post_opcode;
	X8616_OpcodeFields  fields;
	X8616_Operand       operands[2];
	X8616_WidthMode     width;
	X8616_EncodingFlags flags;
	X8616_DigitKind     digit_kind;
	X8616_PairKind      pair_kind;
	X8616_Op            op;
};

typedef Enum_(U1, X8616_Width) {
	x8616_w_byte = 0b0,
	x8616_w_word = 0b1,
};

typedef Enum_(U1, X8616_Direction) {
	x8616_d_rm_dst  = 0b0,
	x8616_d_reg_dst = 0b1,
	x8616_d_acc_dst = x8616_d_rm_dst,
	x8616_d_mem_dst = x8616_d_reg_dst,
	x8616_d_seg_dst = x8616_d_reg_dst,
};

typedef Enum_(U1, X8616_Sign) {
	x8616_s_full   = 0b0,
	x8616_s_extend = 0b1,
};

typedef Enum_(U1, X8616_VariableShift) {
	x8616_v_one = 0b0,
	x8616_v_cl  = 0b1,
};

typedef Enum_(U1, X8616_Repeat) {
	x8616_repne = 0b0,
	x8616_rep   = 0b1,
};

typedef Enum_(U1, X8616_Mod) {
	x8616_mod_mem     = 0b00,
	x8616_mod_mem_d8  = 0b01,
	x8616_mod_mem_d16 = 0b10,
	x8616_mod_reg     = 0b11,
};

typedef Enum_(U1, X8616_Reg8) {
	x8616_al = 0b000,
	x8616_cl = 0b001,
	x8616_dl = 0b010,
	x8616_bl = 0b011,
	x8616_ah = 0b100,
	x8616_ch = 0b101,
	x8616_dh = 0b110,
	x8616_bh = 0b111,
};

typedef Enum_(U1, X8616_Reg16) {
	x8616_ax = 0b000,
	x8616_cx = 0b001,
	x8616_dx = 0b010,
	x8616_bx = 0b011,
	x8616_sp = 0b100,
	x8616_bp = 0b101,
	x8616_si = 0b110,
	x8616_di = 0b111,
};

typedef Enum_(U1, X8616_Segment) {
	x8616_es = 0b00,
	x8616_cs = 0b01,
	x8616_ss = 0b10,
	x8616_ds = 0b11,
};

/* Table 4-12 B(110) / B(111) beside SR. Not opcode, not the SR field. */
typedef Enum_(U1, X8616_SRLow) {
	x8616_sr_low_push = 0b110,
	x8616_sr_low_pop  = 0b111,
};

typedef Enum_(U1, X8616_EA) {
	x8616_ea_bx_si  = 0b000,
	x8616_ea_bx_di  = 0b001,
	x8616_ea_bp_si  = 0b010,
	x8616_ea_bp_di  = 0b011,
	x8616_ea_si     = 0b100,
	x8616_ea_di     = 0b101,
	x8616_ea_bp     = 0b110, // mod 01/10
	x8616_ea_direct = 0b110, // mod 00
	x8616_ea_bx     = 0b111,
};

typedef Enum_(U1, X8616_ALU) {
	x8616_add = 0b000,
	x8616_or  = 0b001,
	x8616_adc = 0b010,
	x8616_sbb = 0b011,
	x8616_and = 0b100,
	x8616_sub = 0b101,
	x8616_xor = 0b110,
	x8616_cmp = 0b111,
};

RO_ global X8616_Op x8616_op_from_alu[] = {
	[x8616_add] = x8616_op_add,
	[x8616_or]  = x8616_op_or,
	[x8616_adc] = x8616_op_adc,
	[x8616_sbb] = x8616_op_sbb,
	[x8616_and] = x8616_op_and,
	[x8616_sub] = x8616_op_sub,
	[x8616_xor] = x8616_op_xor,
	[x8616_cmp] = x8616_op_cmp,
};

typedef Enum_(U1, X8616_Group3) {
	x8616_g3_test = 0b000,
	x8616_g3_not  = 0b010,
	x8616_g3_neg  = 0b011,
	x8616_g3_mul  = 0b100,
	x8616_g3_imul = 0b101,
	x8616_g3_div  = 0b110,
	x8616_g3_idiv = 0b111,
};

typedef Enum_(U1, X8616_IncDec) {
	x8616_inc = 0b000,
	x8616_dec = 0b001,
};

typedef Enum_(U1, X8616_GroupFF) {
	x8616_ff_call_near = 0b010,
	x8616_ff_call_far  = 0b011,
	x8616_ff_jmp_near  = 0b100,
	x8616_ff_jmp_far   = 0b101,
	x8616_ff_push      = 0b110,
};

typedef Enum_(U1, X8616_Shift) {
	x8616_rol = 0b000,
	x8616_ror = 0b001,
	x8616_rcl = 0b010,
	x8616_rcr = 0b011,
	x8616_shl = 0b100,
	x8616_shr = 0b101,
	x8616_sar = 0b111,
};

RO_ global X8616_Op x8616_op_from_shift[] = {
	[x8616_rol] = x8616_op_rol,
	[x8616_ror] = x8616_op_ror,
	[x8616_rcl] = x8616_op_rcl,
	[x8616_rcr] = x8616_op_rcr,
	[x8616_shl] = x8616_op_shl,
	[x8616_shr] = x8616_op_shr,
	[0b110]     = x8616_op_invalid,
	[x8616_sar] = x8616_op_sar,
};

RO_ global X8616_Op x8616_op_from_g3[] = {
	[x8616_g3_test] = x8616_op_test,
	[0b001]         = x8616_op_invalid,
	[x8616_g3_not]  = x8616_op_not,
	[x8616_g3_neg]  = x8616_op_neg,
	[x8616_g3_mul]  = x8616_op_mul,
	[x8616_g3_imul] = x8616_op_imul,
	[x8616_g3_div]  = x8616_op_div,
	[x8616_g3_idiv] = x8616_op_idiv,
};

RO_ global X8616_Op x8616_op_from_ff[] = {
	[0b000]              = x8616_op_invalid,
	[0b001]              = x8616_op_invalid,
	[x8616_ff_call_near] = x8616_op_call,
	[x8616_ff_call_far]  = x8616_op_call,
	[x8616_ff_jmp_near]  = x8616_op_jmp,
	[x8616_ff_jmp_far]   = x8616_op_jmp,
	[x8616_ff_push]      = x8616_op_push,
	[0b111]              = x8616_op_invalid,
};

RO_ global X8616_Op x8616_op_from_incdec[] = {
	[x8616_inc] = x8616_op_inc,
	[x8616_dec] = x8616_op_dec,
};

typedef Enum_(U1, X8616_PushPop) {
	x8616_pushpop_push = 0b0,
	x8616_pushpop_pop  = 0b1,
};

RO_ global X8616_Op x8616_op_from_pushpop[] = {
	[x8616_pushpop_push] = x8616_op_push,
	[x8616_pushpop_pop]  = x8616_op_pop,
};

typedef Enum_(U1, X8616_Condition) {
	x8616_cc_o   = 0b0000,
	x8616_cc_no  = 0b0001,
	x8616_cc_b   = 0b0010,
	x8616_cc_nb  = 0b0011,
	x8616_cc_e   = 0b0100,
	x8616_cc_ne  = 0b0101,
	x8616_cc_be  = 0b0110,
	x8616_cc_a   = 0b0111,
	x8616_cc_s   = 0b1000,
	x8616_cc_ns  = 0b1001,
	x8616_cc_p   = 0b1010,
	x8616_cc_np  = 0b1011,
	x8616_cc_l   = 0b1100,
	x8616_cc_nl  = 0b1101,
	x8616_cc_le  = 0b1110,
	x8616_cc_g   = 0b1111,
};

RO_ global X8616_Op x8616_op_from_cc[] = {
	[x8616_cc_o]  = x8616_op_jo,
	[x8616_cc_no] = x8616_op_jno,
	[x8616_cc_b]  = x8616_op_jb,
	[x8616_cc_nb] = x8616_op_jnb,
	[x8616_cc_e]  = x8616_op_je,
	[x8616_cc_ne] = x8616_op_jne,
	[x8616_cc_be] = x8616_op_jbe,
	[x8616_cc_a]  = x8616_op_ja,
	[x8616_cc_s]  = x8616_op_js,
	[x8616_cc_ns] = x8616_op_jns,
	[x8616_cc_p]  = x8616_op_jp,
	[x8616_cc_np] = x8616_op_jnp,
	[x8616_cc_l]  = x8616_op_jl,
	[x8616_cc_nl] = x8616_op_jnl,
	[x8616_cc_le] = x8616_op_jle,
	[x8616_cc_g]  = x8616_op_jg,
};

typedef Enum_(U1, X8616_Digit) {
	x8616_digit_0 = 0b000,
};

/* Table 4-12 identifying bits. Width is however many bits Intel printed
   before d/w/reg/cccc. Not a constructed header byte.
   INC register = 01000, not 0x40. HLT = 11110100 (opcode fills byte 1). */
typedef Enum_(U1, X8616_Opcode) {
	x8616_opcode_mov_rm_r     = 0b100010,
	x8616_opcode_mov_rm_i     = 0b1100011,
	x8616_opcode_mov_r_i      = 0b1011,
	x8616_opcode_mov_acc_mem  = 0b101000,
	x8616_opcode_mov_seg_rm   = 0b100011,

	x8616_opcode_push_reg     = 0b01010,
	x8616_opcode_pop_reg      = 0b01011,
	x8616_opcode_xchg_rm_r    = 0b1000011,
	x8616_opcode_xchg_ax_reg  = 0b10010,

	x8616_opcode_in_i         = 0b1110010,
	x8616_opcode_in_dx        = 0b1110110,
	x8616_opcode_out_i        = 0b1110011,
	x8616_opcode_out_dx       = 0b1110111,

	x8616_opcode_alu_rm_i     = 0b100000,
	x8616_opcode_incdec_rm    = 0b1111111,
	x8616_opcode_inc_reg      = 0b01000,
	x8616_opcode_dec_reg      = 0b01001,
	x8616_opcode_group3       = 0b1111011,
	x8616_opcode_shift_rm     = 0b110100,
	x8616_opcode_test_rm_r    = 0b1000010,
	x8616_opcode_test_acc_i   = 0b1010100,

	x8616_opcode_rep          = 0b1111001,
	x8616_opcode_movs         = 0b1010010,
	x8616_opcode_cmps         = 0b1010011,
	x8616_opcode_scas         = 0b1010111,
	x8616_opcode_lods         = 0b1010110,
	x8616_opcode_stos         = 0b1010101,

	x8616_opcode_jcc          = 0b0111,

	/* Table 4-12: B(000) SR B(110/111), B(001) SR B(110). Class bits only. */
	x8616_opcode_sr_stack     = 0b000,
	x8616_opcode_sr_override  = 0b001,

	x8616_opcode_pop_rm     = 0b10001111,
	x8616_opcode_xlat       = 0b11010111,
	x8616_opcode_lea        = 0b10001101,
	x8616_opcode_lds        = 0b11000101,
	x8616_opcode_les        = 0b11000100,
	x8616_opcode_lahf       = 0b10011111,
	x8616_opcode_sahf       = 0b10011110,
	x8616_opcode_pushf      = 0b10011100,
	x8616_opcode_popf       = 0b10011101,

	x8616_opcode_aaa        = 0b00110111,
	x8616_opcode_daa        = 0b00100111,
	x8616_opcode_aas        = 0b00111111,
	x8616_opcode_das        = 0b00101111,
	x8616_opcode_aam        = 0b11010100,
	x8616_opcode_aad        = 0b11010101,
	x8616_opcode_cbw        = 0b10011000,
	x8616_opcode_cwd        = 0b10011001,

	x8616_opcode_call_rel16 = 0b11101000,
	x8616_opcode_call_far   = 0b10011010,
	x8616_opcode_jmp_rel16  = 0b11101001,
	x8616_opcode_jmp_rel8   = 0b11101011,
	x8616_opcode_jmp_far    = 0b11101010,
	x8616_opcode_ret        = 0b11000011,
	x8616_opcode_ret_i      = 0b11000010,
	x8616_opcode_retf       = 0b11001011,
	x8616_opcode_retf_i     = 0b11001010,

	x8616_opcode_loopnz     = 0b11100000,
	x8616_opcode_loopz      = 0b11100001,
	x8616_opcode_loop       = 0b11100010,
	x8616_opcode_jcxz       = 0b11100011,

	x8616_opcode_int        = 0b11001101,
	x8616_opcode_int3       = 0b11001100,
	x8616_opcode_into       = 0b11001110,
	x8616_opcode_iret       = 0b11001111,

	x8616_opcode_clc        = 0b11111000,
	x8616_opcode_cmc        = 0b11110101,
	x8616_opcode_stc        = 0b11111001,
	x8616_opcode_cld        = 0b11111100,
	x8616_opcode_std        = 0b11111101,
	x8616_opcode_cli        = 0b11111010,
	x8616_opcode_sti        = 0b11111011,
	x8616_opcode_hlt        = 0b11110100,
	x8616_opcode_wait       = 0b10011011,
	x8616_opcode_lock       = 0b11110000,
};

enum {
	X8616_OPCODE_BIT_WIDTH = 1,
	X8616_BYTE_MASK        = x8616_field_mask(0, 8),

	X8616_POST_OPCODE_AAM_AAD = 0b00001010,

// ModR/M:
//  7 6   5 4 3   2 1 0
// +-----+-------+-------+
// | mod |  reg  |  r/m  |
// +-----+-------+-------+

	X8616_MODRM_RM_SHIFT  = 0,
	X8616_MODRM_RM_WIDTH  = 3,
	X8616_MODRM_REG_SHIFT = 3,
	X8616_MODRM_REG_WIDTH = 3,
	X8616_MODRM_MOD_SHIFT = 6,
	X8616_MODRM_MOD_WIDTH = 2,
	X8616_MODRM_RM_MASK   = x8616_field_mask(X8616_MODRM_RM_SHIFT,  X8616_MODRM_RM_WIDTH),
	X8616_MODRM_REG_MASK  = x8616_field_mask(X8616_MODRM_REG_SHIFT, X8616_MODRM_REG_WIDTH),
	X8616_MODRM_MOD_MASK  = x8616_field_mask(X8616_MODRM_MOD_SHIFT, X8616_MODRM_MOD_WIDTH),

	X8616_MODRM_SR_SHIFT       = 3,
	X8616_MODRM_SR_WIDTH       = 2,
	X8616_MODRM_SR_MASK        = x8616_field_mask(X8616_MODRM_SR_SHIFT, X8616_MODRM_SR_WIDTH),
	X8616_MODRM_SEG_FIXED_BIT  = 5,
	X8616_MODRM_SEG_FIXED_MASK = x8616_field_mask(X8616_MODRM_SEG_FIXED_BIT, 1),

// ... d w

	X8616_OPCODE_DW_W_SHIFT      = 0,
	X8616_OPCODE_DW_D_SHIFT      = 1,
	X8616_OPCODE_DW_HOLE = X8616_OPCODE_DW_D_SHIFT + 1,
	X8616_OPCODE_DW_OPCODE_MASK  = x8616_header_opcode_mask(X8616_OPCODE_DW_HOLE),

// ... s w

	X8616_OPCODE_SW_W_SHIFT      = 0,
	X8616_OPCODE_SW_S_SHIFT      = 1,
	X8616_OPCODE_SW_HOLE = X8616_OPCODE_SW_S_SHIFT + 1,
	X8616_OPCODE_SW_OPCODE_MASK  = x8616_header_opcode_mask(X8616_OPCODE_SW_HOLE),

// ... v w

	X8616_OPCODE_VW_W_SHIFT      = 0,
	X8616_OPCODE_VW_V_SHIFT      = 1,
	X8616_OPCODE_VW_HOLE = X8616_OPCODE_VW_V_SHIFT + 1,
	X8616_OPCODE_VW_OPCODE_MASK  = x8616_header_opcode_mask(X8616_OPCODE_VW_HOLE),

// ... z

	X8616_OPCODE_Z_Z_SHIFT      = 0,
	X8616_OPCODE_Z_HOLE = X8616_OPCODE_Z_Z_SHIFT + 1,
	X8616_OPCODE_Z_OPCODE_MASK  = x8616_header_opcode_mask(X8616_OPCODE_Z_HOLE),

// ... w

	X8616_OPCODE_W_W_SHIFT      = 0,
	X8616_OPCODE_W_HOLE = X8616_OPCODE_W_W_SHIFT + 1,
	X8616_OPCODE_W_OPCODE_MASK  = x8616_header_opcode_mask(X8616_OPCODE_W_HOLE),

// ... reg

	X8616_OPCODE_REG_REG_SHIFT    = 0,
	X8616_OPCODE_REG_REG_WIDTH    = 3,
	X8616_OPCODE_REG_REG_MASK     = x8616_field_mask(X8616_OPCODE_REG_REG_SHIFT, X8616_OPCODE_REG_REG_WIDTH),
	X8616_OPCODE_REG_HOLE = X8616_OPCODE_REG_REG_SHIFT + X8616_OPCODE_REG_REG_WIDTH,
	X8616_OPCODE_REG_OPCODE_MASK  = x8616_header_opcode_mask(X8616_OPCODE_REG_HOLE),

// ... w reg

	X8616_OPCODE_WREG_REG_SHIFT    = 0,
	X8616_OPCODE_WREG_REG_WIDTH    = 3,
	X8616_OPCODE_WREG_REG_MASK     = x8616_field_mask(X8616_OPCODE_WREG_REG_SHIFT, X8616_OPCODE_WREG_REG_WIDTH),
	X8616_OPCODE_WREG_W_SHIFT      = X8616_OPCODE_WREG_REG_SHIFT + X8616_OPCODE_WREG_REG_WIDTH,
	X8616_OPCODE_WREG_W_MASK       = x8616_field_mask(X8616_OPCODE_WREG_W_SHIFT, 1),
	X8616_OPCODE_WREG_HOLE = X8616_OPCODE_WREG_W_SHIFT + 1,
	X8616_OPCODE_WREG_OPCODE_MASK  = x8616_header_opcode_mask(X8616_OPCODE_WREG_HOLE),

// ... d 0  (bit 0 fixed, bit 1 = d, bits 7-2 = stem)

	X8616_OPCODE_D0_D_SHIFT      = 1,
	X8616_OPCODE_D0_HOLE = X8616_OPCODE_D0_D_SHIFT + 1,
	X8616_OPCODE_D0_OPCODE_MASK  = x8616_header_opcode_mask(X8616_OPCODE_D0_HOLE) | x8616_field_mask(0, 1),

// class(3) | sr(2) | low(3)

	X8616_OPCODE_SR_LOW_SHIFT    = 0,
	X8616_OPCODE_SR_LOW_WIDTH    = 3,
	X8616_OPCODE_SR_SHIFT        = 3,
	X8616_OPCODE_SR_WIDTH        = 2,
	X8616_OPCODE_SR_CLASS_SHIFT  = 5,
	X8616_OPCODE_SR_CLASS_WIDTH  = 3,
	X8616_OPCODE_SR_MASK         = x8616_field_mask(X8616_OPCODE_SR_SHIFT, X8616_OPCODE_SR_WIDTH),
	X8616_OPCODE_SR_PATTERN_MASK = u1_(~X8616_OPCODE_SR_MASK),

// 0111 cccc

	X8616_OPCODE_CC_SHIFT        = 0,
	X8616_OPCODE_CC_WIDTH        = 4,
	X8616_OPCODE_CC_MASK         = x8616_field_mask(X8616_OPCODE_CC_SHIFT, X8616_OPCODE_CC_WIDTH),
	X8616_OPCODE_CC_HOLE = X8616_OPCODE_CC_WIDTH,

// 00 ttt ...

	X8616_OPCODE_ALU_CLASS       = 0b00,
	X8616_OPCODE_ALU_CLASS_SHIFT = 6,
	X8616_OPCODE_ALU_CLASS_WIDTH = 2,
	X8616_OPCODE_ALU_TTT_SHIFT   = 3,
	X8616_OPCODE_ALU_TTT_WIDTH   = 3,
	X8616_OPCODE_ALU_OP_SHIFT    = X8616_OPCODE_ALU_TTT_SHIFT,
	X8616_OPCODE_ALU_BIT2_SHIFT  = 2,
	X8616_OPCODE_ALU_RM_FORM     = 0b0,
	X8616_OPCODE_ALU_ACC_FORM    = 0b10,
	X8616_OPCODE_ALU_ACC_FORM_SHIFT = 1,

	X8616_OPCODE_PAIR_SHIFT = 3,
	X8616_OPCODE_PAIR_WIDTH = 1,
	X8616_OPCODE_PAIR_REG_HOLE = X8616_OPCODE_PAIR_SHIFT + X8616_OPCODE_PAIR_WIDTH,
	X8616_OPCODE_IO_CLASS        = 0b1110,
	X8616_OPCODE_IO_CLASS_SHIFT  = 4,
	X8616_OPCODE_IO_DX_SHIFT     = 3,
	X8616_OPCODE_IO_FIXED_SHIFT  = 2,
	X8616_OPCODE_IO_OUT_SHIFT    = 1,
	X8616_OPCODE_IO_FIXED_FORM   = 0b10,
	X8616_OPCODE_IO_CLASS_STEM_SHIFT = X8616_OPCODE_IO_CLASS_SHIFT - X8616_OPCODE_W_HOLE,
	X8616_OPCODE_IO_DX_STEM_SHIFT    = X8616_OPCODE_IO_DX_SHIFT    - X8616_OPCODE_W_HOLE,
	X8616_OPCODE_IO_OUT_STEM_SHIFT   = X8616_OPCODE_IO_OUT_SHIFT   - X8616_OPCODE_W_HOLE,
};

// ============================================================================
// Encoding Helpers

FI_ U1 x8616_byte_pattern_matches(U1 byte, X8616_BytePattern pattern) { return (byte & pattern.mask) == pattern.bits; }
FI_ U1 x8616_bit_field_exists    (X8616_BitField field)               { return field.width != 0; }
FI_ U1 x8616_bit_field_extract   (U1 byte, X8616_BitField field)      { return (byte >> field.shift) & u1_((1u << field.width) - 1u); }
FI_ U1 x8616_modrm_rm            (U1 modrm)                           { return (modrm & X8616_MODRM_RM_MASK)  >> X8616_MODRM_RM_SHIFT; }
FI_ U1 x8616_modrm_reg           (U1 modrm)                           { return (modrm & X8616_MODRM_REG_MASK) >> X8616_MODRM_REG_SHIFT; }
FI_ U1 x8616_modrm_mod           (U1 modrm)                           { return (modrm & X8616_MODRM_MOD_MASK) >> X8616_MODRM_MOD_SHIFT; }
FI_ U1 x8616_modrm_sr            (U1 modrm)                           { return (modrm & X8616_MODRM_SR_MASK)  >> X8616_MODRM_SR_SHIFT; }

// ============================================================================
// Emission DSL
// `enc_*` produces one encoded byte.
// `emit_*` expands to one or more comma-separated bytes.
//
// Intended use:
// RO_ global U1 code[] = {
//     x8616_mov_r16_i(x8616_ax, 0x1234),
//     x8616_mov_r16_r16(x8616_bx, x8616_ax),
//     x8616_add_r16_r16(x8616_ax, x8616_bx),
//     x8616_jne(-4),
// };
// ============================================================================

// Byte fields

#define x8616_enc_dw_opcode(opcode)   ((opcode) << X8616_OPCODE_DW_HOLE)
#define x8616_enc_sw_opcode(opcode)   ((opcode) << X8616_OPCODE_SW_HOLE)
#define x8616_enc_vw_opcode(opcode)   ((opcode) << X8616_OPCODE_VW_HOLE)
#define x8616_enc_z_opcode(opcode)    ((opcode) << X8616_OPCODE_Z_HOLE)
#define x8616_enc_w_opcode(opcode)    ((opcode) << X8616_OPCODE_W_HOLE)
#define x8616_enc_reg_opcode(opcode)  ((opcode) << X8616_OPCODE_REG_HOLE)
#define x8616_enc_wreg_opcode(opcode) ((opcode) << X8616_OPCODE_WREG_HOLE)
#define x8616_enc_d0_opcode(opcode)   ((opcode) << X8616_OPCODE_D0_HOLE)

#define x8616_enc_d(d)          ((d)  << X8616_OPCODE_DW_D_SHIFT)
#define x8616_enc_s(s)          ((s)  << X8616_OPCODE_SW_S_SHIFT)
#define x8616_enc_v(v)          ((v)  << X8616_OPCODE_VW_V_SHIFT)
#define x8616_enc_z(z)          ((z)  << X8616_OPCODE_Z_Z_SHIFT)
#define x8616_enc_width(w)      ((w)  << X8616_OPCODE_W_W_SHIFT)
#define x8616_enc_wreg_width(w) ((w)  << X8616_OPCODE_WREG_W_SHIFT)
#define x8616_enc_opcode_reg(r) ((r)  << X8616_OPCODE_REG_REG_SHIFT)
#define x8616_enc_opcode_sr(sr) ((sr) << X8616_OPCODE_SR_SHIFT)
#define x8616_enc_cc(cc)        ((cc) << X8616_OPCODE_CC_SHIFT)

#define x8616_enc_modrm_mod(mod) ((mod) << X8616_MODRM_MOD_SHIFT)
#define x8616_enc_modrm_reg(reg) ((reg) << X8616_MODRM_REG_SHIFT)
#define x8616_enc_modrm_rm(rm)   ((rm)  << X8616_MODRM_RM_SHIFT)
#define x8616_enc_modrm_sr(sr)   ((sr)  << X8616_MODRM_SR_SHIFT)

#define x8616_enc_dw(opcode,d,w)       C_(U1, x8616_enc_dw_opcode(opcode)   | x8616_enc_d(d)          | x8616_enc_width(w))
#define x8616_enc_sw(opcode,s,w)       C_(U1, x8616_enc_sw_opcode(opcode)   | x8616_enc_s(s)          | x8616_enc_width(w))
#define x8616_enc_vw(opcode,v,w)       C_(U1, x8616_enc_vw_opcode(opcode)   | x8616_enc_v(v)          | x8616_enc_width(w))
#define x8616_enc_zp(opcode,z)         C_(U1, x8616_enc_z_opcode(opcode)    | x8616_enc_z(z))
#define x8616_enc_w(opcode,w)          C_(U1, x8616_enc_w_opcode(opcode)    | x8616_enc_width(w))
#define x8616_enc_reg(opcode,reg)      C_(U1, x8616_enc_reg_opcode(opcode)  | x8616_enc_opcode_reg(reg))
#define x8616_enc_wreg(opcode,w,reg)   C_(U1, x8616_enc_wreg_opcode(opcode) | x8616_enc_wreg_width(w) | x8616_enc_opcode_reg(reg))
#define x8616_enc_d0(opcode,d)         C_(U1, x8616_enc_d0_opcode(opcode)   | x8616_enc_d(d))
#define x8616_enc_sr(class,sr,low)     C_(U1, ((class) << X8616_OPCODE_SR_CLASS_SHIFT) | x8616_enc_opcode_sr(sr) | (low))
#define x8616_enc_jcc(cc)              C_(U1, (x8616_opcode_jcc << X8616_OPCODE_CC_HOLE) | x8616_enc_cc(cc))
#define x8616_enc_modrm(mod,reg,rm)    C_(U1, x8616_enc_modrm_mod(mod) | x8616_enc_modrm_reg(reg) | x8616_enc_modrm_rm(rm))
#define x8616_enc_modrm_seg(mod,sr,rm) C_(U1, x8616_enc_modrm_mod(mod) | x8616_enc_modrm_sr(sr)   | x8616_enc_modrm_rm(rm))

#define x8616_header_w(opcode,w)   ((X8616_BytePattern){ x8616_enc_w((opcode), (w)), X8616_BYTE_MASK })
#define x8616_header_sr(class,low) ((X8616_BytePattern){ u1_(((class) << X8616_OPCODE_SR_CLASS_SHIFT) | (low)), X8616_OPCODE_SR_PATTERN_MASK })
#define x8616_header_d0(opcode)    ((X8616_BytePattern){ u1_((opcode) << X8616_OPCODE_D0_HOLE), X8616_OPCODE_D0_OPCODE_MASK })

#define x8616_enc_alu_class()              (X8616_OPCODE_ALU_CLASS << X8616_OPCODE_ALU_CLASS_SHIFT)
#define x8616_enc_alu_ttt(ttt)             ((ttt) << X8616_OPCODE_ALU_TTT_SHIFT)
#define x8616_enc_alu_op(alu)              x8616_enc_alu_ttt(alu)
#define x8616_enc_alu_rm_r(ttt,d,w)        C_(U1, x8616_enc_alu_class() | x8616_enc_alu_ttt(ttt) | (X8616_OPCODE_ALU_RM_FORM << X8616_OPCODE_ALU_BIT2_SHIFT) | x8616_enc_d(d) | x8616_enc_width(w))
#define x8616_enc_alu_acc_i(ttt,w)         C_(U1, x8616_enc_alu_class() | x8616_enc_alu_ttt(ttt) | (X8616_OPCODE_ALU_ACC_FORM << X8616_OPCODE_ALU_ACC_FORM_SHIFT) | x8616_enc_width(w))
#define x8616_enc_io_class()               (X8616_OPCODE_IO_CLASS << X8616_OPCODE_IO_CLASS_STEM_SHIFT)
#define x8616_enc_io_dx(dx)                ((dx)  << X8616_OPCODE_IO_DX_STEM_SHIFT)
#define x8616_enc_io_out(out)              ((out) << X8616_OPCODE_IO_OUT_STEM_SHIFT)
#define x8616_enc_io_stem(dx, out)         (x8616_enc_io_class() | x8616_enc_io_dx(dx) | X8616_OPCODE_IO_FIXED_FORM | x8616_enc_io_out(out))
#define x8616_enc_io(dx, out, w)           C_(U1, (x8616_enc_io_stem(dx, out) << X8616_OPCODE_W_HOLE) | x8616_enc_width(w))
#define x8616_enc_pair(bit)                ((bit) << X8616_OPCODE_PAIR_SHIFT)

// Scalar / generic packets

#define x8616_emit_u2(value)              u2_lo(value), u2_hi(value)
#define x8616_emit_s2(value)              x8616_emit_u2(value)
#define x8616_emit_op(opcode)             u1_(opcode)
#define x8616_emit_op_i8(opcode,imm)      u1_(opcode), u1_(imm)
#define x8616_emit_op_i16(opcode,imm)     u1_(opcode), x8616_emit_u2(imm)
#define x8616_emit_op_far(opcode,seg,off) u1_(opcode), x8616_emit_u2(off), x8616_emit_u2(seg)

#define x8616_emit_modrm(opcode,mod,reg,rm)              u1_(opcode), x8616_enc_modrm(mod,reg,rm)
#define x8616_emit_modrm_d8(opcode,reg,rm,disp)          x8616_emit_modrm    (opcode,x8616_mod_mem_d8,reg,rm),           u1_(disp)
#define x8616_emit_modrm_d16(opcode,reg,rm,disp)         x8616_emit_modrm    (opcode,x8616_mod_mem_d16,reg,rm),          x8616_emit_s2(disp)
#define x8616_emit_modrm_direct(opcode,reg,addr)         x8616_emit_modrm    (opcode,x8616_mod_mem,reg,x8616_ea_direct), x8616_emit_u2(addr)
#define x8616_emit_modrm_i8(opcode,mod,reg,rm,imm)       x8616_emit_modrm    (opcode,mod,reg,rm),  u1_(imm)
#define x8616_emit_modrm_i16(opcode,mod,reg,rm,imm)      x8616_emit_modrm    (opcode,mod,reg,rm),  x8616_emit_u2(imm)
#define x8616_emit_modrm_d8_i8(opcode,reg,rm,disp,imm)   x8616_emit_modrm_d8 (opcode,reg,rm,disp), u1_(imm)
#define x8616_emit_modrm_d8_i16(opcode,reg,rm,disp,imm)  x8616_emit_modrm_d8 (opcode,reg,rm,disp), x8616_emit_u2(imm)
#define x8616_emit_modrm_d16_i8(opcode,reg,rm,disp,imm)  x8616_emit_modrm_d16(opcode,reg,rm,disp), u1_(imm)
#define x8616_emit_modrm_d16_i16(opcode,reg,rm,disp,imm) x8616_emit_modrm_d16(opcode,reg,rm,disp), x8616_emit_u2(imm)

// r/m + register / immediate packets

#define x8616_emit_rm_r(opcode,d,w,mod,reg,rm)      x8616_enc_dw   (opcode,d,w), x8616_enc_modrm(mod,reg,rm)
#define x8616_emit_rm_r_d8(opcode,d,w,reg,rm,disp)  x8616_emit_rm_r(opcode,d,w,x8616_mod_mem_d8, reg,rm),          u1_(disp)
#define x8616_emit_rm_r_d16(opcode,d,w,reg,rm,disp) x8616_emit_rm_r(opcode,d,w,x8616_mod_mem_d16,reg,rm),          x8616_emit_s2(disp)
#define x8616_emit_rm_r_direct(opcode,d,w,reg,addr) x8616_emit_rm_r(opcode,d,w,x8616_mod_mem,reg,x8616_ea_direct), x8616_emit_u2(addr)

#define x8616_emit_rm_i8_w(opcode,ext,mod,rm,imm)       x8616_enc_w(opcode,x8616_w_byte), x8616_enc_modrm(mod,ext,rm), u1_(imm)
#define x8616_emit_rm_i16_w(opcode,ext,mod,rm,imm)      x8616_enc_w(opcode,x8616_w_word), x8616_enc_modrm(mod,ext,rm), x8616_emit_u2(imm)
#define x8616_emit_rm_i8_w_d8(opcode,ext,rm,disp,imm)   x8616_enc_w(opcode,x8616_w_byte), x8616_enc_modrm(x8616_mod_mem_d8,ext,rm), u1_(disp), u1_(imm)
#define x8616_emit_rm_i16_w_d8(opcode,ext,rm,disp,imm)  x8616_enc_w(opcode,x8616_w_word), x8616_enc_modrm(x8616_mod_mem_d8,ext,rm), u1_(disp), x8616_emit_u2(imm)
#define x8616_emit_rm_i8_w_d16(opcode,ext,rm,disp,imm)  x8616_enc_w(opcode,x8616_w_byte), x8616_enc_modrm(x8616_mod_mem_d16,ext,rm), x8616_emit_s2(disp), u1_(imm)
#define x8616_emit_rm_i16_w_d16(opcode,ext,rm,disp,imm) x8616_enc_w(opcode,x8616_w_word), x8616_enc_modrm(x8616_mod_mem_d16,ext,rm), x8616_emit_s2(disp), x8616_emit_u2(imm)

#define x8616_emit_rm_i8(opcode,s,w,ext,mod,rm,imm)      x8616_enc_sw(opcode,s,w),                       x8616_enc_modrm(mod,ext,rm),               u1_(imm)
#define x8616_emit_rm_i16(opcode,ext,mod,rm,imm)         x8616_enc_sw(opcode,x8616_s_full,x8616_w_word), x8616_enc_modrm(mod,ext,rm),               x8616_emit_u2(imm)
#define x8616_emit_rm_i8_d8(opcode,s,w,ext,rm,disp,imm)  x8616_enc_sw(opcode,s,w),                       x8616_enc_modrm(x8616_mod_mem_d8,ext,rm),  u1_(disp), u1_(imm)
#define x8616_emit_rm_i16_d8(opcode,ext,rm,disp,imm)     x8616_enc_sw(opcode,x8616_s_full,x8616_w_word), x8616_enc_modrm(x8616_mod_mem_d8,ext,rm),  u1_(disp), x8616_emit_u2(imm)
#define x8616_emit_rm_i8_d16(opcode,s,w,ext,rm,disp,imm) x8616_enc_sw(opcode,s,w),                       x8616_enc_modrm(x8616_mod_mem_d16,ext,rm), x8616_emit_s2(disp), u1_(imm)
#define x8616_emit_rm_i16_d16(opcode,ext,rm,disp,imm)    x8616_enc_sw(opcode,x8616_s_full,x8616_w_word), x8616_enc_modrm(x8616_mod_mem_d16,ext,rm), x8616_emit_s2(disp), x8616_emit_u2(imm)

// Segment-register packet

#define x8616_emit_seg_rm(d,mod,sr,rm)      x8616_enc_d0(x8616_opcode_mov_seg_rm,d),               x8616_enc_modrm_seg(mod,sr,rm)
#define x8616_emit_seg_rm_d8(d,sr,rm,disp)  x8616_emit_seg_rm(d,x8616_mod_mem_d8,sr,rm),           u1_(disp)
#define x8616_emit_seg_rm_d16(d,sr,rm,disp) x8616_emit_seg_rm(d,x8616_mod_mem_d16,sr,rm),          x8616_emit_s2(disp)
#define x8616_emit_seg_rm_direct(d,sr,addr) x8616_emit_seg_rm(d,x8616_mod_mem,sr,x8616_ea_direct), x8616_emit_u2(addr)

// MOV

#define x8616_nop()                 x8616_enc_reg    (x8616_opcode_xchg_ax_reg, x8616_ax)
#define x8616_mov_r8_r8(dst,src)    x8616_emit_rm_r  (x8616_opcode_mov_rm_r,x8616_d_reg_dst,x8616_w_byte,x8616_mod_reg,dst,src)
#define x8616_mov_r16_r16(dst,src)  x8616_emit_rm_r  (x8616_opcode_mov_rm_r,x8616_d_reg_dst,x8616_w_word,x8616_mod_reg,dst,src)
#define x8616_mov_r8_i(dst,imm)     x8616_enc_wreg   (x8616_opcode_mov_r_i,x8616_w_byte,dst),                 u1_(imm)
#define x8616_mov_r16_i(dst,imm)    x8616_enc_wreg   (x8616_opcode_mov_r_i,x8616_w_word,dst),                 x8616_emit_u2(imm)
#define x8616_mov_al_moffs(addr)    x8616_enc_dw     (x8616_opcode_mov_acc_mem,x8616_d_acc_dst,x8616_w_byte), x8616_emit_u2(addr)
#define x8616_mov_ax_moffs(addr)    x8616_enc_dw     (x8616_opcode_mov_acc_mem,x8616_d_acc_dst,x8616_w_word), x8616_emit_u2(addr)
#define x8616_mov_moffs_al(addr)    x8616_enc_dw     (x8616_opcode_mov_acc_mem,x8616_d_mem_dst,x8616_w_byte), x8616_emit_u2(addr)
#define x8616_mov_moffs_ax(addr)    x8616_enc_dw     (x8616_opcode_mov_acc_mem,x8616_d_mem_dst,x8616_w_word), x8616_emit_u2(addr)
#define x8616_mov_rm_seg(mod,rm,sr) x8616_emit_seg_rm(x8616_d_rm_dst,mod,sr,rm)
#define x8616_mov_seg_rm(sr,mod,rm) x8616_emit_seg_rm(x8616_d_seg_dst,mod,sr,rm)

// Stack / exchange

#define x8616_push_r16(reg)   x8616_enc_reg   (x8616_opcode_push_reg,reg)
#define x8616_pop_r16(reg)    x8616_enc_reg   (x8616_opcode_pop_reg,reg)
#define x8616_push_seg(seg)   x8616_enc_sr    (x8616_opcode_sr_stack,seg,x8616_sr_low_push)
#define x8616_pop_seg(seg)    x8616_enc_sr    (x8616_opcode_sr_stack,seg,x8616_sr_low_pop)
#define x8616_push_rm(mod,rm) x8616_emit_modrm(x8616_enc_w(x8616_opcode_incdec_rm,x8616_w_word),mod,x8616_ff_push,rm)
#define x8616_pop_rm(mod,rm)  x8616_emit_modrm(x8616_opcode_pop_rm,mod,x8616_digit_0,rm)

#define x8616_xchg_r8_r8(a,b)   x8616_enc_w(x8616_opcode_xchg_rm_r,x8616_w_byte), x8616_enc_modrm(x8616_mod_reg,a,b)
#define x8616_xchg_r16_r16(a,b) x8616_enc_w(x8616_opcode_xchg_rm_r,x8616_w_word), x8616_enc_modrm(x8616_mod_reg,a,b)
#define x8616_xchg_ax_r16(reg)  x8616_enc_reg(x8616_opcode_xchg_ax_reg,reg)

// I/O / address load

#define x8616_in_al_i(port)  x8616_enc_w(x8616_opcode_in_i,x8616_w_byte), u1_(port)
#define x8616_in_ax_i(port)  x8616_enc_w(x8616_opcode_in_i,x8616_w_word), u1_(port)
#define x8616_in_al_dx()     x8616_enc_w(x8616_opcode_in_dx,x8616_w_byte)
#define x8616_in_ax_dx()     x8616_enc_w(x8616_opcode_in_dx,x8616_w_word)
#define x8616_out_i_al(port) x8616_enc_w(x8616_opcode_out_i,x8616_w_byte), u1_(port)
#define x8616_out_i_ax(port) x8616_enc_w(x8616_opcode_out_i,x8616_w_word), u1_(port)
#define x8616_out_dx_al()    x8616_enc_w(x8616_opcode_out_dx,x8616_w_byte)
#define x8616_out_dx_ax()    x8616_enc_w(x8616_opcode_out_dx,x8616_w_word)

#define x8616_xlat()          x8616_emit_op(x8616_opcode_xlat)
#define x8616_lea(reg,mod,rm) x8616_emit_modrm(x8616_opcode_lea,mod,reg,rm)
#define x8616_lds(reg,mod,rm) x8616_emit_modrm(x8616_opcode_lds,mod,reg,rm)
#define x8616_les(reg,mod,rm) x8616_emit_modrm(x8616_opcode_les,mod,reg,rm)
#define x8616_lahf()          x8616_emit_op(x8616_opcode_lahf)
#define x8616_sahf()          x8616_emit_op(x8616_opcode_sahf)
#define x8616_pushf()         x8616_emit_op(x8616_opcode_pushf)
#define x8616_popf()          x8616_emit_op(x8616_opcode_popf)

// Arithmetic / logical

#define x8616_emit_alu_r8_r8(alu,dst,src)   x8616_enc_alu_rm_r (alu,x8616_d_reg_dst,x8616_w_byte), x8616_enc_modrm(x8616_mod_reg,dst,src)
#define x8616_emit_alu_r16_r16(alu,dst,src) x8616_enc_alu_rm_r (alu,x8616_d_reg_dst,x8616_w_word), x8616_enc_modrm(x8616_mod_reg,dst,src)
#define x8616_emit_alu_r8_i(alu,dst,imm)    x8616_emit_rm_i8   (x8616_opcode_alu_rm_i,x8616_s_full,x8616_w_byte,alu,x8616_mod_reg,dst,imm)
#define x8616_emit_alu_r16_i(alu,dst,imm)   x8616_emit_rm_i16  (x8616_opcode_alu_rm_i,alu,x8616_mod_reg,dst,imm)
#define x8616_emit_alu_r16_i8s(alu,dst,imm) x8616_emit_rm_i8   (x8616_opcode_alu_rm_i,x8616_s_extend,x8616_w_word,alu,x8616_mod_reg,dst,imm)
#define x8616_emit_logic_r8_i(alu,dst,imm)  x8616_emit_rm_i8   (x8616_opcode_alu_rm_i,x8616_s_full,x8616_w_byte,alu,x8616_mod_reg,dst,imm)
#define x8616_emit_logic_r16_i(alu,dst,imm) x8616_emit_rm_i16  (x8616_opcode_alu_rm_i,alu,x8616_mod_reg,dst,imm)
#define x8616_emit_alu_al_i(alu,imm)        x8616_enc_alu_acc_i(alu,x8616_w_byte), u1_(imm)
#define x8616_emit_alu_ax_i(alu,imm)        x8616_enc_alu_acc_i(alu,x8616_w_word), x8616_emit_u2(imm)

#define x8616_add_r8_r8(dst,src)   x8616_emit_alu_r8_r8  (x8616_add,dst,src)
#define x8616_add_r16_r16(dst,src) x8616_emit_alu_r16_r16(x8616_add,dst,src)
#define x8616_or_r8_r8(dst,src)    x8616_emit_alu_r8_r8  (x8616_or,dst,src)
#define x8616_or_r16_r16(dst,src)  x8616_emit_alu_r16_r16(x8616_or,dst,src)
#define x8616_adc_r8_r8(dst,src)   x8616_emit_alu_r8_r8  (x8616_adc,dst,src)
#define x8616_adc_r16_r16(dst,src) x8616_emit_alu_r16_r16(x8616_adc,dst,src)
#define x8616_sbb_r8_r8(dst,src)   x8616_emit_alu_r8_r8  (x8616_sbb,dst,src)
#define x8616_sbb_r16_r16(dst,src) x8616_emit_alu_r16_r16(x8616_sbb,dst,src)
#define x8616_and_r8_r8(dst,src)   x8616_emit_alu_r8_r8  (x8616_and,dst,src)
#define x8616_and_r16_r16(dst,src) x8616_emit_alu_r16_r16(x8616_and,dst,src)
#define x8616_sub_r8_r8(dst,src)   x8616_emit_alu_r8_r8  (x8616_sub,dst,src)
#define x8616_sub_r16_r16(dst,src) x8616_emit_alu_r16_r16(x8616_sub,dst,src)
#define x8616_xor_r8_r8(dst,src)   x8616_emit_alu_r8_r8  (x8616_xor,dst,src)
#define x8616_xor_r16_r16(dst,src) x8616_emit_alu_r16_r16(x8616_xor,dst,src)
#define x8616_cmp_r8_r8(dst,src)   x8616_emit_alu_r8_r8  (x8616_cmp,dst,src)
#define x8616_cmp_r16_r16(dst,src) x8616_emit_alu_r16_r16(x8616_cmp,dst,src)

#define x8616_add_r8_i(dst,imm)    x8616_emit_alu_r8_i   (x8616_add,dst,imm)
#define x8616_add_r16_i(dst,imm)   x8616_emit_alu_r16_i  (x8616_add,dst,imm)
#define x8616_add_r16_i8s(dst,imm) x8616_emit_alu_r16_i8s(x8616_add,dst,imm)
#define x8616_or_r8_i(dst,imm)     x8616_emit_logic_r8_i (x8616_or, dst,imm)
#define x8616_or_r16_i(dst,imm)    x8616_emit_logic_r16_i(x8616_or, dst,imm)
#define x8616_adc_r8_i(dst,imm)    x8616_emit_alu_r8_i   (x8616_adc,dst,imm)
#define x8616_adc_r16_i(dst,imm)   x8616_emit_alu_r16_i  (x8616_adc,dst,imm)
#define x8616_adc_r16_i8s(dst,imm) x8616_emit_alu_r16_i8s(x8616_adc,dst,imm)
#define x8616_sbb_r8_i(dst,imm)    x8616_emit_alu_r8_i   (x8616_sbb,dst,imm)
#define x8616_sbb_r16_i(dst,imm)   x8616_emit_alu_r16_i  (x8616_sbb,dst,imm)
#define x8616_sbb_r16_i8s(dst,imm) x8616_emit_alu_r16_i8s(x8616_sbb,dst,imm)
#define x8616_and_r8_i(dst,imm)    x8616_emit_logic_r8_i (x8616_and,dst,imm)
#define x8616_and_r16_i(dst,imm)   x8616_emit_logic_r16_i(x8616_and,dst,imm)
#define x8616_sub_r8_i(dst,imm)    x8616_emit_alu_r8_i   (x8616_sub,dst,imm)
#define x8616_sub_r16_i(dst,imm)   x8616_emit_alu_r16_i  (x8616_sub,dst,imm)
#define x8616_sub_r16_i8s(dst,imm) x8616_emit_alu_r16_i8s(x8616_sub,dst,imm)
#define x8616_xor_r8_i(dst,imm)    x8616_emit_logic_r8_i (x8616_xor,dst,imm)
#define x8616_xor_r16_i(dst,imm)   x8616_emit_logic_r16_i(x8616_xor,dst,imm)
#define x8616_cmp_r8_i(dst,imm)    x8616_emit_alu_r8_i   (x8616_cmp,dst,imm)
#define x8616_cmp_r16_i(dst,imm)   x8616_emit_alu_r16_i  (x8616_cmp,dst,imm)
#define x8616_cmp_r16_i8s(dst,imm) x8616_emit_alu_r16_i8s(x8616_cmp,dst,imm)

// INC / DEC / unary

#define x8616_inc_r16(reg)     x8616_enc_reg(x8616_opcode_inc_reg,reg)
#define x8616_dec_r16(reg)     x8616_enc_reg(x8616_opcode_dec_reg,reg)
#define x8616_inc_rm(w,mod,rm) x8616_emit_modrm(x8616_enc_w(x8616_opcode_incdec_rm,w),mod,x8616_inc,rm)
#define x8616_dec_rm(w,mod,rm) x8616_emit_modrm(x8616_enc_w(x8616_opcode_incdec_rm,w),mod,x8616_dec,rm)

#define x8616_unary_rm(ext,w,mod,rm) x8616_emit_modrm(x8616_enc_w(x8616_opcode_group3,w),mod,ext,rm)
#define x8616_not_rm(w,mod,rm)       x8616_unary_rm(x8616_g3_not,w,mod,rm)
#define x8616_neg_rm(w,mod,rm)       x8616_unary_rm(x8616_g3_neg,w,mod,rm)
#define x8616_mul_rm(w,mod,rm)       x8616_unary_rm(x8616_g3_mul,w,mod,rm)
#define x8616_imul_rm(w,mod,rm)      x8616_unary_rm(x8616_g3_imul,w,mod,rm)
#define x8616_div_rm(w,mod,rm)       x8616_unary_rm(x8616_g3_div,w,mod,rm)
#define x8616_idiv_rm(w,mod,rm)      x8616_unary_rm(x8616_g3_idiv,w,mod,rm)

#define x8616_aaa() x8616_emit_op   (x8616_opcode_aaa)
#define x8616_daa() x8616_emit_op   (x8616_opcode_daa)
#define x8616_aas() x8616_emit_op   (x8616_opcode_aas)
#define x8616_das() x8616_emit_op   (x8616_opcode_das)
#define x8616_aam() x8616_emit_op_i8(x8616_opcode_aam,X8616_POST_OPCODE_AAM_AAD)
#define x8616_aad() x8616_emit_op_i8(x8616_opcode_aad,X8616_POST_OPCODE_AAM_AAD)
#define x8616_cbw() x8616_emit_op   (x8616_opcode_cbw)
#define x8616_cwd() x8616_emit_op   (x8616_opcode_cwd)

// Shift / rotate / TEST

#define x8616_shift_rm(shift,v,w,mod,rm) x8616_emit_modrm(x8616_enc_vw(x8616_opcode_shift_rm,v,w),mod,shift,rm)
#define x8616_shl_r8_1(reg)              x8616_shift_rm(x8616_shl,x8616_v_one,x8616_w_byte,x8616_mod_reg,reg)
#define x8616_shl_r16_1(reg)             x8616_shift_rm(x8616_shl,x8616_v_one,x8616_w_word,x8616_mod_reg,reg)
#define x8616_shl_r8_cl(reg)             x8616_shift_rm(x8616_shl,x8616_v_cl,x8616_w_byte,x8616_mod_reg,reg)
#define x8616_shl_r16_cl(reg)            x8616_shift_rm(x8616_shl,x8616_v_cl,x8616_w_word,x8616_mod_reg,reg)

#define x8616_test_r8_r8(a,b)     x8616_enc_w(x8616_opcode_test_rm_r,x8616_w_byte), x8616_enc_modrm(x8616_mod_reg,b,a)
#define x8616_test_r16_r16(a,b)   x8616_enc_w(x8616_opcode_test_rm_r,x8616_w_word), x8616_enc_modrm(x8616_mod_reg,b,a)
#define x8616_test_r8_i(reg,imm)  x8616_emit_modrm_i8(x8616_enc_w(x8616_opcode_group3,x8616_w_byte),x8616_mod_reg,x8616_g3_test,reg,imm)
#define x8616_test_r16_i(reg,imm) x8616_emit_modrm_i16(x8616_enc_w(x8616_opcode_group3,x8616_w_word),x8616_mod_reg,x8616_g3_test,reg,imm)
#define x8616_test_al_i(imm)      x8616_enc_w(x8616_opcode_test_acc_i,x8616_w_byte), u1_(imm)
#define x8616_test_ax_i(imm)      x8616_enc_w(x8616_opcode_test_acc_i,x8616_w_word), x8616_emit_u2(imm)

// String / prefixes

#define x8616_rep_prefix()        x8616_enc_zp (x8616_opcode_rep,x8616_rep)
#define x8616_repne_prefix()      x8616_enc_zp (x8616_opcode_rep,x8616_repne)
#define x8616_movsb()             x8616_enc_w  (x8616_opcode_movs,x8616_w_byte)
#define x8616_movsw()             x8616_enc_w  (x8616_opcode_movs,x8616_w_word)
#define x8616_cmpsb()             x8616_enc_w  (x8616_opcode_cmps,x8616_w_byte)
#define x8616_cmpsw()             x8616_enc_w  (x8616_opcode_cmps,x8616_w_word)
#define x8616_scasb()             x8616_enc_w  (x8616_opcode_scas,x8616_w_byte)
#define x8616_scasw()             x8616_enc_w  (x8616_opcode_scas,x8616_w_word)
#define x8616_lodsb()             x8616_enc_w  (x8616_opcode_lods,x8616_w_byte)
#define x8616_lodsw()             x8616_enc_w  (x8616_opcode_lods,x8616_w_word)
#define x8616_stosb()             x8616_enc_w  (x8616_opcode_stos,x8616_w_byte)
#define x8616_stosw()             x8616_enc_w  (x8616_opcode_stos,x8616_w_word)
#define x8616_lock_prefix()       x8616_emit_op(x8616_opcode_lock)
#define x8616_segment_prefix(seg) x8616_enc_sr (x8616_opcode_sr_override,seg,x8616_sr_low_push)

// Control transfer

#define x8616_call_rel16(rel)     x8616_emit_op_i16(x8616_opcode_call_rel16,rel)
#define x8616_call_far(seg,off)   x8616_emit_op_far(x8616_opcode_call_far,seg,off)
#define x8616_call_rm(mod,rm)     x8616_emit_modrm(x8616_enc_w(x8616_opcode_incdec_rm,x8616_w_word),mod,x8616_ff_call_near,rm)
#define x8616_call_far_rm(mod,rm) x8616_emit_modrm(x8616_enc_w(x8616_opcode_incdec_rm,x8616_w_word),mod,x8616_ff_call_far,rm)

#define x8616_jmp_rel16(rel)     x8616_emit_op_i16(x8616_opcode_jmp_rel16,rel)
#define x8616_jmp_rel8(rel)      x8616_emit_op_i8 (x8616_opcode_jmp_rel8,rel)
#define x8616_jmp_far(seg,off)   x8616_emit_op_far(x8616_opcode_jmp_far,seg,off)
#define x8616_jmp_rm(mod,rm)     x8616_emit_modrm(x8616_enc_w(x8616_opcode_incdec_rm,x8616_w_word),mod,x8616_ff_jmp_near,rm)
#define x8616_jmp_far_rm(mod,rm) x8616_emit_modrm(x8616_enc_w(x8616_opcode_incdec_rm,x8616_w_word),mod,x8616_ff_jmp_far,rm)

#define x8616_ret()         x8616_emit_op    (x8616_opcode_ret)
#define x8616_ret_i(bytes)  x8616_emit_op_i16(x8616_opcode_ret_i,bytes)
#define x8616_retf()        x8616_emit_op    (x8616_opcode_retf)
#define x8616_retf_i(bytes) x8616_emit_op_i16(x8616_opcode_retf_i,bytes)

#define x8616_jcc(cc,rel) x8616_enc_jcc(cc), u1_(rel)
#define x8616_jo(rel)     x8616_jcc(x8616_cc_o,rel)
#define x8616_jno(rel)    x8616_jcc(x8616_cc_no,rel)
#define x8616_jb(rel)     x8616_jcc(x8616_cc_b,rel)
#define x8616_jnb(rel)    x8616_jcc(x8616_cc_nb,rel)
#define x8616_je(rel)     x8616_jcc(x8616_cc_e,rel)
#define x8616_jne(rel)    x8616_jcc(x8616_cc_ne,rel)
#define x8616_jbe(rel)    x8616_jcc(x8616_cc_be,rel)
#define x8616_ja(rel)     x8616_jcc(x8616_cc_a,rel)
#define x8616_js(rel)     x8616_jcc(x8616_cc_s,rel)
#define x8616_jns(rel)    x8616_jcc(x8616_cc_ns,rel)
#define x8616_jp(rel)     x8616_jcc(x8616_cc_p,rel)
#define x8616_jnp(rel)    x8616_jcc(x8616_cc_np,rel)
#define x8616_jl(rel)     x8616_jcc(x8616_cc_l,rel)
#define x8616_jnl(rel)    x8616_jcc(x8616_cc_nl,rel)
#define x8616_jle(rel)    x8616_jcc(x8616_cc_le,rel)
#define x8616_jg(rel)     x8616_jcc(x8616_cc_g,rel)

#define x8616_loopnz(rel) x8616_emit_op_i8(x8616_opcode_loopnz,rel)
#define x8616_loopz(rel)  x8616_emit_op_i8(x8616_opcode_loopz,rel)
#define x8616_loop(rel)   x8616_emit_op_i8(x8616_opcode_loop,rel)
#define x8616_jcxz(rel)   x8616_emit_op_i8(x8616_opcode_jcxz,rel)

// Interrupt / flags / machine control

#define x8616_int(vector) x8616_emit_op_i8(x8616_opcode_int,vector)
#define x8616_int3()      x8616_emit_op(x8616_opcode_int3)
#define x8616_into()      x8616_emit_op(x8616_opcode_into)
#define x8616_iret()      x8616_emit_op(x8616_opcode_iret)
#define x8616_clc()       x8616_emit_op(x8616_opcode_clc)
#define x8616_cmc()       x8616_emit_op(x8616_opcode_cmc)
#define x8616_stc()       x8616_emit_op(x8616_opcode_stc)
#define x8616_cld()       x8616_emit_op(x8616_opcode_cld)
#define x8616_std()       x8616_emit_op(x8616_opcode_std)
#define x8616_cli()       x8616_emit_op(x8616_opcode_cli)
#define x8616_sti()       x8616_emit_op(x8616_opcode_sti)
#define x8616_hlt()       x8616_emit_op(x8616_opcode_hlt)
#define x8616_wait()      x8616_emit_op(x8616_opcode_wait)
