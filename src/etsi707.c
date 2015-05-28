/* -*- mode: c; c-basic-offset: 8; -*-
 *  vim: expandtab:shiftwidth=4:tabstop=8:encoding=utf-8:textwidth=80 :
 */

/*
 * Copyright (C) 2007 Free Software Foundation, Inc.
 * This file is part of the libgsmc library.
 * Original Author: dwight@supercomputer.org
 *
 * gsm_interface.c: This file contains the global interface which binds
 *	all the chip-specific support modules together.
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

#include <stdio.h>
#include "etsi707.h"

/*
 * The plugin table allows you to link all of the supported GSM implementations
 * together. There's one for each GSM chip that is supported.
 */
extern struct etsi_07_07 telit;		// FIXME - move elsewhere
extern struct etsi_07_07 generic;	// FIXME - move elsewhere
extern struct etsi_07_07 sil;
struct etsi_07_07 * plugin_table[] = {
	&telit,				// The Telit GSM modem.
	&sil,
#ifdef NOTYET                           // FIXME: Add this in
	&generic,			// Basic, generic (limited!) modems.
#endif  // NOTYET
	NULL,
};

struct etsi_07_07 * pmfg = NULL;

void etsi_init(char *str_pmfg) {

	int plugin_table_index = 0;
	int i = 0;

	for (i; i < 2; i++) { // FIXME - 2 needs to be size of plugin table.
		if (!strcmp(plugin_table[i]->mgf, str_pmfg)) {
			plugin_table_index = i;
		}
	}
	if (plugin_table_index != 0) {
		pmfg = plugin_table[plugin_table_index];
	}
	else {
		pmfg = plugin_table[0];		// default
	}
}
