#include "bloat.hpp"

// Octal formatting is used heavily throughout
// See: https://gist.github.com/seanjensengrey/f971c20d05d4d0efc0781f2f3c0353da
//      (x86 is an octal machine)

namespace Op
{
	ct u8 Mask      = 00210;
	ct u8 Mask_Low  = 0b11110000;
	ct u8 Mask_High = 0b00001111;
	ct u8 Mask_Imme = 0b00111000;

	#define D_Opcodes \
		D_Entry( mov_RM_R  ) \
		D_Entry( mov_Im_RM ) \
		D_Entry( mov_Im_R  ) \
		D_Entry( mov_M_Acc ) \
		D_Entry( mov_Acc_M ) \
		D_Entry( mov_RM_SR ) \
		D_Entry( mov_SR_RM ) \

	enum Code : u8
	{
		mov_RM_R  = 0b10001000,
		mov_Im_RM = 0b11001100,
		mov_Im_R  = 0b10110000,
		mov_M_Acc = 0b10100000,
		mov_Acc_M = 0b10100010,
		mov_RM_SR = 0b10001110,
		mov_SR_RM = 0b10001100
	};

	char const* str( Code type )
	{
		switch ( type )
		{
		#define D_Entry( Entry_ )             \
			case Entry_:                      \
				return ZPL_STRINGIFY(Entry_); \

			D_Opcodes
		#undef D_Entry
		}

		return Msg_Invalid_Value;
	}

	char const* meumonic( Code type )
	{
		switch ( type )
		{
			case mov_RM_R  : 
			case mov_Im_RM :
			case mov_Im_R  :
			case mov_M_Acc :
			case mov_Acc_M :
			case mov_RM_SR :
			case mov_SR_RM :
				return "mov";
		}

		return Msg_Invalid_Value;
	}

	char const* intuitive( Code type )
	{
		switch ( type )
		{
			case mov_RM_R  : 
			case mov_Im_RM :
			case mov_Im_R  :
			case mov_M_Acc :
			case mov_Acc_M :
			case mov_RM_SR :
			case mov_SR_RM :
				return "move";
		}

		return Msg_Invalid_Value;
	}

	#undef D_Opcodes
}

namespace Field
{
	inline 
	u8 offset_left_reg( u8 reg )
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
		Mask_Width      = 0b00000001,
		Mask_Width_Imme = 0b00001000,
		Width_Byte      = 0b00,
		Width_Word      = 0b01,
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

	ct u8 Mask_Memory = 0b00000111;	

	// https://i.imgur.com/Tm5roJu.png
	enum Memory : u8
	{
		Add_BX_SI  = 0b000,
		Add_BX_DI  = 0b001,
		Add_BP_SI  = 0b010,
		Add_BP_DI  = 0b011,
		Add_SI     = 0b100,
		Add_DI     = 0b101,
		Add_Direct = 0b110,
		Add_BX     = 0b111,

		Num_Memory
	};

	char const* str_memory( Memory mem )
	{
		char const* mem_to_str[Num_Memory] = 
		{
			"BX + SI",
			"BX + DI",
			"BP + SI",
			"BP + DI",
			"SI",
			"DI",
			"BP",
			"BX"
		};

		return mem_to_str[ mem ];
	}
		
	char const* str_memory_intuitive( Memory mem )
	{
		char const* mem_to_str[Num_Memory] = 
		{
			"Base.16 + Stack.Index",
			"Base.16 + Destination.Index",
			"Stack.Base + Stack.Index",
			"Stack.Base + Destination.Index",
			"Stack.Index",
			"Destination.Index",
			"Stack.Base",
			"BX"
		};

		return mem_to_str[ mem ];
	}
}

namespace Register
{
	ct u8 Mask_Imme  = 0b00000111;
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
	
		return Type_To_Meumonic[ type + Width * ( 7 + 1 ) ];
	}

	char const* intuitive( Type type, u8 Width )
	{
		static char const* 
		Type_To_Intuitive[ Num ] = 
		{
			"Accumulator.Low",
			"Count.Low",
			"Data.Low",
			"Base.Low",
			"Accumulator.High",
			"Count.High",
			"Data.High",
			"Base.High",
			"Accumulator.16",
			"Count.16",
			"Data.16",
			"Base.16",
			"Stack.Pointer",
			"Stack.Base",
			"Source.Index",
			"Destination.Index",
		};
	
		return Type_To_Intuitive[ type + Width * ( 7 + 1 ) ];
	}
}


struct Octal
{
	u8 Low   : 3;
	u8 High  : 3;
	u8 Dir   : 1;
	u8 Write : 1;

	operator u8()
	{
		return * cast( u8*, this);
	}
};

