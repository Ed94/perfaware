#include "bloat.hpp"

zpl_arena BlobArena {};
#define allocator zpl_arena_allocator( & BlobArena)

void setup()
{
	zpl_arena_init_from_allocator( & BlobArena, zpl_heap(), zpl_megabytes(10) );

	if ( BlobArena.total_size == 0 )
	{
		zpl_assert_crash( "Failed to reserve memory for Tests:: BLobArena" );
	}
}

void cleanup()
{
	zpl_arena_free( & BlobArena);
}

// Binary formatting for literals is used heavily as that its
// how the encoding is conveyed in the hardware reference from intel.

namespace Op
{

	#define D_Opcodes \
	D_Entry( mov_88 ) \
	D_Entry( mov_89 ) \

	enum Code : u8
	{
		Mask   = 0b11111100,
		mov_88 = 0b10001000,
		mov_89 = 0b11000100,
	};

	char const* str( Code type )
	{
		switch ( type )
		{
			case mov_88:
				return "mov_88";
			case mov_89:
				return "mov_89";
		}

		return Msg_Invalid_Value;
	}

	ct char const* helper_meumonic( Code type )
	{
		switch ( type )
		{
			case mov_88:
			case mov_89:
				return "mov";
		}

		return Msg_Invalid_Value;
	}

	char const* meumonic( Code type )
	{
		switch ( type )
		{
			case mov_88:
			case mov_89:
				return "mov";
		}

		return Msg_Invalid_Value;
	}

	ct char const* helper_intuitive( Code type )
	{
		switch ( type )
		{
			case mov_88:
			case mov_89:
				return "move";
		}

		return Msg_Invalid_Value;
	}

	char const* intuitive( Code type )
	{
		switch ( type )
		{
			case mov_88:
			case mov_89:
				return "move";
		}

		return Msg_Invalid_Value;
	}

	#undef D_Opcodes
}

namespace Field
{
	// Effective Address offset
	inline 
	u8 effective_address( u8 reg )
	{
		return reg << 3;
	}

	// https://i.imgur.com/drsyYnM.png
	enum Direction : u8
	{
		Mask_Dir = 0b00000010,
		Dir_Src  = 0b00,
		Dir_Dest = 0b10,
	};

	inline
	char const* str_direction( Direction direction )
	{
		switch ( direction )
		{
			case Dir_Dest:
				return "Destination";

			case Dir_Src:
				return "Source";
		}

		return Msg_Invalid_Value;
	}

	// https://i.imgur.com/9Op8Lnd.png
	enum Width : u8
	{
		Mask_Width = 0b00000001,
		Width_Byte = 0b00,
		Width_Word = 0b01,
	};

	inline
	char const* str_width( Width width )
	{
		switch ( width )
		{
			case Width_Byte:
				return "Byte";

			case Width_Word:
				return "Word";
		}

		return Msg_Invalid_Value;
	}

	// https://i.imgur.com/Job2oPd.png
	enum Mode : u8
	{
		Mask_Mode  = 0b11000000,
		Mode_Mem   = 0b00000000,
		Mode_Mem8  = 0b01000000,
		Mode_Mem16 = 0b10000000,
		Mode_Reg   = 0b11000000,
	};

	char const* str_mode( Mode mode )
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

		return Msg_Invalid_Value;
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

	enum Type : u8
	{
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
	
		return Type_To_Intuitive[ type + Width * 7 + 1 ];
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
	using Code      = Op::Code;
	using Direction = Field::Direction;
	using Mode      = Field::Mode;
	using Width     = Field::Width;
	using Reg       = Register::Type;

	inline
	Direction direction()
	{
		Direction 
		direction = cast(Direction, Byte_1 & Field::Mask_Dir); 

		return direction;
	}

	inline 
	Mode mode()
	{
		Mode mode = cast( Mode, Byte_2 & Field::Mask_Mode );

		return mode;
	}

	inline
	Code opcode()
	{
		Op::Code 
		opcode = cast( Op::Code, Byte_1 & Op::Mask );

		return opcode;
	}

	inline
	Reg operand_left()
	{
		u8 
		operand  = Byte_2 & Register::Mask_Left;
		operand >>= 3;

		return cast(Reg, operand);
	}

	inline
	Reg operand_right()
	{
		u8 operand = Byte_2 & Register::Mask_Right;

		return cast( Reg, operand );
	}

	inline
	Width width()
	{
		Width width = cast( Width, Byte_1 & Field::Mask_Width );

		return width;
	}

