#if INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	include "memory.h"
#endif

WinAPI void  ms_exit_process(U4 uExitCode)  asm("ExitProcess"); // Kernel 32

#pragma region IO
enum {
	MS_STD_INPUT_CODE  = u4_(-10),
	MS_STD_OUTPUT_CODE = u4_(-11),
#define MS_STD_INPUT  u4_(MS_STD_INPUT_CODE)
#define MS_STD_OUTPUT u4_(MS_STD_OUTPUT_CODE)
};
typedef Struct_(MS_Handle){U8 id;};
#pragma endregion IO

// --- WinAPI Minimal Definitions ---
typedef struct MS_WNDCLASSA {
    U4    style;
    S8    (*lpfnWndProc)(void*, U4, U8, S8);
    S4    cbClsExtra;
    S4    cbWndExtra;
    void* hInstance;
    void* hIcon;
    void* hCursor;
    void* hbrBackground;
    char const* lpszMenuName;
    char const* lpszClassName;
} MS_WNDCLASSA;
typedef struct MS_POINT       { S4 x, y; } MS_POINT;
typedef struct MS_MSG         { void* hwnd; U4 message; U8 wParam; S8 lParam; U4 time; MS_POINT pt; } MS_MSG;
typedef struct MS_RECT        { S4 left, top, right, bottom; } MS_RECT;
typedef struct MS_PAINTSTRUCT { void* hdc; S4 fErase; MS_RECT rcPaint; S4 fRestore; S4 fIncUpdate; U1 rgbReserved[32]; } MS_PAINTSTRUCT;

// --- Kernel32 ---
WinAPI void      ms_exit_process(U4 uExitCode)                                                   asm("ExitProcess");
WinAPI MS_Handle ms_get_std_handle(U4 handle_type)                                               asm("GetStdHandle");
WinAPI void*     ms_virtual_alloc(void* lpAddress, U8 dwSize, U4 flAllocationType, U4 flProtect) asm("VirtualAlloc");
WinAPI B4 ms_read_console(
    MS_Handle handle, 
    UTF8_R    buffer, 
    U4        to_read, 
    U4_R      num_read, 
    U8        reserved_input_control
) asm("ReadConsoleA");
WinAPI B4 ms_write_console(
    MS_Handle     handle, 
    UTF8 const*R_ buffer, 
    U4            chars_to_write, 
    U4_V           chars_written, 
    U8            reserved
) asm("WriteConsoleA");

// --- User32 ---
WinAPI U2 ms_register_class_a(MS_WNDCLASSA const* lpWndClass) asm("RegisterClassA");
WinAPI void* ms_create_window_ex_a(
    U4          dwExStyle, 
    char const* lpClassName, 
    char const* lpWindowName, 
    U4          dwStyle, 
    S4          X, 
    S4          Y, 
    S4          nWidth, 
    S4          nHeight, 
    void*       hWndParent, 
    void*       hMenu, 
    void*       hInstance, 
    void*       lpParam
) asm("CreateWindowExA");
WinAPI S4    ms_show_window(void* hWnd, S4 nCmdShow)                                         asm("ShowWindow");
WinAPI S4    ms_get_message_a(MS_MSG* lpMsg, void* hWnd, U4 wMsgFilterMin, U4 wMsgFilterMax) asm("GetMessageA");
WinAPI S4    ms_translate_message(MS_MSG const* lpMsg)                                       asm("TranslateMessage");
WinAPI S8    ms_dispatch_message_a(MS_MSG const* lpMsg)                                      asm("DispatchMessageA");
WinAPI S8    ms_def_window_proc_a(void* hWnd, U4 Msg, U8 wParam, S8 lParam)                  asm("DefWindowProcA");
WinAPI void  ms_post_quit_message(S4 nExitCode)                                              asm("PostQuitMessage");
WinAPI S4    ms_invalidate_rect(void* hWnd, MS_RECT const* lpRect, S4 bErase)                asm("InvalidateRect");
WinAPI S2    ms_get_async_key_state(S4 vKey)                                                 asm("GetAsyncKeyState");

