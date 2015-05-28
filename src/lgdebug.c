/*
 *  -*- mode: c; c-basic-offset: 8; -*-
 *  vim: shiftwidth=8:tabstop=8:encoding=utf-8:textwidth=80 :
 */

/* 
 * Copyright 2006 by the Free Software Foundation.
 * Original Author:  dwight at supercomputer.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Modeled after the work done by:
 *      http://www.elitecoders.de/mags/cscene/CS2/CS2-03.html
 */

/* 
 * Debugging facility for libgsmc. This supports the following:
 *  	- Selectable severity levels.
 *	- Vararg (printf-like) formating
 *	- Output can be saved to any specific disk file; default = stderr.
 *	- Timestamping (to microsecond granularity) included on the output.
 *
 *      TBD:
 *	- Optional logging to syslog.
 *	- Optional storing to a circular buffer. default = norollover,
 *		For tiny embedded systems, having this built in is far,
 *		far, better than logrotate.
 */
#include <stdio.h>
#include <sys/param.h>
#include <stdarg.h>
#include <sys/timeb.h>

#define	DEBUG
#include "lgdebug.h"

#define DBGBUFSIZE      1024            // Max size of debug output lines
#define GDTBUFSIZE      128             // Max size time strings

static unsigned int     lgdbg = 0;      // Current debugging level
static FILE *           lgfile;         // Output file for debugging
static int              ldinited = 0;
static char             dbgbuf[DBGBUFSIZE];
static char             gdtbuf[GDTBUFSIZE];
char *                  getdbgtime(void);

/*
 * lgdebug_init(): Set the debugging level, and output file.
 *	Input:	Debugging level.
 *		Name of file to contain debug output. stderr if null.
 *	Output:	None. stderr is used if there are any problems.
 */
void
lgdebug_init(unsigned int level, char *filename)
{
	int	rv = -1;		// Generic return value

	lgdbg = level;
	lgfile = stderr;

	if ((filename != NULL) && (strncmp(filename, "stderr", 6))) {
		lgfile = filename;
		if (strnlen(filename, MAXPATHLEN) >= MAXPATHLEN) {
			fprintf(stderr, "lgdebug_init: path too long\n");
			fprintf(stderr, "Using stderr for output instead\n");
			lgfile = stderr;	// reset here, in any case.
		} 
        }

        if (lgfile != stderr) {
            if ((lgfile = fopen(filename, "w")) == NULL) {
		perror(filename);
		fprintf(stderr, "Using stderr for output instead\n");
		lgfile = stderr;
            }
        }

        if (level & D_REPLAY) {
                fprintf(lgfile, "gsm_replay output. Version 1.0\n");
        }
}

/*
 * lgdbg(): Debugging printfs. NOTE WELL: Varargs!
 */
void
lgdprintf(unsigned int dbglvl, const char * format, ...)
{
        int i;
	va_list args;
	
	if (!(dbglvl & lgdbg)) {
		return;
	}

        i = sprintf(dbgbuf,"%s|%d|", getdbgtime(), getpid());
        
	va_start(args, format);
	vsnprintf(dbgbuf + i, DBGBUFSIZE - i, format, args);
	va_end(args);

        fprintf(lgfile, "%s", dbgbuf);
	fflush(lgfile);
}

void
lgdebug_close(void)
{
	lgdbg = 0;
	fflush(lgfile);
	fclose(lgfile);
}

void
lgdebug_set(unsigned int level)
{
	if (!ldinited) {	// Allow setting without requiring initing
		lgdebug_init(level, NULL);
	}

	lgdbg |= level;
}


int
lgdebug_get(void)
{
        return lgdbg;
}


struct timeb myt;

char *
getdbgtime()
{
        if (ftime(&myt) == -1) {
                return "ftime error";
        }
        snprintf(gdtbuf, GDTBUFSIZE, "%ld.%d", 
                myt.time, myt.millitm);
        return gdtbuf;
}
