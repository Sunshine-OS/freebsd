/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)fseek.c	8.3 (Berkeley) 1/2/94";
#endif
static const char rcsid[] =
  "$FreeBSD$";
#endif /* LIBC_SCCS and not lint */

#include "namespace.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "un-namespace.h"
#include "local.h"
#include "libc_private.h"

#define	POS_ERR	(-(fpos_t)1)

int
fseek(fp, offset, whence)
	register FILE *fp;
	long offset;
	int whence;
{
	int ret;

	/* make sure stdio is set up */
	if (!__sdidinit)
		__sinit();

	FLOCKFILE(fp);
	ret = _fseeko(fp, (off_t)offset, whence, 1);
	FUNLOCKFILE(fp);
	return (ret);
}

int
fseeko(fp, offset, whence)
	FILE *fp;
	off_t offset;
	int whence;
{
	int ret;

	/* make sure stdio is set up */
	if (!__sdidinit)
		__sinit();

	FLOCKFILE(fp);
	ret = _fseeko(fp, offset, whence, 0);
	FUNLOCKFILE(fp);
	return (ret);
}

/*
 * Seek the given file to the given offset.
 * `Whence' must be one of the three SEEK_* macros.
 */
int
_fseeko(fp, offset, whence, ltest)
	FILE *fp;
	off_t offset;
	int whence;
	int ltest;
{
	register fpos_t (*seekfn) __P((void *, fpos_t, int));
	fpos_t target, curoff;
	size_t n;
	struct stat st;
	int havepos;

	/*
	 * Have to be able to seek.
	 */
	if ((seekfn = fp->_seek) == NULL) {
		errno = ESPIPE;		/* historic practice */
		return (-1);
	}

	/*
	 * Change any SEEK_CUR to SEEK_SET, and check `whence' argument.
	 * After this, whence is either SEEK_SET or SEEK_END.
	 */
	switch (whence) {

	case SEEK_CUR:
		/*
		 * In order to seek relative to the current stream offset,
		 * we have to first find the current stream offset a la
		 * ftell (see ftell for details).
		 */
		if (fp->_flags & __SOFF)
			curoff = fp->_offset;
		else {
			curoff = (*seekfn)(fp->_cookie, (fpos_t)0, SEEK_CUR);
			if (curoff == -1)
				return (-1);
		}
		if (fp->_flags & __SRD) {
			curoff -= fp->_r;
			if (curoff < 0) {
				if (HASUB(fp)) {
					fp->_p -= curoff;
					fp->_r += curoff;
					curoff = 0;
				} else {
					errno = EBADF;
					return (-1);
				}
			}
			if (HASUB(fp)) {
				curoff -= fp->_ur;
				if (curoff < 0) {
					if (-curoff <= fp->_r) {
						fp->_p -= curoff;
						fp->_r += curoff;
						curoff = 0;
					} else {
						errno = EBADF;
						return (-1);
					}
				}
			}
		} else if ((fp->_flags & __SWR) && fp->_p != NULL) {
			n = fp->_p - fp->_bf._base;
			if (curoff > OFF_MAX - n) {
				errno = EOVERFLOW;
				return (-1);
			}
			curoff += n;
		}
		if (offset > 0 && curoff > OFF_MAX - offset) {
			errno = EOVERFLOW;
			return (-1);
		}
		offset += curoff;
		if (offset < 0) {
			errno = EINVAL;
			return (-1);
		}
		if (ltest && offset > LONG_MAX) {
			errno = EOVERFLOW;
			return (-1);
		}
		whence = SEEK_SET;
		havepos = 1;
		break;

	case SEEK_SET:
		if (offset < 0) {
			errno = EINVAL;
			return (-1);
		}
	case SEEK_END:
		curoff = 0;		/* XXX just to keep gcc quiet */
		havepos = 0;
		break;

	default:
		errno = EINVAL;
		return (-1);
	}

	/*
	 * Can only optimise if:
	 *	reading (and not reading-and-writing);
	 *	not unbuffered; and
	 *	this is a `regular' Unix file (and hence seekfn==__sseek).
	 * We must check __NBF first, because it is possible to have __NBF
	 * and __SOPT both set.
	 */
	if (fp->_bf._base == NULL)
		__smakebuf(fp);
	if (fp->_flags & (__SWR | __SRW | __SNBF | __SNPT))
		goto dumb;
	if ((fp->_flags & __SOPT) == 0) {
		if (seekfn != __sseek ||
		    fp->_file < 0 || _fstat(fp->_file, &st) ||
		    (st.st_mode & S_IFMT) != S_IFREG) {
			fp->_flags |= __SNPT;
			goto dumb;
		}
		fp->_blksize = st.st_blksize;
		fp->_flags |= __SOPT;
	}

	/*
	 * We are reading; we can try to optimise.
	 * Figure out where we are going and where we are now.
	 */
	if (whence == SEEK_SET)
		target = offset;
	else {
		if (_fstat(fp->_file, &st))
			goto dumb;
		if (offset > 0 && st.st_size > OFF_MAX - offset) {
			errno = EOVERFLOW;
			return (-1);
		}
		target = st.st_size + offset;
		if ((off_t)target < 0) {
			errno = EINVAL;
			return (-1);
		}
		if (ltest && (off_t)target > LONG_MAX) {
			errno = EOVERFLOW;
			return (-1);
		}
	}

	if (!havepos) {
		if (fp->_flags & __SOFF)
			curoff = fp->_offset;
		else {
			curoff = (*seekfn)(fp->_cookie, (fpos_t)0, SEEK_CUR);
			if (curoff == POS_ERR)
				goto dumb;
		}
		curoff -= fp->_r;
		if (curoff < 0) {
			if (HASUB(fp)) {
				fp->_p -= curoff;
				fp->_r += curoff;
				curoff = 0;
			} else
				goto dumb;
		}
		if (HASUB(fp)) {
			curoff -= fp->_ur;
			if (curoff < 0) {
				if (-curoff <= fp->_r) {
					fp->_p -= curoff;
					fp->_r += curoff;
					curoff = 0;
				} else
					goto dumb;
			}
		}
	}

	/*
	 * Compute the number of bytes in the input buffer (pretending
	 * that any ungetc() input has been discarded).  Adjust current
	 * offset backwards by this count so that it represents the
	 * file offset for the first byte in the current input buffer.
	 */
	if (HASUB(fp)) {
		if (curoff > OFF_MAX - fp->_r)
			goto abspos;
		curoff += fp->_r;	/* kill off ungetc */
		n = fp->_extra->_up - fp->_bf._base;
		curoff -= n;
		n += fp->_ur;
	} else {
		n = fp->_p - fp->_bf._base;
		curoff -= n;
		n += fp->_r;
	}
	/* curoff can be negative at this point. */

	/*
	 * If the target offset is within the current buffer,
	 * simply adjust the pointers, clear EOF, undo ungetc(),
	 * and return.  (If the buffer was modified, we have to
	 * skip this; see fgetln.c.)
	 */
	if ((fp->_flags & __SMOD) == 0 &&
	    target >= curoff &&
	    (curoff <= 0 || curoff <= OFF_MAX - n) &&
	    target < curoff + n) {
		size_t o = target - curoff;

		fp->_p = fp->_bf._base + o;
		fp->_r = n - o;
		if (HASUB(fp))
			FREEUB(fp);
		fp->_flags &= ~__SEOF;
		return (0);
	}

abspos:
	/*
	 * The place we want to get to is not within the current buffer,
	 * but we can still be kind to the kernel copyout mechanism.
	 * By aligning the file offset to a block boundary, we can let
	 * the kernel use the VM hardware to map pages instead of
	 * copying bytes laboriously.  Using a block boundary also
	 * ensures that we only read one block, rather than two.
	 */
	curoff = target & ~(fp->_blksize - 1);
	if ((*seekfn)(fp->_cookie, curoff, SEEK_SET) == POS_ERR)
		goto dumb;
	fp->_r = 0;
	fp->_p = fp->_bf._base;
	if (HASUB(fp))
		FREEUB(fp);
	fp->_flags &= ~__SEOF;
	n = target - curoff;
	if (n) {
		if (__srefill(fp) || fp->_r < n)
			goto dumb;
		fp->_p += n;
		fp->_r -= n;
	}
	return (0);

	/*
	 * We get here if we cannot optimise the seek ... just
	 * do it.  Allow the seek function to change fp->_bf._base.
	 */
dumb:
	if (__sflush(fp) ||
	    (*seekfn)(fp->_cookie, (fpos_t)offset, whence) == POS_ERR)
		return (-1);
	if (ltest && fp->_offset > LONG_MAX) {
		errno = EOVERFLOW;
		return (-1);
	}
	/* success: clear EOF indicator and discard ungetc() data */
	if (HASUB(fp))
		FREEUB(fp);
	fp->_p = fp->_bf._base;
	fp->_r = 0;
	/* fp->_w = 0; */	/* unnecessary (I think...) */
	fp->_flags &= ~__SEOF;
	return (0);
}
