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
 *   App.c
 *
 * Description:
 *	 Main module for application.  This module starts the
 *   application, dispatches events, and stops the application.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial public release.
 *   2002 Mar 16     jcbarker     Moved beaming functions to their
 *                                 own file. Other small function-level
 *                                 and global changes.
 *   2002 Aug 23     jcbarker     Added SysUtils and TimeMgr includes
 *                                 for randomizing.
 *   2008 Jul 05     jcbarker     Minor changes for v1.2.
 *
 **********************************************************************/

#include <PalmOS.h>
#include <SysUtils.h>     /* For SysRandom */
#include <TimeMgr.h>      /* For TimGetTicks */

#include "AppRsc.h"
#include "Beam.h"


/**********************************************************************
 * Internal Constants
 **********************************************************************/

#define appInternalName    "Reflect"
#define appFileCreator     'REFL'

#define appPrefID          0
#define appPrefVersion     1

#define appBeamFile        "Reflect.prc"

/* this application tested on Palm OS 3.0 and higher */
#define version30          0x03003000

/* some optional functionality requires PalmOS 3.5 or higher */
#define version35          0x03503000

#define drawMenuHeight     15 

/* Preferences UI settings */
#define drawSpeedSelectable			3
#define defaultDrawSpeedItem		1   /* Medium */

#define drawStyleSelectable			8
#define defaultDrawStyleItem		0   /* Random */

#define drawSizeSelectable			4
#define defaultDrawSizeItem			1   /* Large */


/**********************************************************************
 * Internal Functions
 **********************************************************************/

UInt32 PilotMain (UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags);
static UInt16 StartApplication (void);
static UInt16 SetDefaultPreferences (void);
static void StopApplication (void);
static Boolean MainFormDoCommand (UInt16 command);
static Boolean MainFormHandleEvent (EventPtr event);

static Boolean PrefsFormHandleEvent (EventPtr event);
static void PrefsFormInit (FormPtr formP);
static UInt16 MapToPosition (UInt8 *mappingArray, UInt8 value,
							 UInt16 mappings, UInt16 defaultItem);

static void AppEventLoop (void);
static Boolean AppHandleEvent( EventPtr eventP);

static UInt16 DrawMainForm (void);
static Err RomVersionCompatible (UInt32 requiredVersion,
								 UInt16 launchFlags);

/**********************************************************************
 * Internal Structures
 **********************************************************************/

typedef struct
{
	UInt8                drawSpeed;
	UInt8                drawStyle;
	UInt8                drawSize;
	Boolean              bShowSplashScreen;
	Boolean              bSaveAcrossSession;
} thisAppPreferenceType;


/**********************************************************************
 * Global variables
 **********************************************************************/

/* The following are saved to a state file. */

thisAppPreferenceType     Prefs;

/* Used by the preferences form. */

UInt8					newDrawSpeed;
UInt8					newDrawStyle;
UInt8					newDrawSize;
Boolean					bNewShowSplashScreen;
Boolean					bNewSaveAcrossSession;

/* Mappings for preferences */

UInt8 DrawSpeedMappings[drawSpeedSelectable] =
{
	0,		/* Fast */
	1,		/* Medium */
	2		/* Slow */
};

UInt8 DrawStyleMappings[drawStyleSelectable] =
{
	0,		/* Random */
	1,		/* Top to Bottom */
	2,		/* Bottom to Top */
	3,		/* Left to Right */
	4,		/* Right to Left */
	5,		/* Inside to Outside */
	6,		/* Outside to Inside */
	7,		/* None */

};

UInt8 DrawSizeMappings[drawSizeSelectable] =
{
	0,		/* Maximum */
	2,		/* Large */
	15,		/* Medium */
	30,		/* Small */
};


/**********************************************************************
 *
 * Function:    PilotMain
 *
 * Description: Main entry point for this application. 
 *
 * Parameters:  None.
 *
 * Returned:    0
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *
 **********************************************************************/
 
