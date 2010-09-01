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
#include "CLmkDbUtils.h"
#include "CLmkUiUtils.h"
#include "LmkConsts.h"
#include "CLmkAppCategorySelectorImpl.h"
#include "CLmkLmItemListProvider.h"
#include "LmkListProviderFactory.h"
#include <aknlists.h>
#include <lmkerrors.h>


#if defined(_DEBUG)// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkAppCategorySelectorImpl" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::CLmkAppCategorySelectorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkAppCategorySelectorImpl::CLmkAppCategorySelectorImpl(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TBool aFindBox )
    : CLmkAppLmItemSelectorImpl( aDb, aSender, aFindBox )
    {
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySelectorImpl::ConstructL()
    {
    CLmkAppLmItemSelectorImpl::BaseConstructL();

    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkAppCategorySelectorImpl* CLmkAppCategorySelectorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TBool aFindBox )
    {
    CLmkAppCategorySelectorImpl* self =
        new( ELeave ) CLmkAppCategorySelectorImpl( aDb, aSender, aFindBox );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::~CLmkAppCategorySelectorImpl
// -----------------------------------------------------------------------------
//
CLmkAppCategorySelectorImpl::~CLmkAppCategorySelectorImpl()
    {
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::SetupListProviderL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySelectorImpl::SetupListProviderL()
    {
    if ( !iListProvider )
        {
        iListProvider =
            LmkListProviderFactory::CreateProviderL( iDb, ECategorySelector );
        iListProvider->AddObserverL( *this );
        iListProvider->InitializeL();
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::ListProvider
// -----------------------------------------------------------------------------
//
const CLmkListProviderBase& CLmkAppCategorySelectorImpl::ListProvider() const
    {
    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullMember ) );
    return *iListProvider;
    }
// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::CurrentCategoryId
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkAppCategorySelectorImpl::CurrentCategoryId()
	{
	return CLmkAppLmItemSelectorImpl::CurrentCategoryId();
	}

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySelectorImpl::ProcessCommandL( TInt aCommandId )
	{
	CLmkAppLmItemSelectorImpl::ProcessCommandL( aCommandId );
	}

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySelectorImpl::HandleOperationL(
                                       TOperationTypes /*aType*/,
                                       TReal32 /*aProgress*/,
                                       TInt /*aStatus*/ )
    {
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::ChangeIconL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySelectorImpl::ChangeIconL(
    TPosLmItemId aId,
    TInt aIconIndex,
    TInt aMaskIndex)
    {
    TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();
    iDbUtils->ChangeIconL( iDb, aId, ELmkItemTypeCategory,
                           *iconFile, aIconIndex, aMaskIndex );
	CleanupStack::PopAndDestroy();//iconFile
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::ChangeIconsL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySelectorImpl::ChangeIconsL(
    RArray<TPosLmItemId>& aIdArray,
    TInt aIconIndex, TInt aMaskIndex)
    {
    TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();
    iDbUtils->ChangeIconsL( aIdArray, ELmkItemTypeCategory,
                            *iconFile, aIconIndex, aMaskIndex );
	CleanupStack::PopAndDestroy();//iconFile
    }
// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::HandleLmCreated
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySelectorImpl::HandleLmCreated(TPosLmItemId aNewLmkItemId)
    {
    iNewLmkItemId = aNewLmkItemId;
    iNewListBoxItemAdded = ETrue;
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySelectorImpl::HandleControlEventL
// -----------------------------------------------------------------------------
//
//#ifdef RD_SCALABLE_UI_V2
void CLmkAppCategorySelectorImpl::HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType )
	{
	//Update the listbox a sper the search criteria
	if (iFilter)
		{
		iFilter->HandleControlEventL(aControl, aEventType);
		}
	}

//  End of File
