#if !defined(__linux) || !defined(__i386)
#error This is for Linux i386 source code only.
#endif

#if defined(__cplusplus)
#error C++ not supported yet.
#endif

#include <miniinc1.h>

int errno __asm__("__minidiet_errno");
char **environ __asm__("__minidiet_environ");

/* The entry point function: _start.
 * by pts@fazekas.hu at Wed Feb  1 17:04:13 CET 2017
 *
 * Multiple __asm__ directives for -ansi -pedantic.
 */
__asm__("\n\
.text\n\
.globl _start\n\
.type _start, @function\n\
_start:\n\
/* This is hand-optimized assembly code of 43 bytes, `gcc-4.6 -Os'\n\
 * generates 53 bytes.\n\
 *\n\
 * The addresses in the assembly dump below are relative to _start.\n\
 */\n\
.byte 0x8D, 0x5C, 0x24, 0x04\n\
/* ^^^ 00000000  8D5C2404          lea ebx,[esp+0x4] */\n\
.byte 0x89, 0xDA\n\
/* ^^^ 00000004  89DA              mov edx,ebx */\n\
.byte 0x31, 0xC0\n\
/* ^^^ 00000006  31C0              xor eax,eax */\n\
");
__asm__("\n\
.byte 0x83, 0xC3, 0x04\n\
/* ^^^ 00000008  83C304            add ebx,byte +0x4 */\n\
.byte 0x3B, 0x03\n\
/* ^^^ 0000000B  3B03              cmp eax,[ebx] */\n\
.byte 0x75, 0xF9\n\
/* ^^^ 0000000D  75F9              jnz 0x8 */\n\
.byte 0x8D, 0x4B, 0x04\n\
/* ^^^ 0000000F  8D4B04            lea ecx,[ebx+0x4] */\n\
.byte 0x29, 0xD3\n\
/* ^^^ 00000012  29D3              sub ebx,edx */\n\
.byte 0xC1, 0xEB, 0x02\n\
/* ^^^ 00000014  C1EB02            shr ebx,0x2 */\n\
.byte 0x89, 0x0D\n\
");
__asm__("\n\
.long __minidiet_environ\n\
/* ^^^ 00000017  890D????????      mov [__minidiet_environ],ecx */\n\
.byte 0x51\n\
/* ^^^ 0000001D  51                push ecx */\n\
.byte 0x52\n\
/* ^^^ 0000001E  52                push edx */\n\
.byte 0x53\n\
/* ^^^ 0000001F  53                push ebx */\n\
call main\n\
/* ^^^ 000000??  E8????????        call main */\n\
/* Now eax has the exit code. */\n\
/* Execution continues in __minidiet_exit.s. */\n\
");
__asm__("\n\
.size _start, .-_start\n\
.globl __minidiet_exit_with_fini\n\
.type __minidiet_exit_with_fini, @function\n\
/* Fall throuh here from _start.S after return from main(). */\n\
__minidiet_exit_with_fini:  /* Exit code passed in eax (regparm(1)). */\n\
\n\
.globl __minidiet_exit\n\
.type __minidiet_exit, @function\n\
__minidiet_exit:  /* Exit code passed in eax (regparm(1)). */\n\
.byte 0x93\n\
/* ^^^ 000000??  93                xchg eax,ebx */\n\
");
__asm__("\n\
.byte 0x31, 0xC0\n\
/* ^^^ 000000??  31C0              xor eax,eax */\n\
.byte 0x40\n\
/* ^^^ 000000??  40                inc eax */\n\
.byte 0xCD, 0x80\n\
/* ^^^ 000000??  CD80              int 0x80 */\n\
.size __minidiet_exit_with_fini, .-__minidiet_exit_with_fini\n\
.size __minidiet_exit, .-__minidiet_exit\n\
");

__attribute__((__nothrow__)) __attribute__((regparm(3))) int lseek64set(int fd, off64_t offset) {
  off64_t result;  /* Pacify gcc. */
  return _llseek(fd, offset >> 32, (off_t)offset, &result, SEEK_SET);
}

#if 0  /* NASM syntax. */
		bits 32
		cpu 386


; __attribute__((__nothrow__)) __attribute__((regparm(3))) off64_t lseek64set(int fd, off64_t offset);
; Arguments: EAX == fd, EDX == low dword of offset, ECX == high dword of offset.
global lseek64set
lseek64set:	push ebx
		push esi
		push edi
		push ebx  ; High dword of result (will be ignored).
		push ebx  ; Low dword of result (will be ignored).
		xchg ebx, eax  ; EBX := fd. EAX := scratch.
		mov eax, 140  ; __NR__lseek.
		;mov ecx, arg_ecx  ; offset >> 32.
		;mov edx, arg_edx  ; offset.
		mov esi, esp  ; &result.
		xor edi, edi  ; SEEK_SET.
		int 0x80
		test eax, eax
		jz .ok
		neg eax
		mov dword [__minidiet_errno], eax
		;or eax, -1  ; Set return value to -1. Not needed.
.ok:		pop ebx  ; Low dword of result.
		pop ebx  ; High dword of result.
		pop edi
		pop esi
		pop ebx
		ret
#endif
