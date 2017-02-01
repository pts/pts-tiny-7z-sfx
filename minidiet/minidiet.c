#if !defined(__linux) || !defined(__i386)
#error This is for Linux i386 source code only.
#endif

#if defined(__cplusplus)
#error C++ not supported yet.
#endif

/* Constants from asm-generic/errno-base.h and asm-generic/errno.h */
#define	EPERM		 1	/* Operation not permitted */
#define	ENOENT		 2	/* No such file or directory */
#define	ESRCH		 3	/* No such process */
#define	EINTR		 4	/* Interrupted system call */
#define	EIO		 5	/* I/O error */
#define	ENXIO		 6	/* No such device or address */
#define	E2BIG		 7	/* Argument list too long */
#define	ENOEXEC		 8	/* Exec format error */
#define	EBADF		 9	/* Bad file number */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Try again */
#define	ENOMEM		12	/* Out of memory */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	ENOTBLK		15	/* Block device required */
#define	EBUSY		16	/* Device or resource busy */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Cross-device link */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Not a typewriter */
#define	ETXTBSY		26	/* Text file busy */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Illegal seek */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Math argument out of domain of func */
#define	ERANGE		34	/* Math result not representable */
#define	EDEADLK		35	/* Resource deadlock would occur */
#define	EDEADLOCK	EDEADLK
#define	ENAMETOOLONG	36	/* File name too long */
#define	ENOLCK		37	/* No record locks available */
#define	ENOSYS		38	/* Function not implemented */
#define	ENOTEMPTY	39	/* Directory not empty */
#define	ELOOP		40	/* Too many symbolic links encountered */
#define	EWOULDBLOCK	EAGAIN	/* Operation would block */
#define	ENOMSG		42	/* No message of desired type */
#define	EIDRM		43	/* Identifier removed */
#define	ECHRNG		44	/* Channel number out of range */
#define	EL2NSYNC	45	/* Level 2 not synchronized */
#define	EL3HLT		46	/* Level 3 halted */
#define	EL3RST		47	/* Level 3 reset */
#define	ELNRNG		48	/* Link number out of range */
#define	EUNATCH		49	/* Protocol driver not attached */
#define	ENOCSI		50	/* No CSI structure available */
#define	EL2HLT		51	/* Level 2 halted */
#define	EBADE		52	/* Invalid exchange */
#define	EBADR		53	/* Invalid request descriptor */
#define	EXFULL		54	/* Exchange full */
#define	ENOANO		55	/* No anode */
#define	EBADRQC		56	/* Invalid request code */
#define	EBADSLT		57	/* Invalid slot */
#define	EBFONT		59	/* Bad font file format */
#define	ENOSTR		60	/* Device not a stream */
#define	ENODATA		61	/* No data available */
#define	ETIME		62	/* Timer expired */
#define	ENOSR		63	/* Out of streams resources */
#define	ENONET		64	/* Machine is not on the network */
#define	ENOPKG		65	/* Package not installed */
#define	EREMOTE		66	/* Object is remote */
#define	ENOLINK		67	/* Link has been severed */
#define	EADV		68	/* Advertise error */
#define	ESRMNT		69	/* Srmount error */
#define	ECOMM		70	/* Communication error on send */
#define	EPROTO		71	/* Protocol error */
#define	EMULTIHOP	72	/* Multihop attempted */
#define	EDOTDOT		73	/* RFS specific error */
#define	EBADMSG		74	/* Not a data message */
#define	EOVERFLOW	75	/* Value too large for defined data type */
#define	ENOTUNIQ	76	/* Name not unique on network */
#define	EBADFD		77	/* File descriptor in bad state */
#define	EREMCHG		78	/* Remote address changed */
#define	ELIBACC		79	/* Can not access a needed shared library */
#define	ELIBBAD		80	/* Accessing a corrupted shared library */
#define	ELIBSCN		81	/* .lib section in a.out corrupted */
#define	ELIBMAX		82	/* Attempting to link in too many shared libraries */
#define	ELIBEXEC	83	/* Cannot exec a shared library directly */
#define	EILSEQ		84	/* Illegal byte sequence */
#define	ERESTART	85	/* Interrupted system call should be restarted */
#define	ESTRPIPE	86	/* Streams pipe error */
#define	EUSERS		87	/* Too many users */
#define	ENOTSOCK	88	/* Socket operation on non-socket */
#define	EDESTADDRREQ	89	/* Destination address required */
#define	EMSGSIZE	90	/* Message too long */
#define	EPROTOTYPE	91	/* Protocol wrong type for socket */
#define	ENOPROTOOPT	92	/* Protocol not available */
#define	EPROTONOSUPPORT	93	/* Protocol not supported */
#define	ESOCKTNOSUPPORT	94	/* Socket type not supported */
#define	EOPNOTSUPP	95	/* Operation not supported on transport endpoint */
#define	EPFNOSUPPORT	96	/* Protocol family not supported */
#define	EAFNOSUPPORT	97	/* Address family not supported by protocol */
#define	EADDRINUSE	98	/* Address already in use */
#define	EADDRNOTAVAIL	99	/* Cannot assign requested address */
#define	ENETDOWN	100	/* Network is down */
#define	ENETUNREACH	101	/* Network is unreachable */
#define	ENETRESET	102	/* Network dropped connection because of reset */
#define	ECONNABORTED	103	/* Software caused connection abort */
#define	ECONNRESET	104	/* Connection reset by peer */
#define	ENOBUFS		105	/* No buffer space available */
#define	EISCONN		106	/* Transport endpoint is already connected */
#define	ENOTCONN	107	/* Transport endpoint is not connected */
#define	ESHUTDOWN	108	/* Cannot send after transport endpoint shutdown */
#define	ETOOMANYREFS	109	/* Too many references: cannot splice */
#define	ETIMEDOUT	110	/* Connection timed out */
#define	ECONNREFUSED	111	/* Connection refused */
#define	EHOSTDOWN	112	/* Host is down */
#define	EHOSTUNREACH	113	/* No route to host */
#define	EALREADY	114	/* Operation already in progress */
#define	EINPROGRESS	115	/* Operation now in progress */
#define	ESTALE		116	/* Stale NFS file handle */
#define	EUCLEAN		117	/* Structure needs cleaning */
#define	ENOTNAM		118	/* Not a XENIX named type file */
#define	ENAVAIL		119	/* No XENIX semaphores available */
#define	EISNAM		120	/* Is a named type file */
#define	EREMOTEIO	121	/* Remote I/O error */
#define	EDQUOT		122	/* Quota exceeded */
#define	ENOMEDIUM	123	/* No medium found */
#define	EMEDIUMTYPE	124	/* Wrong medium type */
#define	ECANCELED	125	/* Operation Canceled */
#define	ENOKEY		126	/* Required key not available */
#define	EKEYEXPIRED	127	/* Key has expired */
#define	EKEYREVOKED	128	/* Key has been revoked */
#define	EKEYREJECTED	129	/* Key was rejected by service */
#define	EOWNERDEAD	130	/* Owner died */
#define	ENOTRECOVERABLE	131	/* State not recoverable */
#define ERFKILL		132	/* Operation not possible due to RF-kill */
#define EHWPOISON	133	/* Memory page has hardware error */

