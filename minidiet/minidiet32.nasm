		bits 32
		cpu 386

;extern __minidiet_errno
common __minidiet_errno 4 4

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

global _start
extern main
_start:  ; Program entry point.
		pop eax			; argc.
		mov edx, esp		; argv.
		push edx		; TODO(pts): Remove this if main is regparm(3).
		push eax		; TODO(pts): Remove this if main is regparm(3).
		call main
		; Fall through to exit.

; __attribute__((regparm(3))) __attribute__((noreturn, nothrow)) void exit(int status);
exit:		; Exit code (status) is in EAX.
		xchg eax, ebx		; EBX := exit_code, EAX := junk.
		xor eax, eax
		inc eax			; EAX := __NR_exit == 1.
		int 0x80