	void dissassemble( zpl_string* result_out )
	{
		using namespace Field;
		using namespace Op;
		using namespace Register;

		Direction dir = direction();
		Width width_val = width();

		char const* opcode_str        = Op::meumonic( opcode() );
		char const* direction_str     = Field::str_direction( dir );
		char const* width_str         = Field::str_width( width_val );
		char const* mode_str          = Field::str_mode( mode() );
		char const* operand_left_str  = Register::meumonic( operand_left(), width_val );
		char const* operand_right_str = Register::meumonic( operand_right(), width_val );

		char
		binary_string[9];
		binary_string[8] = '\0';

	#if Build_Debug
		str_binary( binary_string, opcode() );
		zpl_printf("\nOpcode     : %s : %s", binary_string, opcode_str);

		str_binary( binary_string, dir );
		zpl_printf("\nDirection  : %s : %s", binary_string, direction_str);

		str_binary( binary_string, width_val );
		zpl_printf("\nWidth      : %s : %s", binary_string, width_str);

		str_binary( binary_string, mode() );
		zpl_printf("\nMode       : %s : %s", binary_string, mode_str);

		str_binary( binary_string, operand_left() );
		zpl_printf("\nOperand    : %s : %s", binary_string, operand_left_str);

		str_binary( binary_string, operand_right() );
		zpl_printf("\nOperand EA : %s : %s", binary_string, operand_right_str);
	#endif 

		if ( result_out == nullptr )
			return;

		if ( * result_out == nullptr )
			* result_out = zpl_string_make_reserve( allocator, zpl_kilobytes(1) );

		zpl_string assembly = zpl_string_make_reserve( allocator, 32);

		assembly = zpl_string_sprintf( allocator, assembly, zpl_kilobytes(1) - 1, "\n%s %s, %s"
			, opcode_str
			, dir == Dir_Src ? operand_right_str : operand_left_str
			, dir == Dir_Src ? operand_left_str  : operand_right_str
		);

		* result_out = zpl_string_append( * result_out, assembly );

	#if Build_Debug
		zpl_printf("\n\nAssembly: %s\n\n", assembly);
	#endif
	}
};

namespace Tests
{
	void try_mock_instruction()
	{
		using namespace Field;
		using namespace Op;
		using namespace Register;

		Instruction
		mock; // mov CX, BX
		mock.Byte_1 = mov_88           | Dir_Src               | Field::Width_Word;
		mock.Byte_2 = Field::Mode_Reg  | effective_address(BX) | CX;

		zpl_printf("\n\nAttempting Mock Instruction: mov CX, BX");

		print_nl();
		print_as_binary( & mock.Byte_1, 1, " " );
		print_as_binary( & mock.Byte_2, 1, " " );
		print_nl();

		zpl_string dissasembly = nullptr;
		
		mock.dissassemble( & dissasembly);
	}

	void try_blob_single_instruction()
	{
		zpl_printf("\n\nAttempting to read blob: listing_0037_single_register_mov");

		zpl_file_contents 
		blob = zpl_file_read_contents( allocator, false, "tests/listing_0037_single_register_mov" );

		if (blob.data == nullptr )
			return;

		u32 left = blob.size;
		u8* data = cast( u8*, blob.data );

		print_nl();
		print_as_binary( data, left, " " );
		print_nl();

		zpl_string dissasembly = nullptr;

		Instruction
		instr;
		instr.Byte_1 = data[0];
		instr.Byte_2 = data[1];
		instr.dissassemble( & dissasembly);
	}

	void try_blob_many_instructions()
	{
		zpl_printf("\n\nAttempting to read blob: listing_0038_many_register_mov");

		zpl_file_contents 
		blob = zpl_file_read_contents( allocator, false, "tests/listing_0038_many_register_mov" );

		if (blob.data == nullptr )
				return;

		u32 left = blob.size;
		u8* data = cast( u8*, blob.data );

		print_nl();
		print_as_binary( data, left, " " );
		print_nl();

		zpl_string dissasembly = zpl_string_make( allocator, "bits 16\n");

		while ( left )
		{
			Instruction
			instr;
			instr.Byte_1 = data[0];
			instr.Byte_2 = data[1];
			instr.dissassemble( & dissasembly);

			data += 2;
			left -= 2;
		}

		zpl_printf("\n\nDissassemlby:\n%s", dissasembly);
		dissasembly = zpl_string_append_fmt( dissasembly, "\n" );

		zpl_printf("\n\nSaving to file");
		zpl_file_write_contents("tests/listing_0038_many_register_mov.out.asm"
			, dissasembly
			, zpl_string_length(dissasembly)
			, nullptr
		);
	}

	#undef allocator
}


int main()
{
	zpl_printf("sim 8086!");

	setup();
	
	zpl_f64 start = zpl_time_rel();

#if 0
	Tests::try_mock_instruction();
	Tests::try_blob_single_instruction();
#endif
	Tests::try_blob_many_instructions();

	zpl_f64 end = zpl_time_rel();
	zpl_f64 ms  = (end - start) * 100;

	printf("\n\nElapsed Time: %lf ms", ms);
	printf("\n\n");

	cleanup();

	return 0;
}