typedef unsigned int size_t;
__extension__ typedef unsigned long long dev_t;
__extension__ typedef unsigned int uid_t;
__extension__ typedef unsigned int gid_t;
__extension__ typedef unsigned long int ino_t;
__extension__ typedef unsigned long long int ino64_t;
__extension__ typedef unsigned int mode_t;
__extension__ typedef unsigned int nlink_t;
__extension__ typedef long int off_t;
__extension__ typedef long long int off64_t;
__extension__ typedef int pid_t;
__extension__ typedef struct { int __val[2]; } fsid_t;
__extension__ typedef long int clock_t;
__extension__ typedef unsigned long int rlim_t;
__extension__ typedef unsigned long long int lim64_t;
__extension__ typedef unsigned int id_t;
__extension__ typedef long int time_t;
__extension__ typedef unsigned int useconds_t;
__extension__ typedef long int suseconds_t;
__extension__ typedef int daddr_t;
__extension__ typedef long int swblk_t;
__extension__ typedef int key_t;
__extension__ typedef int clockid_t;
__extension__ typedef void *timer_t;
__extension__ typedef long int blksize_t;
__extension__ typedef long int blkcnt_t;
__extension__ typedef long long int blkcnt64_t;
__extension__ typedef unsigned long int fsblkcnt_t;
__extension__ typedef unsigned long long int fsblkcnt64_t;
__extension__ typedef unsigned long int fsfilcnt_t;
__extension__ typedef unsigned long long int fsfilcnt64_t;
__extension__ typedef int ssize_t;
typedef off64_t loff_t;
__extension__ typedef long long int *qaddr_t;
typedef char *caddr_t;
__extension__ typedef int intptr_t;
__extension__ typedef unsigned int uintptr_t;
__extension__ typedef unsigned int socklen_t;
typedef int wchar_t;
typedef int register_t __attribute__ ((__mode__ (__word__)));
typedef long int fd_mask;
typedef unsigned long int pthread_t;
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;
typedef __volatile__ int pthread_spinlock_t;
typedef unsigned short umode_t;
typedef unsigned long sigset_t;

