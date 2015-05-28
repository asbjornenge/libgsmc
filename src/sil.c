/*
 * -*- mode: c; c-basic-offset: 8; -*-
 *  vim: expandtab:shiftwidth=4:tabstop=8:encoding=utf-8:textwidth=80 :
 *
 * Copyright (C) 2007 Free Software Foundation, Inc.
 * Original Author: Asbjorn Andersen <asbjorn@hanafjedle.net>
 * This file is part of the libgsmc library.
 *
 *  sil.c: This file contains the ETSI 07.07 implementation required to
 *  support Silicon Laboratories GSM modules.
 * 
 *  Supported phones:
 *     HTC Blueangel,
 *     other phones that use this chip here...
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

/* Refereneces:
 *	A: ETSI 07.07 standard. www.etsi.org.
 *  B: HTC Blueangel GSM Referance - http://wiki.xda-developers.com/index.php?pagename=BlueangelGSM
 */

#include <stdio.h>
#include <sys/time.h>
#include "etsi707.h"

/* Notes: 
 * 	1. The module always sends back <CR><LF>OK<CR><LF>. Ref A. S 3.2.5 
 *		p. 16 
 *	2. The default timeout, is 100 ms (A: S. 3.2.4, p. 14) unless
 *		specified on pps. 14-16. We use twice this value in case
 *		of some odd clock skew.
 */

struct timeval sil_TelTimeDefault = {
	.tv_sec = 0,
	.tv_usec = 2*100,
};

struct timeval sil_TelTime20secs = {
	.tv_sec = 20,
	.tv_usec = 0,
};

struct timeval sil_CallTimeDefault = {
	.tv_sec = 60,
};

/* The GCR elements containing the currently supported functionality:	*/

/* sil set phone functionality:
 * Parameters:	
 *      0 - Minimum functionality. AT interface not accessible.
 *		1 - Mobile full functionality, power saving disabled. DEFAULT.
 *		2 - Disabled TX.
 *		4 - Disabled both TX and RX.
 *		5 - Mobile full functionality with power saving enabled.
 * SEL 0 Page: 83, SEL 1 Page: 286, SEL 2 Page: 484
 *
 * Ref. B: p. 20; S. 3.5.2.4. p 83.
 */
	
static GCR sil_cfun = {
	.cmd = 		"cfun=",
	.csz =		sizeof sil_cfun.cmd,
	.resp = 	"OK",
	.nrparams = 	1,
	.tv = 	&sil_TelTimeDefault,
};

/* 
 * sil Enter PIN, ETSI 8.3:
 * AT+CPIN[=<pin> [,<newpin>]]
 * AT+CPIN?
 *
 * SEL 0 Page: 84, SEL 1 Page: 287, SEL 2 Page: 485
 *
 * Timeout: 20 secs 
 * Ref. B: p. 20; S. 3.5.2.4.3 p 84.
 */
static GCR sil_cpin = {
	.cmd = 	"at+cpin=",
	.csz =	sizeof sil_cpin.cmd+1, // +1 because of =
	.resp = "OK",
	.resp = 	"READY",
	.nrparams = 	1,		// Maximum of 2 response words 
	.tv = 	&sil_TelTime20secs,
};


/* 
 * sil dial:
 * 	This is a shortcut, by combining the AT and D commands,
 * 	just to get this running. 
 * FIXME: Add more atomic commands.
 */
static GCR sil_dial = {
	.cmd =		"atd",
	.resp = 	"",
	.nrparams = 	0,
	.tv = 		&sil_CallTimeDefault,
};

#ifdef NOTYET
/********** Not fully implemented yet **********************************/

/* sil set default operator:						*/
GCR sil_SetDefaultOperator = {
	.cmd = 	"at+cops=0",
	.resp = "OK",
	.tv = {10, 0},
};

/* sil set operator:						*/
GCR sil_SetOperator = {
	.cmd = 	"at+cops=%d",
	.resp = "OK",
	.tv = {10, 0},
};

/* sil get fun operator:						*/
GCR sil_activate = {
	.cmd = 	"at+cfun?",
	.resp = "OK",
	.tv = {10, 0},
};

/* sil set default operator:						*/
GCR sil_GetOperators = {
	.cmd = 	"at+cops=?",
	.resp = "OK",
	.tv = {10, 0},
};
#endif // NOTYET

