/* 
 *   -*- mode: c; c-basic-offset: 8; -*-
 *  vim: shiftwidth=4:tabstop=8:encoding=utf-8:textwidth=80 :
 *
 * Declarations for libgsmc debugging.
 *
 * Copyright (C) 2006,2007 Free Software Foundation, Inc.
 * Author: dwight at supercomputer.org.
 * 
 * This free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.  
 */

#ifndef LIBGSMC_DEBUG
#define LIBGSMC_DEBUG	1

/* 
 * definitions for debugging
 */

// All debug values are bit-wise dependent.

// Call establishment and clearing:
#define D_CALLED	(1<<0)		// Function being called
#define D_CALLING	(1<<1)		// Calling other subroutines
#define D_FUNC		(1<<2)		// General function debugging
#define D_GROT1		(1<<3)		// Lower level details
#define D_GROT2		(1<<4)		// Even more Lower level details
#define D_GROT3		(1<<5)		// Lowest level details
#define D_REPLAY	(1<<5)		// Logging GSM replay messages
#define D_TERM		(1<<14)		// tty interface debugging
#define D_ALL		(0xffff)	// All debugging messages

// API:
#if !defined(DEBUG)
#define lgdebug_init(level, filename)
#define lgdprintf(dbglvl, format, ...)
#define lgdebug_close(void)
#define lgdebug_set(level)
#define lgdebug_get(void)	0
#else 	// DEBUG
// If a non-debugging version is being built, this API is stubbed out:
void lgdebug_init(unsigned int level, char *filename);
void lgdprintf(unsigned int dbglvl, const char * format, ...);
void lgdebug_close(void);
void lgdebug_set(unsigned int level);
int lgdebug_get(void);
#endif 	// ! DEBUG

#endif // ! LIBGSMC_DEBUG
