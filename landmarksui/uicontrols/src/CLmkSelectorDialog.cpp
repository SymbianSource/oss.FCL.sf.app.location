/*
 * Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <StringLoader.h>
#include <lmkui.rsg>
#include <aknsfld.h>
#include <aknlists.h>
#include "CLmkSelectorDialog.h"
#include "MLmkNavigationObserver.h"
#include "CLmkLmItemListMemento.h"
#include "LmkConsts.h"
#include "CLmkListProviderBase.h"
#include <Landmarks.rsg>
#include <eikclbd.h>

_LIT(KWildCard, "*");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::CLmkSelectorDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSelectorDialog::CLmkSelectorDialog(
    TInt& aOpenedItemIndex,
    MDesCArray* aLbModel,
    MEikCommandObserver* aCmdObserver,
    MLmkListMemento* aMemento, CLmkListProviderBase* aListProvider)
    : CAknSelectionListDialog( aOpenedItemIndex, aLbModel, aCmdObserver )
    {
    iMemento = aMemento;
    iExited = EFalse;
    iListProvider = aListProvider;
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkSelectorDialog* CLmkSelectorDialog::NewL(
    TInt& aOpenedItemIndex,
    MDesCArray* aLbModel,
    TInt aMenuBarResourceId,
    MEikCommandObserver* aCmdObserver,
    MLmkListMemento* aMemento,
    CLmkListProviderBase* aListProvider)
    {
    CLmkSelectorDialog* self =
        new( ELeave ) CLmkSelectorDialog( aOpenedItemIndex, aLbModel,
                                          aCmdObserver, aMemento, aListProvider );

    CleanupStack::PushL( self );
    self->ConstructL( aMenuBarResourceId );
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkSelectorDialog::ConstructL
// ----------------------------------------------------
//
void CLmkSelectorDialog::ConstructL( TInt aMenuBarResourceId )
    {
    CAknSelectionListDialog::ConstructL( aMenuBarResourceId );
    }
// ----------------------------------------------------
// CLmkSelectorDialog::~CLmkSelectorDialog
// ----------------------------------------------------
//
CLmkSelectorDialog::~CLmkSelectorDialog()
    {
    }

// ----------------------------------------------------
// CLmkSelectorDialog::HasFindBox
// ----------------------------------------------------
//
TBool CLmkSelectorDialog::HasFindBox() const
    {
    if ( FindBox() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::NumberOfItems
// -----------------------------------------------------------------------------
//
TInt CLmkSelectorDialog::NumberOfItems() const
    {
    return ListBox()->Model()->NumberOfItems();
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::CurrentItemIndex
// -----------------------------------------------------------------------------
//
TInt CLmkSelectorDialog::CurrentItemIndex() const
    {
    return ListBox()->CurrentItemIndex();
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::SelectionIndexes
// -----------------------------------------------------------------------------
//
const CArrayFix<TInt>* CLmkSelectorDialog::SelectionIndexes() const
    {
    return ListBox()->SelectionIndexes();
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::SetHelpContext
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::SetHelpContext( TCoeHelpContext aContext )
    {
    iHelpContext = aContext;
    }

// ----------------------------------------------------
// CLmkSelectorDialog::ListBox
// ----------------------------------------------------
//
CEikListBox* CLmkSelectorDialog::ListBox() const
    {
    return CAknSelectionListDialog::ListBox();
    }

// ----------------------------------------------------
// CLmkSelectorDialog::SetNavigationObserver
// ----------------------------------------------------
//
void CLmkSelectorDialog::SetNavigationObserver(
    MLmkNavigationObserver& aObserver )
    {
    iNavigationObserver = &aObserver;
    }

// ----------------------------------------------------
// CLmkSelectorDialog::HandleListProviderEvent
// ----------------------------------------------------
//
void CLmkSelectorDialog::HandleListProviderEvent(
    TLmkListProviderEventType aEvent )
    {
    CEikListBox* listbox = ListBox();
    if ( listbox && ( aEvent == ELmkEventListReady || aEvent == ELmkEventItemAdditionComplete || aEvent == ELmkEventFindListReady ) )
        {
        TRAPD( error, listbox->HandleItemAdditionL() );
        if ( error != KErrNone )
            {
            listbox->Reset();
            listbox->DrawNow();
            }
        }
    if (ListBox()->Model()->NumberOfItems()== 0)
    	{
    	if (aEvent == ELmkEventFindListReady)
	    	{
	    	TRAP_IGNORE(MakeEmptyTextListBoxL(iDlgEmptyLabelPrimResource,iDlgEmptyLabelSecResource, ETrue));
	    	}
	    else
		    {
	       	TRAP_IGNORE(MakeEmptyTextListBoxL(iDlgEmptyLabelPrimResource,iDlgEmptyLabelSecResource));
		    }
    	}
    else
    	{
    	ShowFindBox(ETrue);
    	}
    }

// ----------------------------------------------------
// CLmkSelectorDialog::HandleListProviderError
// ----------------------------------------------------
//
void CLmkSelectorDialog::HandleListProviderError( TInt /*aError*/ )
    {
    CEikListBox* listbox = ListBox();
    if ( listbox )
        {
        listbox->Reset();
        listbox->DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::GetHelpContext
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iContext = iHelpContext.iContext;
    aContext.iMajor = iHelpContext.iMajor;
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CLmkSelectorDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType )
    {
    // Do not process navigation handling till UI gets from engine
	if (!ListBox())
		{
		return EKeyWasConsumed;
		}
	if (aKeyEvent.iRepeats == 0 || aKeyEvent.iCode == EKeyLeftArrow || aKeyEvent.iCode == EKeyRightArrow )
    	{
	    if (KeyEventForNavigationL(aKeyEvent, aType) == EKeyWasConsumed)
	        {
	        TryExitL(EAknSoftkeyCancel);
		    return EKeyWasConsumed;
	        }
	    if	(!FindBox()->IsVisible())
		    {
		    return 	EKeyWasConsumed;
		    }

    	}
    TKeyResponse keyResponce;
	keyResponce = CAknSelectionListDialog::OfferKeyEventL(aKeyEvent, aType);
	CEikButtonGroupContainer &buttonContainer = ButtonGroupContainer();
	if(!iExited)
		{
		UpdateMskL();
		}
	return keyResponce;
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::PreLayoutDynInitL()
    {
    CAknSelectionListDialog::PreLayoutDynInitL();
    if ( iNavigationObserver )
        {
        iNavigationObserver->HandleLaunchingEventL();
        }
    ((CEikColumnListBox*)ListBox())->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    }
// -----------------------------------------------------------------------------
// CLmkSelectorDialog::PostLayoutDynInitL
// -----------------------------------------------------------------------------
void CLmkSelectorDialog::PostLayoutDynInitL()
    {
    CAknSelectionListDialog::PostLayoutDynInitL();
    ShowFindBox(EFalse);
    if (iMemento)
	   	{
	   	EnableLeftSoftKey(EFalse);
	   	CLmkLmItemListMemento* memento =
							static_cast<CLmkLmItemListMemento*>(iMemento);
		/*
		When launching any application which uses selector dialog,
		initially, the find box is updated from the HandleListProviderEvent
		of the CLmkDlgSelectorImplBase class,but once the selector's model is updated
		dialog has to update the findbox by itself.
		the
		*/
		if(memento->IsModelAvailable())
		  	{
			iListProvider->StartSearchingLandmarksL(KWildCard,EFalse);
	    	if(ListBox()->Model()->NumberOfItems()== 0)
		    	{
		    	// Disable the find box
		    	ShowFindBox(EFalse);
		    	MakeEmptyTextListBoxL(iDlgEmptyLabelPrimResource ,iDlgEmptyLabelSecResource);
		    	}
	    	else
	    		{
	    		ShowFindBox(ETrue);
	    		}
	    	UpdateMskL();
		  	}
	   	}
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CLmkSelectorDialog::OkToExitL( TInt aButtonId )
    {
	if (!ListBox())
		{
		return EFalse;
		}

	if (NumberOfItems() <= 0 && aButtonId == EAknSoftkeyOk)
		{
		return EFalse;
		}
    TBool b = CAknSelectionListDialog::OkToExitL( aButtonId );
    if ( b && iNavigationObserver )
        {
        iNavigationObserver->HandleClosingEventL();
        }
    iExited = b;
    return b;
    }
// ----------------------------------------------------
// CLmkSelectorDialog::KeyEventForNavigationL
// ----------------------------------------------------
//
TKeyResponse CLmkSelectorDialog::KeyEventForNavigationL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( iNavigationObserver && ( aType == EEventKey ) &&
         ( aKeyEvent.iCode == EKeyLeftArrow ||
           aKeyEvent.iCode == EKeyRightArrow ) )
        {
        return iNavigationObserver->HandleNavigationEventL( aKeyEvent );
        }
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CLmkSelectorDialog::FindBox()
// ----------------------------------------------------
//
CAknSearchField* CLmkSelectorDialog::FindBox() const
    {
    return CAknSelectionListDialog::FindBox();
    }

// ----------------------------------------------------
// CLmkSelectorDialog::ShowFindBox()
// ----------------------------------------------------
//
void CLmkSelectorDialog::ShowFindBox(TBool aVisible)
	{

	CEikListBox* iListBox = ListBox();
	CAknSearchField* iSearchBox =  FindBox();
	const TRect rect(Rect());
	if (iSearchBox)
		{
		iSearchBox->SetFocus(aVisible);
		iSearchBox->MakeVisible(aVisible);
#ifdef RD_SCALABLE_UI_V2
	CAknFilteredTextListBoxModel* model =
	static_cast<CAknFilteredTextListBoxModel*> ( iListBox->Model() );

	iFilter = model->Filter();
	iSearchBox->SetObserver(this);
#endif// RD_SCALABLE_UI_V2
		static_cast<CAknColumnListBoxView*>
		(iListBox->View())->SetFindEmptyListState(aVisible);

		if (iListBox && iSearchBox)
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
		}

	if ( !aVisible )
		{
		iListBox->SetRect(rect);
		iListBox->DrawDeferred();
		}
	}
// ----------------------------------------------------
// CLmkSelectorDialog::MakeEmptyTextListBoxL
// ----------------------------------------------------
//
void CLmkSelectorDialog::MakeEmptyTextListBoxL( TInt aResourcePrimText,TInt aResourceSecText, TBool aRequestFromFindBox ) const
    {
        if (aRequestFromFindBox)
	    {
	    HBufC* text1 = StringLoader::LoadLC( R_LMK_EMPTY_NO_MATCH, iCoeEnv );
		TPtr ptr1 = text1->Des();
		ListBox()->View()->SetListEmptyTextL( ptr1 );
		CleanupStack::PopAndDestroy(text1);
	    }
	else if (aResourcePrimText && aResourceSecText)
    	{
    	_LIT(KNChar,"\n");
    	HBufC* text1 = StringLoader::LoadLC( aResourcePrimText, iCoeEnv );
    	HBufC* text2 = StringLoader::LoadLC( aResourceSecText, iCoeEnv );
    	TPtr ptr1 = text1->Des();
    	TPtr ptr2 = text2->Des();

    	HBufC* buffer = HBufC::NewLC(text1->Length() + text2->Length() + 2);
    	TPtr textFromResourceFile = buffer->Des();
    	textFromResourceFile.Copy(ptr1);
    	textFromResourceFile.Append(KNChar);
    	textFromResourceFile.Append(ptr2);

    	ListBox()->View()->SetListEmptyTextL( textFromResourceFile );
    	CleanupStack::PopAndDestroy(3);//buffer,text2,text1
    	}
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::HandleResourceChange()
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::HandleResourceChange (TInt aType)
	{
	if(aType==KEikDynamicLayoutVariantSwitch)
		{
		CAknSelectionListDialog::HandleResourceChange(aType);
		if (iMemento)
		   	{
		   	CLmkLmItemListMemento* memento =
								static_cast<CLmkLmItemListMemento*>(iMemento);
			if(memento->IsModelAvailable())
			  {
		    	if(ListBox()->Model()->NumberOfItems()== 0 && FindBox()->TextLength() <= 0 )
		    	{
	    	    ShowFindBox(EFalse);
	 	    	}
			  }
		   	}
		}
	else if (aType == KAknsMessageSkinChange )
	    {
        CAknSelectionListDialog::HandleResourceChange(aType);
	    }
	}


// -----------------------------------------------------------------------------
// CLmkSelectorDialog::UpdateMskL()
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::UpdateMskL()
	{
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	if (ListBox()->Model()->NumberOfItems()== 0)
		{
		cba->MakeCommandVisible(EAknSoftkeyOk, EFalse);
		cba->MakeCommandVisibleByPosition(CEikButtonGroupContainer::EMiddleSoftkeyPosition,EFalse);
		}
	else
		{
		cba->SetCommandSetL(iCbaResourceId);
		cba->MakeCommandVisible(EAknSoftkeyOk, ETrue);
		cba->MakeCommandVisibleByPosition(CEikButtonGroupContainer::EMiddleSoftkeyPosition,ETrue);
		}
	cba->DrawDeferred();
	}
// -----------------------------------------------------------------------------
// CLmkSelectorDialog::PrepareLC()
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::PrepareLC(TInt aDlgResourceId)
	{
	CAknSelectionListDialog::PrepareLC(aDlgResourceId);
	SetCbaResourceAndCommandId(aDlgResourceId);
	}

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::SetCbaResourceAndCommandId()
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::SetCbaResourceAndCommandId(TInt aaDlgResourceId)
    {
    switch (aaDlgResourceId)
        {
        case R_LMK_SELECTOR_OK_BACK_DLG:
        case R_LMK_SELECTOR_OK_BACK_MULTI_DLG:
            {
            iCbaResourceId = R_AVKON_SOFTKEYS_BACK;
            iMskCmdId = EAknSoftkeyOk;
            break;
            }
        case R_LMK_CATEGORY_SELECTOR_OK_CANCEL_MULTI_DLG:
            {
            iCbaResourceId = R_AVKON_SOFTKEYS_CANCEL;
            iMskCmdId = EAknSoftkeyDone;
            break;
            }
        case R_LMK_SELECTOR_OPEN_CANCEL_DLG:
            {
            iCbaResourceId = R_AVKON_SOFTKEYS_CANCEL;
            iMskCmdId = EAknSoftkeyOk;
            break;
            }
        case R_LMK_SELECTOR_OK_CANCEL_DLG:
        case R_LMK_SELECTOR_OK_CANCEL_MULTI_DLG:
            iCbaResourceId = R_AVKON_SOFTKEYS_CANCEL;
            iMskCmdId = EAknSoftkeyOk;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::SetDlgEmptyResource()
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::SetDlgEmptyResource(TInt aEmptyPrimLabel ,TInt aEmptySecLabel)
	{
	iDlgEmptyLabelPrimResource = aEmptyPrimLabel;
	iDlgEmptyLabelSecResource = aEmptySecLabel;
	}

// -----------------------------------------------------------------------------
// CLmkSelectorDialog::EnableLeftSoftKey()
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::EnableLeftSoftKey(TBool aEnable)
	{
	CEikButtonGroupContainer &pButtonContainer = ButtonGroupContainer();
	TInt id = pButtonContainer.ButtonGroup()->CommandId(0);
	pButtonContainer.MakeCommandVisible(id,aEnable);
	}

#ifdef RD_SCALABLE_UI_V2
// -----------------------------------------------------------------------------
// CLmkSelectorDialog::HandleControlEventL()
// -----------------------------------------------------------------------------
//
void CLmkSelectorDialog::HandleControlEventL( CCoeControl* aControl,TCoeEvent aEventType )
	{
	//Update the listbox as per the search criteria
	if (!iListProvider)
		{
		if( iFilter )
			{
			iFilter->HandleControlEventL(aControl, aEventType);
			UpdateMskL();
			}
		return;
		}

	CEikButtonGroupContainer &buttonContainer = ButtonGroupContainer();
	//Update the msk depending upon the search results while using the virtual keyboard.
	if( aEventType == EEventStateChanged)
		{
		if ( aControl == FindBox() )
			{
			if(FindBox()->IsVisible())
				{
				if( NumberOfItems() == 0 )
					{
					buttonContainer.MakeCommandVisible(EAknSoftkeyOk, EFalse);
					}
				else
					{
					buttonContainer.MakeCommandVisible(EAknSoftkeyOk, ETrue);
					}
				UpdateMskL();

				// Fetch filter from search field
				TInt searchTextLength = FindBox()->TextLength();
				HBufC* filterBuf = HBufC::NewLC(searchTextLength+1);
				TPtr filter = filterBuf->Des();
				FindBox()->GetSearchText(filter);

				// If filter has changed repopulate list box. (It is necessary to check
				// if filter has changed since each filter change generates several
				// EEventStateChanged events for find box)

				if (iListProvider)
					{
					filter.Append(KWildCard);
					if ( iListProvider->StartSearchingLandmarksL(filter,EFalse) == EFalse)
						{
						//Update the listbox as per the search criteria
						if( iFilter )
							{
							iFilter->HandleControlEventL(aControl, aEventType);
							}
						}
					}
			  	CleanupStack::PopAndDestroy(filterBuf);
				}
			}
		}
	}
#endif //RD_SCALABLE_UI_V2
//  End of File
