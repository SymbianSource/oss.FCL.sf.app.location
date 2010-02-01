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
* Description:    LandmarksUi Content File -
*
*/








// INCLUDE FILES
#include "CLmkSelectorLBModelBase.h"
#include "CLmkListProviderBase.h"
#include "CLmkSelectorCache.h"
#include <EPos_Landmarks.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
// Enough space for raw text + formatting:
const TInt KLmkSelectorLbModelLength = KPosLmMaxTextFieldLength+10;
}  // namespace
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSelectorLBModelBase::CLmkSelectorLBModelBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSelectorLBModelBase::CLmkSelectorLBModelBase()
    {
    }

// -----------------------------------------------------------------------------
// CLmkSelectorLBModelBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkSelectorLBModelBase::BaseConstructL(
                                  CLmkListProviderBase& aListProvider )
    {
    iCache = CLmkSelectorCache::NewL( aListProvider );
    iBuffer = HBufC::NewL( KLmkSelectorLbModelLength );
    }

// ----------------------------------------------------
// CLmkSelectorLBModelBase::~CLmkSelectorLBModelBase
// ----------------------------------------------------
//
CLmkSelectorLBModelBase::~CLmkSelectorLBModelBase()
    {
    delete iCache;
    delete iBuffer;
    }

// -----------------------------------------------------------------------------
// CLmkSelectorLBModelBase::MdcaCount
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkSelectorLBModelBase:: MdcaCount() const
    {
    return iCache->ItemCount();
    }

// -----------------------------------------------------------------------------
// CLmkSelectorLBModelBase::MdcaPoint
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC16 CLmkSelectorLBModelBase::MdcaPoint( TInt aIndex ) const
    {
    TRAPD( err, MdcaPointL( aIndex ) );
    if ( err != KErrNone && err != KErrLocked)
        {
        //*iBuffer = KNullDesC;
        }
	return *iBuffer;
    }

//  End of File
