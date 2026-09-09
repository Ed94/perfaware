#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#endif

#define asm_out     /* outputs  */
#define asm_in      /* inputs   */
#define asm_clobber /* clobbers */

#define asm_out_r(name)  [name] "=r"(name)
#define asm_in_r(name)   [name] "r"(name)
#define asm_out_x(name)  [name] "=&x"(name) /* XMM, earlyclobber */
#define asm_out_x0(name) [name] "=x"(name)  /* XMM, no earlyclobber */
#define asm_in_x(name)   [name] "x"(name)

#define x64_r(name)   "%[" #name "]"
#define x64_m(name)   "(%[" #name "])"
#define x64_xmm(name) "%x[" #name "]"

typedef U8 U8x2  attribute(vector_size(16));
typedef U8 U8x4  attribute(vector_size(32));
typedef U1 U1x16 attribute(vector_size(16));

#define x64_u4_from_byte_hits(dst, src)        "pmovmskb "        x64_r(src) ", " x64_r(dst) "\n"
#define x64_u4_count_trailing_zeros(dst, src)  "tzcntl "          x64_r(src) ", " x64_r(dst) "\n"

#define x64_u1x16_repeat_dwords(dst, src, n)   "pshufd $" #n ", " x64_r(src) ", " x64_r(dst) "\n"
#define x64_u1x16_load_mem4(dst, src)          "movd "            x64_r(src) ", " x64_r(dst) "\n"
#define x64_u1x16_load_mem8(dst, src)          "movq "            x64_m(src) ", " x64_r(dst) "\n"
#define x64_u1x16_load_mem16(dst, src)         "movdqu "          x64_m(src) ", " x64_r(dst) "\n"
#define x64_u1x16_match_bytes(dst, src)        "pcmpeqb "         x64_r(src) ", " x64_r(dst) "\n"

#define x64_u8x2_load_u8(dst, src)             "vmovq "                x64_m(src) ", " x64_r(dst) "\n"
#define x64_u8x2_insert_u8(dst, src, n)        "vpinsrq $" #n ", "     x64_m(src) ", " x64_r(dst) ", " x64_r(dst) "\n"
#define x64_u8x4_insert_xmm(dst, src, n)       "vinserti128 $" #n ", " x64_r(src) ", " x64_r(dst) ", " x64_r(dst) "\n"
#define x64_u8x4_broadcast(dst, src)           "vpbroadcastq "         x64_r(src) ", " x64_r(dst) "\n"
#define x64_u8x4_match(dst, a, b)              "vpcmpeqq "             x64_r(b)   ", " x64_r(a)   ", " x64_r(dst) "\n"
#define x64_u4_from_qword_hits(dst, src)       "vmovmskpd "            x64_r(src) ", " x64_r(dst) "\n"

FI_ U4 count_trailing_zeros_u4(U4 mask) {
	U4 n;
	asm volatile(
		x64_u4_count_trailing_zeros(n, mask)
		asm_out : asm_out_r(n)
		asm_in  : asm_in_r(mask)
	);
	return n;
}

FI_ U1x16 splat_u4_u1x16(U4 b4) {
	U1x16 v;
	asm volatile(
		x64_u1x16_load_mem4    (v, b4)
		x64_u1x16_repeat_dwords(v, v, 0)
		asm_out : asm_out_x0(v)
		asm_in  : asm_in_r(b4)
	);
	return v;
}

FI_ U4 mask_eq8_u1x16(U1_R p, U1x16 needle) {
	U1x16 chunk;
	U4    mask;
	asm volatile(
		x64_u1x16_load_mem8  (chunk, p)
		x64_u1x16_match_bytes(chunk, needle)
		x64_u4_from_byte_hits(mask, chunk)
		asm_out : asm_out_x(chunk), asm_out_r(mask)
		asm_in  : asm_in_r(p), asm_in_x(needle)
		asm_clobber : "memory"
	);
	return mask; /* movq zero-fills the high 8 bytes; bits 8–15 stay 0 */
}

FI_ U4 mask_eq16_u1x16(U1_R p, U1x16 needle) {
	U1x16 chunk;
	U4    mask;
	asm volatile(
		x64_u1x16_load_mem16 (chunk, p)
		x64_u1x16_match_bytes(chunk, needle)
		x64_u4_from_byte_hits(mask, chunk)
		asm_out : asm_out_x(chunk), asm_out_r(mask)
		asm_in  : asm_in_r(p), asm_in_x(needle)
		asm_clobber : "memory"
	);
	return mask;
}

I_ U8 find_u1_via_u1x16(U1_R p, U8 len, U1 ch, U1x16 needle) {
	U8 i = 0;
	while (len - i >= 16) {
		U4 mask = mask_eq16_u1x16(p + i, needle);
		if (mask) return i + C_(U8, count_trailing_zeros_u4(mask));
		i += 16;
	}
	while (len - i >= 8) {
		U4 mask = mask_eq8_u1x16(p + i, needle);
		if (mask) return i + C_(U8, count_trailing_zeros_u4(mask));
		i += 8;
	}
	while (i < len) {
		if (p[i] == ch) return i;
		++ i;
	}
	return len; /* not found */
}

FI_ U4 find_aos_keys_mask_u8x4(U8_R p0, U8_R p1, U8_R p2, U8_R p3, U8 key) {
	U8x2 lo, hi;
	U8x4 keys, splat, eq;
	U4   mask;
	asm volatile(
		x64_u8x2_load_u8  (lo, p0)       x64_u8x2_insert_u8(lo, p1, 1)
		x64_u8x2_load_u8  (hi, p2)       x64_u8x2_insert_u8(hi, p3, 1)
		x64_u8x4_insert_xmm(keys, lo, 0) x64_u8x4_insert_xmm(keys, hi, 1)
		x64_u8x4_broadcast(splat, key)   x64_u8x4_match(eq, keys, splat)
		x64_u4_from_qword_hits(mask, eq)
		asm_out : asm_out_x(lo), asm_out_x(hi), asm_out_x(keys), asm_out_x(splat), asm_out_x(eq), asm_out_r(mask)
		asm_in  : asm_in_r(p0), asm_in_r(p1), asm_in_r(p2), asm_in_r(p3), asm_in_r(key)
		asm_clobber : "memory"
	);
	return mask;
}
