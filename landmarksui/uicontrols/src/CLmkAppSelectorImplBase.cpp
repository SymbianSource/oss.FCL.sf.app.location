/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is a base class for Selector implementation
*
*/



// INCLUDE FILES
#include <avkon.mbg>
#include <AknsConstants.h>
#include <aknlists.h>
#include <eikclbd.h>
#include <AknIconArray.h>
#include <StringLoader.h>
#include <landmarks.rsg>
#include <aknsfld.h>
#include <aknlayout.cdl.h>
#include "LmkConsts.h"
#include "CLmkSelectorIconMgr.h"
#include "MLmkListMemento.h"
#include "CLmkUiUtils.h"
#include "CLmkSingleGraphicLBModel.h"
#include "CLmkDoubleGraphicLBModel.h"
#include "CLmkAppSelectorImplBase.h"
#include "MLmkMskObserver.h"
#include "CLmkLmItemListProvider.h"
#include <lmkerrors.h>

#include <lmkui.rsg>


const TInt KTextLimit( 40 );
_LIT(KFetchingData, " ");
#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkAppSelectorImplBase" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace

#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::CLmkAppSelectorImplBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkAppSelectorImplBase::CLmkAppSelectorImplBase(
    CPosLandmarkDatabase& aDb,
    TBool aFindBox )
    : CLmkSelectorImplBase( aDb ),
      iFindBox( aFindBox )
    {
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkAppSelectorImplBase::BaseConstructL()
    {
    CLmkSelectorImplBase::BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::~CLmkAppSelectorImplBase
// -----------------------------------------------------------------------------
//
CLmkAppSelectorImplBase::~CLmkAppSelectorImplBase()
    {
    delete iListBox;
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::StoreMemento
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkAppSelectorImplBase::StoreMemento()
    {
    __ASSERT_DEBUG( iListBox && iListProvider, Panic( KLmkPanicNullMember ) );
    Store( *iListBox, *iListProvider, iFindBox );
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::RestoreMemento
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkAppSelectorImplBase::RestoreMemento()
    {
    __ASSERT_DEBUG( iListBox && iListProvider, Panic( KLmkPanicNullMember ) );
    Restore( *iListBox, *iListProvider, iFindBox );
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::ListVisibleItemCount
// -----------------------------------------------------------------------------
//
TInt CLmkAppSelectorImplBase::ListVisibleItemCount()
    {
    __ASSERT_DEBUG( iListBox, Panic( KLmkPanicNullMember ) );
    return iListBox->Model()->NumberOfItems();
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::ListMarkedItemCountL
// -----------------------------------------------------------------------------
//
TInt CLmkAppSelectorImplBase::ListMarkedItemCountL()
    {
    __ASSERT_DEBUG( iListBox, Panic( KLmkPanicNullMember ) );
    const CArrayFix<TInt>* markedIndexes = NULL;
    if ( iFindBox )
        {
        CAknFilteredTextListBoxModel* model =
            static_cast<CAknFilteredTextListBoxModel*>( iListBox->Model() );
        model->Filter()->UpdateSelectionIndexesL();
        markedIndexes = model->Filter()->SelectionIndexes();
        }
    else
        {
        markedIndexes = iListBox->SelectionIndexes();
        }
    return markedIndexes->Count();
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::CreateListBoxL
// -----------------------------------------------------------------------------
//
CEikTextListBox& CLmkAppSelectorImplBase::CreateListBoxL(
    CCoeControl* aParent,
    TInt aControlType,
    TInt aFlags,
    TInt aEmptyLBPrimResource,TInt aEmptyLBSecResource )
    {
    if ( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }

    // For maintaining empty listbox resource
    iEmptyLBPrimResource = aEmptyLBPrimResource;
    iEmptyLBSecResource = aEmptyLBSecResource;

    TBool isFormattedCellListBox = EFalse;
    switch ( aControlType )
        {
        case EAknCtSingleGraphicListBox:
            {
            iListBox = new( ELeave ) CAknSingleGraphicStyleListBox;
            break;
            }
        case EAknCtDoubleGraphicListBox:
            {
            iListBox = new( ELeave ) CAknDoubleGraphicStyleListBox;
            isFormattedCellListBox = ETrue;
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    iListBox->SetMopParent(aParent);
    iListBox->SetParent(aParent);
    iListBox->ConstructL( aParent, aFlags );

	HBufC* textForFetchingData;
	textForFetchingData = HBufC::NewL(20);
	CleanupStack::PushL(textForFetchingData);  // textForFetchingData
	*textForFetchingData = KFetchingData;
    iListBox->View()->SetListEmptyTextL( *textForFetchingData);
    CleanupStack::PopAndDestroy();  // textForFetchingData.

    // create icon array & icons
    CAknIconArray* icons = SetupIconMgrLC();
	TFileName* iconFile = CLmkUiUtils::AvkonIconFileLC();
    iIconMgr->SetIconAtIndexZeroL( *iconFile,
                                   KAknsIIDQgnIndiMarkedAdd,
                                   EMbmAvkonQgn_indi_marked_add,
                                   EMbmAvkonQgn_indi_marked_add_mask );

    if ( isFormattedCellListBox )
	    {
	    ( static_cast<CEikFormattedCellListBox*>( iListBox ) )
            ->ItemDrawer()->FormattedCellData()->SetIconArray( icons );
         ((CEikFormattedCellListBox*)iListBox)->ItemDrawer()->FormattedCellData()
		            ->EnableMarqueeL( ETrue );
	    }
    else
	    {
	    ( static_cast<CEikColumnListBox*>( iListBox ) )
            ->ItemDrawer()->ColumnData()->SetIconArray( icons );
          ((CEikColumnListBox*)iListBox)->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
	    }
	CleanupStack::PopAndDestroy(); // iconFile
    CleanupStack::Pop( icons ); // ownership was transferred

    // create listbox model for listbox
    CreateListBoxModelL( *iListBox, aControlType, *iListProvider );
    return *iListBox;
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::HandleListProviderEvent
// -----------------------------------------------------------------------------
//
void CLmkAppSelectorImplBase::HandleListProviderEvent(
    TLmkListProviderEventType aEvent )
    {
    if( !iListBox )
        return;
    
    if ( aEvent == ELmkEventItemAdditionComplete )
    	{
        TRAPD( error, iListBox->HandleItemAdditionL() );
        if ( error != KErrNone )
            {
            iListBox->Reset();
            iListBox->DrawNow();
            }
    	return;
    	}
    if ( aEvent == ELmkEventListReady || aEvent == ELmkEventFindListReady )
        {
        TBool handled( EFalse );
        TRAPD( error, handled = HandleEventListReadyL() );
        if ( !handled || error != KErrNone )
            {
            //CLmkLmItemListProvider& lstpr = static_cast<CLmkLmItemListProvider&>( *iListProvider );
            //TInt currentItemIndex = lstpr.SelectedIndex(iNewLmkItemId);	    //iListBox->CurrentItemIndex();
            TInt currentItemIndex = iListBox->CurrentItemIndex();
            TRAP( error, iListBox->HandleItemAdditionL() );
			if( currentItemIndex >= 0 && currentItemIndex < iListBox->Model()->NumberOfItems() )
				iListBox->SetCurrentItemIndex(currentItemIndex);
            if ( error != KErrNone )
                {
                iListBox->DrawNow();
                }
            }
        }

    TInt count = static_cast<CTextListBoxModel*>(iListBox->Model())->ItemTextArray()->MdcaCount();
    if( count == 0 )
        {
        TRAPD( error, MakeEmptyTextListBoxL( *iListBox, iEmptyLBPrimResource,iEmptyLBSecResource ));
        if( error != KErrNone )
            {
            return;
            }
        }

    //Enable Find box only if at least one item is there in the list box
	if( count )
		{
		if( aEvent != ELmkEventFindListReady )
			{
			TRAP_IGNORE( ShowFindBoxL( ETrue ) );
			}
		CLmkSelectorImplBase::HandleListProviderEvent( aEvent );
		}
    else
	    {
	    if ( aEvent == ELmkEventFindListReady )
		    {
		    TRAPD( error, MakeEmptyTextListBoxL( *iListBox, iEmptyLBPrimResource,iEmptyLBSecResource, ETrue ));
		    if( error != KErrNone )
		    	{
		        return;
		        }
		    }
		else
			{
	        TRAPD( error, MakeEmptyTextListBoxL( *iListBox, iEmptyLBPrimResource,iEmptyLBSecResource ));
	        if( error != KErrNone )
	            {
	            return;
	            }
			TRAP_IGNORE( ShowFindBoxL( EFalse ) );
			}
	    }
	if(iMskObserver)
		{
		TRAP_IGNORE(iMskObserver->UpdateMskContainerL());
		}
    }

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::HandleListProviderError
// -----------------------------------------------------------------------------
//
void CLmkAppSelectorImplBase::HandleListProviderError( TInt /*aError*/ )
    {
    if ( iListBox )
        {
        iListBox->Reset();
        TRAPD( error, iListBox->HandleItemAdditionL() );
        if ( error != KErrNone )
            {
            iListBox->DrawNow();
            }
        }
    }

// ----------------------------------------------------
// CLmkAppSelectorImplBase::MakeEmptyTextListBoxL
// ----------------------------------------------------
//
 void CLmkAppSelectorImplBase::MakeEmptyTextListBoxL(
    CEikTextListBox& aListBox,
    TInt aResourcePrimText ,TInt aResourceSecText, TBool aRequestFromFindBox ) const
    {
    _LIT( KNChar,"\n" );

    if (aRequestFromFindBox)
	    {
	    HBufC* text1 = StringLoader::LoadLC( R_LMK_EMPTY_NO_MATCH, const_cast<CEikonEnv *>( iEnv ));
		TPtr ptr1 = text1->Des();
		aListBox.View()->SetListEmptyTextL( ptr1 );
		CleanupStack::PopAndDestroy(text1);
	    }
    else
	    {
		HBufC* text1 = StringLoader::LoadLC( aResourcePrimText, const_cast<CEikonEnv *>( iEnv ));
		HBufC* text2 = StringLoader::LoadLC( aResourceSecText, const_cast<CEikonEnv *>( iEnv ));

		TPtr ptr1 = text1->Des();
		TPtr ptr2 = text2->Des();

		HBufC* buffer = HBufC::NewLC(text1->Length() + text2->Length() + 2);
		TPtr textFromResourceFile = buffer->Des();
    	textFromResourceFile.Copy(ptr1);
    	textFromResourceFile.Append(KNChar);
    	textFromResourceFile.Append(ptr2);

 		aListBox.View()->SetListEmptyTextL( textFromResourceFile );
		CleanupStack::PopAndDestroy(3);//buffer,text2,text1
	    }
    }

// ----------------------------------------------------
// CLmkAppSelectorImplBase::CreateListBoxModelL
// ----------------------------------------------------
//
void CLmkAppSelectorImplBase::CreateListBoxModelL(
    CEikTextListBox& aListBox,
    TInt aControlType,
    CLmkListProviderBase& aProvider ) const
    {
    CLmkSelectorLBModelBase* lbModel = NULL;
    // create listbox model, pass ownership
    switch( aControlType )
        {
        case EAknCtSingleGraphicListBox:
            {
            lbModel = CLmkSingleGraphicLBModel::NewL( aProvider );
            break;
            }
        case EAknCtDoubleGraphicListBox:
            {
            lbModel = CLmkDoubleGraphicLBModel::NewL( aProvider );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    aListBox.Model()->SetItemTextArray( lbModel );
    aListBox.Model()->SetOwnershipType( ELbmOwnsItemArray );
    }

// ----------------------------------------------------
// CLmkAppSelectorImplBase::HandleEventListReadyL()
// ----------------------------------------------------
//
TBool CLmkAppSelectorImplBase::HandleEventListReadyL()
    {
    // default implementation
    return EFalse;
    }

// ----------------------------------------------------
// CLmkAppSelectorImplBase::CreateFindBoxL()
// ----------------------------------------------------
//
EXPORT_C CAknSearchField* CLmkAppSelectorImplBase::CreateFindBoxL(
                                            const CCoeControl &aParent)
	{
	CAknFilteredTextListBoxModel* model =
	static_cast<CAknFilteredTextListBoxModel*> ( iListBox->Model() );
	iSearchBox =
	CAknSearchField::NewL( aParent, CAknSearchField::ESearch, NULL, KTextLimit );

	// Creates CAknListBoxFilterItems object.
	model->CreateFilterL( iListBox, iSearchBox );

#ifdef RD_SCALABLE_UI_V2
	iFilter = model->Filter();
	iSearchBox->SetObserver(this);
#endif// RD_SCALABLE_UI_V2
	iSearchBox->SetFocus( EFalse );
	iSearchBox->MakeVisible( EFalse );     //by defult disable the find box
	return iSearchBox;
	}

// ----------------------------------------------------
// CLmkAppSelectorImplBase::ShowFindBoxL()
// ----------------------------------------------------
//
void CLmkAppSelectorImplBase::ShowFindBoxL( TBool aVisible )
	{
	const TRect rect(iListBox->Parent()->Rect());
	if (iSearchBox)
		{
	//	iSearchBox->ResetL();
		iSearchBox->SetFocus(aVisible);
		iSearchBox->MakeVisible(aVisible);
		static_cast<CAknColumnListBoxView*>
		(iListBox->View())->SetFindEmptyListState(aVisible);

		if (aVisible && iListBox && iSearchBox)
			{
			AknLayoutUtils::LayoutControl(iListBox, rect,
			AKN_LAYOUT_WINDOW_list_gen_pane(1));
			AknLayoutUtils::LayoutControl(iSearchBox, rect,
			AKN_LAYOUT_WINDOW_find_pane);
			if (iSearchBox->IsVisible() && iListBox->IsVisible())
				{
				// The correct line position to use is 2, which corresponds
				// EABColumn in Avkon (not a public enumeration,
				// hence hard-coding used here)
				const TInt KSeparatorLinePos = 2;
				iSearchBox->SetLinePos(KSeparatorLinePos);
				}
			}
		else if (iListBox)
			{
			AknLayoutUtils::LayoutControl(iListBox, rect,
			AKN_LAYOUT_WINDOW_list_gen_pane(0));
			}
		iSearchBox->DrawDeferred();
		}

	if ( !aVisible )
		{
		iListBox->SetRect(rect);
		iListBox->DrawDeferred();
		}
	}
// ----------------------------------------------------
// CLmkAppSelectorImplBase::HandleScreenSizeChange()
// ----------------------------------------------------
//
void CLmkAppSelectorImplBase::HandleScreenSizeChange()
    {
    // Dummy implementation in base class.
	}

// ----------------------------------------------------
// CLmkAppLmSelectorImpl::AttachToAIWMenuL()
// ----------------------------------------------------
//
void CLmkAppSelectorImplBase::AttachToAIWMenuL(
		TInt /*aMenuResourceId*/,
		TInt /*aInterestResourceId*/)
	{
	 // Dummy function
	}

// ----------------------------------------------------
// CLmkAppSelectorImplBase::AttachInterestL()
// ----------------------------------------------------
//
void CLmkAppSelectorImplBase::AttachInterestL (
		TInt /*aInterestResourceId*/)
	{
	// Dummy function
	}


// ----------------------------------------------------
// CLmkAppLmSelectorImpl::InitializeMenuPaneL()
// ----------------------------------------------------
//
void CLmkAppSelectorImplBase::InitializeMenuPaneL(
		CEikMenuPane& /*aMenuPane */,
		TInt /* aMenuResourceId */)
	{
	// Dummy function
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::ExecuteAIWCallCmdL()
// -----------------------------------------------------------------------------
//
void CLmkAppSelectorImplBase::ExecuteAIWCallCmdL(
		 TInt /*aCommandId*/ )
	{
	// Dummy function
	}

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::IsThisLandmarkFieldDefined
// -----------------------------------------------------------------------------
//
TBool CLmkAppSelectorImplBase::IsLmWithThisFieldExistL ( TLmkAppCmdId  /*aFieldId*/)
    {
    // Dummy function
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::MarkedIndexes
// -----------------------------------------------------------------------------
//
EXPORT_C const CArrayFix<TInt>* CLmkAppSelectorImplBase::MarkedIndexes()
    {
      const CArrayFix<TInt>* markedIndexes = NULL;
      markedIndexes = iListBox->SelectionIndexes();
      return markedIndexes;
    }
// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::CurrentItemIndex
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkAppSelectorImplBase::CurrentItemIndex()
	{
	return iListBox->CurrentItemIndex();
	}
// -----------------------------------------------------------------------------
// CLmkAppLmSelectorImpl::SetMskObserver
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkAppSelectorImplBase::SetMskObserver(MLmkMskObserver* aMskObserver)
	{
	iMskObserver = aMskObserver;
	}

// -----------------------------------------------------------------------------
// CLmkAppSelectorImplBase::ServiceCmdByMenuCmd
// -----------------------------------------------------------------------------
//
TInt CLmkAppSelectorImplBase::ServiceCmdByMenuCmd( TInt /*aMenuCmd*/ )
	{
	//dummy function
	return 0;
	}

CAknSearchField* CLmkAppSelectorImplBase::FindBox()
	{
	return iSearchBox;
	}

#ifdef RD_SCALABLE_UI_V2
void CLmkAppSelectorImplBase::HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType )
	{
	//Update the listbox a sper the search criteria
	if (iFilter)
		{
		iFilter->HandleControlEventL(aControl, aEventType);
		}
	}
#endif //RD_SCALABLE_UI_V2
//  End of File
