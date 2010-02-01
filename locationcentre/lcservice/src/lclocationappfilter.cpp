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
* Description:  Location Application Filters
*
*/


// SYSTEM INCLUDES

// USER INCLUDES
#include "lclocationappfilter.h"

// CONSTANT DEFINITIONS

// This value is obtained by ORing all the System characteristics
// available
const TUint32   KMaxSystemCharValue = 1;

// This value is obtained by ORing all the Application characteristics
// available
const TUint32   KMaxAppCharValue = 1023;


// ----- Member funtions for TLcLocationAppFilter ------------------

// ---------------------------------------------------------------------------
// TUint32 TLcLocationAppFilter::TLcLocationAppFilter
// ---------------------------------------------------------------------------
//
EXPORT_C TLcLocationAppFilter::TLcLocationAppFilter()
    :iSystemChar(0),
    iSysFilterConfig( TLcLocationAppFilter::EFilterStrict ),
    iApplicationChar(0),
    iAppFilterConfig( TLcLocationAppFilter::EFilterStrict )
    {
    }
    
// ---------------------------------------------------------------------------
// TUint32 TLcLocationAppFilter::GetSystemCharacteristics
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 TLcLocationAppFilter::SystemCharacteristics() const
    {
    return iSystemChar;
    }

// ---------------------------------------------------------------------------
// TInt TLcLocationAppFilter::AddSystemCharacteristic
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TLcLocationAppFilter::SetSystemCharacteristic( 
                                                TUint32     aSystemChar  )
    {
    // Check if the System characteristics value is a valid bit-mask.
    if( aSystemChar > KMaxSystemCharValue )
        {
        return KErrArgument;
        }
        
    iSystemChar = aSystemChar;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TLcLocationAppFilter::TLocationAppFilterConfig TLcLocationAppFilter::
//                                                  SysCharFilterConfiguration
// ---------------------------------------------------------------------------
//
EXPORT_C TLcLocationAppFilter::TLcFilterConfig 
                TLcLocationAppFilter::SysCharFilterConfiguration() const
    {
    return iSysFilterConfig;
    }
    
// ---------------------------------------------------------------------------
// void TLcLocationAppFilter::SetSysCharacFilterConfig
// ---------------------------------------------------------------------------
//
EXPORT_C void TLcLocationAppFilter::SetSysCharFilterConfiguration( 
                     TLcLocationAppFilter::TLcFilterConfig   aFilterConfig )
    {
    iSysFilterConfig = aFilterConfig;
    }

// ---------------------------------------------------------------------------
// TUint32 TLcLocationAppFilter::GetApplicationCharacteristics
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 TLcLocationAppFilter::ApplicationCharacteristics() const
    {
    return iApplicationChar;
    }

// ---------------------------------------------------------------------------
// TInt TLcLocationAppFilter::SetApplicationCharacteristics
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TLcLocationAppFilter::SetApplicationCharacteristics( 
                                                TUint32     aAppChar  )
    {
    // Check if the Application characteristics value is a valid bit-mask.
    if( aAppChar > KMaxAppCharValue )
        {
        return KErrArgument;
        }
        
    iApplicationChar = aAppChar;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TLcLocationAppFilter::TLcFilterConfig AppCharFilterConfiguration
// ---------------------------------------------------------------------------
//
EXPORT_C TLcLocationAppFilter::TLcFilterConfig 
            TLcLocationAppFilter::AppCharFilterConfiguration() const
    {
    return iAppFilterConfig;
    }
    
// ---------------------------------------------------------------------------
// void TLcLocationAppFilter::SetAppCharFilterConfiguration
// ---------------------------------------------------------------------------
//
EXPORT_C void TLcLocationAppFilter::SetAppCharFilterConfiguration( 
                     TLcLocationAppFilter::TLcFilterConfig   aFilterConfig )
    {
    iAppFilterConfig = aFilterConfig;
    }

// End of File