/* Constants from asm/unistd_32.h */
#define __NR_restart_syscall      0
#define __NR_exit		  1
#define __NR_fork		  2
#define __NR_read		  3
#define __NR_write		  4
#define __NR_open		  5
#define __NR_close		  6
#define __NR_waitpid		  7
#define __NR_creat		  8
#define __NR_link		  9
#define __NR_unlink		 10
#define __NR_execve		 11
#define __NR_chdir		 12
#define __NR_time		 13
#define __NR_mknod		 14
#define __NR_chmod		 15
#define __NR_lchown		 16
#define __NR_break		 17
#define __NR_oldstat		 18
#define __NR_lseek		 19
#define __NR_getpid		 20
#define __NR_mount		 21
#define __NR_umount		 22
#define __NR_setuid		 23
#define __NR_getuid		 24
#define __NR_stime		 25
#define __NR_ptrace		 26
#define __NR_alarm		 27
#define __NR_oldfstat		 28
#define __NR_pause		 29
#define __NR_utime		 30
#define __NR_stty		 31
#define __NR_gtty		 32
#define __NR_access		 33
#define __NR_nice		 34
#define __NR_ftime		 35
#define __NR_sync		 36
#define __NR_kill		 37
#define __NR_rename		 38
#define __NR_mkdir		 39
#define __NR_rmdir		 40
#define __NR_dup		 41
#define __NR_pipe		 42
#define __NR_times		 43
#define __NR_prof		 44
#define __NR_brk		 45
#define __NR_setgid		 46
#define __NR_getgid		 47
#define __NR_signal		 48
#define __NR_geteuid		 49
#define __NR_getegid		 50
#define __NR_acct		 51
#define __NR_umount2		 52
#define __NR_lock		 53
#define __NR_ioctl		 54
#define __NR_fcntl		 55
#define __NR_mpx		 56
#define __NR_setpgid		 57
#define __NR_ulimit		 58
#define __NR_oldolduname	 59
#define __NR_umask		 60
#define __NR_chroot		 61
#define __NR_ustat		 62
#define __NR_dup2		 63
#define __NR_getppid		 64
#define __NR_getpgrp		 65
#define __NR_setsid		 66
#define __NR_sigaction		 67
#define __NR_sgetmask		 68
#define __NR_ssetmask		 69
#define __NR_setreuid		 70
#define __NR_setregid		 71
#define __NR_sigsuspend		 72
#define __NR_sigpending		 73
#define __NR_sethostname	 74
#define __NR_setrlimit		 75
#define __NR_getrlimit		 76   /* Back compatible 2Gig limited rlimit */
#define __NR_getrusage		 77
#define __NR_gettimeofday	 78
#define __NR_settimeofday	 79
#define __NR_getgroups		 80
#define __NR_setgroups		 81
#define __NR_select		 82
#define __NR_symlink		 83
#define __NR_oldlstat		 84
#define __NR_readlink		 85
#define __NR_uselib		 86
#define __NR_swapon		 87
#define __NR_reboot		 88
#define __NR_readdir		 89
#define __NR_mmap		 90
#define __NR_munmap		 91
#define __NR_truncate		 92
#define __NR_ftruncate		 93
#define __NR_fchmod		 94
#define __NR_fchown		 95
#define __NR_getpriority	 96
#define __NR_setpriority	 97
#define __NR_profil		 98
#define __NR_statfs		 99
#define __NR_fstatfs		100
#define __NR_ioperm		101
#define __NR_socketcall		102
#define __NR_syslog		103
#define __NR_setitimer		104
#define __NR_getitimer		105
#define __NR_stat		106
#define __NR_lstat		107
#define __NR_fstat		108
#define __NR_olduname		109
#define __NR_iopl		110
#define __NR_vhangup		111
#define __NR_idle		112
#define __NR_vm86old		113
#define __NR_wait4		114
#define __NR_swapoff		115
#define __NR_sysinfo		116
#define __NR_ipc		117
#define __NR_fsync		118
#define __NR_sigreturn		119
#define __NR_clone		120
#define __NR_setdomainname	121
#define __NR_uname		122
#define __NR_modify_ldt		123
#define __NR_adjtimex		124
#define __NR_mprotect		125
#define __NR_sigprocmask	126
#define __NR_create_module	127
#define __NR_init_module	128
#define __NR_delete_module	129
#define __NR_get_kernel_syms	130
#define __NR_quotactl		131
#define __NR_getpgid		132
#define __NR_fchdir		133
#define __NR_bdflush		134
#define __NR_sysfs		135
#define __NR_personality	136
#define __NR_afs_syscall	137 /* Syscall for Andrew File System */
#define __NR_setfsuid		138
#define __NR_setfsgid		139
#define __NR__llseek		140
#define __NR_getdents		141
#define __NR__newselect		142
#define __NR_flock		143
#define __NR_msync		144
#define __NR_readv		145
#define __NR_writev		146
#define __NR_getsid		147
#define __NR_fdatasync		148
#define __NR__sysctl		149
#define __NR_mlock		150
#define __NR_munlock		151
#define __NR_mlockall		152
#define __NR_munlockall		153
#define __NR_sched_setparam		154
#define __NR_sched_getparam		155
#define __NR_sched_setscheduler		156
#define __NR_sched_getscheduler		157
#define __NR_sched_yield		158
#define __NR_sched_get_priority_max	159
#define __NR_sched_get_priority_min	160
#define __NR_sched_rr_get_interval	161
#define __NR_nanosleep		162
#define __NR_mremap		163
#define __NR_setresuid		164
#define __NR_getresuid		165
#define __NR_vm86		166
#define __NR_query_module	167
#define __NR_poll		168
#define __NR_nfsservctl		169
#define __NR_setresgid		170
#define __NR_getresgid		171
#define __NR_prctl              172
#define __NR_rt_sigreturn	173
#define __NR_rt_sigaction	174
#define __NR_rt_sigprocmask	175
#define __NR_rt_sigpending	176
#define __NR_rt_sigtimedwait	177
#define __NR_rt_sigqueueinfo	178
#define __NR_rt_sigsuspend	179
#define __NR_pread64		180
#define __NR_pwrite64		181
#define __NR_chown		182
#define __NR_getcwd		183
#define __NR_capget		184
#define __NR_capset		185
#define __NR_sigaltstack	186
#define __NR_sendfile		187
#define __NR_getpmsg		188	/* some people actually want streams */
#define __NR_putpmsg		189	/* some people actually want streams */
#define __NR_vfork		190
#define __NR_ugetrlimit		191	/* SuS compliant getrlimit */
#define __NR_mmap2		192
#define __NR_truncate64		193
#define __NR_ftruncate64	194
#define __NR_stat64		195
#define __NR_lstat64		196
#define __NR_fstat64		197
#define __NR_lchown32		198
#define __NR_getuid32		199
#define __NR_getgid32		200
#define __NR_geteuid32		201
#define __NR_getegid32		202
#define __NR_setreuid32		203
#define __NR_setregid32		204
#define __NR_getgroups32	205
#define __NR_setgroups32	206
#define __NR_fchown32		207
#define __NR_setresuid32	208
#define __NR_getresuid32	209
#define __NR_setresgid32	210
#define __NR_getresgid32	211
#define __NR_chown32		212
#define __NR_setuid32		213
#define __NR_setgid32		214
#define __NR_setfsuid32		215
#define __NR_setfsgid32		216
#define __NR_pivot_root		217
#define __NR_mincore		218
#define __NR_madvise		219
#define __NR_madvise1		219	/* delete when C lib stub is removed */
#define __NR_getdents64		220
#define __NR_fcntl64		221
/* 223 is unused */
#define __NR_gettid		224
#define __NR_readahead		225
#define __NR_setxattr		226
#define __NR_lsetxattr		227
#define __NR_fsetxattr		228
#define __NR_getxattr		229
#define __NR_lgetxattr		230
#define __NR_fgetxattr		231
#define __NR_listxattr		232
#define __NR_llistxattr		233
#define __NR_flistxattr		234
#define __NR_removexattr	235
#define __NR_lremovexattr	236
#define __NR_fremovexattr	237
#define __NR_tkill		238
#define __NR_sendfile64		239
#define __NR_futex		240
#define __NR_sched_setaffinity	241
#define __NR_sched_getaffinity	242
#define __NR_set_thread_area	243
#define __NR_get_thread_area	244
#define __NR_io_setup		245
#define __NR_io_destroy		246
#define __NR_io_getevents	247
#define __NR_io_submit		248
#define __NR_io_cancel		249
#define __NR_fadvise64		250
/* 251 is available for reuse (was briefly sys_set_zone_reclaim) */
#define __NR_exit_group		252
#define __NR_lookup_dcookie	253
#define __NR_epoll_create	254
#define __NR_epoll_ctl		255
#define __NR_epoll_wait		256
#define __NR_remap_file_pages	257
#define __NR_set_tid_address	258
#define __NR_timer_create	259
#define __NR_timer_settime	(__NR_timer_create+1)
#define __NR_timer_gettime	(__NR_timer_create+2)
#define __NR_timer_getoverrun	(__NR_timer_create+3)
#define __NR_timer_delete	(__NR_timer_create+4)
#define __NR_clock_settime	(__NR_timer_create+5)
#define __NR_clock_gettime	(__NR_timer_create+6)
#define __NR_clock_getres	(__NR_timer_create+7)
#define __NR_clock_nanosleep	(__NR_timer_create+8)
#define __NR_statfs64		268
#define __NR_fstatfs64		269
#define __NR_tgkill		270
#define __NR_utimes		271
#define __NR_fadvise64_64	272
#define __NR_vserver		273
#define __NR_mbind		274
#define __NR_get_mempolicy	275
#define __NR_set_mempolicy	276
#define __NR_mq_open 		277
#define __NR_mq_unlink		(__NR_mq_open+1)
#define __NR_mq_timedsend	(__NR_mq_open+2)
#define __NR_mq_timedreceive	(__NR_mq_open+3)
#define __NR_mq_notify		(__NR_mq_open+4)
#define __NR_mq_getsetattr	(__NR_mq_open+5)
#define __NR_kexec_load		283
#define __NR_waitid		284
/* #define __NR_sys_setaltroot	285 */
#define __NR_add_key		286
#define __NR_request_key	287
#define __NR_keyctl		288
#define __NR_ioprio_set		289
#define __NR_ioprio_get		290
#define __NR_inotify_init	291
#define __NR_inotify_add_watch	292
#define __NR_inotify_rm_watch	293
#define __NR_migrate_pages	294
#define __NR_openat		295
#define __NR_mkdirat		296
#define __NR_mknodat		297
#define __NR_fchownat		298
#define __NR_futimesat		299
#define __NR_fstatat64		300
#define __NR_unlinkat		301
#define __NR_renameat		302
#define __NR_linkat		303
#define __NR_symlinkat		304
#define __NR_readlinkat		305
#define __NR_fchmodat		306
#define __NR_faccessat		307
#define __NR_pselect6		308
#define __NR_ppoll		309
#define __NR_unshare		310
#define __NR_set_robust_list	311
#define __NR_get_robust_list	312
#define __NR_splice		313
#define __NR_sync_file_range	314
#define __NR_tee		315
#define __NR_vmsplice		316
#define __NR_move_pages		317
#define __NR_getcpu		318
#define __NR_epoll_pwait	319
#define __NR_utimensat		320
#define __NR_signalfd		321
#define __NR_timerfd_create	322
#define __NR_eventfd		323
#define __NR_fallocate		324
#define __NR_timerfd_settime	325
#define __NR_timerfd_gettime	326
#define __NR_signalfd4		327
#define __NR_eventfd2		328
#define __NR_epoll_create1	329
#define __NR_dup3		330
#define __NR_pipe2		331
#define __NR_inotify_init1	332
#define __NR_preadv		333
#define __NR_pwritev		334
#define __NR_rt_tgsigqueueinfo	335
#define __NR_perf_event_open	336
#define __NR_recvmmsg		337
#define __NR_fanotify_init	338
#define __NR_fanotify_mark	339
#define __NR_prlimit64		340
#define __NR_name_to_handle_at	341
#define __NR_open_by_handle_at  342
#define __NR_clock_adjtime	343
#define __NR_syncfs             344
#define __NR_sendmmsg		345
#define __NR_setns		346
#define __NR_process_vm_readv	347
#define __NR_process_vm_writev	348

