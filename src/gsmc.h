/* gsmc.h
 * 
 * Gsm communications library
 * 
 * Asbjorn Andersen <mind@hanafjedle.net>
 * 
 * Based upon: 
 * gomunicator by Robert Woerle <robert@linuxdevelopment.org>
 * picocom by Nick Patavalis <npat@efault.net>
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
 
/* File: gsmc.h */
 
#ifndef PHONECOMMS_H
#define PHONECOMMS_H
#define BUFFER_SIZE 1024

/*********************************************************
 *  Struct: phoneOptions
 * 
 * The phoneOptions structure is the heart of the libgsmc library. 
 * It is used to pass information from the library to the application and
 * holds all the useful phone-variables. Applications using this library
 * should implement this struct.
 * 
 * Variables:
 * 
 * 	cops - (int) Defines the number of available operators
 * 	number - (int) Set the numer from wich a call or sms is recieved
 * 	fun - (int) The level functionality of the phone ( 1 = active, 0 = inactive )
 * 	clcc - (int) List of current calls
 * 	sms - (int) Number of availabe sms's
 * 	carrier - (char [20]) Defines the current carrier used by the modem
 * 	prefix - (char [2]) The modem's prefix command (if any)
 * 	postfix - (char [2]) The modem's postfix command (if any)
 * 	has_prefix - (bool) Does the modem needs a prefix ?
 * 	has_postfix - (bool) Does the modem needs a postfix ?
 * 	state - (int) Defines the state of the phone
 * 				   -1 = idle, 
 * 					0 = online, 
 * 					1 = ringing
 * security - (int) Defines the security of the phone
 * 				   -1 = need PIN, 
 * 					0 = unknown, 
 * 					1 = ready
 *
 */

typedef struct 
{
	int		cops;
	int 	number;
	int		fun;
	int 	clcc;
	int		sms;
	char carrier[20];
	char prefix[2], postfix[2];
	bool has_prefix, has_postfix;
	int state;
	int security;
}
phoneOptions;

/* Section: Functions */

/*********************************************************
 * Function: openPhone 
 * 
 * Initializes the phone variables and opens a connection
 * to the modem specified by devName.
 * 
 * Parameters:
 * 
 * 	devName - Specifies the device to open a connection to.
 * 
 * Returns: 
 * 
 * 	True if connection open, false otherwise.
 */
bool openPhone(char *devName, char *pmfg);

/*********************************************************
 * Function: checkPIN
 * 
 * Updates the phone.security variable.
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns:
 * 
 * 	True if update successful, false otherwise.
 */
bool checkPIN(void);

/********************************************************
 * Function: setPrefix
 * 
 * If you're modem requires a prefix,
 * use this function to set it.
 * 
 * Parameters:
 * 
 * 	fix - The modems prefix
 * 
 * Returns: 
 * 
 * 	none
 */
void setPrefix(char fix[2]);

/********************************************************
 * Function: setPostfix
 * 
 * If you're modem requires a postfix,
 * use this function to set it.
 * 
 * Parameters:
 * 
 * 	fix - The modems postfix
 * 
 * Returns: 
 * 
 * 	none
 */

void setPostfix(char fix[2]);

/***************************************************************************
 * Function: setPIN
 * 
 * Tries to set the pin code according to PINString.
 * 
 * Parameters:
 * 
 * 	PINString - The PIN number in the form of a char*
 * 
 * Returns: True if phoneGetOK from sending the command,  and 
 * updatePhoneSecurity() is true. False otherwise.
 */
bool setPIN(int PIN);

/***************************************************************************
 * Function: activatePhone
 * 
 * Sets the phones mobile station level functionality to
 * 1 (=active).
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	True if OK, false otherwise
 */
bool activatePhone(void);

/***************************************************************************
 * Function: setDefaultOperator
 * 
 * Set default / home-network operator.
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	True if OK,
 * 	False otherwise.
 */
bool setDefaultOperator(void);

