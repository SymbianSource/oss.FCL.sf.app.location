/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides blid main view's container class methods.
*
*/


// INCLUDE FILES
#include <aknview.h> 
#include <aknnavide.h>
#include <eikmenub.h>
#include <aknViewAppUi.h> 
#include <aknlists.h>   // CAknSingleLargeStyleListBox
#include <Blid.rsg>
#include <AknsConstants.h>
#include <AknIconArray.h>
#include <aknnavide.h>
#include <AknIconUtils.h>
#include <avkon.mbg>
#include <avkon.hrh>
#include <gulicon.h>
#include <eikclbd.h>
#include <AknDef.h>
#include <csxhelp/blid.hlp.hrh>

#include "CBlidSavedWayptsContainer.h"
#include "Blid.hrh"
#include "CBlidSavedWayptsLBModel.h"

// CONSTANTS
#ifdef _DEBUG
_LIT( KPanicText, "CBlidSavedWayptsContainer" );

enum TPanicCode
    {
    KBlidNullPointer = 1
    };
#endif
// Unnamed namespace for local definitions
namespace {
const TInt KViewListboxGranularity = 2; // list box granularity
}  // namespace


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidSavedWayptsContainer::NewL
// ----------------------------------------------------------------------------
//
CBlidSavedWayptsContainer* CBlidSavedWayptsContainer::NewL(    
                            MKeyProcessor& aKeyProcessor,
                            MObjectProvider* aParent,
		                    const TRect& aRect,
                            MBlidLocation* aLocation,
                            MBlidSettings* aSettings,
                            MBlidRouter* aRouter,
                            CBlidBaseView& aView)
	{
	CBlidSavedWayptsContainer* self =
		new (ELeave) CBlidSavedWayptsContainer( aKeyProcessor, aView );
	CleanupStack::PushL(self);
	self->SetMopParent( aParent );
	self->ConstructL( aRect, aLocation, aSettings,aRouter );
    CleanupStack::Pop( self ); // self
	return self;
	}

// ----------------------------------------------------------------------------
// CBlidSavedWayptsContainer::CBlidSavedWayptsContainer
// ----------------------------------------------------------------------------
//
CBlidSavedWayptsContainer::CBlidSavedWayptsContainer( MKeyProcessor& aKeyProcessor,
                                                      CBlidBaseView& aView ) 
    :CBlidBaseContainer( aKeyProcessor, KBLID_HLP_WAYPOINTS ), iView(aView)
    {
    }

