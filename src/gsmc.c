/*
 *  -*- mode: c; c-basic-offset: 8; -*-
 *  vim: expandtab:shiftwidth=4:tabstop=8:encoding=utf-8:textwidth=80 :
 */

/* gsmc.c
 * 
 * Gsm communications library
 * 
 * Asbjorn Andersen <mind@hanafjedle.net>
 * 
 * Based upon: 
 * gomunicator by Robert Woerle <robert@linuxdevelopment.org>
 * picocom by Nick Patavalis <npat@inaccessnetworks.com>
 *    
 * Documentation can be found in the header file "gsmc.h" or online at
 * http://libgsmc.hanafjedle.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA 
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>

#include "gsmc.h"
#include "term.h"
#include "lgdebug.h"
#include "etsi707.h"

phoneOptions processModel(phoneOptions pPhone,char *pline);

/***************************************************************************/

phoneOptions phone;

int tty_fd = -1, bufIndex, in_command; 
bool gotOK, got_reply, TRUE, FALSE;
const char CRLFString[] = "\r\n";;
char at_buf[BUFFER_SIZE];

/***************************************************************************/

 void initPhone(void) 
 {	
 	TRUE,FALSE = true,false;
 	gotOK = got_reply = false;
 	phone.has_prefix = phone.has_postfix = false;
 	phone.security = 0;
 	phone.state = -1;
 	sprintf(phone.carrier,"UNKNOWN");
 	phone.clcc = 0;
 	phone.cops = 0;
 	phone.fun = 0;
 	phone.number = 0;
 	phone.postfix[2] = 0,0;
 	phone.prefix[2] = 0,0;
 	phone.sms = 0;
 }

/***************************************************************************/
 
int open_phone_connection_from_tty_interface(char *interface)
{	
	int	retVal;
	int r;
	
	retVal = -1;
	lgdprintf(D_CALLED, "open_phone_connection_from_tty_interface:\n");
	lgdprintf(D_CALLED, "interface=%s\n", interface);
	tty_fd = open(interface, O_RDWR | O_NONBLOCK);
	lgdprintf(D_CALLED, "tty_fd=%d\n", tty_fd);
	if (tty_fd >= 0) {
		r	= term_set(tty_fd,1,115200,P_NONE,8,FC_NONE,1,1);
		lgdprintf(D_FUNC, "term_set returned %d\n", r);
		if (r >= 0) {
			r	= term_apply(tty_fd);
			if (r >= 0) {
				retVal	=  r;
			}
		}
	}
	return retVal;
}

/***************************************************************************/
 