UInt32 PilotMain (UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	UInt16 error;

	/* Error if not PalmOS 3.0 or greater */
	error = RomVersionCompatible (version30, launchFlags);
	if (error) return (error);

	/* Normal Launch */
	if ( cmd == sysAppLaunchCmdNormalLaunch )
	{
		error = StartApplication ();
		if (error) return (error);

		AppEventLoop ();
		StopApplication ();
	}
	else
	{
		return sysErrParamErr;
	}

	return (0);
}


/**********************************************************************
 *
 * Function:    StartApplication
 *
 * Description: Initialize application.
 *
 * Parameters:  None.
 *
 * Returned:    0
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *   2002 Mar 16     jcbarker     Moved SetDefaultPreferences to its
 *                                 own function.
 *   2002 Aug 23     jcbarker     Added optional splash screen.
 *
 **********************************************************************/

static UInt16 StartApplication (void)
{
	UInt16 prefsSize;
	FormPtr frm;

	/* Restore preferences. */
	prefsSize = sizeof (thisAppPreferenceType);
	if (PrefGetAppPreferences (appFileCreator, appPrefID, &Prefs,
		&prefsSize, true) == noPreferenceFound)
	{
		/* There aren't any preferences */
		SetDefaultPreferences();
	}

	if(true == Prefs.bShowSplashScreen)
	{
		frm = FrmInitForm(AboutForm);	/* load the form */
		FrmDoDialog(frm);				/* display until any button tapped */
		FrmDeleteForm(frm);				/* delete from memory, erase from display */
	}

	FrmGotoForm (MainForm);

	return (0);
}


/**********************************************************************
 *
 * Function:    SetDefaultPreferences
 *
 * Description: Write default preferences to memory.
 *
 * Parameters:  None.
 *
 * Returned:    0
 *
 * History:
 *   2002 Mar 16     jcbarker     Taken from StartApplication.
 *   2002 Aug 23     jcbarker     Added two new preferences.
 *   2008 Jul 05     jcbarker     Default 'false' for show splash screen.
 *
 **********************************************************************/

static UInt16 SetDefaultPreferences (void)
{
	Prefs.drawSpeed          = DrawSpeedMappings[defaultDrawSpeedItem];
	Prefs.drawStyle          = DrawStyleMappings[defaultDrawStyleItem];
	Prefs.drawSize           = DrawSizeMappings[defaultDrawSizeItem];
	Prefs.bShowSplashScreen  = false;
	Prefs.bSaveAcrossSession = true;

	return (0);
}


/**********************************************************************
 *
 * Function:	StopApplication
 *
 * Description:	Save the current state of the application and close
 *              all forms.
 *
 * Parameters:	None.
 *
 * Returned:	Nothing.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *   2002 Aug 23     jcbarker     Added optional preferences save.
 *
 **********************************************************************/

static void StopApplication (void)
{
	/* Save state/preferences. */
	if (false == Prefs.bSaveAcrossSession)
	{
		/* Load default preferences before stopping */
		SetDefaultPreferences();
	}

	PrefSetAppPreferences (appFileCreator, appPrefID, appPrefVersion, 
		&Prefs, sizeof (Prefs), true);	

	FrmCloseAllForms ();
}


/**********************************************************************
 *
 * Function:   MainFormDoCommand
 *
 * Desciption: Performs the menu command specified.
 *
 * Parameters: command - menu item id
 *
 * Returned:   True if the command was handled.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *   2002 Mar 16     jcbarker     Now MainOptionsRefresh case calls
 *                                 consolidated DrawMainForm function.
 *   2002 Aug 23     jcbarker     Removed Exit command.
 *
 **********************************************************************/

static Boolean MainFormDoCommand (UInt16 command)
{
	Boolean handled = false;
	FormPtr frm;

	MenuEraseStatus (0);

	switch (command)
	{
		case MainOptionsBeam:
			/* Send this application, via IR beam. */
			BeamThis (appBeamFile, appInternalName); 
			FrmGotoForm (MainForm);
			handled = true;
			break;

		case MainOptionsRefresh:
			FrmGotoForm (MainForm);
			handled = true;
			break;

		case MainOptionsPrefs:
			FrmPopupForm (PrefsForm);
			handled = true;
			break;

		case MainOptionsTips:
			FrmHelp (TipsStr);
			FrmGotoForm (MainForm);
			handled = true;
			break;
		
		case MainOptionsAbout:
			frm = FrmInitForm(AboutForm);	/* load the form */
			FrmDoDialog(frm);				/* display until any button tapped */
			FrmDeleteForm(frm);				/* delete from memory, erase from display */
			FrmGotoForm (MainForm);
			handled = true;
			break;
	}
		
	return handled;
}


