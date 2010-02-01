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
* Description:    Landmarks category selector dialog
*
*/









// INCLUDE FILES
#include "TLmkItemIdDbCombiInfo.h"
#include "CLmkCategorySelectorDlg.h"
#include "CLmkDlgCategorySelectorImpl.h"
#include <lmkui.rsg>
#include <Landmarks.rsg>
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

//const TInt KSingleSelectedGranularity = 1;
_LIT( KPanicMsg, "CLmkCategorySelectorDlg" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::CLmkCategorySelectorDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkCategorySelectorDlg::CLmkCategorySelectorDlg()
    : iIsMultiSelector( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkCategorySelectorDlg::ConstructL(
    TBool aShowEmptyCategories )
    {
    iDbs.Append( CPosLandmarkDatabase::OpenL());
    iSelector = CLmkDlgCategorySelectorImpl::NewL( *(iDbs[0]), aShowEmptyCategories );
    iSelector->SetEmptyDlgLabel(R_LMK_EMPTY_NO_CATEGORIES ,R_LMK_GUIDE_CREATE_LANDMARK);
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
 EXPORT_C CLmkCategorySelectorDlg* CLmkCategorySelectorDlg::NewL(
 								 TBool aShowEmptyCategories )
    {
    CLmkCategorySelectorDlg* self = new ( ELeave ) CLmkCategorySelectorDlg();
    CleanupStack::PushL( self );
    self->ConstructL(aShowEmptyCategories );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::~CLmkCategorySelectorDlg
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkCategorySelectorDlg::~CLmkCategorySelectorDlg()
    {
    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
        delete iSelector;
    if (iDbs.Count() > 0 )
        {
        delete iDbs[0];
        iDbs.Close();
        }
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::SetMopParent
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkCategorySelectorDlg::SetMopParent( MObjectProvider* aParent )
    {
    __ASSERT_DEBUG( iSelector, Panic( KLmkPanicNullMember ) );
    iSelector->SetMopParent( aParent );
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
// For multiple database support
EXPORT_C TInt CLmkCategorySelectorDlg::ExecuteLD( TLmkItemIdDbCombiInfo& aSelected )
    {
    __ASSERT_ALWAYS( iSelector, Panic( KLmkPanicNullMember ) );

    // "D" function semantics
    CleanupStack::PushL( this );
    TBool thisDestroyed( EFalse );
    iDestroyedPtr = &thisDestroyed;

    TPosLmItemId selectedId;

    TInt result = iSelector->ExecuteL( selectedId );

   	//If user has selected Ok then only return the selected items
    if(result)
		{
		aSelected.SetItemId(selectedId);
		aSelected.SetLmDb(CPosLandmarkDatabase::OpenL());
		}	//end if(result)

    if ( thisDestroyed )
        {
        // this object has been deleted already
        CleanupStack::Pop( this );
        }
    else
        {
        CleanupStack::PopAndDestroy( this );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
// For multiple database support
  EXPORT_C TInt CLmkCategorySelectorDlg::ExecuteLD(
    RArray<TLmkItemIdDbCombiInfo>& aSelectedItems )
    {
	__ASSERT_ALWAYS( iSelector, Panic( KLmkPanicNullMember ) );

	// "D" function semantics
	CleanupStack::PushL( this );
	TBool thisDestroyed( EFalse );
	iDestroyedPtr = &thisDestroyed;
	RArray<TPosLmItemId> selectedItems;
	selectedItems.Reset();
	for(TInt i = 0; i < aSelectedItems.Count(); i++)
        {
        TLmkItemIdDbCombiInfo getItem = aSelectedItems[i];
        selectedItems.Append(getItem.GetItemId());
        }
	TInt result = iSelector->ExecuteL( selectedItems );
    aSelectedItems.Reset();
	//If user has clicked on Ok then only return the selection
	TInt itemCount = selectedItems.Count();
	if(itemCount > 0)
		{

		CPosLandmarkDatabase* pDb = CPosLandmarkDatabase::OpenL();

		for (TInt i = 0; i < itemCount; i++)
			{
			TLmkItemIdDbCombiInfo selItem;
			selItem.SetItemId(selectedItems[i]);
			selItem.SetLmDb(pDb);
			aSelectedItems.Append(selItem);
			}

	}	//end if(result)

	selectedItems.Close();
	if ( thisDestroyed )
		{
		// this object has been deleted already
		CleanupStack::Pop( this );
		}
	else
		{
		CleanupStack::PopAndDestroy( this );
		}

	return result;
 }
//  End of File
