/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container class for Events Management UI startup view.
*
*/


// SYSTEM INCLUDES
#include <avkon.hrh>
#include <barsread.h> 
#include <aknlists.h>
#include <AknIconArray.h>		// Icon Array
#include <eikclbd.h>			// Column Listbox
#include <gulicon.h>			// Gul Icon
#include <barsread.h>   		// TResourceReader
#include <eikspane.h>       // Status pane
#include <akntitle.h>       // CAknTitlePane
#include <StringLoader.h>   // String Loader
#include <evtmgmtui.rsg>
#include <csxhelp/loc.hlp.hrh>
#include <AknUtils.h>
#include <evtmgmtui.mbg>
#include <featmgr.h>
#include <data_caging_path_literals.hrh>
#include <AknUtils.h> 
#include <eikmenup.h> 
#include <AknsConstants.h> 		// Skin Ids
#include <AknIconUtils.h>
#include <AknLayout.lag>          // AVKON LAF layouts
#include <AknLayoutDef.h>
#include <aknsfld.h> // For SearchField

// USER INCLUDES
#include "evtmgmtui.hrh"
#include "evtmgmtuiuid.hrh"
#include "evtmgmtuiappui.h"
#include "evtmgmtuimodel.h"
#include "evtmgmtuistartupcontainer.h"
#include "evtmgmtuilistbox.h"
#include "evtmgmtuilistboxmodel.h"
#include "evtdebug.h"

// CONSTANT DEFINITIONS
const TInt KIconArrayGranularity = 5; // The number of default icons
const TInt KFindBoxTextLength = 32;	// Find Box text length.
const TInt KMinCountForFilter = 2;
_LIT( KEvtMgmtUiIconFileName, "evtmgmtui.mif" );
 
// ----------------- Member funtions for CEvtMgmtUiStartupContainer class ------------------

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::CEvtMgmtUiStartupContainer
// ---------------------------------------------------------------------------
//
CEvtMgmtUiStartupContainer::CEvtMgmtUiStartupContainer( MEvtMgmtUiCmdHdlr& aCmdHandler )
									:iCmdHandler(aCmdHandler)
    {
    }


// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::~CEvtMgmtUiStartupContainer
// ---------------------------------------------------------------------------
//
CEvtMgmtUiStartupContainer::~CEvtMgmtUiStartupContainer()
    {
    // Delete the List box
	delete iListBox;
	
	delete iFindBox;
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::NewL
// ---------------------------------------------------------------------------
//
CEvtMgmtUiStartupContainer* CEvtMgmtUiStartupContainer::NewL( 
                                    const TRect&	aRect, CEvtMgmtUiModel* aModel,
                                    MEvtMgmtUiCmdHdlr& aCmdHandler )     
    {
    CEvtMgmtUiStartupContainer* self = new ( ELeave ) CEvtMgmtUiStartupContainer( aCmdHandler );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aModel );
    CleanupStack::Pop();    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::CEvtMgmtUiStartupContainer
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::ConstructL( const TRect& 	aRect, CEvtMgmtUiModel* aModel )
    {
    // This is the cheif control for this application. This has to be made a 
    // Window owning control
    CreateWindowL();
    
    // Create the List box
    EVTUIDEBUG( "Listbox creation" );
    CreateListboxL(aModel); 
    CreateFindBoxL();
    
    // Check for FindBox Visiblity
    HandleFindBoxVisibility();
    
    // Get the Help context
    iContextName = KHLP_LOC_EVENTS_VIEW();
    
    // Set Title
    MakeTitleL();
            
    SetRect(aRect);
    ActivateL();
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupContainer::GetHelpContext
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
    aContext.iContext = iContextName;
    aContext.iMajor = TUid::Uid( KEvtMgmtUiUid );	
	}

// --------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::FocusChanged
// --------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::FocusChanged( TDrawNow aDrawNow )
    {
    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused(), aDrawNow );
        }
        
    if ( iFindBox && iFindBox->IsVisible() )
        {
        iFindBox->SetFocus( IsFocused(), aDrawNow );
        if ( aDrawNow == EDrawNow && IsFocused() )
            {
            iFindBox->DrawDeferred();
            }
        }
    }
		
// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::SizeChanged()
    {
    EVTUIDEBUG("+ CEvtMgmtUiStartupContainer::SizeChanged()" );
	const TRect rect( Rect() );
    TAknLayoutRect controlRect;

    // Since iFindBox->IsVisible() returns EFalse if Adaptive Chars are shown, 
    // We check on FilteredNumberOfItems meaning the FindBox should be visible when 
    // FilteredNumberOfItems is greater than 1
    if( iListBox && iFindBox && 
            ( iFindBox->IsVisible() || iListBox->Model()->Filter()->FilteredNumberOfItems() > 1 ) )
        {
        controlRect.LayoutRect(rect, AknLayout::list_gen_pane( 1 ) );
        iListBox->SetRect( controlRect.Rect() );
        controlRect.LayoutRect(rect, AknLayout::find_pane() );
        iFindBox->SetRect( controlRect.Rect() );  
        }
    else if( iListBox )
        {
        controlRect.LayoutRect(rect, AknLayout::list_gen_pane( 0 ) );
        iListBox->SetRect( controlRect.Rect() );
        }
    EVTUIDEBUG("- CEvtMgmtUiStartupContainer::SizeChanged()" );
    }

// ---------------------------------------------------------------------------
// TInt CEvtMgmtUiStartupContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiStartupContainer::CountComponentControls() const
    {
    TInt count = 0;
    CCoeControl* controls[] = { iListBox, iFindBox };

    for ( TUint i = 0; i < sizeof( controls ) / sizeof( CCoeControl* ); i++ )
    {
        if ( controls[i] ) // Test for valid controls
        {
        count++;
        }
    }

    return count;
    }

// ---------------------------------------------------------------------------
// CCoeControl* CEvtMgmtUiStartupContainer::CEvtMgmtUiStartupContainer
// ---------------------------------------------------------------------------
//
CCoeControl* CEvtMgmtUiStartupContainer::ComponentControl(TInt aIndex ) const
    {
    CCoeControl* controls[] = { iListBox,
                                iFindBox};

    for ( TUint i = 0; i < sizeof( controls ) / sizeof( CCoeControl* ); i++ )
    {
        // Index is decremented only if control is not NULL
        if ( controls[i] && aIndex-- == 0 ) 
        {
        return controls[i];
        }
    }

    return NULL;
    }

// ---------------------------------------------------------------------------
// TKeyResponse CEvtMgmtUiStartupContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//    
TKeyResponse CEvtMgmtUiStartupContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
								  				               TEventCode aType )
    {
    TKeyResponse keyresponse;
    
    TBool needRefresh = EFalse;
    TInt keyConsumed = iFindBox->TextLength();
	
	// Key is first offered to the Find box based on the key press.
    keyresponse = AknFind::HandleFindOfferKeyEventL(aKeyEvent, aType,
                        this, iListBox, iFindBox, EFalse, needRefresh);
    
    // Handle the ListBox Filter count changed event
    iCmdHandler.HandleCmdL( MEvtMgmtUiCmdHdlr::EListBoxFilterCountChanged ); 
    
	// This is to avoid the Mark for the First item of the list box.
	// We reset the modifier attribute
    TKeyEvent& keyEvent = const_cast<TKeyEvent&>(aKeyEvent);
    TInt index = CurrentItemIndex();
    if(index == 0 || 
       (index == iListBox->Model()->NumberOfItems() - 1) && aKeyEvent.iScanCode == EStdKeyDownArrow ||
       (index == 1) && aKeyEvent.iScanCode == EStdKeyUpArrow )
        {
        if(keyEvent.iModifiers & EModifierShift)
            keyEvent.iModifiers&= ~EModifierShift;
        if(keyEvent.iModifiers & EModifierLeftShift)
            keyEvent.iModifiers&= ~EModifierLeftShift;
        if(keyEvent.iModifiers & EModifierRightShift)
            keyEvent.iModifiers&= ~EModifierRightShift;
        if(keyEvent.iModifiers & EModifierCtrl)
            keyEvent.iModifiers&= ~EModifierCtrl;
        if(keyEvent.iModifiers & EModifierRightCtrl)
            keyEvent.iModifiers&= ~EModifierRightCtrl;
        }
    
	// Now handle the Key Event based on the Key type
    switch ( aKeyEvent.iScanCode )
        {            
        case EStdKeyUpArrow:
        case EStdKeyDownArrow:
            {
    	    keyresponse = iListBox->OfferKeyEventL( aKeyEvent, aType );
            iCmdHandler.HandleCmdL( MEvtMgmtUiCmdHdlr::EListBoxItemFocused );    
            return keyresponse;        	
            }
        case EStdKeyBackspace:
        case EStdKeyDelete:
            {
            if(aType == EEventKey && !keyConsumed )
                {
                keyresponse = iListBox->OfferKeyEventL( aKeyEvent, aType );
                iCmdHandler.HandleCmdL( MEvtMgmtUiCmdHdlr::EListBoxItemDeleted );
                }
            return keyresponse;               
            }
        case EStdKeyHash:
			{
            if(!index)
                {
                keyresponse = EKeyWasConsumed;
                }
            else
                {
                TBool shiftKeyPressed = ( aKeyEvent.iModifiers & EModifierShift );
                keyresponse = iListBox->OfferKeyEventL( aKeyEvent, aType );
                if( aType == EEventKeyUp && ( EKeyWasConsumed == keyresponse || !shiftKeyPressed ) )
                    iCmdHandler.HandleCmdL( MEvtMgmtUiCmdHdlr::EListBoxItemFocused );  
                }
            return keyresponse; 
			}
        default:
            {
            break;  
            }
        }
    // Now it's List box's job to process the key event
    return iListBox->OfferKeyEventL( aKeyEvent, aType ); 
    }   

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupContainer::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::HandleResourceChange( TInt aType )
	{
    // Pass the event to the base class
    CCoeControl::HandleResourceChange(aType);
    
    switch( aType )
        {
    	case KAknsMessageSkinChange:
        	{
        	TRAPD( error, UpdateIconsL() );
        	if ( error != KErrNone )
            	{
            	iEikonEnv->HandleError( error );
            	}                 
        	DrawDeferred();
        	   	
        	break;
        	}
        // Dynamic Layout switch
        case KEikDynamicLayoutVariantSwitch:
            {
            // Handle the screen change event
            // Trap and Ignore the error
            TRAP_IGNORE(
                iCmdHandler.HandleCmdL( MEvtMgmtUiCmdHdlr::EScreenSizeChanged ));
            break;
            }
        default:
            {
            break;
            }
        }
	}

// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupContainer::HandleListBoxEventL
// ---------------------------------------------------------------------------
//	
void CEvtMgmtUiStartupContainer::HandleListBoxEventL( 
                                    CEikListBox*      /* aListBox */, 
                          		    TListBoxEvent      aEventType  )
	{
    switch (aEventType)
        {
        // List box Item Selection
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {            
            iCmdHandler.HandleCmdL( MEvtMgmtUiCmdHdlr::EListBoxItemDoubleClicked );
            break;  
            }
        case EEventItemClicked:
            {
            iCmdHandler.HandleCmdL( MEvtMgmtUiCmdHdlr::EListBoxItemFocused);
            break;
            }
        default:
           break;
        }	
	}
	
// ---------------------------------------------------------------------------
// void CEvtMgmtUiStartupContainer::CreateListboxL
// ---------------------------------------------------------------------------
// 	
void CEvtMgmtUiStartupContainer::CreateListboxL( CEvtMgmtUiModel* aModel )
    {	
    EVTUIDEBUG("+ CEvtMgmtUiStartupContainer::CreateListboxL()" );
	// Create the List box
	iListBox = new ( ELeave ) CEvtMgmtUiListBox();
    iListBox->ConstructL( this, EAknListBoxStylusMarkableList );
    iListBox->SetContainerWindowL( *this ); 
    iListBox->SetListBoxObserver( this );
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
    													 CEikScrollBarFrame::EAuto ); 
 
    UpdateIconsL();
     
	// Create the Listbox model
    CEvtMgmtUiListBoxModel* model = static_cast<CEvtMgmtUiListBoxModel*>(iListBox->Model());
    model->SetItemTextArray( aModel );	
    model->SetOwnershipType( ELbmDoesNotOwnItemArray ); 
    model->SetItemTextArrayToFilterModel();
   								
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
        								
    iListBox->ActivateL();    
    
    EVTUIDEBUG("- CEvtMgmtUiStartupContainer::CreateListboxL()" );
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::CreateFindBoxL()
//
// Creates the member iFindBox
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::CreateFindBoxL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiStartupContainer::CreateFindBoxL()" );

    // Create the findbox.
    iFindBox = CAknSearchField::NewL( *this, CAknSearchField::EAdaptiveSearch, 
        NULL, KFindBoxTextLength );
    
    // Set default input mode to Katakana if current UI language is Japanese
    if (FeatureManager::FeatureSupported(KFeatureIdJapanese) &&
        (User::Language() == ELangJapanese))
        {
        iFindBox->Editor().SetAknEditorInputMode(EAknEditorKatakanaInputMode);
        }
    else
        {
        iFindBox->Editor().SetAknEditorCurrentInputMode(EAknEditorTextInputMode);
        }

    // Set Column mask
    iFindBox->SetListColumnFilterFlags(2); // This is set to search only "Subject" column

    STATIC_CAST(CAknFilteredTextListBoxModel*, iListBox->Model())->CreateFilterL(iListBox,iFindBox);
    STATIC_CAST(CAknFilteredTextListBoxModel*, iListBox->Model())->Filter()->SetParentControl( this );
    iListBox->Model()->SetFindToFilterModel(iFindBox);
        
    iFindBox->ActivateL();    
    
    EVTUIDEBUG("- CEvtMgmtUiStartupContainer::CreateFindBoxL()" );
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::MakeFindBoxVisible()
//
// Deletes the member iFindBox
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::HandleFindBoxVisibility()
    {
    if( iListBox && iFindBox )
        {
        if(iListBox->Model()->Filter()->FilteredNumberOfItems() < KMinCountForFilter )
            {
            iFindBox->SetFocusing( EFalse );
            iFindBox->MakeVisible( EFalse );
            iListBox->SetFocusing( ETrue );
            }
        else
            {
            iFindBox->SetFocusing( ETrue );
            iFindBox->MakeVisible( ETrue );
            iListBox->SetFocusing( EFalse );
            }
        
        SizeChanged();
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::UpdateIconsL
// -----------------------------------------------------------------------------
void CEvtMgmtUiStartupContainer::UpdateIconsL()
    {
    EVTUIDEBUG( "+ CEvtMgmtUiStartupContainer::UpdateIconsL()" );
    
    CAknIconArray* icons = new( ELeave ) CAknIconArray( KIconArrayGranularity );
    CleanupStack::PushL( icons );

	TFileName *iconFile = IconFileNameLC();

//Create new event
    AddIconL( *icons, 
    		  *iconFile,
    		  KAknsIIDQgnPropLocevNew, 
    		  EMbmEvtmgmtuiQgn_prop_locev_new, EMbmEvtmgmtuiQgn_prop_locev_new_mask);
    
    //active
     AddIconL( *icons, 
             *iconFile,
             KAknsIIDQgnPropLocevTrigger, 
             EMbmEvtmgmtuiQgn_prop_locev_trigger, EMbmEvtmgmtuiQgn_prop_locev_trigger_mask);
            
    //draft            
    AddIconL( *icons, 
             *iconFile,
             KAknsIIDQgnPropLocevDraft, 
             EMbmEvtmgmtuiQgn_prop_locev_draft, EMbmEvtmgmtuiQgn_prop_locev_draft_mask);
       
   //completed       
    AddIconL( *icons, 
                *iconFile,
                KAknsIIDQgnPropLocevCompleted, 
                EMbmEvtmgmtuiQgn_prop_locev_completed, EMbmEvtmgmtuiQgn_prop_locev_completed_mask);
               
  // repeat                
	AddColorIconL( *icons, 
                *iconFile,
                KAknsIIDQgnIndiRepeatAdd, 
                EMbmEvtmgmtuiQgn_indi_repeat_add, EMbmEvtmgmtuiQgn_indi_repeat_add_mask);
                
    CleanupStack::PopAndDestroy(iconFile);

    //get old icons and delete those
    CAknIconArray* oldIcons = static_cast<CAknIconArray*>(
        iListBox->ItemDrawer()->ColumnData()->IconArray() );    
    delete oldIcons;

    //set new icons to array
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );  
    CleanupStack::Pop(icons);
    
    EVTUIDEBUG( "- CEvtMgmtUiStartupContainer::UpdateIconsL()" );
        
    }
    
// -----------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::AddIconL
// -----------------------------------------------------------------------------
void CEvtMgmtUiStartupContainer::AddIconL( CAknIconArray&  aIcons,
                                           const TDesC&    aIconFileWithPath,
                                           TAknsItemID     aSkinID,
                                           TInt            aIconGraphicsIndex,
                                           TInt            aIconGraphicsMaskIndex )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    EVTUIDEBUG( "AknsUtils::SkinInstance" );
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    TRAPD( err, AknsUtils::CreateIconL( skin,
                            aSkinID,
                            bitmap,
                            bitmapMask,
                            aIconFileWithPath,
                            aIconGraphicsIndex,
                            aIconGraphicsMaskIndex ) );

	
    EVTUIDEBUG1( "AknsUtils::CreateIconL - %d", err );
    if(err != KErrNone)
	    {
	    TRAP( err, AknIconUtils::CreateIconL( bitmap,
	                            bitmapMask,
	                            aIconFileWithPath,
	                            aIconGraphicsIndex,
	                            aIconGraphicsMaskIndex ) );
    	EVTUIDEBUG1( "AknIconUtils::CreateIconL - %d", err );
	    }
    User::LeaveIfError( err );
    
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( bitmapMask );
    
    CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );
    
    // Remove the Bitmap and the Bitmap Icon from the Clean up stack since 
    // the ownership is taken over by the icon
    CleanupStack::Pop( bitmapMask );
    CleanupStack::Pop( bitmap ); 
    
    CleanupStack::PushL( icon );
    aIcons.AppendL( icon );
    CleanupStack::Pop( icon ); // icon
    }
    
