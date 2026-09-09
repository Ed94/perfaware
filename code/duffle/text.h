#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	include "memory.h"
#	include "encoding.h"
#	include "hashing.h"
#	include "tables.h"
#	include "analysis.h"
#	include "asm.h"
#endif

// NOTE(rjf): Includes reverses for uppercase and lowercase hex.
RO_ U8 integer_symbol_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

FI_ B4   char_is_space(UTF8 c) { return(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v'); }
FI_ B4   char_is_upper(UTF8 c) { return('A' <= c && c <= 'Z'); }
FI_ UTF8 char_to_lower(UTF8 c) { if (char_is_upper(c)) { c += ('a' - 'A'); } return(c); }
FI_ B4   char_is_digit(UTF8 c, U4 base) {
  B4 result = 0; if (0 < base && base <= 16) {
    if (integer_symbol_reverse[c] < base) result = 1;
  }
  return result;
}
FI_ UTF8 integer_symbols(UTF8 value) {
	LP_ UTF8 lookup_table[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F', }; 
	return lookup_table[C_(UTF8, value)]; 
}
FI_ U8 u8_from_str8(Str8 str, U4 radix) {
  U8 x = 0; if(1 < radix && radix <= 16) {
    for each_iter(U8, cursor, str.len) { 
      x *= radix;
      x += integer_symbol_reverse[str.ptr[cursor] & 0x7F];
    }
  }
  return x;
}

typedef Struct_(Info_str8_from_u4) {
	Str8 prefix;
	U4   digit_group_size;
	U4   needed_leading_zeros;
	U4   size_required;
};
I_ Info_str8_from_u4 str8_from_u4_info(U4 num, U4 radix, U4 min_digits, U4 digit_group_separator)
{
	Info_str8_from_u4 info = {0};
	LP_ Str8 tbl_prefix[] = { slit8("0x"), slit8("0o"), slit8("0b") };
	switch (radix) {
	case 16: { info.prefix = tbl_prefix[0]; } break;
	case 8:  { info.prefix = tbl_prefix[1]; } break;
	case 2:  { info.prefix = tbl_prefix[2]; } break;
	}
	info.digit_group_size = 3;
	switch (radix) {
	default: break;
	case 2:
	case 8:
	case 16: {
		info.digit_group_size = 4;
	}
	break;
	}
	info.needed_leading_zeros = 0;
	{
		U4 needed_digits = 1;
		{
			U4 u32_reduce = num;
			for(;;)
			{
				u32_reduce /= radix;
				if (u32_reduce == 0) {
					break;
				}
				needed_digits += 1;
			}
		}
		info.needed_leading_zeros = (min_digits > needed_digits) ? min_digits - needed_digits : 0;
		U4 needed_separators       = 0;
		if (digit_group_separator != 0)
		{
			needed_separators = (needed_digits + info.needed_leading_zeros) / info.digit_group_size;
			if (needed_separators > 0 && (needed_digits + info.needed_leading_zeros) % info.digit_group_size == 0) {
				needed_separators -= 1;
			}
		}
		info.size_required = info.prefix.len + info.needed_leading_zeros + needed_separators + needed_digits;
	}
	return info;
}
I_ Str8 str8_from_u4_buf(Slice buf, U4 num, U4 radix, U4 min_digits, U4 digit_group_separator, Info_str8_from_u4 info)
{
	assert(buf.len >= info.size_required);
	Str8 result = { C_(UTF8*, buf.ptr), info.size_required };
	/*Fill Content*/ {
		U4 num_reduce             = num;
		U4 digits_until_separator = info.digit_group_size;
		for (U8 idx = 0; idx < result.len; idx += 1)
		{
			U8 separator_pos = result.len - idx - 1;
			if (digits_until_separator == 0 && digit_group_separator != 0) {
				result.ptr[separator_pos] = u1_(digit_group_separator);
				digits_until_separator    = info.digit_group_size + 1;
			}
			else {
				result.ptr[separator_pos] = (U1) char_to_lower(integer_symbols(u1_(num_reduce % radix)));
				num_reduce /= radix;
			}
			digits_until_separator -= 1;
			if (num_reduce == 0) break;
		}
		for (U8 leading_0_idx = 0; leading_0_idx < info.needed_leading_zeros; leading_0_idx += 1) {
			result.ptr[info.prefix.len + leading_0_idx] = '0';
		}
	}
	/*Fill Prefix*/ if (info.prefix.len > 0) { slice_copy(result, info.prefix); }
	return result;
}

I_ Str8 str8_fmt_ktl_buf(Slice buffer, KTL_Str8 table, Str8 fmt_template){
	slice_assert(buffer);
	slice_assert(table);
	slice_assert(fmt_template);
	UTF8_R cursor_buffer    = C_(UTF8_R, buffer.ptr);
	U8     buffer_remaining = buffer.len;
	UTF8_R cursor_fmt       = fmt_template.ptr;
	U8     left_fmt         = fmt_template.len;
	U1x16  needle_lt        = splat_u4_u1x16(u4_byte_fill('<'));
	U1x16  needle_gt        = splat_u4_u1x16(u4_byte_fill('>'));
	while (left_fmt && buffer_remaining)
	{
		// Forward until we hit the delimiter '<' or the template's contents are exhausted.
		U8 copy_offset = 0;
		if (cursor_fmt[0] == '<') {
			UTF8_R sig     = cursor_fmt + 1;
			U8     sig_max = slice_end(fmt_template) - sig;
			U8     sig_len = find_u1_via_u1x16(sig, sig_max, '>', needle_gt);
			assert(sig_len < sig_max);

			Str8_R value = ktl_str8_find(table, hash64_fnv1a_ret(slice_ut(sig, sig_len), 0));
			U8     n     = min(buffer_remaining, value->len); 
			assert((buffer_remaining - n) > 0); mem_copy(u8_(cursor_buffer), u8_(value->ptr), n);
			cursor_buffer    += n;
			buffer_remaining -= n;
			cursor_fmt        = sig + sig_len + 1;
			left_fmt         -= sig_len + 2;
			continue;
		}
		U8 n = find_u1_via_u1x16(cursor_fmt, min(left_fmt, buffer_remaining), '<', needle_lt);
		assert((buffer_remaining - 1) > 0);  n = min(buffer_remaining, n);
		mem_copy(u8_(cursor_buffer), u8_(cursor_fmt), n);
		cursor_buffer    += n;
		cursor_fmt       += n;
		buffer_remaining -= n;
		left_fmt         -= n;
	}
	return str8(C_(UTF8*,buffer.ptr), buffer.len - buffer_remaining);
}

typedef Struct_(Str8Gen) { UTF8* ptr; U8 cap, len; };
FI_ Str8Gen str8gen_make(Slice s) { return (Str8Gen){C_(UTF8*,s.ptr), s.len, 0}; }

FI_ Slice str8gen_buf(Str8Gen_R gen) { return (Slice){u8_(gen->ptr) + gen->len, gen->cap - gen->len}; }

FI_ void str8gen_append_str8(Str8Gen_R gen, Str8 str) { assert(gen != nullptr);
	U8 ptr = u8_(gen->ptr) + gen->len;
	mem_bump_u8(u8_(gen->ptr), gen->cap, & gen->len, str.len);
	mem_copy(ptr, u8_(str.ptr), str.len);
}
FI_ void str8gen_append_fmt(Str8Gen_R gen, Str8 fmt, KTL_Str8 tbl) {
	Str8 result = str8_fmt_ktl_buf(str8gen_buf(gen), tbl, fmt);
	gen->len += result.len;
}
#define str8gen_append_str8_(gen, s) str8gen_append_str8(gen, str8(s))

// Dealing with Wides (UTF16)

typedef U4 UTF16;
typedef Struct_(Str16) { UTF16* ptr; U8 len; };
typedef Str16 Slice_UTF16;
#define str16(p,l) (Str16){p,l}

internal Str16
str16_from_8(FArena* arena, Str8 in) {
  Str16 result = {0}; if (in.len) {
    U8       pre_pos = farena_save(arena[0]);
    U8       cap     = in.len * 2;
    Slice_U2 str     = farena_push_array(arena, U2, cap + 1);
    U1* ptr = in.ptr;
    U1* opl = ptr + in.len;
    U8 size = 0;
    UnicodeDecode consume;
    for(;ptr < opl; ptr += consume.inc) {
      consume = utf8_decode(ptr, opl - ptr);
      size   += utf16_encode(str.ptr + size, consume.codepoint);
    }
    str.ptr[size] = 0;
    U8 committed = align_pow2((size + 1) * S_(U2), MEM_ALIGNMENT_DEFAULT);
    farena_rewind(arena, pre_pos + committed);
    result = str16(C_(UTF16*, str.ptr), size);
  }
  return result;
}

// Formatter where serial operation is done on-demand per-entry.

// enum {
// 	KTL_Str8FmtEntry_InlaidSize = (S_(Str8) * 2) - S_(Str8Fmt_TokenKind),
// };
// typedef Enum_(U4, Str8Fmt_SerialOpKind) {
// 	Str8Fmt_TKind_Str8,
// 	Str8Fmt_Base16_U1,
// 	Str8Fmt_Base16_U2,
// 	Str8Fmt_Base10_U4,
// };
// typedef Struct_(KTL_Str8Fmt_SerialOp_Entry) {
// 	union {
// 		U1    InlaidData[KTL_Str8FmtEntry_InlaidSize];
// 		Str8  str;
// 		void* Ptr;
// 	};
// 	Str8Fmt_SerialOpKind kind;
// };
// typedef KTL_Slot_(KTL_Str8Fmt_SerialOp_Entry);
