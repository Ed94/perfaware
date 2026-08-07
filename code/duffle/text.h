#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	include "memory.h"
#	include "hashing.h"
#	include "tables.h"
#	include "analysis.h"
#endif

// NOTE(rjf): Includes reverses for uppercase and lowercase hex.
RO_ global U8 integer_symbol_reverse[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

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
I_ Str8 str8_fmt_ktl_buf(Slice buffer, KTL_Str8 table, Str8 fmt_template)
{
	slice_assert(buffer);
	slice_assert(table);
	slice_assert(fmt_template);
	UTF8_R cursor_buffer    = C_(UTF8_R, buffer.ptr);
	U8     buffer_remaining = buffer.len;
	UTF8_R cursor_fmt       = fmt_template.ptr;
	U8     left_fmt         = fmt_template.len;
	while (left_fmt && buffer_remaining)
	{
		// Forward until we hit the delimiter '<' or the template's contents are exhausted.
		U8 copy_offset = 0;
		if (cursor_fmt[0] == '<')
		{
			UTF8_R potential_token_cursor = cursor_fmt + 1; // Skip '<'
			U8     potential_token_len    = 0;
			B4     fmt_overflow           = false;
			while(true) {
				UTF8_R cursor       = potential_token_cursor + potential_token_len;
				fmt_overflow        = cursor >= slice_end(fmt_template);
				B4 found_terminator = potential_token_cursor[potential_token_len] == '>';
				if (fmt_overflow || found_terminator) { break; }
				++ potential_token_len;
			}
			if (fmt_overflow) { 
				// Failed to find a subst and we're at end of fmt, just copy segment.
				copy_offset = 1 + potential_token_len; // '<' + token
				goto write_to_buffer; 
			}
			// Hashing the potential token and cross checking it with our token table
			U8 key = hash64_fnv1a_ret(slice_ut(u8_(potential_token_cursor), potential_token_len), 0);
			Str8_R value = nullptr; for slice_iter(table, token) {
				// We do a linear iteration instead of a hash table lookup because the user should never subst with more than 100 unqiue tokens..
				if (token->key == key) { value = & token->value; break; }
			}
			if (value)
			{
				// We're going to appending the string, make sure we have enough space in our buffer.
				// NOTE(Ed): this version doesn't support growing the buffer (No Allocator Interface)
				assert((buffer_remaining - potential_token_len) > 0);
				copy_offset = min(buffer_remaining, value->len); // Prevent Buffer overflow.
				mem_copy(u8_(cursor_buffer), u8_(value->ptr), buffer_remaining);
				// Sync cursor format to after the processed token
				cursor_buffer    += copy_offset;
				buffer_remaining -= copy_offset;
				cursor_fmt        = potential_token_cursor + 1 + potential_token_len; // '<' + token
				left_fmt         -= potential_token_len    + 2; // The 2 here are the '<' & '>' delimiters being omitted.
				continue;
			}
			// If not a subsitution, we copy the segment and continue.
			copy_offset = 1 + potential_token_len; // '<' + token
			goto write_to_buffer;
		}
		else do {
			++ copy_offset;
		} 
		while ( (cursor_fmt[copy_offset] != '<' && (cursor_fmt + copy_offset) < slice_end(fmt_template)) );
	write_to_buffer:
		assert((buffer_remaining - copy_offset) > 0);
		copy_offset = min(buffer_remaining, copy_offset); // Prevent buffer overflow.
		mem_copy(u8_(cursor_buffer), u8_(cursor_fmt), copy_offset);
		buffer_remaining -= copy_offset;
		left_fmt         -= copy_offset;
		cursor_buffer    += copy_offset;
		cursor_fmt       += copy_offset;
	}
	return (Str8){C_(UTF8*, buffer.ptr), buffer.len - buffer_remaining};
}

typedef Struct_(Str8Gen) { UTF8* ptr; U8 cap, len; };
FI_ Slice str8gen_buf(Str8Gen_R gen) { return (Slice){u8_(gen->ptr) + gen->len, gen->cap - gen->len}; }

FI_ void str8gen_append_str8(Str8Gen_R gen, Str8 str) { assert(gen != nullptr);
	mem_bump_u8(u8_(gen->ptr), gen->cap, & gen->len, str.len);
	U8 ptr = u8_(gen->ptr) + gen->len;
	mem_copy(ptr, u8_(str.ptr), str.len);
}
FI_ void str8gen_append_fmt(Str8Gen_R gen, Str8 fmt, KTL_Str8 tbl) {
	Str8 result = str8_fmt_ktl_buf(str8gen_buf(gen), tbl, fmt);
	gen->len += result.len;
}
#define str8gen_append_str8_(gen, s) str8gen_append_str8(gen, str8(s))
