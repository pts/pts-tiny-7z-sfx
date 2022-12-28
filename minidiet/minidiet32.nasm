		bits 32
		cpu 386

global memcpy__RP3__
memcpy__RP3__:	push edi
		xchg esi, edx
		xchg edi, eax		; EDI := dest; EAX := junk.
		push edi
		rep movsb
		pop eax			; Will return dest.
		xchg esi, edx		; Restore ESI.
		pop edi
		ret

; Implemented using sys_brk(2). Equivalent to the following C code, but was
; size-optimized.
;
; A simplistic allocator which creates a heap of 64 KiB first, and then
; doubles it when necessary. It is implemented using Linux system call
; brk(2), exported by the libc as sys_brk(...). free(...)ing is not
; supported. Returns an unaligned address (which is OK on x86).
;
; static void *malloc_far(size_t size) {
;     static char *base, *free, *end;
;     ssize_t new_heap_size;
;     if ((ssize_t)size <= 0) return NULL;  /* Fail if size is too large (or 0). */
;     if (!base) {
;         if (!(base = free = (char*)sys_brk(NULL))) return NULL;  /* Error getting the initial data segment size for the very first time. */
;         new_heap_size = 64 << 10;  /* 64 KiB. */
;         goto grow_heap;  /* TODO(pts): Reset base to NULL if we overflow below. */
;     }
;     while (size > (size_t)(end - free)) {  /* Double the heap size until there is `size' bytes free. */
;         new_heap_size = (end - base) << 1;  /* !! TODO(pts): Don't allocate more than 1 MiB if not needed. */
;       grow_heap:
;         if ((ssize_t)new_heap_size <= 0 || (size_t)base + new_heap_size < (size_t)base) return NULL;  /* Heap would be too large. */
;         if ((char*)sys_brk(base + new_heap_size) != base + new_heap_size) return NULL;  /* Out of memory. */
;         end = base + new_heap_size;
;     }
;     free += size;
;     return free - size;
; }
global malloc__RP3__
malloc__RP3__:
		push ebx
		test eax, eax
		jle .18
		mov ebx, eax
		cmp dword [__malloc_base], 0
		jne .7
		xor eax, eax
		call sys_brk__RP3__
		mov [__malloc_free], eax
		mov [__malloc_base], eax
		test eax, eax
		jz short .18
		mov eax, 0x10000	; 64 KiB minimum allocation.
.9:		add eax, [__malloc_base]
		jc .18
		push eax
		call sys_brk__RP3__	; It also destroys ECX and EDX.
		pop edx			; This (and the next line) could be ECX instead.
		cmp eax, edx
		jne .18
		mov [__malloc_end], eax
.7:		mov edx, [__malloc_end]
		mov eax, [__malloc_free]
		mov ecx, edx
		sub ecx, eax
		cmp ecx, ebx
		jb .21
		add ebx, eax
		mov [__malloc_free], ebx
		jmp short .17
.21:		sub edx, [__malloc_base]
		lea eax, [edx+edx]
		test eax, eax
		jg .9
.18:		xor eax, eax
.17:		pop ebx
		ret

section .bss
__malloc_base	resd 1  ; char *base;
__malloc_free	resd 1  ; char *free;
__malloc_end	resd 1  ; char *end;
section .text

;extern __minidiet_errno
common __minidiet_errno 4 4

; __attribute__((__nothrow__)) __attribute__((regparm(3))) off64_t lseek64set(int fd, off64_t offset);
; Arguments: EAX == fd, EDX == low dword of offset, ECX == high dword of offset.
global lseek64set__RP3__
lseek64set__RP3__:
		push ebx
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
		pop ebx  ; Low dword of result.
		pop ebx  ; High dword of result.
		pop edi
		pop esi
		jmp strict short ..@after_syscall3

%macro __LIBC_LINUX_SYSCALL 2
global %1__RP3__
%1__RP3__:	push %2
		jmp strict short __do_syscall3
%endm

__LIBC_LINUX_SYSCALL sys_brk, 45       ; void* __LIBC_FUNC(sys_brk, (void *addr));  ; Currently unused.
__LIBC_LINUX_SYSCALL unlink, 10        ; int __LIBC_FUNC(unlink, (const char *pathname));
__LIBC_LINUX_SYSCALL close, 6          ; int __LIBC_FUNC(close, (int fd));
__LIBC_LINUX_SYSCALL open, 5           ; int __LIBC_FUNC(open, (const char *pathname, int flags, mode_t mode));
__LIBC_LINUX_SYSCALL read, 3           ; ssize_t __LIBC_FUNC(read, (int fd, void *buf, size_t count));
__LIBC_LINUX_SYSCALL write, 4          ; ssize_t __LIBC_FUNC(write, (int fd, const void *buf, size_t count));
__LIBC_LINUX_SYSCALL mkdir, 39         ; int __LIBC_FUNC(mkdir, (const char *pathname, mode_t mode));
__LIBC_LINUX_SYSCALL chmod, 15         ; int __LIBC_FUNC(chmod, (const char *path, mode_t mode));
__LIBC_LINUX_SYSCALL fchmod, 94        ; int __LIBC_FUNC(fchmod, (int fd, mode_t mode));
__LIBC_LINUX_SYSCALL gettimeofday, 78  ; int __LIBC_FUNC(gettimeofday, (struct timeval *tv, struct timezone *tz));
__LIBC_LINUX_SYSCALL umask, 60         ; mode_t __LIBC_FUNC(umask, (mode_t mask));
__LIBC_LINUX_SYSCALL symlink, 83       ; int __LIBC_FUNC(symlink, (const char *oldpath, const char *newpath));
__LIBC_LINUX_SYSCALL utimes, 271       ; int __LIBC_FUNC(utimes, (const char *filename, const struct timeval *times));
__LIBC_LINUX_SYSCALL lstat64, 196      ; int __LIBC_FUNC(lstat64, (const char *path, struct stat64 *buf));

global _start
extern main
_start:  ; Program entry point.
		pop eax			; argc.
		mov edx, esp		; argv.
		push edx		; Not needed iff main is regparm(3).
		push eax		; Not needed iff main is regparm(3).
		call main
		; Fall through to exit.

; __attribute__((regparm(3))) __attribute__((noreturn, nothrow)) void exit(int status) __asm__("exit__RP3__");
global exit__RP3__
exit__RP3__:	; Exit code (status) is in EAX.
		push byte 1		; __NR_exit.
		; Fall through to __do_syscall3.

; Do Linux i386 syscall (system call) of up to 3 arguments:
;
; * in dword[ESP(+4)]: syscall number, will be popped upon return
; * maybe in EAX: arg1
; * maybe in EDX: arg2
; * maybe in ECX: arg3
; * out EAX: result or -1 on error
; * out EBX: kept intact
; * out ECX: destroyed
; * out EDX: destroyed
;
; For rp3 syscall*, EBX has to be kept intact.
__do_syscall3:	xchg ebx, [esp]	; Keep EBX pushed.
		xchg eax, ebx
		xchg ecx, edx
		int 0x80
..@after_syscall3:
		test eax, eax
		jns .ok
		neg eax
		mov dword [__minidiet_errno], eax
		or eax, byte -1  	; Set return value to -1.
.ok:		pop ebx
		ret