#undef  __HAS_GCC_4_5
#if defined(__GNUC_MINOR__) && __GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ >= 5
#define __HAS_GCC_4_5 1
#endif

#ifndef __HAS_GCC_4_5
#define __builtin_unreachable() for (;;)
#endif

int errno __asm__("__minidiet_errno");
char **environ __asm__("__minidiet_environ");

struct timeval;
struct timezone;
struct stat;

#define __syscall_return(type, res) \
do { \
  if ((unsigned long)(res) >= (unsigned long)(-200)) { \
    errno = -(res); \
    res = -1; \
  } \
  return (type) (res); \
} while (0)

#define _syscall0_nomemory(type,name) \
type name(void) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name)); \
__syscall_return(type,__res); \
}

#define _syscall1(type,name,type1,arg1) \
type name(type1 __##arg1) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)) \
  : "memory");  /* No need to specify "cc" (for eflags, also tried it) or "esi" or "edi" here, see http://lxr.free-electrons.com/source/arch/x86/kernel/entry_32.S?v=3.14#L17 and SAVE_ALL */ \
__syscall_return(type,__res); \
}

#define _syscall1_nomemory(type,name,type1,arg1) \
type name(type1 __##arg1) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1))); \
__syscall_return(type,__res); \
}

#define _syscall1_noreturn_nomemory(name,type1,arg1) \
void __attribute__((noreturn, nothrow)) name(type1 __##arg1) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1))); \
__builtin_unreachable(); \
}