/*
 * sil:	The support for the sils implementation of ETSI 07 
 */

struct etsi_07_07 sil = {		// The Full ETSI 7.07 Specification:
	.mgf = 		"sil",	// Manufacturer name
	.model =	"Si4205",	// Model

	/* General Commands. A: Section 5, pps 14-19			*/
	.CGMI =  NULL,	// +CGMI: Req. Mfg Id		5.1/p14
	.CGMM =  NULL,	// +CGMM: Req. Model Id. 	5.2/p15
	.CGMR =  NULL,	// +CGMR: Req. revision Id. 	5.3/p15
	.CGSN =  NULL,	// +CGMR: Req. Serial #. 	5.4/p16
	.CSCS =  NULL,	// +CSCS: Sel. TE char set	5.5/p16
	.CIMI =  NULL,	// +CIMI: Req. Itl Mob Subs ID	5.6/p17
	.CMUX =  NULL,	// +CMUX: Mux mode              5.7/p17
	.CMUX =  NULL,	// +CMUX: Mux mode              5.7/p17
	.GTAZ =  NULL,	// Generic TA: Z                5.8/p19
	.GTAF =  NULL,	// Generic TA: F 
	.WS46 =  NULL,	// +WS46: Select Wireless Net   5.9/p19

	/* Call control commands and methods. A: Section 6, pps 21-35	*/
	.CSTA =  NULL,	// +CSTA: Sel. type of address	6.1/p21
	.D =     NULL,	// D: Dial command		6.2/p21
	.DPB =   NULL,	// D: Dir. Dir Phone Book       6.3/p22
	.CMOD =  NULL,	// +CMOD: Call Mode             6.4/p23
	.CHUP =  NULL,	// +CCHUP: Call Hangup          6.5/p24
	.CBST =  NULL,	// +CBST: Sel Bearer Service    6.7/p26
	.CRLP =  NULL,	// +CRLP: Radio Link Protocol   6.8/p28
	.CR =    NULL,	// +CR: Service Reporting Ctl   6.9/p28
	.CEER =  NULL,	// +CEER: Ext. Error Report     6.10/p29
	.CEER =  NULL,	// +CEER: Ext. Error Report     6.10/p29
	.CRC =   NULL,	// +CRC: Cell Result Codes      6.11/p30
	.CHSD =  NULL,	// +CHSD: HSCSD param           6.12/p30
	.CHST =  NULL,	// +CHST: HSCSD transparent cfg 6.13/p31
	.CHSN =  NULL,	// +CHSN: HSCSD non-tprnt cfg   6.14/p31
	.CHSR =  NULL,	// +CHSR: HSCSD param report    6.16/p33
	.CHNS =  NULL,	// +CHNS: Single # scheme       6.17/p33
	.CVHU =  NULL,	// +CVHU: Voice Hangup Ctl      6.18/p34
	.CV120 = NULL,	// +CV120: V.120 Rate Protocol  6.19/p35
					// V.25ter Call Ctl Cmds TBD    6.20/p35
					// V.25ter Data Comp Cmds TBD   6.20/p35

	/* Network Service Related Commands. A: Section 7, pps 37-54	*/
	.CNUM = NULL,	// +CNUM: Subscriber Number    7.1/p37
	.CREG = NULL,	// +CREG: Network Registration 7.2/p38
	.COPS = NULL,	// +COPS: Operator Selection   7.3/p39
	.CLCK = NULL,	// +CLCK: Facility Lock        7.4/p40
	.CPWD = NULL,	// +CPWD: Change Password      7.5/p42
	.CLIP = NULL,	// +CLIP: Calling Line ID      7.6/p42
	.CLIR = NULL,	// +CLIP: Calling Line Restict 7.7/p43
	.COLP = NULL,	// +COLP: Con. Line ID Pres.   7.8/p44
	.CCUG = NULL,	// +CCUG: Closed User Group.   7.9/p45
	.CCFC = NULL,	// +CCFC: Call Fwd # & Conds   7.10/p46
	.CCWA = NULL,	// +CCWA: Call waiting         7.11/p47
	.CHLD = NULL,	// +CHLD: Call supp. services  7.12/p48
	.CTFR = NULL,	// +CTFR: Call deflection      7.13/p49
	.CUSD = NULL,	// +CUSD: Unstructure Sup Serv 7.14/p49
	.CAOC = NULL,	// +CAOC: Advice of Charge     7.15/p50
	.CSSN = NULL,	// +CSSN: Supp Serv Notifictn  7.16/p51
	.CLCC = NULL,	// +CLCC: List curr calls      7.17/p52
	.CPOL = NULL,	// +CPOL: Preferred Op. List   7.18/p53
	.COPN = NULL,	// +COPN: Read Operator Name   7.19/p54