// ---------------------------------------------------------
// CBlidSavedWayptsContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CBlidSavedWayptsContainer::ConstructL( const TRect& aRect, 
                                      MBlidLocation* aLocation,
                                      MBlidSettings* aSettings,MBlidRouter* aRouter )
    {
    CreateWindowL();
    SetBlank();

    iDecoratedTabGroup = CreateNaviPaneL( EFalse );
    ActivateTabL( *iDecoratedTabGroup, EBlidSavedWayptsView );

    CreateListboxL(aLocation, aSettings,aRouter);

    MakeTitleL(R_BLID_TITLE_WAYPOINTS);
    SetRect(aRect);
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsContainer::~CBlidSavedWayptsContainer
// ----------------------------------------------------------------------------
//
CBlidSavedWayptsContainer::~CBlidSavedWayptsContainer()
    {
    delete iDecoratedTabGroup;
    delete iListBox;
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsContainer::UpdateL
// ----------------------------------------------------------------------------
//
void CBlidSavedWayptsContainer::UpdateL()
    {
    __ASSERT_DEBUG( iListBox, User::Panic( KPanicText, KBlidNullPointer ) );
    iListBox->DrawNow();
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsContainer::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CBlidSavedWayptsContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType)
    {    
    if ( aKeyEvent.iCode == EKeyOK )
		{
		TBool shiftKeyPressed = (aKeyEvent.iModifiers & EModifierShift);
		if(shiftKeyPressed)
            {
            iListBox->OfferKeyEventL( aKeyEvent, aType);
            return EKeyWasConsumed;
            }
        CEikMenuBar* menubar = static_cast<CAknViewAppUi*>
            (iAvkonAppUi)->View(TUid::Uid( EBlidSavedWayptsView ))->MenuBar();
        if ( menubar )
            {
            menubar->SetMenuTitleResourceId(R_BLID_SAVEDWAYPTS_VIEW_OK_MENUBAR); 

            menubar->StopDisplayingMenuBar();
            TRAPD(err,menubar->TryDisplayMenuBarL());                        
            User::LeaveIfError(err);
            menubar->SetMenuTitleResourceId(R_BLID_SAVEDWAYPTS_MENUBAR); 
            }
        
		return EKeyWasConsumed;
		}
    else if( aKeyEvent.iCode == EKeyBackspace )
        {
        static_cast<CAknViewAppUi*>
            (iAvkonAppUi)->View(TUid::Uid( EBlidSavedWayptsView ))->HandleCommandL(EBlidCmdDeleteWaypt);
        }
    return CBlidBaseContainer::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CBlidSavedWayptsContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CBlidSavedWayptsContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CBlidSavedWayptsContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CBlidSavedWayptsContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CBlidSavedWayptsContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CBlidSavedWayptsContainer::ComponentControl(TInt /*aIndex*/) const
    {    
    return iListBox;
    }

// ---------------------------------------------------------
// CBlidSavedWayptsContainer::CreateListboxL
// ---------------------------------------------------------
//
void CBlidSavedWayptsContainer::CreateListboxL( MBlidLocation* aLocation,
                                           MBlidSettings* aSettings,MBlidRouter* aRouter )
    {
    delete iListBox;
    iListBox = NULL;    
    iListBox = new(ELeave) CAknSingleGraphicStyleListBox();
    iListBox->SetContainerWindowL(*this);
    
    #ifndef RD_SCALABLE_UI_V2
    iListBox->ConstructL( this, EAknGenericListBoxFlags
                                | EAknListBoxMarkableList);
    #else
    iListBox->ConstructL( this, EAknGenericListBoxFlags
                                | EAknListBoxStylusMarkableList);
    #endif                                    

    MDesCArray* model = CBlidSavedWayptsLBModel::NewLC( aLocation, aSettings,aRouter );
    // listbox takes the ownership
    iListBox->Model()->SetItemTextArray( model ); 
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop(); // model 
	
	 UpdateIconsL();
	 
    // Make scroll arrows
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
        CEikScrollBarFrame::EAuto,CEikScrollBarFrame::EAuto );
    MakeEmptyTextListBoxL(R_BLID_NO_WAYPOINTS);

    iListBox->ActivateL();
    }

// -----------------------------------------------------------------------------
// CBlidSavedWayptsContainer::UpdateIconsL
// -----------------------------------------------------------------------------
//
void CBlidSavedWayptsContainer::UpdateIconsL()
    {
    TAknsItemID skinId = KAknsIIDNone;

    CAknIconArray* icons = new( ELeave ) CAknIconArray( 
    											KViewListboxGranularity );
    CleanupStack::PushL( icons );

    TFileName* aknIconFile = new (ELeave) TFileName;
	CleanupStack::PushL(aknIconFile);
    *aknIconFile = AknIconUtils::AvkonIconFileName();

    AddIconL( icons, skinId, *aknIconFile, EMbmAvkonQgn_indi_marked_add , 
    			EMbmAvkonQgn_indi_marked_add_mask );
    AddIconL( icons, skinId, *aknIconFile, EMbmAvkonQgn_prop_empty , 
    			EMbmAvkonQgn_prop_empty_mask );


    //get old icons and delete those
    CAknIconArray* Oldicons = static_cast<CAknIconArray*>(
        iListBox->ItemDrawer()->ColumnData()->IconArray() );    
    delete Oldicons;

    //set new icons to array
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );  
    CleanupStack::PopAndDestroy(aknIconFile);
    CleanupStack::Pop(icons);
    }

// -----------------------------------------------------------------------------
// CLocMainContainer::SelectedRow
// -----------------------------------------------------------------------------
//
TInt CBlidSavedWayptsContainer::SelectedRow() const
    {
    return iListBox->CurrentItemIndex();
    }

// -----------------------------------------------------------------------------
// CLocMainContainer::SetSelectedRow
// -----------------------------------------------------------------------------
//
void CBlidSavedWayptsContainer::SetSelectedRow(TInt aIndex) const
    {
    iListBox->SetCurrentItemIndex(aIndex);
    }

// -----------------------------------------------------------------------------
// CLocMainContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CBlidSavedWayptsContainer::HandleResourceChange(TInt aType)
    {    
    CBlidBaseContainer::HandleResourceChange( aType );
    if( aType == KAknsMessageSkinChange )
        {             
        TRAPD( error, UpdateIconsL() );
        if ( error != KErrNone )
        	{
            iCoeEnv->HandleError( error );
            }                 
        iListBox->DrawDeferred();
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iView.Update(this);
        }
    }

// ----------------------------------------------------------------------------
// CBlidSavedWayptsContainer::FocuChanged()
// ----------------------------------------------------------------------------
//
void CBlidSavedWayptsContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if (iListBox)
		{
		iListBox->SetFocus(IsFocused(), aDrawNow);			
		}		
	}

// -----------------------------------------------------------------------------
// CLocMainContainer::HandleMarkCommandL
// -----------------------------------------------------------------------------
//
void CBlidSavedWayptsContainer::HandleMarkCommandL(TInt aCommand)
	{
	AknSelectionService::HandleMarkableListProcessCommandL(aCommand,iListBox); 
	}

// ----------------------------------------------------
// CBlidSavedWayptsContainer::MakeEmptyTextListBoxL
// ----------------------------------------------------
//
void CBlidSavedWayptsContainer::MakeEmptyTextListBoxL( TInt aResourceText )
    {
    HBufC* textFromResourceFile;
    textFromResourceFile = iCoeEnv->AllocReadResourceLC( aResourceText );
    iListBox->View()->SetListEmptyTextL( *textFromResourceFile );
    CleanupStack::PopAndDestroy();  // textFromResourceFile.
    }

#ifdef RD_SCALABLE_UI_V2 
// ---------------------------------------------------------------------------
// CBlidSavedWayptsContainer::SetListBoxObserver
// ---------------------------------------------------------------------------
//
void CBlidSavedWayptsContainer::SetListBoxObserver( MEikListBoxObserver* aObserver )
    {
    iListBox->SetListBoxObserver( aObserver );
    }
#endif // RD_SCALABLE_UI_V2    	
// End of File
