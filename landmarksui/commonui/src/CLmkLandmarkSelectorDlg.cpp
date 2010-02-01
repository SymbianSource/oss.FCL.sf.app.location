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
* Description:    Landmarks Selector dialog
*
*/



// INCLUDE FILES
#include "TLmkItemIdDbCombiInfo.h"
#include "CLmkLandmarkSelectorDlg.h"
#include "CLmkDlgCombiSelectorImpl.h"
#include <lmkui.rsg>
#include <Landmarks.rsg>
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkLandmarkSelectorDlg" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::CLmkLandmarkSelectorDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkLandmarkSelectorDlg::CLmkLandmarkSelectorDlg()
    : iIsMultiSelector( ETrue ),
    iDatabaseUri( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkLandmarkSelectorDlg::ConstructL()
    {
    // Open user specified db if it was set, else open the default landmarks db.
    if( iDatabaseUri )
        {
        iDbs.Append( CPosLandmarkDatabase::OpenL( iDatabaseUri->Des() ) );
        }
    else
        {
        iDbs.Append( CPosLandmarkDatabase::OpenL() );
        }
	
	iSelector = CLmkDlgCombiSelectorImpl::NewL( *(iDbs[0]) );
	iSelector->SetEmptyDlgLabel(R_LMK_EMPTY_NO_LANDMARKS,R_LMK_GUIDE_CREATE_LANDMARK);
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkLandmarkSelectorDlg* CLmkLandmarkSelectorDlg::NewL( )
    {
    CLmkLandmarkSelectorDlg* self = new( ELeave ) CLmkLandmarkSelectorDlg();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::NewL
// Two-phased constructor. Also sets the user specified landmark database whose
// entries are to be shown in the selector ui.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkLandmarkSelectorDlg* CLmkLandmarkSelectorDlg::NewL( const TDesC&  aDatabaseUri )
    {
    if( aDatabaseUri.Length() == 0 )
		    {
		    User::Leave( KErrArgument );
		  	}
    CLmkLandmarkSelectorDlg* self = new( ELeave ) CLmkLandmarkSelectorDlg();
    CleanupStack::PushL( self );
    self->iDatabaseUri = HBufC::NewLC( aDatabaseUri.Length() + 1 );
    CleanupStack::Pop( self->iDatabaseUri );
    self->iDatabaseUri->Des().FillZ();
    self->iDatabaseUri->Des().Copy( aDatabaseUri.Ptr() );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::~CLmkLandmarkSelectorDlg
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkLandmarkSelectorDlg::~CLmkLandmarkSelectorDlg()
    {
    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }

    if ( iDatabaseUri )
        {
        delete iDatabaseUri;
        }
    delete iSelector;
    if (iDbs.Count() > 0 )
        {
        delete iDbs[0];
        iDbs.Close();
        }
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::SetMopParent
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLandmarkSelectorDlg::SetMopParent( MObjectProvider* aParent )
    {
    __ASSERT_ALWAYS( iSelector, Panic( KLmkPanicNullMember ) );
    iSelector->SetMopParent( aParent );
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::ExecuteLD
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
// For multiple database support

EXPORT_C TInt CLmkLandmarkSelectorDlg::ExecuteLD( TLmkItemIdDbCombiInfo& aSelected )
    {
    __ASSERT_ALWAYS( iSelector, Panic( KLmkPanicNullMember ) );

    // "D" function semantics
    CleanupStack::PushL( this );
    TBool thisDestroyed( EFalse );
    iDestroyedPtr = &thisDestroyed;

    TPosLmItemId selectedId;
    TInt result = iSelector->ExecuteL( selectedId );

	//Fill selected items only if user has clicked Ok
    if( result )
		{
		aSelected.SetItemId( selectedId );
		
		// if user specified db present, set this as the selected item's db,
		// else set default landmarks db.
		if( iDatabaseUri )
		    {
		    aSelected.SetLmDb( CPosLandmarkDatabase::OpenL( iDatabaseUri->Des() ) );
		    }
		else
		    {
		    aSelected.SetLmDb( CPosLandmarkDatabase::OpenL() );
		    }
		
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
// CLmkLandmarkSelectorDlg::ExecuteLD
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 // For multiple database support
EXPORT_C TInt CLmkLandmarkSelectorDlg::ExecuteLD(
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
   	//Fill items only if user has clicked Ok
   	TInt itemCount = selectedItems.Count();
    if(itemCount > 0)
		{
		CPosLandmarkDatabase* pDb;
		if( iDatabaseUri )
		    {
		    pDb = CPosLandmarkDatabase::OpenL( iDatabaseUri->Des() );
		    }
		else
		    {
		    pDb = CPosLandmarkDatabase::OpenL();
		    }
		

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

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::SetDialogTitleL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLandmarkSelectorDlg::SetDialogTitleL(const TDesC& aTitle)
    {
    __ASSERT_ALWAYS( iSelector, Panic( KLmkPanicNullMember ) );
    iSelector->SetDialogTitleL(aTitle);
    }

//  End of File
