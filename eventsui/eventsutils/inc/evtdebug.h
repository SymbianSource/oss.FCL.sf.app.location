/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Debug File.
*
*/


#ifndef _EVTDEBUG_H_
#define _EVTDEBUG_H_

// INCLUDES
#include <e32std.h>
#include "evtdebugconfig.hrh"

// MACROS
#define DEBUG(TEXT) 				 { \
									  _LIT( KText, #TEXT ); \
									   Debug(KText); \
									 }
									 
#define EVTUI(TEXT, ARG1) 			 { \
									   _LIT( KText, #TEXT ); \
									   Debug( KText, (ARG1) ); \
									 }
									 
#define EVTUI2(TEXT, ARG1, ARG2) 		 { \
									   _LIT( KText, #TEXT ); \
									   Debug( KText,(ARG1), (ARG2) ); \
									 }
									 
#define EVTUI3(TEXT, ARG1, ARG2, ARG3) { \
									   _LIT( KText, #TEXT ); \
									   Debug( KText, (ARG1), (ARG2), (ARG3) ); \
									 }

IMPORT_C void CreateDir();

IMPORT_C void Debug( TRefByValue<const TDesC> aText, ...);

/**
* By using component specific debug macros unnecessary 
* debug prints can be avoided. 
* Also component specific debugging is possible by compiling 
* only certain components when EVTUI_ENABLE_DEBUG_PRINT is defined.
*/

#ifdef EVTUI_ENABLE_DEBUG_PRINT

#define CREATELOGDIR()						 { CreateDir();};									 
#define EVTUIDEBUG(TEXT)                     DEBUG(TEXT);
#define EVTUIDEBUG1(TEXT, ARG1)              EVTUI(TEXT, ARG1);
#define EVTUIDEBUG2(TEXT, ARG1, ARG2)        EVTUI2(TEXT, ARG1, ARG2);
#define EVTUIDEBUG3(TEXT, ARG1, ARG2, ARG3)  EVTUI3(TEXT, ARG1, ARG2, ARG3);

#else

#define CREATELOGDIR()						 ;
#define EVTUIDEBUG(TEXT)                     ;
#define EVTUIDEBUG1(TEXT, ARG1)              ;
#define EVTUIDEBUG2(TEXT, ARG1, ARG2)        ;
#define EVTUIDEBUG3(TEXT, ARG1, ARG2, ARG3)  ;

#endif

#endif // _EVTDEBUG_H_
