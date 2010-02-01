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
* Description:  Error and Panic defintions for Location Centre API
*
*/

#ifndef LCERRORS_H
#define LCERRORS_H

/**
 *  @file lcerrors.h
 *
 *  This file provides the defintions for Error and Panics which are
 *  used in Location Centre.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
 
// CONSTANT DEFINTIONS
#include <e32base.h>

/**
 * Location Centre Panic Category
 */
_LIT( KLcPanicCategory, "LocationCentre");

/**
 * @enum TLCPanicCodes
 * Panic Codes for Location Centre
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0 
 */
enum TLcPanicCodes
    {
    /**
     * Code with which Location Centre panics when an attempt is made
     * to access an element of @ref CLcLocationAppInfoArray from an index
     * which does not exist.
     */
    ELcInvalidIndex                 = 0
    };
    
#endif // LCERRORS_H