	/* 
	 * Mobile Equip Control and Status Commands: Section 8, pps 58-76
	 * Commands for ME power, keypad, display, indicator handling, 
	 * phonebooks and setting real-time clock facilities.
	 */

	.CFUN = &sil_cfun,	// +CFUN: Set Phone Funcs      8.2/p58
	.CPIN = &sil_cpin,	// +CPIN: Enter Pin            8.3/p59
	.CBC =  NULL,	// +CBC: Battery Charge        8.4/p60
	.CSQ =  NULL,	// +CSQ: Signal Quality        8.5/p60
	.CMEC = NULL,	// +CMEC: Mob. Eq. Ctl Mode    8.6/p61
	.CKPD = NULL,	// +CKPD: Keypad control       8.7/p61
	.CDIS = NULL,	// +CDIS: Display Control      8.8/p62
	.CIND = NULL,	// +CIND: Indicator Control    8.9/p63
	.CMER = NULL,	// +CMER: Mob. Eq. Event Rpt   8.10/p64
	.CMER = NULL,	// +CMER: Mob. Eq. Event Rpt   8.10/p64
	.CPBS = NULL,	// +CPBS: Sel. pbook storage   8.11/p65
	.CPBR = NULL,	// +CPBR: Rd. pbook storage    8.12/p66
	.CPBF = NULL,	// +CPBF: Find pbook storage   8.13/p67
	.CPBW = NULL,	// +CPBW: Write pbook storage  8.14/p68
	.CCLK = NULL,	// +CCLK: Realtime Clk params  8.15/p69
	.CALA = NULL,	// +CALA: Alarm time           8.16/p69
	.CSIM = NULL,	// +CSIM: Gen. SIM access      8.17/p70
	.CRSM = NULL,	// +CSIM: Restrict. SIM access 8.18/p71
	.CSCC = NULL,	// +CSCC: Secure Control cmd   8.19/p72
	.CALM = NULL,	// +CALM: Alert Sound Mode     8.20/p73
	.CRSL = NULL,	// +CRSL: Ring Sound Level     8.21/p73
	.CVIB = NULL,	// +CVIB: Vibrator Mode        8.22/p74
	.CLVL = NULL,	// +CLVL: Loudspeaker Volume   8.23/p74
	.CMUT = NULL,	// +CMUT: Mute Control         8.24/p75
	.CACM = NULL,	// +CACM: Acc. Call Meter      8.25/p76
	.CAMM = NULL,	// +CAMM: Acc. Call Meter Max  8.26/p76
	.CPUC = NULL,	// +CPUC: Price/Unit Currency  8.27/p76

	/*
	 * Mobile Equipment Errors: Section 9, p 80.
	 */
	.CMEE = NULL,	// +CMEE: Rpt Mob. Eq error    9.1/p80

	/*
	 * TIA IS-101 Voice Control Commands: Section C.2, pps 87-93
	 */
	.FCLASS = NULL,	// +FCLASS: Select mode        C2.1/p87
	.VBT =    NULL,	// +VBT: Buffer Threshold      C2.2/p87
	.VCID =   NULL,	// +VCID: Calling ID present   C2.3/p87
	.VGR =    NULL,	// +VGR: Rx Gain selection     C2.4/p88
	.VGT =    NULL,	// +VGT: Tx Gain selection     C2.5/p88
	.VIP =    NULL,	// +VIP: Init Voice parms      C2.6/p88
	.VIT =    NULL,	// +VIT: Inactivity Timer      C2.7/p89
	.VLS =    NULL,	// +VLS: Line selection        C2.8/p89
	.VRX =    NULL,	// +VRX: Rx data state         C2.9/p90
	.VSM =    NULL,	// +VSM: Sel. compress method  C2.10/p90
	.VTS =    NULL,	// +VTS: DTMF & tone gen.      C2.11/p91
	.VTD =    NULL,	// +VTD: Tone duration         C2.12/p92
	.VTX =    NULL,	// +VTX: Tx data state         C2.13/p93
};