#define _syscall2_nomemory(type,name,type1,arg1,type2,arg2) \
type name(type1 __##arg1,type2 __##arg2) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2))); \
__syscall_return(type,__res); \
}

#define _syscall2(type,name,type1,arg1,type2,arg2) \
type name(type1 __##arg1,type2 __##arg2) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)) : "memory"); \
__syscall_return(type,__res); \
}

#define _syscall3_nomemory(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 __##arg1,type2 __##arg2,type3 __##arg3) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), "d" ((long)(__##arg3))); \
__syscall_return(type,__res); \
}

#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 __##arg1,type2 __##arg2,type3 __##arg3) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), "d" ((long)(__##arg3)) : "memory"); \
__syscall_return(type,__res); \
}

#define _syscall3_nomemory(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 __##arg1,type2 __##arg2,type3 __##arg3) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), \
      "d" ((long)(__##arg3))); \
__syscall_return(type,__res); \
}

#define _syscall4_nomemory(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 __##arg1, type2 __##arg2, type3 __##arg3, type4 __##arg4) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), \
    "d" ((long)(__##arg3)),"S" ((long)(__##arg4))); \
__syscall_return(type,__res); \
}

#define _syscall4(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4) \
type name (type1 __##arg1, type2 __##arg2, type3 __##arg3, type4 __##arg4) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), "d" ((long)(__##arg3)),"S" ((long)(__##arg4)) : "memory"); \
__syscall_return(type,__res); \
}

#define _syscall5_nomemory(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
type name (type1 __##arg1,type2 __##arg2,type3 __##arg3,type4 __##arg4,type5 __##arg5) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), \
    "d" ((long)(__##arg3)),"S" ((long)(__##arg4)),"D" ((long)(__##arg5))); \
__syscall_return(type,__res); \
}

#define _syscall5(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5) \
type name (type1 __##arg1,type2 __##arg2,type3 __##arg3,type4 __##arg4,type5 __##arg5) { \
long __res; \
__asm__ __volatile__ ("int $0x80" \
  : "=a" (__res) \
  : "0" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), "d" ((long)(__##arg3)),"S" ((long)(__##arg4)),"D" ((long)(__##arg5)) : "memory"); \
__syscall_return(type,__res); \
}

#define _syscall6_nomemory(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
type name (type1 __##arg1,type2 __##arg2,type3 __##arg3,type4 __##arg4,type5 __##arg5,type6 __##arg6) { \
long __res; \
__asm__ __volatile__ ("push %%ebp ; movl %%eax,%%ebp ; movl %1,%%eax ; int $0x80 ; pop %%ebp" \
  : "=a" (__res) \
  : "i" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), \
    "d" ((long)(__##arg3)),"S" ((long)(__##arg4)),"D" ((long)(__##arg5)), \
    "0" ((long)(__##arg6))); \
__syscall_return(type,__res); \
}

#define _syscall6(type,name,type1,arg1,type2,arg2,type3,arg3,type4,arg4,type5,arg5,type6,arg6) \
type name (type1 __##arg1,type2 __##arg2,type3 __##arg3,type4 __##arg4,type5 __##arg5,type6 __##arg6) { \
long __res; \
__asm__ __volatile__ ("push %%ebp ; movl %%eax,%%ebp ; movl %1,%%eax ; int $0x80 ; pop %%ebp" \
  : "=a" (__res) \
  : "i" (__NR_##name),"b" ((long)(__##arg1)),"c" ((long)(__##arg2)), "d" ((long)(__##arg3)),"S" ((long)(__##arg4)),"D" ((long)(__##arg5)), "0" ((long)(__##arg6)) : "memory"); \
__syscall_return(type,__res); \
}

_syscall1_nomemory(int,close,int,fd)
_syscall3(int,open,const char*,pathname,int,flags,mode_t,mode)
_syscall2(int,chmod,const char*,path,mode_t,mode)
_syscall2_nomemory(int,fchmod,int,fd,mode_t,mode)
_syscall2(int,gettimeofday,struct timeval*,tv,struct timezone*,tz)
_syscall3(ssize_t,write,int,fd,const void*,buf,size_t,count)
_syscall3(ssize_t,read,int,fd,void*,buf,size_t,count)
_syscall1_nomemory(mode_t,umask,mode_t,mask)
_syscall3_nomemory(off_t,lseek,int,fd,off_t,offset,int,whence)
_syscall2(int,lstat,const char*,path,struct stat*,buf)
_syscall2(int,mkdir,const char*,pathname,mode_t,mode)
_syscall2(int,symlink,const char*,oldpath,const char*,newpath)
_syscall1(int,unlink,const char*,pathname)
_syscall2(int,utimes,const char*,filename,const struct timeval*,times)

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
