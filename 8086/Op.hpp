#include "bloat.hpp"

namespace Op
{
	enum Code : u8
	{
	#define D_Entry( Entry_ ) Entry,
		#include "opcodes.inline"
		Num,
	#undef D_Entry
	};

	char const* str( Code code )
	{
		static
		char const* code_to_str[ Num] =
		{
		#define D_Entry( Entry_ ) ZPL_STRINGIFY( Entry_ ),
			#include "opcodes.inline"
		#undef D_Entry
		};

		return code_to_str[ code ];
	}

	char const* meumonic( Code code )
	{
		static
		char const* code_to_str[ Num] = 
		{
		#define D_Entry( Entry_ ) ZPL_STRINGIFY( Entry_ ),
			#include "opcodes.meumonics.inline"
		#undef D_Entry
		};

		return code_to_str[ code ];
	}

	char const* intuitive( Code code )
	{
		static
		char const* code_to_str[ Num] = 
		{
		#define D_Entry( Entry_ ) ZPL_STRINGIFY( Entry_ ),
			#include "opcodes.intuitive.inline"
		#undef D_Entry
		};

		return code_to_str[ Num];
	}
}