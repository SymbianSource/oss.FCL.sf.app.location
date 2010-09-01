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
#include "CLmkSelectorImplBase.h"
#include "CLmkListProviderBase.h"
#include "CLmkSelectorIconMgr.h"
#include "MLmkListMemento.h"
#include "CLmkUiUtils.h"
#include "LmkConsts.h"
#include <avkon.mbg>
#include <AknIconArray.h>
#include <eikenv.h>
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
const TInt KIconArrayGranularity( 10 );
#if defined(_DEBUG)

_LIT( KPanicMsg, "CLmkSelectorImplBase" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace


// -----------------------------------------------------------------------------
// CLmkSelectorImplBase::TLmkRestoreData::TLmkRestoreData
// Constructor of an internal helper class.
// -----------------------------------------------------------------------------
//
CLmkSelectorImplBase::TLmkRestoreData::TLmkRestoreData()
    : iRestoreRequested( EFalse ),
      iListBox( NULL ),
      iProvider( NULL ),
      iFiltered( EFalse )
    {
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSelectorImplBase::CLmkSelectorImplBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSelectorImplBase::CLmkSelectorImplBase( CPosLandmarkDatabase& aDb )
    : iDb( aDb ),
      iSelectorImplBaseLatestListEvent( ELmkEventNull )

    {
    }

// -----------------------------------------------------------------------------
// CLmkSelectorImplBase::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkSelectorImplBase::BaseConstructL()
    {
    iEnv = CEikonEnv::Static();
    iLmkUiUtils = CLmkUiUtils::NewL();
    iLmkUiUtils->ReadLmkUiResourceFileL();
    SetupListProviderL();
    }

// ----------------------------------------------------
// CLmkSelectorImplBase::~CLmkSelectorImplBase
// ----------------------------------------------------
//
CLmkSelectorImplBase::~CLmkSelectorImplBase()
    {
    delete iLmkUiUtils;
    delete iListProvider;
    delete iIconMgr;
    }

// -----------------------------------------------------------------------------
// CLmkSelectorImplBase::SetMemento
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkSelectorImplBase::SetMemento( MLmkListMemento& aMemento )
    {
    iMemento = &aMemento;
    }

// ----------------------------------------------------
// CLmkSelectorImplBase::SetupIconMgrLC
// ----------------------------------------------------
//
CAknIconArray* CLmkSelectorImplBase::SetupIconMgrLC()
    {
    CAknIconArray* icons =
        new( ELeave ) CAknIconArray( KIconArrayGranularity );
    CleanupStack::PushL( icons );

    delete iIconMgr;
    iIconMgr = NULL;
    iIconMgr = CLmkSelectorIconMgr::NewL( *icons );
    TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();

    iIconMgr->SetDefaultIconL( CLmkSelectorIconMgr::ELandmarkDefaultIcon,
                        *iconFile, KLmkDefaultId );

    iIconMgr->SetDefaultIconL( CLmkSelectorIconMgr::ECategoryDefaultIcon,
                        *iconFile, KCategoryDefaultId );

    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullListProvider ) );
    iListProvider->SetIconMgrL( *iIconMgr );
    CleanupStack::PopAndDestroy();//iconFile
    return icons;
    }

// -----------------------------------------------------------------------------
// CLmkSelectorImplBase::Store
// -----------------------------------------------------------------------------
//
void CLmkSelectorImplBase::Store(
    const CEikListBox& aListBox,
    const CLmkListProviderBase& aProvider,
    TBool aFiltered )
    {
    if ( iMemento && iSelectorImplBaseLatestListEvent == ELmkEventListReady )
        {
        iMemento->Store( aListBox, aProvider, aFiltered );
        }
    // If list is not ready then store doesn't make sense and we don't do
    // anything.
    }

// -----------------------------------------------------------------------------
// CLmkSelectorImplBase::Restore
// -----------------------------------------------------------------------------
//
void CLmkSelectorImplBase::Restore(
    const CEikListBox& aListBox,
    const CLmkListProviderBase& aProvider,
    TBool aFiltered )
    {
    if ( iMemento )
        {
        if ( iSelectorImplBaseLatestListEvent == ELmkEventListReady )
            {
            iMemento->Restore( aListBox, aProvider, aFiltered );
            }
        else
            {
            // restore cannot be done yet, it is postponed:
            iRestoreData.iRestoreRequested = ETrue;
            iRestoreData.iListBox = &aListBox;
            iRestoreData.iProvider = &aProvider;
            iRestoreData.iFiltered = aFiltered;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkSelectorImplBase::HandleListProviderEvent
// -----------------------------------------------------------------------------
//
void CLmkSelectorImplBase::HandleListProviderEvent(
    TLmkListProviderEventType aEvent )
    {
    iSelectorImplBaseLatestListEvent = aEvent;

    if ( ( iSelectorImplBaseLatestListEvent == ELmkEventListReady
    		|| iSelectorImplBaseLatestListEvent == ELmkEventItemAdditionComplete ) &&
         iRestoreData.iRestoreRequested )
        { // Postponed memento restoration
        __ASSERT_DEBUG( iRestoreData.iListBox && iRestoreData.iProvider,
                        Panic( KLmkPanicPostponedRestoration ) );
        iMemento->Restore( *iRestoreData.iListBox,
                           *iRestoreData.iProvider,
                           iRestoreData.iFiltered );
        // It is enough to turn off iRestoreRequested:
        iRestoreData.iRestoreRequested = EFalse;
        }
    }

//  End of File