/**********************************************************************
 *
 * Function:    MainFormHandleEvent
 *
 * Description: Event handler for the main form.
 *
 * Parameters:  event - a pointer to an EventType structure
 *
 * Returned:    True if the event has been handled and should not be
 *              passed to a higher level handler.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *   2002 Mar 16     jcbarker     Now frmOpenEvent calls consolidated
 *                                 DrawMainForm function.
 *   2002 Aug 23     jcbarker     Added actions on Page Up, Page Down,
 *                                 and penDownEvent.
 *
 **********************************************************************/

static Boolean MainFormHandleEvent (EventPtr event)
{
	Boolean handled = false;
	UInt32 romVersion;

	if (event->eType == menuEvent)
	{
		return MainFormDoCommand (event->data.menu.itemID);
	}
		
	else if (event->eType == frmOpenEvent)
	{
		/* TODO: Check that form is not already open */
		
		DrawMainForm ();
		handled = true;
	}

	else if (event->eType == menuCmdBarOpenEvent)
	{
		/* MenuCmdBarAddButton in PalmOS 3.5 and greater */
		FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
		if (romVersion >= version35)
		{
			MenuCmdBarAddButton (menuCmdBarOnLeft, MyBarInfoBitmap,
				menuCmdBarResultMenuItem, MainOptionsTips, NULL);
			MenuCmdBarAddButton (menuCmdBarOnLeft, MyBarPrefsBitmap,
				menuCmdBarResultMenuItem, MainOptionsPrefs, NULL);
			MenuCmdBarAddButton (menuCmdBarOnLeft, MyBarRefreshBitmap,
				menuCmdBarResultMenuItem, MainOptionsRefresh, NULL);
			MenuCmdBarAddButton (menuCmdBarOnLeft, MyBarBeamBitmap,
				menuCmdBarResultMenuItem, MainOptionsBeam, NULL);
		}
		/* else do nothing */
	}
	else if (event->eType == keyDownEvent)
	{
		switch (event->data.keyDown.chr)
		{
			/* Preferences on PageUp key press */
			case vchrPageUp:
				MainFormDoCommand(MainOptionsPrefs); 
				break;
			/* Refresh on PageDown key press */
			case vchrPageDown:
				MainFormDoCommand(MainOptionsRefresh); 
				break;
			/* vchrSendData in PalmOS 3.5 and greater. Should also 
			   work with sendDataChr in PalmOS 3.0 - 3.3 */
			case vchrSendData:
				/* Occurs only when set in preferences.
				   Send this application, via IR beam. */
				BeamThis (appBeamFile, appInternalName); 
				break;
		}
	}
	else if (event->eType == penDownEvent)
	{
		/* Refresh on a screen tap (that is not on the form menu
		   area at the top of the screen). */
		if (event->screenY >= drawMenuHeight)
		{
			MainFormDoCommand(MainOptionsRefresh);
		}
	}

	return (handled);
}


/**********************************************************************
 *
 * Function:    PrefsFormHandleEvent
 *
 * Description: Event handler for the preferences form.
 *
 * Parameters:  event - a pointer to an EventType structure
 *
 * Returned:    True if the event has handle and should not be passed
 *              to a higher level handler.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *   2002 Mar 16     jcbarker     Now PrefsFormOkButton will refresh
 *                                 the display.
 *   2002 Aug 23     jcbarker     Added new preferences and Defaults
 *                                 button.
 *
 **********************************************************************/

