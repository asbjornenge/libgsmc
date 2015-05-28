/*
 *   -*- mode: c; c-basic-offset: 8; -*-
 *  vim: expandtab:shiftwidth=4:tabstop=8:encoding=utf-8:textwidth=80 :
 * 
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
 * gsm_emulator.c: 	Acts as a psuedo-modem.
 *	This program will provide designated responses to a given GSM command.
 *
 * Since the ETSI standard consists of command-response strings, one can 
 * think of this as the backend to the front-end applications.
 *
 * In short, this program provides the response part of the ETSI
 * command-response mechanism for GSM commands.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include "lgdebug.h"

int mfd;                                // Master fd
int sfd;                                // Slave fd

#define CMDSIZE		1024	        // Max size of commands:
char rbuf[2*CMDSIZE];
char cmdbuf[2*CMDSIZE];		        // Buffer containing the commands

char *slavename;
extern char *ptsname();
static fd_set rfds;
static struct termios mtios;            // master termio structure


void docmd(char *);
static void  gsm_sigdebug();      // sighandler for changing dbg levels

main()
{
	int rv = -1;		        // generic return val
	int cbi;		        // cmd buffer index;

        // First set up our debugging signal handlers:   
        if ((signal(SIGUSR1, gsm_sigdebug)) == SIG_ERR) {
                printf("Cannot register SIGUSR1");
                exit(-1);
        }

        if ((signal(SIGUSR2, gsm_sigdebug)) == SIG_ERR) {
                printf("Cannot register SIGUSR2");
                exit(-1);
        }

	if ((mfd = open("/dev/ptmx", O_RDWR)) == -1) {	// Get master pty fd 
		perror("Can't get master pty");
		exit(-1);
	}

	if (grantpt(mfd) || unlockpt(mfd) || 
	    ((slavename = ptsname(mfd)) == NULL)) {
		perror("Can't init pty");
		close(mfd);
		exit(-1);
	}

        // FIXME: add baud rate parameter

	// We write the master; the other side writes the slave:
	printf("Psuedo Modem is on device: %s\n", slavename);

	if ((sfd = open(slavename, O_RDWR)) == -1) {	// Get slave pty fd 
		perror("Can't get slave pty");
		close(mfd);
		exit(-1);
	}

	FD_ZERO(&rfds);
	FD_SET(mfd, &rfds);

	cbi = 0;
	while(1) {
		rv = select(mfd+1, &rfds, NULL, NULL, NULL);
		if (rv != 1) {
			printf("Bad select: 0x%x\n", rv);
			continue;
		}

		if (FD_ISSET(mfd, &rfds)) {
			rv = read(mfd, rbuf, CMDSIZE);
			printf("read: #chars=0x%x, 0x%x\n", rv, rbuf[0]);

			if (rv > CMDSIZE) {
				rv = CMDSIZE;
			}

			if (rv + cbi >= CMDSIZE) {
				rv = CMDSIZE - cbi - 1;
			}

			strncpy(&cmdbuf[cbi], rbuf, rv);

			cbi += rv;

			if (strchr(rbuf, 0x04) != NULL) {
				printf("Got EOF - terminating\n");
				goto outs;
			}

			if ((strchr(rbuf, '\r') != NULL) ||
			   (strchr(rbuf, '\n') != NULL)) {
				cmdbuf[CMDSIZE + 1] = '\0';
				docmd(cmdbuf);
                                bzero(cmdbuf, sizeof cmdbuf);
				cbi = 0;
			}
			
                        bzero(rbuf, sizeof rbuf);
		}
	}

outs: 
	close(mfd);
	exit(0);
}

void
docmd(char *cmdbuf)
{
	printf("Got command: %s ", cmdbuf);
	if (strncmp(cmdbuf, "OK", 2) == 0) {	
		printf(" - ignoring\n");
		return;
	}
	printf("\n");
	write(mfd, "\r\nOK\r\n", 6);
	printf("wrote OK: \r\n");
        tcflush(mfd, TCIFLUSH);
}

static void  
gsm_sigdebug(int sig)                   // Change debug level
{
        int debuglevel;

        debuglevel = lgdebug_get();
        printf("Sig handler: sig = %d, ", sig);
        printf("old debug level = 0x%x\n", debuglevel);

        if (sig == SIGUSR1) {
                debuglevel++;
                printf("Got SIGUSR1. New debug level = 0x%x\n", debuglevel);
                lgdebug_set(debuglevel);
                
        } else if (sig == SIGUSR2) {
                if (debuglevel > 0) {
                    debuglevel--;
                    printf("Got SIGUSR2. New debug level = 0x%x\n", debuglevel);
                    lgdebug_set(debuglevel);
                } else {
                    printf("Got SIGUSR2. Not lowering debug level\n");
                }
        }
        return;
}
