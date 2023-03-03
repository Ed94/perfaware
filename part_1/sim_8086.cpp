#if __clang__
#pragma clang diagnostic ignored "-Wunused-const-variable"
#endif


#pragma region 							ZPL INCLUDE

#if __clang__
	#pragma clang diagnostic push 
	#pragma clang diagnostic ignored "-Wmissing-braces"
	#pragma clang diagnostic ignored "-Wbraced-scalar-init"
#endif

#define ZPL_IMPLEMENTATION
#include "zpl.h"

#if __clang__
	#pragma clang diagnostic pop
#endif

#pragma endregion 						ZPL INCLUDE


// This has me screwing around with generating lookup tables.
// Going to measure the difference between them and jump tables instructions at some point...

// Binary formatting for literals is used heavily as that is 
// how the encoding is conveyed in the hardware reference from intel.


#define bit( value ) ( 1 << value )
#define bitfield_is_equal( field_, mask_ ) ( ( mask_ & field_ ) == mask_ )
#define ct constexpr
#define forceinline ZPL_ALWAYS_INLINE
#define MSG_INVALID_VALUE "INVALID VALUE PROVIDED"


using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned long;

// using bytecode = u8;


inline
char char_binary( u8 value, u8 pos )
{
	constexpr u8 baseline = 1;
	u8 mask = baseline << pos;

	return ( (baseline << pos) & value) == mask ? '1' : '0';
}

inline
void str_binary( char* result, u8 value )
{
#if 0
	result[0] = char_binary( value, 0);
	result[1] = char_binary( value, 1);
	result[2] = char_binary( value, 2);
	result[3] = char_binary( value, 3);
	result[4] = char_binary( value, 4);
	result[5] = char_binary( value, 5);
	result[6] = char_binary( value, 6);
	result[7] = char_binary( value, 7);
#else
	result[0] = char_binary( value, 7);
	result[1] = char_binary( value, 6);
	result[2] = char_binary( value, 5);
	result[3] = char_binary( value, 4);
	result[4] = char_binary( value, 3);
	result[5] = char_binary( value, 2);
	result[6] = char_binary( value, 1);
	result[7] = char_binary( value, 0);
#endif
}


#define USE_LOOKUP_TABLE 0
namespace Op
{
	ct u8 Mask = 0b11111100;

	#define D_Opcodes \
	D_Entry( mov_88 ) \
	D_Entry( mov_89 ) \

	using Type = u8;
	enum
	{
	#if USE_LOOKUP_TABLE
		#define D_Entry( Entry_ ) Entry_,
		D_Opcodes
		#undef D_Entry

	#else 
		mov_88 = 0b10001000,
		mov_89 = 0b11000100,

	#endif
	
		Num = 2
	};

#if USE_LOOKUP_TABLE
	ct u8 code( Type type ) 
	{
		constexpr u8 
		type_to_code[ Num ] =
		{
			0b01000100, // mov_88
			0b11000100, // mov_89
		};

		return type_to_code[ type ];
	}

	ct u8 type( u8 code )
	{
		switch ( code )
		{
			#define D_Entry
			case 0b01000100: 
				return mov_88;

			#undef D_Entry
		}

		return Num;
	}
#endif

	char const* str( Type type )
	{
	#if Op_USE_LOOKUP_TABLE
		static char const* 
		type_to_str[ Num ] =
		{
			#define D_Entry( Entry_ ) #Entry_,
			D_Opcodes
			#undef D_Entry
		};

		return type_to_str[ type ];

	#else
		switch ( type )
		{
			case mov_88:
				return "mov_88";
			case mov_89:
				return "mov_89";
		}

		return MSG_INVALID_VALUE;
	#endif
	}

	ct char const* helper_meumonic( Type type )
	{
		switch ( type )
		{
			case mov_88:
			case mov_89:
				return "mov";
		}

		return MSG_INVALID_VALUE;
	}

	char const* meumonic( Type type )
	{
	#if USE_LOOKUP_TABLE
		static char const* 
		type_to_meumonic[ Num ] = 
		{
			#define D_Entry( Entry_ ) helper_meumonic( Entry_ ),
			D_Opcodes
			#undef D_Entry
		};

		return type_to_meumonic[ type ];

	#else
		switch ( type )
		{
			case mov_88:
			case mov_89:
				return "mov";
		}

		return MSG_INVALID_VALUE;
	#endif
	}

	ct char const* helper_intuitive( Type type )
	{
		switch ( type )
		{
			case mov_88:
			case mov_89:
				return "move";
		}

		return MSG_INVALID_VALUE;
	}

	char const* intuitive( Type type )
	{
	#if USE_LOOKUP_TABLE
		static char const* 
		type_to_meumonic[ Num ] = 
		{
			#define D_Entry( Entry_ ) helper_intuitive( Entry_ ),
			D_Opcodes
			#undef D_Entry
		};
	
		return type_to_meumonic[ type ];
		
	#else

		switch ( type )
		{
			case mov_88:
			case mov_89:
				return "move";
		}

		return MSG_INVALID_VALUE;
	#endif
	}