static Boolean PrefsFormHandleEvent (EventPtr event)
{
	FormPtr formP;
	ListPtr listP;
	Boolean handled = false;

	formP = FrmGetFormPtr (PrefsForm);	

	if (event->eType == popSelectEvent)
	{
		switch (event->data.popSelect.listID)
		{
			case PrefsDrawSpeedsList:
				newDrawSpeed = DrawSpeedMappings[event->data.popSelect.selection];

				listP = FrmGetObjectPtr (formP, FrmGetObjectIndex (formP, PrefsDrawSpeedsList));

				CtlSetLabel(
					FrmGetObjectPtr (formP,
						FrmGetObjectIndex (formP, PrefsDrawSpeedsPopupTrigger) ),
							LstGetSelectionText (listP, event->data.popSelect.selection) );

				handled = true;
				break;

			case PrefsDrawStylesList:
				newDrawStyle = DrawStyleMappings[event->data.popSelect.selection];

				listP = FrmGetObjectPtr (formP, FrmGetObjectIndex (formP, PrefsDrawStylesList));

				CtlSetLabel(
					FrmGetObjectPtr (formP,
						FrmGetObjectIndex (formP, PrefsDrawStylesPopupTrigger) ),
							LstGetSelectionText (listP, event->data.popSelect.selection) );


				handled = true;
				break;

			case PrefsDrawSizesList:
				newDrawSize = DrawSizeMappings[event->data.popSelect.selection];

				listP = FrmGetObjectPtr (formP, FrmGetObjectIndex (formP, PrefsDrawSizesList));

				CtlSetLabel(
					FrmGetObjectPtr (formP,
						FrmGetObjectIndex (formP, PrefsDrawSizesPopupTrigger) ),
							LstGetSelectionText (listP, event->data.popSelect.selection) );


				handled = true;
				break;
		}
	}

	else if (event->eType == ctlSelectEvent)
	{
		switch (event->data.ctlSelect.controlID)
		{
			case PrefsShowSplashScreenChk:
				if (true == bNewShowSplashScreen)
				{
					bNewShowSplashScreen = false;
				}
				else
				{
					bNewShowSplashScreen = true;
				}
				handled = true;
				break;

			case PrefsSaveAcrossSessionChk:
				if (true == bNewSaveAcrossSession)
				{
					bNewSaveAcrossSession = false;
				}
				else
				{
					bNewSaveAcrossSession = true;
				}
				handled = true;
				break;
		
			case PrefsFormOkButton:
				/* Save preferences */
				Prefs.drawSpeed          = newDrawSpeed;
				Prefs.drawStyle          = newDrawStyle;
				Prefs.drawSize           = newDrawSize;
				Prefs.bSaveAcrossSession = bNewSaveAcrossSession;
				Prefs.bShowSplashScreen  = bNewShowSplashScreen;

				FrmReturnToForm (0);
				DrawMainForm();            /* Auto-refresh */

				handled = true;
				break;

			case PrefsFormCancelButton:
				FrmReturnToForm (0);
				DrawMainForm();            /* Auto-refresh */

				handled = true;
				break;

			case PrefsFormDefaultsButton:
				if(0 == FrmAlert(PrefsDefaultsWarning))
				{
					/* If 0, the first button 'OK' was selected.
					   So set defaults. */
					SetDefaultPreferences();

					/* Refresh the preferences form. */
					PrefsFormInit (formP);
					FrmDrawForm (formP);
				}
				handled = true;
				break;
		}
	}

	if (event->eType == frmOpenEvent)
	{
		PrefsFormInit (formP);
		FrmDrawForm (formP);

		handled = true;
	}
		
	return (handled);
}


/***********************************************************************
 *
 * Function:    PrefsFormInit
 *
 * Description: Initialize the form.  Set preferences.
 *
 * Parameters:  formP - pointer to the preferences form
 *
 * Returned:    Nothing.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *   2002 Mar 16     jcbarker     Now newDraw* variables are used to
 *                                 MapToPosition.
 *   2002 Aug 23     jcbarker     Added new preferences.
 *
 ***********************************************************************/