/***************************************************************************
 * Function: setOperator
 * 
 * Set operator (PLMN) according to its number.
 * (0 = home network / default)
 * 
 * Parameters:
 * 
 * 	cops - The value of the selected operator.
 * 
 * Returns: 
 * 
 * 	True if OK,
 * 	False otherwise.
 */
bool setOperator(int cops);


/***************************************************************************
 * Function: getFUN
 * 
 * Ask phones mobile station level funtionality. 
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	The value of phone.fun. Valid options are 1 or 0:
 * 	1 - Phone is in full functionality
 * 	0 - Phone is turned off
 */
int getFUN(void);

/***************************************************************************
 * Function: getOperators
 * 
 * Ask number of operators (PLMN).
 * Asking this also sets the phone.carrier to the current carrier.
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	The value of phone.cops. This value represents the number of
 * 	available operators.
 */
int getOperators(void);

/***************************************************************************
 * Function: getModemFD
 * 
 * Returns the modems filedescriptor.
 * Useful if you want to set up external listeners.
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	Modems filedescriptor
 */
int getModemFD(void);

/***************************************************************************
 * Functions: listenToModem
 * 
 * Sets up a listener to see if there is data to read from the modems 
 * filedescriptor. If so, the read gets processed.
 * (If you're building an application around this library, this function
 * would be a good idea to call in youre main loop).
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	none
 */
void listenToModem(void);

/***************************************************************************
 * Functions: tryReadPhone
 * 
 * Try to read the output from the modem. If there is any it is
 * processed.
 * 
 * Parameters:
 * 
 * 	timeout - Timeout period before giving up
 * 
 * Returns: 
 * 
 * 	none
 */
void tryReadPhone(int timeout);

/***************************************************************************
 * Function phoneGetOK
 * 
 * Sends a desired command, and checks if the modem
 * replys OK.
 * phoneGetOK's read timeouts after 3 seconds.
 * 
 * Parameters:
 * 
 * 	query - The desired command to send to the modem
 * 
 * Returns:
 * 
 * 	True if modem replys OK, 
 * 	false otherwise.
 */
bool phoneGetOK(const char *query);

/***************************************************************************
 * Function phoneGetOK_slow
 * 
 * Sends a desired command, and checks if the modem
 * replys OK.
 * phoneGetOK's read timeouts after 10 seconds.
 * 
 * Parameters:
 * 
 * 	query - The desired command to send to the modem
 * 
 * Returns:
 * 
 * 	True if modem replys OK, 
 * 	false otherwise.
 */
bool phoneGetOK_slow(const char *query);

/***************************************************************************
 * Function: answerPhone
 * 
 * Tries to answer a incoming call.
 * If successful the phone.state is set to 1 (ringing).
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	True if successful, false otherwise.
 */
bool answerPhone(void);

/***************************************************************************
 * Function: hangupPhone
 * 
 * Hangs up. Sets phone.state to -1 (Idle), phone.numer to 0, and sends the
 * ATH0 (hangup) command to the modem.
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	none
 */
void hangupPhone(void);

/***************************************************************************
 * Function: makeCall
 * 
 * Make a call to the selected number spesified by dialString.
 * If successful phone.state is set to 0 (online).
 * 
 * Parameters:
 * 
 * 	dialstring - The selected number to call
 * 
 * Returns: 
 * 
 * 	True if success, false otherwise.
 */
bool makeCall(char *dialString);

/***************************************************************************
 * Function: printPhoneStatus
 * 
 * Prints the current status of the phone with all its variables.
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	none
 */
void printPhoneStatus(void);

/***************************************************************************
 * Function: getPhoneStatus
 * 
 * This is an important function in this library.
 * It is used to pass all the variables of the current phoneOptions structure
 * handeled by the modem. So in that sense it is used to pass information
 * from the library to an application.
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	phoneConfig of current phoneOptions.
 */
phoneOptions getPhoneStatus(void);

/***************************************************************************
 * Function: closePhone
 * 
 * Close the file descriptors and reset the phone variables.
 * 
 * Parameters:
 * 
 * 	none
 * 
 * Returns: 
 * 
 * 	none
 */
void closePhone(void);

#endif // ifndef PHONECOMMS_H

