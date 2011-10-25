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
 *   AppRsc.h
 *
 * Description:
 *	 Header file for application.
 *
 * History:
 *   2000 Sep 19     jcbarker     Initial public release.
 *   2002 Aug 23     jcbarker     Minor changes for version 1.1.
 *
 *****************************************************************************/

/* Forms */
#define MainForm						1000
#define PrefsForm						1001
#define AboutForm						1002

/* Bitmaps */
#define InfoBitmap                      1000
#define IconBitmap                      1001

#define MyBarBeamBitmap                 1005
#define MyBarRefreshBitmap              1006
#define MyBarPrefsBitmap                1007
#define MyBarInfoBitmap                 1008



/* Buttons */
#define AboutFormOkButton				1000
#define PrefsFormOkButton				1001
#define PrefsFormCancelButton			1002
#define PrefsFormDefaultsButton			1003

/* Lists */
#define PrefsDrawSpeedsList				1010
#define PrefsDrawStylesList				1011
#define PrefsDrawSizesList				1012

/* Checkboxes */
#define PrefsSaveAcrossSessionChk		1013
#define PrefsShowSplashScreenChk		1014


/* Popup Triggers */
#define PrefsDrawSpeedsPopupTrigger		1020
#define PrefsDrawStylesPopupTrigger		1021
#define PrefsDrawSizesPopupTrigger		1022

/* Help */


/* Menus */
#define MainFormMenuBar					1000

/* Menu Items */
#define MainOptionsBeam					100
#define MainOptionsRefresh				101
#define MainOptionsPrefs				102
#define MainOptionsTips 				103
#define MainOptionsAbout				104

/* Strings */
#define TipsStr							1000

/* Alerts */
#define PrefsDefaultsWarning			1002
#define RomIncompatibleAlert			1003