static void PrefsFormInit (FormPtr formP)
{
	ListPtr listP;
	UInt16 mappedValue;
	
	/* Retrieve saved preferences */
	newDrawSpeed          = Prefs.drawSpeed;
	newDrawStyle          = Prefs.drawStyle;
	newDrawSize           = Prefs.drawSize;
	bNewSaveAcrossSession = Prefs.bSaveAcrossSession;
	bNewShowSplashScreen  = Prefs.bShowSplashScreen;
	
	/* Select the saved preferences values */
	/* DrawSpeed */
	listP = FrmGetObjectPtr (formP, FrmGetObjectIndex (formP, PrefsDrawSpeedsList));
	mappedValue = MapToPosition ((UInt8 *) DrawSpeedMappings,
										  newDrawSpeed,
										  drawSpeedSelectable,
										  defaultDrawSpeedItem);
	LstSetSelection(listP, mappedValue);
	CtlSetLabel(
		FrmGetObjectPtr (formP,
			FrmGetObjectIndex (formP, PrefsDrawSpeedsPopupTrigger) ),
				LstGetSelectionText (listP, mappedValue) );

	/* DrawStyle */
	listP = FrmGetObjectPtr (formP, FrmGetObjectIndex (formP, PrefsDrawStylesList));
	mappedValue = MapToPosition ((UInt8 *) DrawStyleMappings,
										  newDrawStyle,
										  drawStyleSelectable,
										  defaultDrawStyleItem);
	LstSetSelection(listP, mappedValue);
	CtlSetLabel(
		FrmGetObjectPtr (formP,
			FrmGetObjectIndex (formP, PrefsDrawStylesPopupTrigger) ),
				LstGetSelectionText (listP, mappedValue) );

	/* DrawSize */
	listP = FrmGetObjectPtr (formP, FrmGetObjectIndex (formP, PrefsDrawSizesList));
	mappedValue = MapToPosition ((UInt8 *) DrawSizeMappings,
										  newDrawSize,
										  drawSizeSelectable,
										  defaultDrawSizeItem);
	LstSetSelection(listP, mappedValue);
	CtlSetLabel(
		FrmGetObjectPtr (formP,
			FrmGetObjectIndex (formP, PrefsDrawSizesPopupTrigger) ),
				LstGetSelectionText (listP, mappedValue) );

	/* ShowSplashScreen */
	CtlSetValue(
		FrmGetObjectPtr (formP,
			FrmGetObjectIndex (formP, PrefsShowSplashScreenChk) ),
			(Int16)bNewShowSplashScreen);

	/* SaveAcrossSession */
	CtlSetValue(
		FrmGetObjectPtr (formP,
			FrmGetObjectIndex (formP, PrefsSaveAcrossSessionChk) ),
			(Int16)bNewSaveAcrossSession);
}


/**********************************************************************
 *
 * Function:    MapToPosition
 *
 * Description:	Map a value to it's position in an array.  If the
 *              passed value is not found in the mappings array,
 *              a default mappings item will be returned.
 *
 * Parameters:  mappingArray - array to search for value
 *              value        - value to look for
 *              mappings     - number of items in array
 *              defaultItem  - item to use if value not found
 *
 * Returned:    Position value found in, or default.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *
 **********************************************************************/

static UInt16 MapToPosition (UInt8 *mappingArray, UInt8 value,
							 UInt16 mappings, UInt16 defaultItem)
{
	UInt16 i;
	
	i = 0;
	while (mappingArray[i] != value && i < mappings)
	{
		i++;
	}

	if (i >= mappings)
	{
		return defaultItem;
	}
	else
	{
		return i;
	}

}


/**********************************************************************
 *
 * Function:    AppEventLoop
 *
 * Description: Event loop for the application.  
 *
 * Parameters:  None.
 *
 * Returned:    Nothing.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *
 **********************************************************************/

static void AppEventLoop(void)
{
	UInt16 error;
	EventType event;

	do
	{
		EvtGetEvent(&event, evtWaitForever);
		
		if (! SysHandleEvent(&event))
			if (! MenuHandleEvent(0, &event, &error))
				if (! AppHandleEvent(&event))
					FrmDispatchEvent(&event);
	}
	while (event.eType != appStopEvent);
}


/**********************************************************************
 *
 * Function:    AppHandleEvent
 *
 * Description: Loads form resources and sets the event handler for
 *              the form loaded.
 *
 * Parameters:  event - a pointer to an EventType structure
 *
 * Returned:    True if the event has handle and should not be passed
 *              to a higher level handler.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *
 **********************************************************************/

