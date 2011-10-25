/*
Reflect
Copyright (c) 2000-2008, Jay C Barker
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
/**********************************************************************
 *
 * File:
 *   Beam.c
 *
 * Description:
 *	 Beaming functions.  
 *
 * History:
 *   2002 Mar 16     jcbarker     Moved beaming function from main
 *                                 source file.
 *
 **********************************************************************/

#include <PalmOS.h>

#include "Beam.h"


/**********************************************************************
 * Internal Functions
 **********************************************************************/

static Err SendDatabase (UInt16 cardNo, LocalID dbID, Char* nameP,
						 Char* descriptionP);
static Err WriteDBData (const void* dataP, UInt32* sizeP,
						void* userDataP);


/**********************************************************************
 *
 * Function:    BeamThis
 *
 * Description: Sends the application to another device via beaming.
 *
 * Parameters:  appBeamFile     - Name of *.prc file
 *              appInternalName - Name of application
 *
 * Returned:    Error code or zero for no error.
 *
 * History:
 *   2000 Sep 19     jcbarker     Adapted from:
 *                                 http://www.palmos.com/dev/support/docs/
 *                                  palmos/ExchangeManagerConcept.html#996714
 *   2002 Mar 08     jcbarker     Added parameters to allow source
 *                                file move.
 *
 **********************************************************************/

extern Err BeamThis (Char* appBeamFile, Char* appInternalName)
{
	Err error;

	/* Find our app using its internal name */
	LocalID dbID = DmFindDatabase(0, appInternalName);

	if (dbID)
	{
		/* send it giving internal name */
		error = SendDatabase(0, dbID, appBeamFile, appInternalName);
	}
	else
	{
		error = DmGetLastErr();
	}

	return error;
}


/**********************************************************************
 *
 * Function:    SendDatabase
 *
 * Description: Sends data in the input field using the Exg API
 *
 * Parameters:  cardNo      - card number of db to send (usually 0)
 *              dbID        - databaseId of database to send
 *              nameP       - public filename for this database
 *                            This is the name as it appears on a
 *                            PC file listing. It should end with a
 *                            .prc or .pdb extension.
 *              description - Optional text description of db to show
 *                            to user who receives the database.
 *
 * Returned:    Error code or zero for no error.
 *
 * History:
 *   2000 Sep 19     jcbarker     Adapted from:
 *                                 http://www.palmos.com/dev/support/docs/
 *                                  palmos/ExchangeManagerConcept.html#996714
 *
 **********************************************************************/

static Err SendDatabase (UInt16 cardNo, LocalID dbID, Char* nameP,
						 Char* descriptionP)
{
	ExgSocketType exgSocket;
	Err           err;

	/* Create exgSocket structure */
	MemSet(&exgSocket, sizeof(exgSocket), 0);
	exgSocket.description = descriptionP;
	exgSocket.name = nameP;

	/* Start and exchange put operation */
	err = ExgPut(&exgSocket);
	if (!err)
	{
		/* This function converts a PalmOS database into its external
		   (public) format. The first parameter is a callback that
		   will be passed parts of the database to send or write. */
		err = ExgDBWrite(WriteDBData, &exgSocket, NULL, dbID, cardNo);

		/* Disconnect Exg and pass error */
		err = ExgDisconnect(&exgSocket, err);
	}
	return err;
}


/**********************************************************************
 *
 * Function:    WriteDBData
 *
 * Description: Callback for ExgDBWrite to send data with exchange
 *              manager.
 *
 * Parameters:  dataP     - buffer containing data to send
 *              sizeP     - number of bytes to send
 *              userDataP - app defined buffer for context
 *                          (holds exgSocket when using ExgManager)
 *
 * Returned:    Error if non-zero.
 *
 * History:
 *   2000 Sep 19     jcbarker     Adapted from:
 *                                 http://www.palmos.com/dev/support/docs/
 *                                  palmos/ExchangeManagerConcept.html#996714
 *
 **********************************************************************/

static Err WriteDBData (const void* dataP, UInt32* sizeP, void* userDataP)
{
	Err err;

	/* Try to send as many bytes as were requested by the caller */
	*sizeP = ExgSend((ExgSocketPtr)userDataP, (void*)dataP, *sizeP, &err);

	return err;
}
