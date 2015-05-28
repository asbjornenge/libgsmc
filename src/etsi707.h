/*
 * Copyright (C) 2007 Free Software Foundation, Inc.
 * Original Author: dwight at supercomputer.org
 *
 * This file is part of the libgsmc library.
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
 * etsi707.h: low-level API for programming a GSM modem that
 * follows the ETSI 07.07 standard.
 *
 * Everything according to the ETSI spec follows a command/response
 * finite state machine. Thus, we base our data structures on that.
 *
 * This include file also serves as a reference into the Standards doc.
 *
 * References:
 *	A: ETSI 07.07
 */

#ifndef GSM_INTERFACE_H
#define GSM_INTERFACE_H

/*
 * The GSM Command/Response structure. This is the heart of things.
 */
struct gsm_cr {			// Command/Response structure
	char *			cmd;		// Command string
	size_t 			csz;		// Size of the command string
	char *			resp;		// Expected response
	unsigned int		nrparams; 	// # of params for resp string
	struct timeval *	tv;		// Time limit for a response
};

typedef struct gsm_cr GCR;

/*
 * etsi_07_07:	The support for the full ETSI 07 standard, 
 *	Each manufacturer is supported by creating a manufacturer (and 
 *	optionally model number) specfic data structure and filling it in
 *	with the appropriate values.
 *
 *	Every supported pluging for a GSM chip must have at least these first
 *	two entries filled out. This is how general library support for a 
 *	specific chip is recognized. 
 *
 * 	See the Ref. A for the general ETSI details, and see your
 *	GSM chip for the specifics.
 *
 *	NULL entries here mean the functionality isn't supported by your
 *	plugin.
 */

struct etsi_07_07 {			// The Full ETSI 7.07 Specification:
	char *		mgf;		// Manufacturer name
	char *		model;		// Model

	/* General Commands. A: Section 5, pps 14-19			*/
	GCR *		CGMI;		// +CGMI: Req. Mfg Id		5.1/p14
	GCR *		CGMM;		// +CGMM: Req. Model Id. 	5.2/p15
	GCR *		CGMR;		// +CGMR: Req. revision Id. 	5.3/p15
	GCR *		CGSN;		// +CGMR: Req. Serial #. 	5.4/p16
	GCR *		CSCS;		// +CSCS: Sel. TE char set	5.5/p16
	GCR *		CIMI;		// +CIMI: Req. Itl Mob Subs ID	5.6/p17
	GCR *		CMUX;		// +CMUX: Mux mode              5.7/p17
	GCR *		GTAZ;		// Generic TA: Z                5.8/p19
	GCR *		GTAF;		// Generic TA: F 
	GCR *		WS46;		// +WS46: Select Wireless Net   5.9/p19

	/* Call control commands and methods. A: Section 6, pps 21-35	*/
	GCR *		CSTA;		// +CSTA: Sel. type of address	6.1/p21
	GCR *		D;		// D: Dial command		6.2/p21
	GCR *		DPB;		// D: Dir. Dir Phone Book       6.3/p22
	GCR *		CMOD;		// +CMOD: Call Mode             6.4/p23
	GCR *		CHUP;		// +CCHUP: Call Hangup          6.5/p24
	GCR *		CBST;		// +CBST: Sel Bearer Service    6.7/p26
	GCR *		CRLP;		// +CRLP: Radio Link Protocol   6.8/p28
	GCR *		CR;		// +CR: Service Reporting Ctl   6.9/p28
	GCR *		CEER;		// +CEER: Ext. Error Report     6.10/p29
	GCR *		CRC;		// +CRC: Cell Result Codes      6.11/p30
	GCR *		CHSD;		// +CHSD: HSCSD param           6.12/p30
	GCR *		CHST;		// +CHST: HSCSD transparent cfg 6.13/p31
	GCR *		CHSN;		// +CHSN: HSCSD non-tprnt cfg   6.14/p31
	GCR *		CHSR;		// +CHSR: HSCSD param report    6.16/p33
	GCR *		CHNS;		// +CHNS: Single # scheme       6.17/p33
	GCR *		CVHU;		// +CVHU: Voice Hangup Ctl      6.18/p34
	GCR *		CV120;		// +CV120: V.120 Rate Protocol  6.19/p35
					// V.25ter Call Ctl Cmds TBD    6.20/p35
					// V.25ter Data Comp Cmds TBD   6.20/p35