static Boolean AppHandleEvent (EventPtr eventP)
{
	UInt16 formId;
	FormPtr frmP;

	if (eventP->eType == frmLoadEvent)
	{
		/* Load the form resource. */
		formId = eventP->data.frmLoad.formID;
		frmP = FrmInitForm(formId);
		FrmSetActiveForm(frmP);

		/* Set the event handler for the form.  The handler of the
		   currently active form is called by FrmHandleEvent each
		   time is receives an event. */
		switch (formId)
		{
			case MainForm:
				FrmSetEventHandler(frmP, MainFormHandleEvent );
				break;

			case PrefsForm:
				FrmSetEventHandler(frmP, PrefsFormHandleEvent );
				break;

			default:
				ErrNonFatalDisplay("Invalid Form Load Event");
				break;
		}
		return true;
	}

	return false;
}


/**********************************************************************
 *
 * Function:    DrawMainForm
 *
 * Description: Draw custom stuff in the active form.
 *
 * Parameters:  None.
 *
 * Returned:    Nothing.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial implementation.
 *   2002 Mar 16     jcbarker     Modified "Inside to Outside" case to
 *                                 draw a rectangle. Added "None" and
 *                                 "Outside to Inside" cases.
 *   2002 Aug 23     jcbarker     Added "Random" draw style as default.
 *
 **********************************************************************/

