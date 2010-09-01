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
* Description:    LandmarksUi Content File -    Contains engine panic function and panic enum values.
*
*/







#ifndef LMKENG_DLLMAIN_H
#define LMKENG_DLLMAIN_H

//  INCLUDES
#include <e32std.h>

// DATA TYPES
/// Landmarks application's engine panic codes.
enum TLmkEnginePanicCode
    {
    };


// FUNCTION PROTOTYPES
/**
 * Stop the program execution with module specific text and error code.
 * Call if an unrecoverable error occurs in this module's code.
 *
 * @param aCode     error code
 *
 * @see TLmkEngPanicCode
 */
void LmkEngPanic(TInt aCode);

#endif // LMKENG_DLLMAIN_H

// End of File
