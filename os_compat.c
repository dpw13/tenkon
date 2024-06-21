/* See newlib/libgloss/or1k/syscalls.c */

#include <errno.h>
#include <reent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

/* Write is actually the only thing we provide. All other are stubs.. */

void __attribute__((optimize("O0"))) _serial_outbyte(char c) {
    // TODO
}

_ssize_t
_write_r(struct _reent * reent, int fd, const void *buf, size_t nbytes)
{
	int i;
	char* b = (char*) buf;

	for (i = 0; i < nbytes; i++) {
		_serial_outbyte (*(b++));
	}
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
	reent->_errno = ENOSYS;
	return -1;
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
	reent->_errno = ENOSYS;
	return -1;
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
	reent->_errno = ENOSYS;
	return -1;
}

int
_readlink_r(struct _reent *reent, const char *path, char *buf, size_t bufsize)
{
	reent->_errno = ENOSYS;
	return -1;
}

void *_sbrk_r (struct _reent *reent, ptrdiff_t incr) {
	reent->_errno = ENOSYS;
	return NULL;
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