bool openPhone(char *devName, char *pmfg)
{	
	initPhone();
	int r,err_msg_len;
	char *errmsg;
	
	lgdprintf(D_CALLED, "openPhone:\n");
	etsi_init(pmfg);
	r	= term_lib_init();
	if (r >= 0) {
		r	= open_phone_connection_from_tty_interface(devName);
		if (r == -1) {
			printf("cannot open connection to phone module in %s: %s\n", 
					devName, strerror(errno));
		}
	}
	else
	{
		printf("term_lib_init() failed: %s\n",  
					term_strerror(term_errno, errno));
	}
	if (r >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/***************************************************************************/

ssize_t writen_ni(int fd, const void *buff, size_t n) 
{
	size_t nl;
	ssize_t nw;
	const char *p;

	p = buff;
	nl = n;
	while (nl > 0) {
		lgdprintf(D_GROT3, "writen_ni: %s\n", buff);
		do {
		       nw = write(fd, p, nl);
		} while ( nw < 0 && errno == EINTR );
		if ( nw <= 0 ) break;
		nl -= nw;
		p += nw;
	}

	return n - nl;
}

/***************************************************************************/

bool sendCommand(const char *cmd)
{
	if(phone.has_prefix)
	{
		if(writen_ni(tty_fd,phone.prefix,2) < 2)
			return false;
	}
	
	if (writen_ni(tty_fd, cmd, strlen(cmd)) < strlen(cmd))
		return false;
	if (writen_ni(tty_fd, CRLFString, 2) < 2)
		return false;
		
	if(phone.has_postfix)
	{
		if(writen_ni(tty_fd,phone.postfix, 2) < 2)
			return true;
	}
		
	in_command=1;
	return true;
}

/***************************************************************************/

bool checkPIN(void)
{
	if (tty_fd <= 0)
		return false;

	if (!pmfg || !pmfg->CPIN)  {
		printf("checkPIN: CPIN not supported for this modem\n");
		return false;
	}


	if (!sendCommand("at+cpin?"))
		return false;
	
	gotOK = false;
	got_reply = false;
	
	tryReadPhone(4);
	
	return gotOK;
}

/***************************************************************************/

void setPrefix(char fix[2])
{
	phone.has_prefix = true;
	memcpy(phone.prefix,fix,2);
}

/***************************************************************************/

void setPostfix(char fix[2])
{
	phone.has_postfix = true;
	memcpy(phone.postfix,fix,2);
}

/***************************************************************************/

bool setPIN(int PIN)
{
	snprintf(at_buf, pmfg->CPIN->csz+SZ_AT1, "%s%d", pmfg->CPIN->cmd, PIN);
	lgdprintf(D_CALLED, "setPIN: atbuf=%s\n", at_buf);
	if(sendCommand(at_buf) && checkPIN()) {
		return true;
	}
	else {
		return false;
	}
}

/***************************************************************************/
 
bool activatePhone(void)
{
	if (!pmfg || !pmfg->CFUN)  {
		printf("activatePhone: CFUN not supported for this modem\n");
		return false;
	}

	// ETSI 7.07: "1" here == full functionality
	snprintf(at_buf, pmfg->CFUN->csz+SZ_AT1, "at+%s%d", pmfg->CFUN->cmd, 1);
	lgdprintf(D_CALLED, "activatePhone: atbuf=%s\n", at_buf);
	return(phoneGetOK(at_buf));	
}

/***************************************************************************/

bool setDefaultOperator(void)
{
	sprintf(at_buf,"at+cops=0");
	return phoneGetOK(at_buf);
}

/***************************************************************************/

bool setOperator(int cops)
{
	sprintf(at_buf,"at+cops=%d", cops);
	return phoneGetOK(at_buf);
}

/***************************************************************************/
 
int getFUN(void)
{
	phone.fun=-1;
	if (!phoneGetOK("at+cfun?"))
		return -1;
	return phone.fun;
}

/***************************************************************************/

int getOperators(void)
{
	phone.cops = -1;
	if (!phoneGetOK("at+cops?"))
		return -1;
	return phone.cops;
}

/***************************************************************************/

int getModemFD(void)
{
	return tty_fd;
}

/***************************************************************************/

void processReadLine(char *line) 
{
	
	if (!strcmp(line, "OK")) {
		gotOK = true;
		got_reply = true;
		return;
	}
	else if (!strcmp(line, "ERROR")) {
		printf("Recieved ATC-ERROR from modem\n");
		gotOK = false;
		got_reply = true;
		return;
	}
	else if (!strncmp(line, "+CMS ERROR:", 11)) {
		printf("Recieved ATC-CMS-ERROR from modem\n");
		gotOK = false;
		got_reply = true;
		return;
	}
	else if (!strcmp(line, "RING") || !strcmp(line,"+CRING: VOICE"))  {
		phone.state = 1;
		return;
	}
	else if (!strcmp(line, "3")) {
		hangupPhone();
		return;
	}
	else if (!strcmp(line, "NO CARRIER"))  {
		if (phone.state == 1 || phone.state == 0)
			hangupPhone();
		
		phone.state = -1;
		sprintf(phone.carrier,"NO CARRIER");
		return;
	}
	else if (!strncmp(line, "+CPIN: ", 7)) {
		line+=7;
		if (!strcmp(line, "READY"))
		{
			gotOK = true;
			phone.security = 1;
			return;
		}
		else if (!strcmp(line, "SIM PIN")) {
				phone.security = -1;
				return;
			}
		else {
			printf("Phone security is unknown...\n");
			phone.security = 0;	
			return;
		}
	}
	else if (!strncmp(line, "+CFUN: ", 7)) {
		line+=7;
		phone.fun=atoi(line);		
		return;
	}
	else if (!strncmp(line, "+COPS: ", 7)) {
		char *end;
		line+=7;
		if (line[1] == ',') {
			phone.cops=1;
			line+=2;
			if (line[1] == ',') {
				line+=2;
				if (line[0]=='"') {
					line++;
					end=index(line,'"');
					if (end) {
						*end='\0';
					}
					sprintf(phone.carrier,"%s",line);
				}
			}
		} else
			phone.cops=0;
		return;
	}
	else if (!strncmp(line, "+CLIP: ", 7)) {
		char str[64],*end;
		line+=7;
		if (line[0]=='"') {
			line++;
			end=index(line,'"');
			if (end) {
				*end='\0';
			}
			sprintf(str,"Call from %s", line);
			phone.state = 1;
			phone.number = atoi(line);
		}
		return;
	}
	else if (!strncmp(line, "+CMTI: \"SM\",", 12)) {
		line+=12;
		phone.sms++;
		return;
	}
	else if (!strncmp(line, "+CLCC: ", 7)) {
		phone.clcc++;
		return;
	}
	else if (!strncmp(line, "+CMGS: ", 7)) {
		phone.number = atoi(line+7);
		return;
	}
	else if (!strncmp(line, "+CPMS: ", 7)) {
		printf("%s\r\n", line);
		return;
	}
}

/***************************************************************************/

int phone_strip_garbage(int *end) 
{	
	int pos=0,removed=0;
	char *src=at_buf;
	char *dst=at_buf;
	while (pos <= *end) {
		if (*src >= ' ' || *src == '\r' || *src == '\n' || *src == '\0')
			*dst++=*src;
		else
			removed++;
		src++;
		pos++;
	}
	*dst='\0';
	*end-=removed;
	return removed;
}

/***************************************************************************/

int phone_check_complete(int *end) {
	
	int pos=0;
	while (pos <= *end) {
		if (at_buf[pos] == '\r' || at_buf[pos] == '\n' || (at_buf[0] == '>' && pos > 0)) {
			char line[pos+1];
			memcpy(line, at_buf, pos);
			line[pos++]='\0';
			memcpy(at_buf, at_buf+pos, BUFFER_SIZE-pos);
			*end-=pos;
			if (line[0])
				processReadLine(line);
			return pos;
		}
		pos++;
	}
	return 0;
}

/***************************************************************************/

void dump_buffer(char *text, char *str, int len) {
	
	fprintf(stderr,"%s",text);
	while (len > 0) {
		if (*str >= ' ')
			fprintf(stderr,"%c", *str++);
		else
			fprintf(stderr,"(%02x)", *str++);
		len--;
	}
}

/***************************************************************************/

void tryReadPhone(int timeout) 
{	
	int readLength, sel_res, remove;
	int bufEndIndex = -1;
	fd_set sel_readfds;
	struct timeval sel_timeout;

	if (tty_fd <= 0)
		return;
	
	FD_ZERO(&sel_readfds);
	sel_timeout.tv_sec=timeout;
	sel_timeout.tv_usec=0;
	while (bufEndIndex < 1023 && (!got_reply || !timeout))
	{	
		FD_SET(tty_fd, &sel_readfds);
		sel_res=select(tty_fd+1, &sel_readfds, NULL, NULL, &sel_timeout);

		if (sel_res == -1) {
			perror("select()");
			return;
		} else
		if (sel_res == 0) {
			return;
		}
				
		readLength = read(tty_fd, at_buf+bufEndIndex+1, 1023 - bufEndIndex);

		if (readLength > 0) {
			bufEndIndex+=readLength;
			do {
				if (bufEndIndex >= 1023) {
					dump_buffer("Buffer is ",at_buf,bufEndIndex+1);
					char *errmsg = " the at output is too big...\n";
					printf("%s\n",errmsg);
				}
				remove=phone_strip_garbage(&bufEndIndex);
				remove=phone_check_complete(&bufEndIndex);
			} while (remove);
		}
	}
}

/***************************************************************************/

void listenToModem(void) 
{	
	got_reply = false;
	gotOK = false;
	
	struct pollfd phonePoll; 

	if(tty_fd <= 0)
		return;

	phonePoll.fd = tty_fd;
	phonePoll.events = POLLIN;

	if (poll(&phonePoll, 1, -1))
	{
		if(phonePoll.revents & POLLIN) 
		{
			tryReadPhone(0);
		}
		else if(phonePoll.revents & POLLHUP)
		{
			printf("A hangup has occured on modem.\n");
		}
		else if(phonePoll.revents & POLLERR)
		{
			printf("An error has occured on the modem.\n");
		}
		else if(phonePoll.revents & POLLNVAL)
		{
			printf("A NVAL error occured on the modem.\n");
			printf("This might indicate that the modems fildescriptor is closed.\n");
		}
	}
	else perror("poll:");
}

/***************************************************************************/

bool phoneGetOK(const char *query) 
{
	
	if (tty_fd <= 0) {
		return false;
	}

	if (!sendCommand(query)) {
		return false;
	}

	gotOK = false;
	got_reply = false;
	
	tryReadPhone(3);

	return gotOK;
}

/***************************************************************************/

bool phoneGetOK_slow(const char *query) 
{
	if (tty_fd <= 0)
		return false;

	if (!sendCommand(query))
		return false;

	gotOK = false;
	got_reply = false;
	
	tryReadPhone(10);

	return gotOK;
}

/***************************************************************************/

bool answerPhone(void)
{
		phoneGetOK("ata");
		phone.state = 0;
}


/***************************************************************************/

void hangupPhone(void)
{
	if (!pmfg || !pmfg->CFUN)  {
		// Not much one can do here except print a warning.
		printf("hangupPhone: CFUN not supported for this modem\n");
	}
	phoneGetOK("ath0");
	phone.state = -1;	
	phone.number = 0;
		
	tryReadPhone(2);
}

/***************************************************************************/

bool makeCall(char *dialString)
{
	char callCmd[20];

	sprintf(callCmd,"atdt%s;",dialString);
	
	if (phone.state != -1)
		return false;

	if (!sendCommand(callCmd))
		return false;
		
	phone.state = 0;
	return true;
}


/***************************************************************************/

void printPhoneStatus(void)
{
	
	printf("***************************************************\n");
	printf("Phone level functionality, FUN: %d\n",phone.fun);
	printf("Number of available operators: 	%d\n",phone.cops);
	printf("Currently handeling number: 	%d\n",phone.number);
	printf("List of current calls: 		%d\n",phone.clcc);
	printf("Number of available sms's:	%d\n",phone.sms);
	printf("Current carrier:		%s\n",phone.carrier);
	if(phone.has_prefix)
	printf("Has prefix:			yes\n");
	else
	printf("Has prefix:			no\n");
	if(phone.has_postfix)
	printf("Has postfix:			yes\n");
	else
	printf("Has postfix:			no\n");
	printf("Current prefixvalues:		%x,%x\n",phone.prefix[0],phone.prefix[1]);
	printf("Current postfixvalues:		%x,%x\n",phone.postfix[0],phone.postfix[1]);
	printf("Current phone state: 		%d\n",phone.state);
	printf("Current phone security state:	%d\n",phone.security);
	printf("***************************************************\n");
}

/***************************************************************************/

phoneOptions getPhoneStatus(void)
{
	return phone;
}

/***************************************************************************/

void closePhone(void)
{
	if(phone.state != -1)
		hangupPhone();
	
	if(tty_fd > 0) {
		close(tty_fd);
		tty_fd = -1;
	}
}