// -----------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::AddColorIconL
// -----------------------------------------------------------------------------
void CEvtMgmtUiStartupContainer::AddColorIconL( CAknIconArray&  aIcons,
                                           const TDesC&    aIconFileWithPath,
                                           TAknsItemID     aSkinID,
                                           TInt            aIconGraphicsIndex,
                                           TInt            aIconGraphicsMaskIndex )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    // Repeat icon. Color skinned with day view icon color 
    // FIXME: highlight coloring in lists doesn't work for icons.
    // When Avkon suplies some method to do that, change these 
    TAknsItemID colorGroup = KAknsIIDQsnIconColors;
    TInt colorIndex = EAknsCIQsnIconColorsCG19;
    TRgb nonSkinColor = AKN_LAF_COLOR(215);
    
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    AknsUtils::CreateColorIconLC( skin,
                            aSkinID,
                            colorGroup,
                            colorIndex,
                            bitmap,
                            bitmapMask,
                            aIconFileWithPath,
                            aIconGraphicsIndex,
                            aIconGraphicsMaskIndex,
                            nonSkinColor );
    
    CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );
    
    // Remove the Bitmap and the Bitmap Icon from the Clean up stack since 
    // the ownership is taken over by the icon
    CleanupStack::Pop( bitmapMask );
    CleanupStack::Pop( bitmap ); 
    
    CleanupStack::PushL( icon );
    aIcons.AppendL( icon );
    CleanupStack::Pop( icon ); // icon
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::IconFileNameLC
// -----------------------------------------------------------------------------
TFileName* CEvtMgmtUiStartupContainer::IconFileNameLC() const
    {
    TFileName* fileName = new(ELeave) TFileName;
    CleanupStack::PushL( fileName );
    
	TFileName* tmpName = new(ELeave) TFileName;
    CleanupStack::PushL( tmpName );
	
    tmpName->Append( KDC_BITMAP_DIR );
    tmpName->Append( KEvtMgmtUiIconFileName );

    CEikAppUi *appUi = ( CEikAppUi *)( CEikonEnv::Static()->AppUi());
    TFileName appDrive = appUi->Application()->AppFullName();
    
    TParse parse;
    User::LeaveIfError( parse.Set( appDrive, NULL, NULL ) );
    User::LeaveIfError( parse.Set( parse.Drive(), tmpName, NULL ) );
    
	fileName->Copy(parse.FullName());

    CleanupStack::PopAndDestroy( tmpName );

    return fileName;
    }    