static UInt16 DrawMainForm (void)
{
	FormPtr frm;
	WinHandle myWindow;
	
	Coord windowSizeX;
	Coord windowSizeY;
	Coord xCount;
	Coord yCount;

	RectangleType rectangle;
	UInt16        radius = 0;

	Int16         iRandom = 0;
	UInt8         iCurrentDrawStyle = 0;

	frm = FrmGetActiveForm();
	FrmEraseForm (frm);
	FrmDrawForm (frm);				/* needs other function? */

	myWindow = FrmGetWindowHandle (frm);
	WinGetWindowExtent (&windowSizeX, &windowSizeY);


	/* If 'Random' is selected, pick a random drawStyle.
	   Else, the current drawStyle is the saved drawStyle. */
	if (0 == Prefs.drawStyle)
	{
		iRandom = (SysRandom(TimGetTicks()));

		/* Choose from all styles except None. */
		iCurrentDrawStyle = 1 + (UInt8)iRandom % 6;
	}
	else
	{
		iCurrentDrawStyle = Prefs.drawStyle;
	}


	/* Draw! */
	switch (iCurrentDrawStyle)
	{
	
		case 1:
			/* Top to Bottom */
			for ( yCount = (drawMenuHeight + Prefs.drawSize);
				yCount < (windowSizeY - Prefs.drawSize);
				yCount++ )
			{
				WinDrawLine (Prefs.drawSize, yCount,
					windowSizeX - Prefs.drawSize - 1, yCount);

				if (0 < Prefs.drawSpeed)
				{
					/* 10ms times draw speed */ 
					SysTaskDelay ( SysTicksPerSecond() / 100 * Prefs.drawSpeed );
				}
			}
			break;	
		
		case 2:
			/* Bottom to Top */
			for ( yCount = (windowSizeY - Prefs.drawSize);
				yCount >= (drawMenuHeight + Prefs.drawSize);
				yCount-- )
			{
				WinDrawLine (Prefs.drawSize, yCount,
					windowSizeX - Prefs.drawSize - 1, yCount);

				if (0 < Prefs.drawSpeed)
				{
					/* 10ms times draw speed */ 
					SysTaskDelay ( SysTicksPerSecond() / 100 * Prefs.drawSpeed );
				}
			}
			break;

		case 3: 
			/* Left to Right */
			for ( xCount = Prefs.drawSize;
				xCount < (windowSizeX - Prefs.drawSize);
				xCount++ )
			{
				WinDrawLine (xCount, (drawMenuHeight + Prefs.drawSize),
					xCount, windowSizeY - Prefs.drawSize - 1);

				if (0 < Prefs.drawSpeed)
				{
					/* 10ms times draw speed */ 
					SysTaskDelay ( SysTicksPerSecond() / 100 * Prefs.drawSpeed );
				}
			}
			break;	

		case 4:
			/* Right to Left */
			for ( xCount= (windowSizeX - Prefs.drawSize - 1);
				xCount >= Prefs.drawSize;
				xCount-- )
			{
				WinDrawLine (xCount, (drawMenuHeight + Prefs.drawSize),
					xCount, windowSizeY - Prefs.drawSize - 1);

				if (0 < Prefs.drawSpeed)
				{
					/* 10ms times draw speed */ 
					SysTaskDelay ( SysTicksPerSecond() / 100 * Prefs.drawSpeed );
				}
			}
			break;

		case 5:
			/* Inside to Outside */
			for ( yCount = windowSizeY -
					(windowSizeY - drawMenuHeight) / 2,
				xCount = (windowSizeX / 2) - (drawMenuHeight / 2);
				yCount >= (drawMenuHeight + Prefs.drawSize) && xCount >= Prefs.drawSize;
				yCount--, xCount--)
			{
				RctSetRectangle (&rectangle, xCount, yCount,
					(windowSizeX / 2 - xCount) * 2,
					( (windowSizeY + drawMenuHeight) / 2 - yCount) * 2 + 1);
				
				WinDrawRectangle (&rectangle, radius);

				if (0 < Prefs.drawSpeed)
				{
					/* 10ms times draw speed */ 
					SysTaskDelay ( SysTicksPerSecond() / 100 * Prefs.drawSpeed );
				}
			}
			break;

		case 6:
			/* Outside to Inside */
			for ( yCount = drawMenuHeight + Prefs.drawSize + 1,
				xCount = Prefs.drawSize + 1;
				yCount <= windowSizeY - (windowSizeY - drawMenuHeight) / 2
					&& xCount <= windowSizeX / 2;
				yCount++, xCount++)
			{
				FrameType frame;
				frame = rectangleFrame;

				RctSetRectangle (&rectangle, xCount, yCount,
					(windowSizeX / 2 - xCount) * 2,
					( (windowSizeY + drawMenuHeight) / 2 - yCount) * 2 + 1);
				WinDrawRectangleFrame (frame, &rectangle);

				if (0 < Prefs.drawSpeed)
				{
					/* 10ms times draw speed */ 
					SysTaskDelay ( SysTicksPerSecond() / 100 * Prefs.drawSpeed );
				}
			}
			break;
		case 7:
			/* None */
			RctSetRectangle (&rectangle, Prefs.drawSize, drawMenuHeight + Prefs.drawSize,
				windowSizeX - 2 * Prefs.drawSize, windowSizeY - 2 * Prefs.drawSize - drawMenuHeight);
			WinDrawRectangle (&rectangle, radius);

			break;
	}

	return (0);
}


/**********************************************************************
 *
 * Function:    RomVersionCompatible
 *
 * Description: Checks that the ROM is of a minimum version.
 *
 * Parameters:  requiredVersion - minimum rom version required
 *                                (see sysFtrNumROMVersion in
 *                                SystemMgr.h for format)
 *              launchFlags     - flags that indicate if the
 *                                application UI is initialized.
 *
 * Returned:    Error code, or zero if rom is compatible.
 *
 * History:
 *   2000 Sep 19     jcbarker     Adapted from PalmOS 3.5 SDK.
 *
 **********************************************************************/

static UInt16 RomVersionCompatible (UInt32 requiredVersion, UInt16 launchFlags)
{
	UInt32 romVersion;

	/* Get ROM version */
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if (romVersion < requiredVersion)
	{
		if ((launchFlags & (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
			(sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp))
		{
			FrmAlert (RomIncompatibleAlert);
		
			/* Pilot 1.0 will continuously relaunch this app unless
			   we switch to another safe one. */
			if (romVersion < 0x02003000)
			{
				AppLaunchWithCommand(sysFileCDefaultApp,
					sysAppLaunchCmdNormalLaunch,
					NULL);
			}
		} 
		
		return (sysErrRomIncompatible);
	}

	return (0);
}