struct POD_Instruction
{
	// 8086 Instructions are 1 to 6 bytes in length.
	union
	{
		u8* Ptr;
		u8 Byte[6];

		Octal Instr;

		struct 
		{
			u8        Pad[2];
			u16_Split Disp;
			u16_Split Data;
		} 
		EffAddr;

		struct
		{
			u8        Pad[1];
			u16_Split Data;
		} 
		Imme;

		struct 
		{
			u8 Pad;
			u16_Split Data;
		}
		Addr;
	};

	// If directly referencing blob data:
	// Part of next instruction, do not use as stratch memory.
	u16 Pad;
};

struct Instruction : public POD_Instruction
{
	using Code      = Op::Code;
	using Direction = Field::Direction;
	using Mode      = Field::Mode;
	using Memory    = Field::Memory;
	using Width     = Field::Width;
	using Reg       = Register::Type;

	inline
	Direction get_direction()
	{
		Direction direction = cast(Direction, Byte[0] & Field::Mask_Dir); 

		return direction;
	}

	inline 
	Mode get_mode()
	{
		Mode mode = cast( Mode, Byte[1] & Field::Mask_Mode );

		return mode;
	}

	inline
	Code get_opcode( u8 mask )
	{
		Op::Code opcode = cast( Op::Code, Byte[0] & mask );

		return opcode;
	}

	inline
	Width get_width( u8 mask )
	{
		Width width = cast( Width, Byte[0] & mask );

		return width;
	}

	inline
	Reg mode_operand_left_reg()
	{
		u8 
		operand   = Byte[1] & Register::Mask_Left;
		operand >>= 3;

		return cast(Reg, operand);
	}

	inline
	Reg  mode_operand_right_reg()
	{
		u8 operand = Byte[1] & Register::Mask_Right;

		return cast( Reg, operand );
	}

	forceinline
	void dissassemble_mode( char const*& str_operand, Width& width, u8& length )
	{
		using namespace Field;
		using namespace Op;
		using namespace Register;

		switch ( get_mode() )
		{
			case Mode_Mem:
			{
				Memory operand_right = cast( Memory, Byte[1] & Mask_Memory );

				length += 2;

				if ( operand_right == Add_Direct )
				{
					u16 address = EffAddr.Disp;

					str_operand = string_format( "[ %u ]", address );
				}
				else
				{
					str_operand = str_memory( operand_right );
				}
			}
			break;

			case Mode_Mem8:
			{
				Memory operand_right = cast( Memory, Byte[1] & Mask_Memory );

				length += 1;

				str_operand = string_format( "[ %s + %u ]", str_memory( operand_right), EffAddr.Disp.Low );
			}
			break;

			case Mode_Mem16:
			{
				length += 2;

				Memory operand_right = cast( Memory, Byte[1] & Mask_Memory );
				u16    address       = EffAddr.Disp;

				str_operand = string_format( "[ %s + %u ]", str_memory( operand_right), address );
			}
			break;

			case Mode_Reg:
				str_operand = Register::meumonic( mode_operand_right_reg(), width );
			break;
		}
	}

