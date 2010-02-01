/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility class for Debug traces
*
*/


#ifndef LCDEBUG_H
#define LCDEBUG_H

// INCLUDES
#include <e32std.h>

// #define _LCDEBUG
    
// MACROS
#ifdef _LCDEBUG

#define DEBUG( TEXT) Debug( _L( #TEXT ));
#define DEBUG1( TEXT, ARG1 ) Debug( _L( #TEXT ), ( ARG1 ));
#define DEBUG2( TEXT, ARG1, ARG2 ) Debug( _L( #TEXT ), ( ARG1 ), ( ARG2 ));
#define DEBUG3( TEXT, ARG1, ARG2, ARG3 ) Debug( _L( #TEXT ), ( ARG1 ), ( ARG2 ), ( ARG3 ));

#else

#define DEBUG( TEXT ) ;
#define DEBUG1( TEXT, ARG1 ) ;
#define DEBUG2( TEXT, ARG1, ARG2 ) ;
#define DEBUG3( TEXT, ARG1, ARG2, ARG3 ) ;

#endif // _LCDEBUG

IMPORT_C void Debug( TRefByValue<const TDesC> aText, ...);

#endif // LCDEBUG_H
