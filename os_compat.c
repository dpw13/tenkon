/* See newlib/libgloss/or1k/syscalls.c */

#include <errno.h>
#include <reent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "serial.h"
#include "print_utils.h"

_ssize_t
_write_r(struct _reent * reent, int fd, const void *buf, size_t nbytes)
{
	writeStringToSerial((const char *)buf, nbytes);
	return (nbytes);
}

void
_exit(int rc)
{
	while (1) {}
}

int
_close_r(struct _reent *reent, int fildes)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_execve_r(struct _reent *reent, const char *name, char * const *argv,
		char * const *env)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_fstat_r(struct _reent *reent, int fildes, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int
_getpid_r(struct _reent *reent)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_gettimeofday(struct _reent *reent, struct timeval  *ptimeval, void *ptimezone)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_isatty_r(struct _reent *reent, int file)
{
	reent->_errno = ENOSYS;
	return 0;
}

int
_kill_r(struct _reent *reent, int pid, int sig)
{
	reent->_errno = ENOSYS;
	return -1;
}

int
_link_r(struct _reent *reent, const char *existing, const char *new)
{
	reent->_errno = ENOSYS;
	return -1;
}

_off_t
_lseek_r(struct _reent *reent, int file, _off_t ptr, int dir)
{
	return 0; //TODO: this does nothing cause serial can't seek
}

int
_open_r(struct _reent *reent, const char *file, int flags, int mode)
{
	reent->_errno = ENOSYS;
	return -1;
}

_ssize_t
_read_r(struct _reent *reent, int file, void *ptr, size_t len)
{
	int read = 0;
	char *buf = (char *)ptr;

	//writeStringToSerial("! read <- 0x", 13);
	//printU32(len);
	//writeSerial('\n');

	for (; len > 0; --len) {
		readSerial(buf);
		read++;
		/* echo */
		writeSerial(*buf);
		/* Exit on linebreak. Recommended terminal settings are that
		 * <enter> sends CRLF and to add an implicit CR to all LF */
		if (*buf == '\n') {
			break;
		}
		if (*buf == '\b' || *buf == 127) {
			/* model backspace */
			*--buf = 0;
		} else {
			buf++;
		}
	}
	//writeStringToSerial("! read -> 0x", 13);
	//printU32(len);
	//writeSerial('\n');
	return read;
}

int
_readlink_r(struct _reent *reent, const char *path, char *buf, size_t bufsize)
{
	reent->_errno = ENOSYS;
	return -1;
}

void *_sbrk_r (struct _reent *reent, ptrdiff_t incr) {
	extern int _heap_start;
	extern int _heap_end;

	static int *heap_end = NULL;
	int *prev_heap_end;

	//writeStringToSerial("\n! sbrk 0x", 10);
	//printU32(incr);
	//writeSerial('\n');

	if (heap_end == NULL) {
		heap_end = &_heap_start;
	}

	prev_heap_end = heap_end;
	heap_end += incr;

	if(heap_end >= &_heap_end) {
		reent->_errno = ENOMEM;
		return (void *)-1;
	}
	return (void *) prev_heap_end;
}

int
_stat_r(struct _reent *reent, const char *path, struct stat *buf)
{
	reent->_errno = EIO;
	return -1;
}

int
_unlink_r(struct _reent *reent, const char * path)
{
	reent->_errno = EIO;
	return (-1);
}