// --- GDI32 ---
WinAPI void* ms_begin_paint(void* hWnd, MS_PAINTSTRUCT* lpPaint)              asm("BeginPaint");
WinAPI S4    ms_end_paint(void* hWnd, MS_PAINTSTRUCT const* lpPaint)          asm("EndPaint");
WinAPI U4    ms_set_text_color(void* hdc, U4 color)                           asm("SetTextColor");
WinAPI U4    ms_set_bk_color(void* hdc, U4 color)                             asm("SetBkColor");
WinAPI S4    ms_text_out_a(void* hdc, S4 x, S4 y, char const* lpString, S4 c) asm("TextOutA");
WinAPI void* ms_get_stock_object(S4 i)                                        asm("GetStockObject");
WinAPI void* ms_create_font_a(
    S4          cHeight,
    S4          cWidth,
    S4          cEscapement,
    S4          cOrientation,
    S4          cWeight,
    U4          bItalic,
    U4          bUnderline,
    U4          bStrikeOut,
    U4          iCharSet,
    U4          iOutPrecision,
    U4          iClipPrecision,
    U4          iQuality,
    U4          iPitchAndFamily,
    char const* pszFaceName
) asm("CreateFontA");
WinAPI void* ms_create_compatible_dc(void* hdc)                              asm("CreateCompatibleDC");
WinAPI void* ms_create_compatible_bitmap(void* hdc, S4 cx, S4 cy)            asm("CreateCompatibleBitmap");
WinAPI B4    ms_bit_blt(void* hdcDest, S4 x, S4 y, S4 w, S4 h, void* hdcSrc, S4 xSrc, S4 ySrc, U4 rop) asm("BitBlt");
WinAPI B4    ms_delete_dc(void* hdc)                                         asm("DeleteDC");
WinAPI B4    ms_get_client_rect(void* hwnd, MS_RECT* lpRect)                 asm("GetClientRect");
WinAPI void* ms_select_object(void* hdc, void* h)                          asm("SelectObject");
WinAPI S4    ms_rectangle(void* hdc, S4 left, S4 top, S4 right, S4 bottom) asm("Rectangle");
WinAPI S4    ms_set_bk_mode(void* hdc, S4 mode)                            asm("SetBkMode");
WinAPI void* ms_create_solid_brush(U4 color)                               asm("CreateSolidBrush");
WinAPI S4    ms_delete_object(void* ho)                                    asm("DeleteObject");

enum {
	MS_MEM_COMMIT          = 0x00001000,
	MS_MEM_RESERVE         = 0x00002000,
	MS_PAGE_READWRITE      = 0x04,
	MS_SRCCOPY             = 0x00CC0020,
	MS_WM_DESTROY          = 0x0002,
	MS_WM_SIZE             = 0x0005,
	MS_WM_PAINT            = 0x000F,
	MS_WM_ERASEBKGND       = 0x0014,
	MS_WM_KEYDOWN          = 0x0100,
	MS_WM_KEYUP            = 0x0101,
	MS_WM_MOUSEMOVE        = 0x0200,
	MS_WM_LBUTTONDOWN      = 0x0201,
	MS_WM_LBUTTONUP        = 0x0202,
	MS_WM_RBUTTONDOWN      = 0x0204,
	MS_WM_RBUTTONUP        = 0x0205,
	MS_WM_MBUTTONDOWN      = 0x0207,
	MS_WM_MBUTTONUP        = 0x0208,
	MS_WM_MOUSEWHEEL       = 0x020A,
	MS_WS_OVERLAPPEDWINDOW = 0x00CF0000,
	MS_WS_VISIBLE          = 0x10000000,
	MS_VK_LEFT             = 0x25,
	MS_VK_UP               = 0x26,
	MS_VK_RIGHT            = 0x27,
	MS_VK_DOWN             = 0x28,

	MS_PAGE_EXECUTE_READWRITE = 0x40,

	MS_WM_CHAR   = 0x0102,
	MS_VK_RETURN = 0x0D,
	MS_VK_BACK   = 0x08,
	MS_VK_TAB    = 0x09,
	MS_VK_SPACE  = 0x20,
	MS_VK_F5     = 0x74,
	MS_VK_PRIOR  = 0x21,
	MS_VK_NEXT   = 0x22,

	MS_VK_SHIFT =0x10,
};


#if BUILD_DEBUG
FI_ void assert(U8 cond) { if(cond){return;} else{debug_trap(); ms_exit_process(1);} }
#endif