	u8 dissassemble( zpl_string* result_out )
	{
		using namespace Field;
		using namespace Op;
		using namespace Register;

		u8 length = 1;

		zpl_string assembly = string_make( 32);

		Code      code = get_opcode( Op::Mask );
		Direction dir  = get_direction();

		char const* str_operand_left  = nullptr;
		char const* str_operand_right = nullptr;

		const u8 code_low  = Byte[0] & Op::Mask_Low;
		const u8 code_high = Byte[0] & Op::Mask_High;
		const u8 sig_imme  = Byte[0] & Op::Mask_Imme;

		if ( get_opcode( Op::Mask_Low ) == mov_Im_R )
		{
			Width width     = get_width( Mask_Width_Imme );
			Reg   reg       = cast(Reg, Byte[0] & Register::Mask_Right );
			u16   immediate = width == Width_Byte ? 
				Imme.Data.Low : Imme.Data;

			str_operand_left  = Register::meumonic( reg, width );
			str_operand_right = string_format( "%u", immediate );
		}
		else
		{
			switch ( code )
			{
			#pragma region mov
				case mov_RM_R:
				{
					length++;

					Width width = get_width( Mask_Width );

					str_operand_left = Register::meumonic( mode_operand_left_reg(), width );
					
					dissassemble_mode( str_operand_right, width, length );
				}
				break;

				case mov_Im_RM:
				{
					Width width = get_width( Mask_Width_Imme );

					Memory operand_left = cast( Memory, Byte[1] & Mask_Memory );
					u16    address      = EffAddr.Disp;
					u16    immediate    = width == Width_Byte ?
						EffAddr.Data.Low : EffAddr.Data;

					dissassemble_mode( str_operand_left, width, length );

					str_operand_right = string_format( "%u", immediate );
				}
				break;

				case mov_M_Acc:
				{
					Width width   = get_width( Mask_Width );
					u16   address = width == Width_Byte ? 
						Addr.Data.Low : Addr.Data;

					str_operand_left  = Register::meumonic( Reg::AX, Width_Word );
					str_operand_right = string_format( "%u", address );
				}
				break;

				case mov_Acc_M:
				{
					Width width   = get_width( Mask_Width );
					u16   address = width == Width_Byte ? 
						Addr.Data.Low : Addr.Data;

					str_operand_right = string_format( "%u", address );
					str_operand_left  = Register::meumonic( Reg::AX, Width_Word );
				}
				break;

				case mov_RM_SR:
				{
					
				}
				break;

				case mov_SR_RM:
				{

				}
				break;
			#pragma endregion mov
			}
		}

		assembly = string_format( assembly, 32, "\n%s %s, %s"
			// opcode operand_right, operand_left
			, Op::meumonic( code)
			, str_operand_right
			, str_operand_left
		);

		if ( result_out == nullptr )
			return length;

		if ( * result_out == nullptr )
			* result_out = string_make( zpl_kilobytes(1) );

		* result_out = string_append( * result_out, assembly );

		zpl_printf("\n\nAssembly: %s\n\n", assembly);

		return length;
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
		mock.Byte[0] = mov_RM_R         | Dir_Src             | Field::Width_Word;
		mock.Byte[1] = Field::Mode_Reg  | offset_left_reg(BX) | CX;

		zpl_printf("\n\nAttempting Mock Instruction: mov CX, BX");

		print_nl();
		print_as_binary( & mock.Byte[0], 1, " " );
		print_as_binary( & mock.Byte[1], 1, " " );
		print_nl();

		zpl_string dissasembly = nullptr;
		
		mock.dissassemble( & dissasembly);
	}

	void try_blob_single_move()
	{
		zpl_printf("\n\nAttempting to read blob: listing_0037_single_register_mov");

		zpl_file_contents 
		blob = zpl_file_read_contents( g_allocator, false, "tests/listing_0037_single_register_mov" );

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
		instr.Byte[0] = data[0];
		instr.Byte[1] = data[1];
		instr.dissassemble( & dissasembly);
	}

	void try_blob_many_moves()
	{
		zpl_printf("\n\nAttempting to read blob: listing_0038_many_register_mov");

		zpl_file_contents 
		blob = zpl_file_read_contents( g_allocator, false, "tests/listing_0038_many_register_mov" );

		if (blob.data == nullptr )
				return;

		u32 left = blob.size;
		u8* data = cast( u8*, blob.data );

		print_nl();
		print_as_binary( data, left, " " );
		print_nl();

		zpl_string dissasembly = string_make( "bits 16\n");

		while ( left )
		{
			Instruction
			instr;
			instr.Byte[0] = data[0];
			instr.Byte[1] = data[1];
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

	void try_blob_more_moves()
	{
		zpl_printf("\n\nAttempting to read blob: listing_0039_more_movs");

		zpl_file_contents 
		blob = zpl_file_read_contents( g_allocator, false, "tests/listing_0039_more_movs" );

		if (blob.data == nullptr )
				return;

		u32 left = blob.size;
		u8* data = cast( u8*, blob.data );

		print_nl();
		print_as_binary( data, left, " " );
		print_nl();

		zpl_string dissasembly = string_make( "bits 16\n");

		while ( left )
		{
			Instruction 
			instr;
			instr.Ptr = data;

			u8 length = instr.dissassemble( & dissasembly);

			data += length;
			left -= length;
		}

		zpl_printf("\n\nDissassemlby:\n%s", dissasembly);
		dissasembly = zpl_string_append_fmt( dissasembly, "\n" );

		zpl_printf("\n\nSaving to file");
		zpl_file_write_contents("tests/listing_0039_more_movs.asm.out.asm"
			, dissasembly
			, zpl_string_length(dissasembly)
			, nullptr
		);
	}
}


int main()
{
	zpl_printf("sim 8086!");

	setup();
	
	f64 start = zpl_time_rel();

	// Tests::try_blob_single_move();
	// Tests::try_blob_many_moves();
	Tests::try_blob_more_moves();

	f64 end = zpl_time_rel();
	f64 ms  = (end - start) * 100;

	printf("\n\nElapsed Time: %lf ms", ms);
	printf("\n\n");

	cleanup();

	return 0;
}
