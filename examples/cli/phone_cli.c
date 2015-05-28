/*
 *   -*- mode: c; c-basic-offset: 8; -*-
 *  vim: expandtab:shiftwidth=4:tabstop=8:encoding=utf-8:textwidth=80 :
 */

/* 
 * Copyright 2006, 2007 @ Free Software Foundation. Licensed under the GPL 
 * Original author: dwight at supercomputer dot org
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
 */

/* 
 * phone.c: Sample program to show how libgsmc works, via cli.  
 * Usage:	./phone modem_line_from_ex phone_number
 * Example:
 *		1. Run ../gsm_emulator/gsm_emulator
 *			It will report which tty device to use, 
 *			like "/dev/pts/15"
 *	
 *		2. ./phone /dev/pts/15 867-5309
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gsmc.h"
#include "term.h"
#include "lgdebug.h"

void usage(char *av);

int
main(int ac, char **av) 
{
	lgdebug_init(D_ALL, NULL);

	if (ac != 3) {
		usage(av[0]);
		printf("Invalid number of arguments: %d\n", ac);
		exit(0);
	}

	if (!openPhone(av[1], "telit")) {
		printf("Cannot open device for the phone\n");
		exit(-1);
	}

	if (!activatePhone()) {
		printf("Cannot activate phone\n");
		closePhone();
		exit(-1);
	}

	printf("Placing a call to: %s\n", av[2]);
	if (!makeCall(av[2])) {
		printf("Jenny's not home\n");
		closePhone();
		exit(-1);
	}

	printf("Jenny is home!\n");
	closePhone();
	exit(0);
}

void usage(char *av)
{
	printf("Usage: %s modem_line_from_ex phone_number\n", av);
	printf("Example: phone /dev/pts/4 8675309\n");
}