	#undef D_Opcodes
}

namespace Field
{
#if 0
	using Type = u8;
	enum
	{
		Dir_REG_Dest = 0b00,
		Dir_REG_Src  = 0b10,

		
	};
#endif
	ct u8 Mask_Dir   = 0b00000010;
	ct u8 Mask_Width = 0b00000001;
	ct u8 Mask_Mode  = 0b11000000;

	// Mask: Effective Address
	inline u8 mask_reg_operand( u8 reg )
	{
		return reg << 3;
	}

	// https://i.imgur.com/drsyYnM.png
	ct u8 Dir_REG_Src   = 0b00;
	ct u8 Dir_REG_Dest  = 0b10;

	inline
	char const* str_direction( u8 direction )
	{
		switch ( direction )
		{
			case Dir_REG_Dest:
				return "Destination";

			case Dir_REG_Src:
				return "Source";
		}

		return MSG_INVALID_VALUE;
	}

	// https://i.imgur.com/9Op8Lnd.png
	ct u8 Width_Byte = 0b00; 
	ct u8 Width_Word = 0b01;

	inline
	char const* str_width( u8 width )
	{
		switch ( width )
		{
			case Width_Byte:
				return "Byte";

			case Width_Word:
				return "Word";
		}

		return MSG_INVALID_VALUE;
	}

	// https://i.imgur.com/Job2oPd.png
	ct u8 Mode_Mem   = 0b00000000;
	ct u8 Mode_Mem8  = 0b01000000;
	ct u8 Mode_Mem16 = 0b10000000;
	ct u8 Mode_Reg   = 0b11000000;

	char const* str_mode( u8 mode )
	{
		switch (mode)
		{
			case Mode_Mem:
				return "Memory: No Displacement";

			case Mode_Mem8:
				return "Memory: 8-bit Displacment";

			case Mode_Mem16:
				return "Memory: 16-bit Displacement";

			case Mode_Reg:
				return "Register";
		}

		return MSG_INVALID_VALUE;
	}

#if 0
	ct u8 RegMem_AL        = 0b00000000;
	ct u8 RegMem_AX        = 0b00000000;
	ct u8 RegMem_BX_SI     = 0b00000000;
	ct u8 RegMem_BX_SI_D8  = 0b00000000;
	ct u8 RegMem_BX_SI_D16 = 0b00000000;
#endif
}

namespace Register
{
	ct u8 Mask_Left  = 0b00111000;
	ct u8 Mask_Right = 0b00000111;

	#define D_Opcodes \
	D_Entry( AL ) \
	D_Entry( CL ) \
	D_Entry( DL ) \
	D_Entry( BL ) \
	D_Entry( AH ) \
	D_Entry( CH ) \
	D_Entry( DH ) \
	D_Entry( BH ) \
	D_Entry( AX ) \
	D_Entry( CX ) \
	D_Entry( DX ) \
	D_Entry( BX ) \
	D_Entry( SP ) \
	D_Entry( BP ) \
	D_Entry( SI ) \
	D_Entry( DI ) \

	using Type = u8;
	enum
	{
		// Endianness is prob wrong...
		AL = 0b000,
		CL = 0b001,
		DL = 0b010,
		BL = 0b011,

		AH = 0b100,
		CH = 0b101,
		DH = 0b110,
		BH = 0b111,

		AX = 0b000,
		CX = 0b001,
		DX = 0b010,
		BX = 0b011,

		SP = 0b100,
		BP = 0b101,
		SI = 0b110,
		DI = 0b111,

		Num = 16
	};

	char const* meumonic( Type type, u8 Width )
	{
		static char const* 
		Type_To_Meumonic[ Num ] = 
		{
			#define D_Entry( Entry_ ) #Entry_,
			D_Opcodes
			#undef D_Entry
		};
	
		return Type_To_Meumonic[ type + Width * 7 + 1 ];
	}

	char const* intuitive( Type type, u8 Width )
	{
		static char const* 
		Type_To_Intuitive[ Num ] = 
		{
			"A.Low",
			"C.Low",
			"D.Low",
			"B.Low",
			"A.High",
			"C.High",
			"D.High",
			"B.High",
			"A.16",
			"C.16",
			"D.16",
			"B.16",
			"Stack.Pointer",
			"Stack.Base",
			"Source.Index",
			"Destination.Index",
		};
	
		return Type_To_Intuitive[ type + 8 * Width ];
	}
}

// 8086 Instructions are 1 to 6 bytes in length.
struct POD_Instruction
{
	u8 Byte_1;
	u8 Byte_2;
	u8 Byte_3;
	u8 Byte_4;
	u8 Byte_5;
	u8 Byte_6;

	u16 Pad;
};

struct Instruction : public POD_Instruction
{
	inline
	u8 direction()
	{
		u8 direction = Byte_1 & Field::Mask_Dir; 

		return direction;
	}

