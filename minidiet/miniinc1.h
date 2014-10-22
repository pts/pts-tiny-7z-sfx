/* Minimalistic macro definitions and declarations, subset of dietlibc. */

#ifndef _MINIINC1_H_
#define _MINIINC1_H_ 1

#ifndef __linux__
#error Linux is needed.
#endif
#ifndef __i386__
#error i386 is needed.  /* This fails on amd64 (__amd64__, __x86_64__). Good. */
#endif

#if 0
#include <stddef.h>  /* This is a compiler-specific #include, for size_t etc. */
#else
typedef unsigned size_t;
typedef int ptrdiff_t;
#define NULL ((void *)0)  /* For C++, it should be 0. */
__extension__ typedef long long int64_t;
__extension__ typedef unsigned long long uint64_t;
#endif

#define __WAIT_INT(status)    (*(__const int *) &(status))
#define __WTERMSIG(status)    ((status) & 0x7f)
#define __WEXITSTATUS(status) (((status) & 0xff00) >> 8)
#define __WIFEXITED(status)   (__WTERMSIG(status) == 0)
#define WIFEXITED(status)     __WIFEXITED(__WAIT_INT(status))
#define WEXITSTATUS(status)   __WEXITSTATUS(__WAIT_INT(status))

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2

#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

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

__extension__ typedef unsigned long long int __u_quad_t;
__extension__ typedef __u_quad_t __dev_t;
__extension__ typedef unsigned int __uid_t;
__extension__ typedef unsigned int __gid_t;
__extension__ typedef unsigned long int __ino_t;
__extension__ typedef unsigned int __mode_t;
__extension__ typedef unsigned int __nlink_t;
__extension__ typedef long int __off_t;
__extension__ typedef int __ssize_t;
__extension__ typedef long int __blksize_t;
__extension__ typedef long int __blkcnt_t;
__extension__ typedef long int __time_t;
__extension__ typedef int __pid_t;

typedef __ssize_t ssize_t;
typedef __pid_t pid_t;
typedef __time_t time_t;
typedef __mode_t mode_t;
typedef signed long suseconds_t;

struct stat {
  __dev_t st_dev;
  unsigned short int __pad1;
  __ino_t st_ino;
  __mode_t st_mode;
  __nlink_t st_nlink;
  __uid_t st_uid;
  __gid_t st_gid;
  __dev_t st_rdev;
  unsigned short int __pad2;
  __off_t st_size;
  __blksize_t st_blksize;
  __blkcnt_t st_blocks;
  __time_t st_atime;
  unsigned long int st_atimensec;
  __time_t st_mtime;
  unsigned long int st_mtimensec;
  __time_t st_ctime;
  unsigned long int st_ctimensec;
  unsigned long int __unused4;
  unsigned long int __unused5;
};

struct utsname {
  char sysname[65];
  char nodename[65];
  char release[65];
  char version[65];
  char machine[65];
  char domainname[65];
};

struct timeval {
  time_t tv_sec;
  suseconds_t tv_usec;
};

struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

struct __stdio_file;
typedef struct __stdio_file FILE;

extern int errno;
extern FILE *stdin, *stdout, *stderr;

extern FILE *fopen(const char *path, const char *mode) ;
extern int fclose(FILE *stream) ;
extern int ferror(FILE *stream) ;
extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) ;
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) ;
extern int fseek(FILE *stream, long offset, int whence) ;
extern long ftell(FILE *stream) ;
extern int fputs(const char *s, FILE *stream) ;
extern int utimes(const char * filename, struct timeval * tvp);
extern int fputc(int c, FILE *stream) ;
static __inline__ int putchar(int c) { return fputc(c, stdout); }
extern int fileno(FILE *stream) ;

extern int symlink(const char *oldpath, const char *newpath) ;
extern int gettimeofday(struct timeval *tv, struct timezone *tz) ;
extern int chmod (const char *__file, mode_t __mode) ;
extern int fchmod (int __fd, mode_t __mode) ;
extern int mkdir (const char *__path, mode_t __mode) ;
extern mode_t umask (mode_t __mask);
extern size_t strlen(__const char *__s) __attribute__((__nothrow__)) __attribute__((__pure__)) __attribute__((__nonnull__(1)));
extern void *malloc(size_t __size) __attribute__((__nothrow__)) __attribute__((__malloc__)) ;
extern void *realloc(void *__ptr, size_t __size) __attribute__((__nothrow__)) __attribute__((__malloc__)) __attribute__((__warn_unused_result__));
extern void free(void *__ptr) __attribute__((__nothrow__));
extern void *memcpy(void *__restrict __dest,   __const void *__restrict __src, size_t __n) __attribute__((__nothrow__)) __attribute__((__nonnull__(1, 2)));
extern char *strcpy(char *__restrict __dest, __const char *__restrict __src) __attribute__((__nothrow__)) __attribute__((__nonnull__(1, 2)));
extern ssize_t readlink(__const char *__restrict __path, char *__restrict __buf, size_t __len) __attribute__((__nothrow__)) __attribute__((__nonnull__(1, 2))) ;
extern int lstat(__const char *__restrict __file, struct stat *__restrict __buf) __attribute__((__nothrow__)) __attribute__((__nonnull__(1, 2)));
extern char *strcat(char *__restrict __dest, __const char *__restrict __src) __attribute__((__nothrow__)) __attribute__((__nonnull__(1, 2)));
extern char *strdup(__const char *__s) __attribute__((__nothrow__)) __attribute__((__malloc__)) __attribute__((__nonnull__(1)));
extern int strcmp(__const char *__s1, __const char *__s2) __attribute__((__nothrow__)) __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
extern int strncmp(__const char *__s1, __const char *__s2, size_t __n) __attribute__((__nothrow__)) __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
extern ssize_t write(int __fd, __const void *__buf, size_t __n) ;
extern void exit(int __status) __attribute__((__nothrow__)) __attribute__((__noreturn__));
extern char *strstr(__const char *__haystack, __const char *__needle) __attribute__((__nothrow__)) __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
extern int stat(__const char *__restrict __file, struct stat *__restrict __buf) __attribute__((__nothrow__)) __attribute__((__nonnull__(1, 2)));
extern char *getenv(__const char *__name) __attribute__((__nothrow__)) __attribute__((__nonnull__(1))) ;
extern int execv(__const char *__path, char *__const __argv[]) __attribute__((__nothrow__)) __attribute__((__nonnull__(1)));
extern int uname(struct utsname *__name) __attribute__((__nothrow__));
extern int open(__const char *__file, int __oflag, ...) __attribute__((__nonnull__(1)));
extern ssize_t read(int __fd, void *__buf, size_t __nbytes) ;
extern int close(int __fd);
extern int memcmp(__const void *__s1, __const void *__s2, size_t __n) __attribute__((__nothrow__)) __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
extern __pid_t fork(void) __attribute__((__nothrow__));
extern __pid_t waitpid(__pid_t __pid, int *__stat_loc, int __options);
extern int unlink(__const char *__name) __attribute__((__nothrow__)) __attribute__((__nonnull__(1)));

#endif  /* _MINIINC1_H_ */