// ---------------------------------------------------------------------------
// CEvtMgmtUiModel::UpdateListBoxL()
// It will update listbox
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::UpdateListBoxL(TInt aSelectedItemIndex)
    {   
    iListBox->Model()->Filter()->ResetFilteringL();
    iFindBox->ResetL();
    iListBox->ClearSelection();
    
    TInt count = iListBox->Model()->NumberOfItems();
    if( iPrevCount > count )
		{
        iListBox->HandleItemRemovalL();
		iListBox->DrawNow();
		}
    else
		{
        iListBox->HandleItemAdditionL();
		}
            
    iPrevCount = count;
               
    if( aSelectedItemIndex >= count )
        aSelectedItemIndex = count-1;
    if(aSelectedItemIndex<0)
          aSelectedItemIndex = 0;

    iListBox->SetCurrentItemIndexAndDraw( aSelectedItemIndex );   
    
    // Check for FindBox Visiblity
    HandleFindBoxVisibility();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::CurrentItemIndex()
// It will return the index of the selected item 
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiStartupContainer::CurrentItemIndex()
    {
        return iListBox->Model()->Filter()->FilteredItemIndex( iListBox->CurrentItemIndex() );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::HandleMarkCmd()
// It will handle mark functionality.
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::HandleMarkCmdL( TInt aIndex, TBool aIsMarked )
    {
    CListBoxView* listBoxView = iListBox->View();
    CAknFilteredTextListBoxModel* model = iListBox->Model();
    if( aIndex != KErrNotFound )
        {
        TInt visibleIndex = model->Filter()->VisibleItemIndex(aIndex);
        if (aIsMarked)
             {
             listBoxView->SelectItemL(visibleIndex);
             }
        else
            {
            listBoxView->DeselectItem(visibleIndex);
            }
        }
    else
        {
        TInt itemCount = model->NumberOfItems();
        for (TUint i = 1; i < itemCount; i++)
            {
            TInt visibleIndex = model->Filter()->VisibleItemIndex(i);
            if (aIsMarked)
                {
                listBoxView->SelectItemL(i);
                }
            else
                {
                listBoxView->DeselectItem(i);
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::HandleMarkableListDynInitMenuPane()
// It will display mark menu items dynamically
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::HandleMarkableListDynInitMenuPane( TInt /*ResourceId */, CEikMenuPane *aMenuPane )
    {
    TInt pos;
    if( !aMenuPane->MenuItemExists(EAknCmdMark, pos) )
        return;
    
    TInt markItems = iListBox->SelectionIndexes()->Count();
    if( iListBox->CurrentItemIndex() == 0 ) //if 'Create New Event'
        {
        aMenuPane->SetItemDimmed( EAknCmdMark, ETrue );
        aMenuPane->SetItemDimmed( EAknCmdUnmark, ETrue );        
        }
    else
        {
        if(iListBox->View()->ItemIsSelected(iListBox->CurrentItemIndex()) )
            {
            aMenuPane->SetItemDimmed( EAknCmdMark, ETrue );
            aMenuPane->SetItemDimmed( EAknCmdUnmark, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EAknCmdMark, EFalse );
            aMenuPane->SetItemDimmed( EAknCmdUnmark, ETrue );
            }       
        }
    if( markItems < 1 )// if no item is selected
        {
        aMenuPane->SetItemDimmed( EAknUnmarkAll, ETrue );
        }
    else
        {
       aMenuPane->SetItemDimmed( EAknUnmarkAll, EFalse );
        }
    
    if( markItems == iListBox->Model()->NumberOfItems()-1 )// if all items are selected
        {
        aMenuPane->SetItemDimmed( EAknMarkAll, ETrue );
        }
    else
        {
        aMenuPane->SetItemDimmed( EAknMarkAll, EFalse );
        }   
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::NumberOfMarkedItems()
// It will return count of marked items
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiStartupContainer::NumberOfMarkedItems()
    {
    return iListBox->SelectionIndexes()->Count();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::MarkItems()
// It will return marked items
// ---------------------------------------------------------------------------
//
const CArrayFix<TInt>* CEvtMgmtUiStartupContainer::MarkItems()
    {
    return iListBox->SelectionIndexes();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::FilteredIndex()
// It will return marked items
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiStartupContainer::FilteredIndex(TInt aIndex)
    {
    return iListBox->Model()->Filter()->FilteredItemIndex( aIndex );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::FilteredCount()
// It will return count of marked items
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiStartupContainer::FilteredCount()
    {
    return iListBox->Model()->Filter()->FilteredNumberOfItems();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiStartupContainer::MakeTitleL()
// Set the Title Text
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiStartupContainer::MakeTitleL()
    {
    // Obtain the title from the Status Pane
    CAknTitlePane* title = static_cast<CAknTitlePane*>( iEikonEnv->AppUiFactory()->
    	StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
       
    // Set the Title's buffer    
    HBufC* buf = StringLoader::LoadL( R_EVTUI_TITLE );    
    title->SetText( buf ); // Takes ownership of buf 
    }    
// End of File
