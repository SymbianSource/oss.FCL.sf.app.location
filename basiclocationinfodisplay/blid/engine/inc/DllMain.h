/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Dll entry point
*
*/


#ifndef __BLIDENG_DLLMAIN_H__
#define __BLIDENG_DLLMAIN_H__

//  INCLUDES
#include <e32std.h>

// DATA TYPES
/// Basic Location info display application's engine panic codes.
enum TBlidEnginePanicCode
    {
    ELCEModelTypeNotSupported= 0
    };

// FUNCTION PROTOTYPES
/**
 * Stop the program execution with module specific text and error code.
 * Call if an unrecoverable error occurs in this module's code.
 *
 * @param aCode     error code
 *
 * @see TBlidEngPanicCode
 */
void BlidEngPanic(TInt aCode);

#endif // __BLIDENG_DLLMAIN_H__

// End of File
