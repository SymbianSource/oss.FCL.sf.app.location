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
#include "CLmkListProviderBase.h"
#include "MLmkSelectorIconMgr.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

// two observers assumed
const TInt KObserverArraySize = 2;
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkListProviderBase::CLmkListProviderBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkListProviderBase::CLmkListProviderBase()
    : iObservers( KObserverArraySize )
    {
    }

// -----------------------------------------------------------------------------
// CLmkListProviderBase::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkListProviderBase::BaseConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CLmkListProviderBase::~CLmkListProviderBase
// -----------------------------------------------------------------------------
//
CLmkListProviderBase::~CLmkListProviderBase()
    {
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CLmkListProviderBase::AddObserverL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkListProviderBase::AddObserverL(
    MLmkListProviderObserver& aObserver )
    {
    User::LeaveIfError( iObservers.Append( &aObserver ) );
    }

// -----------------------------------------------------------------------------
// CLmkListProviderBase::RemoveObserver
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkListProviderBase::RemoveObserver(
    MLmkListProviderObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if ( index >= 0)
        {
        iObservers.Remove( index );
        return KErrNone;
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CLmkListProviderBase::SetIconMgrL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkListProviderBase::SetIconMgrL(
    MLmkSelectorIconMgr& aIconMgr )
    {
    iIconMgr = &aIconMgr;
    }

// -----------------------------------------------------------------------------
// CLmkListProviderBase::NotifyObservers
// -----------------------------------------------------------------------------
//
void CLmkListProviderBase::NotifyObservers( TLmkListProviderEventType aEvent )
    {
    for ( TInt i( 0 ); i < iObservers.Count(); ++i )
        {
        iObservers[i]->HandleListProviderEvent( aEvent );
        }
    }

// -----------------------------------------------------------------------------
// CLmkListProviderBase::NotifyError
// -----------------------------------------------------------------------------
//
void CLmkListProviderBase::NotifyError( TInt aError )
    {
    for ( TInt i( 0 ); i < iObservers.Count(); ++i )
        {
        iObservers[i]->HandleListProviderError( aError );
        }
    }

//  End of File