	/* Network Service Related Commands. A: Section 7, pps 37-54	*/
	GCR *		CNUM;		// +CNUM: Subscriber Number    7.1/p37
	GCR *		CREG;		// +CREG: Network Registration 7.2/p38
	GCR *		COPS;		// +COPS: Operator Selection   7.3/p39
	GCR *		CLCK;		// +CLCK: Facility Lock        7.4/p40
	GCR *		CPWD;		// +CPWD: Change Password      7.5/p42
	GCR *		CLIP;		// +CLIP: Calling Line ID      7.6/p42
	GCR *		CLIR;		// +CLIP: Calling Line Restict 7.7/p43
	GCR *		COLP;		// +COLP: Con. Line ID Pres.   7.8/p44
	GCR *		CCUG;		// +CCUG: Closed User Group.   7.9/p45
	GCR *		CCFC;		// +CCFC: Call Fwd # & Conds   7.10/p46
	GCR *		CCWA;		// +CCWA: Call waiting         7.11/p47
	GCR *		CHLD;		// +CHLD: Call supp. services  7.12/p48
	GCR *		CTFR;		// +CTFR: Call deflection      7.13/p49
	GCR *		CUSD;		// +CUSD: Unstructure Sup Serv 7.14/p49
	GCR *		CAOC;		// +CAOC: Advice of Charge     7.15/p50
	GCR *		CSSN;		// +CSSN: Supp Serv Notifictn  7.16/p51
	GCR *		CLCC;		// +CLCC: List curr calls      7.17/p52
	GCR *		CPOL;		// +CPOL: Preferred Op. List   7.18/p53
	GCR *		COPN;		// +COPN: Read Operator Name   7.19/p54

	/* 
	 * Mobile Equip Control and Status Commands: Section 8, pps 58-76
	 * Commands for ME power, keypad, display, indicator handling, 
	 * phonebooks and setting real-time clock facilities.
	 */

	GCR *		CFUN;		// +CFUN: Set Phone Funcs      8.2/p58
	GCR *		CPIN;		// +CPIN: Enter Pin            8.3/p59
	GCR *		CBC;		// +CBC: Battery Charge        8.4/p60
	GCR *		CSQ;		// +CSQ: Signal Quality        8.5/p60
	GCR *		CMEC;		// +CMEC: Mob. Eq. Ctl Mode    8.6/p61
	GCR *		CKPD;		// +CKPD: Keypad control       8.7/p61
	GCR *		CDIS;		// +CDIS: Display Control      8.8/p62
	GCR *		CIND;		// +CIND: Indicator Control    8.9/p63
	GCR *		CMER;		// +CMER: Mob. Eq. Event Rpt   8.10/p64
	GCR *		CPBS;		// +CPBS: Sel. pbook storage   8.11/p65
	GCR *		CPBR;		// +CPBR: Rd. pbook storage    8.12/p66
	GCR *		CPBF;		// +CPBF: Find pbook storage   8.13/p67
	GCR *		CPBW;		// +CPBW: Write pbook storage  8.14/p68
	GCR *		CCLK;		// +CCLK: Realtime Clk params  8.15/p69
	GCR *		CALA;		// +CALA: Alarm time           8.16/p69
	GCR *		CSIM;		// +CSIM: Gen. SIM access      8.17/p70
	GCR *		CRSM;		// +CSIM: Restrict. SIM access 8.18/p71
	GCR *		CSCC;		// +CSCC: Secure Control cmd   8.19/p72
	GCR *		CALM;		// +CALM: Alert Sound Mode     8.20/p73
	GCR *		CRSL;		// +CRSL: Ring Sound Level     8.21/p73
	GCR *		CVIB;		// +CVIB: Vibrator Mode        8.22/p74
	GCR *		CLVL;		// +CLVL: Loudspeaker Volume   8.23/p74
	GCR *		CMUT;		// +CMUT: Mute Control         8.24/p75
	GCR *		CACM;		// +CACM: Acc. Call Meter      8.25/p76
	GCR *		CAMM;		// +CAMM: Acc. Call Meter Max  8.26/p76
	GCR *		CPUC;		// +CPUC: Price/Unit Currency  8.27/p76

	/*
	 * Mobile Equipment Errors: Section 9, p 80.
	 */
	GCR *		CMEE;		// +CMEE: Rpt Mob. Eq error    9.1/p80

	/*
	 * TIA IS-101 Voice Control Commands: Section C.2, pps 87-93
	 */
	GCR *		FCLASS;		// +FCLASS: Select mode        C2.1/p87
	GCR *		VBT;		// +VBT: Buffer Threshold      C2.2/p87
	GCR *		VCID;		// +VCID: Calling ID present   C2.3/p87
	GCR *		VGR;		// +VGR: Rx Gain selection     C2.4/p88
	GCR *		VGT;		// +VGT: Tx Gain selection     C2.5/p88
	GCR *		VIP;		// +VIP: Init Voice parms      C2.6/p88
	GCR *		VIT;		// +VIT: Inactivity Timer      C2.7/p89
	GCR *		VLS;		// +VLS: Line selection        C2.8/p89
	GCR *		VRX;		// +VRX: Rx data state         C2.9/p90
	GCR *		VSM;		// +VSM: Sel. compress method  C2.10/p90
	GCR *		VTS;		// +VTS: DTMF & tone gen.      C2.11/p91
	GCR *		VTD;		// +VTD: Tone duration         C2.12/p92
	GCR *		VTX;		// +VTX: Tx data state         C2.13/p93
};

/* The plugin table for all supported GSM implementations.		*/
extern struct etsi_07_07 *	plugin_table[];
extern struct etsi_07_07 *	pmfg; 		// Current manufacturer

/* 
 * Standard sizes of common additions to strings:
 * These are required, because we explicitly limit the size of all string
 * operations.
 */
#define SZ_AT1          4               // AT plus 1 digit: "at+...[0-9]"

#endif // GSM_INTERFACE_H
