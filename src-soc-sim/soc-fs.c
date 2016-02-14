/*-****************************************************************************/
/*-                                                                           */
/*-            Copyright (c) of hyperCOS.                                     */
/*-                                                                           */
/*-  This software is copyrighted by and is the sole property of socware.net. */
/*-  All rights, title, ownership, or other interests in the software remain  */
/*-  the property of socware.net. The source code is FREE for short-term      */
/*-  evaluation, educational or non-commercial research only. Any commercial  */
/*-  application may only be used in accordance with the corresponding license*/
/*-  agreement. Any unauthorized use, duplication, transmission, distribution,*/
/*-  or disclosure of this software is expressly forbidden.                   */
/*-                                                                           */
/*-  Knowledge of the source code may NOT be used to develop a similar product*/
/*-                                                                           */
/*-  This Copyright notice may not be removed or modified without prior       */
/*-  written consent of socware.net.                                          */
/*-                                                                           */
/*-  socware.net reserves the right to modify this software                   */
/*-  without notice.                                                          */
/*-                                                                           */
/*-  To contact socware.net:                                                  */
/*-                                                                           */
/*-             socware.help@gmail.com                                        */
/*-                                                                           */
/*-****************************************************************************/

#include "_soc.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int _close(int fd)
{
	return -1;
}

int _stat(const char *filename, struct stat *st)
{
	return -1;
}

int _fstat(int file, struct stat *st)
{
	return -1;
}

int _getpid(void)
{
	return 1;
}

int _gettimeofday(void *tp, void *tzp)
{
	return -1;
}

int _isatty(int fd)
{
	return (fd ^ STDOUT_FILENO) ^ (fd ^ STDERR_FILENO);
}

int _kill(int pid, int sig)
{
	return 0;
}

off_t _lseek(int fd, off_t off, int whence)
{
	return -1;
}

int _open(const char *buf, int flags, int mode)
{
	return -1;
}

int _write(int fd, const char *buf, int nbytes)
{
	switch (fd) {
	case 0:
	case 1:
		uart_w(&u0, buf, nbytes);
		break;

	}
	return 0;
}

int _read(int fd, char *buf, int nbytes)
{
	return 0;
}

void *core_alloc(unsigned sz, int align_bits);

char *_sbrk(int nbytes)
{
	return core_alloc(nbytes, 0);
}

int _rename(const char *oldpath, const char *newpath)
{
	return -1;
}

int _unlink(const char *pathname)
{
	return -1;
}