	inline 
	u8 mode()
	{
		u8 mode = Byte_2 & Field::Mask_Mode;

		return mode;
	}

	inline
	u8 opcode()
	{
		u8 
		opcode = Byte_1 & Op::Mask;

		return opcode;
	}

	inline
	u8 operand_left()
	{
		u8 
		operand  = Byte_2 & Register::Mask_Left;
		operand >>= 3;

		return operand;
	}

	inline
	u8 operand_right()
	{
		u8 
		operand = Byte_2 & Register::Mask_Right;
		// operand <<= 1;

		return operand;
	}

	inline
	u8 width()
	{
		u8 width = Byte_1 & Field::Mask_Width;

		return width;
	}

	void disassemble()
	{
		using namespace Field;
		using namespace Op;
		using namespace Register;

		u8 width_val = width();

		char const* opcode_str        = Op::meumonic( opcode() );
		char const* direction_str     = Field::str_direction( direction() );
		char const* width_str         = Field::str_width( width_val );
		char const* mode_str          = Field::str_mode( mode() );
		char const* operand_left_str  = Register::meumonic( operand_left(), width_val );
		char const* operand_right_str = Register::meumonic( operand_right(), width_val );

		char
		binary_string[9];
		binary_string[8] = '\0';

		str_binary( binary_string, opcode() );
		zpl_printf("\nOpcode     : %s : %s", binary_string, opcode_str);

		str_binary( binary_string, direction() );
		zpl_printf("\nDirection  : %s : %s", binary_string, direction_str);

		str_binary( binary_string, width_val );
		zpl_printf("\nWidth      : %s : %s", binary_string, width_str);

		str_binary( binary_string, mode() );
		zpl_printf("\nMode       : %s : %s", binary_string, mode_str);

		str_binary( binary_string, operand_left() );
		zpl_printf("\nOperand    : %s : %s", binary_string, operand_left_str);

		str_binary( binary_string, operand_right() );
		zpl_printf("\nOperand EA : %s : %s", binary_string, operand_right_str);
	}
};

namespace Tests
{
	zpl_arena BlobArena {};
	void Init()
	{
		zpl_arena_init_from_allocator( & BlobArena, zpl_heap(), zpl_megabytes(1) );

		if ( BlobArena.total_size == 0 )
		{
			zpl_assert_crash( "Failed to reserve memory for Tests:: BLobArena" );
		}
	}

	void Try_MockInstruction()
	{
		using namespace Field;
		using namespace Op;
		using namespace Register;

		Instruction
		mock; // mov AX, BX
	#if USE_LOOKUP_TABLE
		mock.Byte_1 = Op::code(mov_88) | Dir_REG_Dest                | Field::Width_Word;
	#else
		mock.Byte_1 = mov_88           | Dir_REG_Src                 | Field::Width_Word;
	#endif
		mock.Byte_2 = Field::Mode_Reg  | Field::mask_reg_operand(BX) | CX;

		zpl_printf("\n\nAttempting Mock Instruction: mov CX, BX");

		char
		binary_string[9];
		binary_string[8] = '\0';

		str_binary( binary_string, mock.Byte_1 );
		zpl_printf("\n%s", binary_string);

		str_binary( binary_string, mock.Byte_2 );
		zpl_printf("\n%s", binary_string);

		mock.disassemble();
	}

	void Try_Blob_SingleRegisterMove()
	{
		zpl_printf("\n\nAttempting to read blob: listing_0037_single_register_mov");

		zpl_file_contents 
		blob = zpl_file_read_contents( zpl_arena_allocator( & BlobArena), false, 
		"tests/listing_0037_single_register_mov"
		// "tests/listing_0038_many_register_mov"
		);

		if (blob.data == nullptr )
		{
			return;
		}

		printf("\nContents:\n");

		u32 left = blob.size;
		u8* data = cast(u8*)blob.data 
		// + blob.size - 1
		;

		char
		binary_string[9];
		binary_string[8] = '\0';

		while ( left-- )
		{
			str_binary( binary_string, data[0]);

			printf("%s\n", binary_string );

			data += 1;
		}

		printf("\n");

		left = blob.size;
		// data += blob.size;
		data -= blob.size;
		while ( left-- )
		{
			str_binary( binary_string, data[0]);

			printf("%X", data[0] );

			data += 1;
		}

		Instruction
		instr;
		instr.Byte_1 = ((u8*)blob.data)[0];
		instr.Byte_2 = ((u8*)blob.data)[1];
		instr.disassemble();
	}
}


int main()
{
	zpl_printf("sim 8086!");

	Tests::Init();
	
	zpl_f64 start = zpl_time_rel();

	Tests::Try_MockInstruction();
	Tests::Try_Blob_SingleRegisterMove();

	zpl_f64 end = zpl_time_rel();

	printf("\n\nElapsed Time: %lf", end - start);
	printf("\n\n");

	return 0;
}
