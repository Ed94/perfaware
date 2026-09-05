#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "duffle/tables.h"
#	include "duffle/text.h"
#	include "info.h"
#endif

/* Numeric representation remains a presentation decision.
   Feed whatever Duffle-formatted Str8 values you want here (hex for opcode
   facts, decimal for capacities, etc.) and the existing <key> formatter does
   the composition. */
typedef Struct_(X8616_InfoTextValues) { Str8 offset; Str8 expected; Str8 actual; };

FI_ Str8
x8616_info_render(Slice output, X8616_InfoMsg_R msg, X8616_InfoTextValues values) {
	KTL_Slot_Str8 slots[] = {
		{ ktl_str8_key("offset"),   values.offset   },
		{ ktl_str8_key("expected"), values.expected },
		{ ktl_str8_key("actual"),   values.actual   },
	};
	return str8_fmt_ktl_buf(output, ktl_str8_from_arr(slots), msg->text);
}
